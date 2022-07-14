#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/ip.h>
#include <arpa/inet.h>
#include <string.h>

int main(int argc,char *argv[]){
    if(argc<2){
        perror("invalid args");
        exit(1);
    }
    int port;
    port = atoi(argv[1]);

    int s=socket(AF_INET,SOCK_DGRAM,IPPROTO_UDP);
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
    int bufsize=256;
    char rec[bufsize];
    int r;
    while(1){
        if((r=recvfrom(s,rec,sizeof(rec),0,(struct sockaddr *)&addr,&size))<0){
            perror("receive error");
            exit(1);
        }
        if(sendto(s,rec,r,0,(struct sockaddr *)&addr,size)<0){
            perror("send error");
            exit(1);
        }
    }

}