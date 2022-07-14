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

    int s=socket(PF_INET,SOCK_DGRAM,IPPROTO_UDP);
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
    int datasize;
    char input[bufsize],rec[bufsize];

    while((datasize=fread(input,sizeof(char),sizeof(input),stdin))==bufsize){
        if(sendto(s,input,sizeof(input),0,(struct sockaddr *)&addr,size)<datasize){
            perror("send error");
            exit(1);
        }
        if((datasize=recvfrom(s,rec,sizeof(rec),0,(struct sockaddr *)&addr,&(size)))<0){
            perror("receive error");
            exit(1);
        }
        if(fwrite(rec,sizeof(char),datasize,stdout)<0){
            perror("write error");
            exit(1);
        }
    }
    if(datasize!=0){
    if(sendto(s,input,datasize,0,(struct sockaddr *)&addr,size)<0){
            perror("send error");
            exit(1);
        }
    if((datasize=recvfrom(s,rec,sizeof(rec),0,(struct sockaddr *)&addr,&(size)))<0){
            perror("receive error");
            exit(1);
        }
    if(fwrite(rec,sizeof(char),datasize,stdout)<0){
            perror("write error");
            exit(1);
        }
    }
    return 0;
}