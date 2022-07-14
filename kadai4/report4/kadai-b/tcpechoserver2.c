#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/ip.h>
#include <arpa/inet.h>
#include <string.h>
#include <sys/epoll.h>
#include <unistd.h>
#include <fcntl.h>

int bufsize=256;//バッファのサイズ
int maxbacklog = 50; //処理できない接続要求が貯められるキューの長さの最大値
int N = 10;//同時に処理可能な入力の最大値

//実装にはmanページを参考にした
void setnonblocking(int sock){//ソケットをノンブロッキングする関数
    int flag = fcntl(sock,F_GETFL,0);
    fcntl(sock,F_SETFL,flag | O_NONBLOCK);
}
void do_use_fd(int fd,int epollfd,struct epoll_event ev){//接続済みで使える状態になったファイルを使う関数
    char rec[bufsize];
    int datasize;
    if((datasize=recv(fd,rec,sizeof(rec),0))<0){
        perror("receive error");
        exit(1);
    }
    else if(send(fd,rec,datasize,0)<0){
        perror("send error");
        exit(1);
    }
    if(datasize==0){
        epoll_ctl(epollfd,EPOLL_CTL_DEL,fd,&ev);
        close(fd);
    }

    return;
}

int main(int argc,char *argv[]){
    if(argc<2){
        perror("invalid args");
        exit(1);
    }
    int port;
    port = atoi(argv[1]);

    int listen_sock=socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);
    if(listen_sock<0){
        perror("socket error");
        exit(1);
    }
    struct sockaddr_in addr;
    socklen_t size=sizeof(addr);
    memset(&addr,0,size);
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = INADDR_ANY;

    if(bind(listen_sock,(struct sockaddr *)&addr,size)<0){
        perror("bind error");
        exit(1);
    }
    if(listen(listen_sock,maxbacklog)<0){
        perror("listen error");
        exit(1);
    }
    struct epoll_event ev,events[N];
    int conn_sock,nfds,epollfd;
    if((epollfd=epoll_create1(0))<0){
        perror("epoll create");
        exit(1);
    }
    ev.events=EPOLLIN;ev.data.fd=listen_sock;
    if(epoll_ctl(epollfd,EPOLL_CTL_ADD,listen_sock,&ev)<0){
        perror("epoll_ctl:listen_sock");
        exit(1);
    }

    for(;;){
        nfds=epoll_wait(epollfd,events,N,-1);
        if(nfds<0){
            perror("wait error");
            exit(1);
        }

        for(int n=0;n<nfds;n++){
            if(events[n].data.fd == listen_sock){
                conn_sock = accept(listen_sock,(struct sockaddr *)&addr,&size);
                if(conn_sock<0){
                    perror("accept");
                    exit(1);
                }
                setnonblocking(conn_sock);
                ev.events=EPOLLIN | EPOLLET;
                ev.data.fd = conn_sock;
                if(epoll_ctl(epollfd,EPOLL_CTL_ADD,conn_sock,&ev)==-1){
                    perror("epoll_ctl:conn_sock");
                    exit(1);
                }
            }else{
                do_use_fd(events[n].data.fd,epollfd,ev);
            }
        }
    }
}