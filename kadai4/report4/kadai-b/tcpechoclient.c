#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/ip.h>
#include <arpa/inet.h>
#include <string.h>

int bufsize=256;//バッファの長さ
int main(int argc,char *argv[]){
    if(argc<3){
        perror("invalid args");
        exit(1);
    }
    uint16_t port= atoi(argv[2]);
    char *host = (char*)argv[1];

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
    addr.sin_addr.s_addr = inet_addr(host);
    int readsize,datasize;
    char input[bufsize],rec[bufsize];

    if(connect(s,(struct sockaddr *)&addr, size)<0){
        perror("connect error");
        exit(1);
    }
    while((readsize=fread(input,sizeof(char),sizeof(input),stdin))==bufsize){
        if(send(s,input,readsize,0)<0){
            perror("send error");
            exit(1);
        }
        if((datasize=recv(s,rec,readsize,0))<0){
            perror("receive error");
            exit(1);
        }
        if(fwrite(rec,sizeof(char),datasize,stdout)<0){
            perror("write error");
            exit(1);
        }
    }
    if(send(s,input,readsize,0)<0){
            perror("send error");
            exit(1);
        }
    if((datasize=recv(s,rec,readsize,0))<0){
            perror("receive error");
            exit(1);
        }
    if(fwrite(rec,sizeof(char),datasize,stdout)<0){
            perror("write error");
            exit(1);
        }
    return 0;
}