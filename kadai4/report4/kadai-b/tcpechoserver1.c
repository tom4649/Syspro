#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/ip.h>
#include <arpa/inet.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>

int bufsize=256;
int maxbacklog = 50; 
int NTHREADS = 50;


void *server (void * arg){//各スレッドが実行する関数
    int s,ss = *(int *) arg;
    struct sockaddr_in peer_addr;
    socklen_t size_peer =sizeof(peer_addr);
    while(1){
    if((s=accept(ss,(struct sockaddr *)&peer_addr,&size_peer))<0){
        perror("accept error");
        exit(1);
    }
    char rec[bufsize];
    int datasize;
    while(1){
    if((datasize=recv(s,rec,bufsize,0))<0){
        perror("receive error");
        exit(1);
    }
    if(send(s,rec,datasize,0)<datasize){
        perror("send error");
        exit(1);
    }
    if(datasize<bufsize)break;
    }
    close(s);
    }
}

int main(int argc,char *argv[]){
    if(argc<2){
        perror("invalid args");
        exit(1);
    }
    int port;
    port = atoi(argv[1]);

    int s=socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);
    if(s<0){
        perror("socket error");
        exit(1);
    }
    struct sockaddr_in addr;
    socklen_t size=sizeof(addr);
    memset(&addr,0,size);
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = INADDR_ANY;

    if(bind(s,(struct sockaddr *)&addr,size)<0){
        perror("bind error");
        exit(1);
    }
    if(listen(s,maxbacklog)<0){
        perror("listen error");
        exit(1);
    }
    pthread_t *threads=malloc(sizeof(pthread_t)*NTHREADS);
    for(int i=0;i<NTHREADS;i++)if(pthread_create(&(threads[i]),NULL,server,&s)!=0){
        printf("error: pthread_create\n");
        exit(1);
    }
    for(int i=0;i<NTHREADS;i++)if(pthread_join(threads[i],NULL)!=0){
        printf("error: pthread_join\n");
        exit(1);
    }
    free(threads);

}