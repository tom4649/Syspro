#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/ip.h>
#include <arpa/inet.h>
#include <string.h>
#include <time.h>
#include <stdio.h>
#include <unistd.h>


long maxdata = 8e10;//送受信するデータサイズ
long bufsize=1e9;//バッファのサイズ

long min(long a,long b){
    if(a<b)return a;else return b;
}


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
    long datasize;
    struct timespec start,end;
    char *data = malloc(sizeof(char)*bufsize);
    for(int i=0;i<bufsize;i++)data[i]='a';
    if(connect(s,(struct sockaddr *)&addr, size)<0){
        perror("connect error");
        exit(1);
    }
    //RTTの測定
    clock_gettime(CLOCK_MONOTONIC,&start);
    if(send(s,data,1,0)<1){
        perror("send error");
        exit(1);
    }
    if((datasize=recv(s,data,1,0))<1){
        perror("receive error");
        exit(1);
    }
    clock_gettime(CLOCK_MONOTONIC,&end);
    long rtt=(long)(end.tv_sec-start.tv_sec)*1e9+(end.tv_nsec-start.tv_nsec);

    //スループット+RTTの測定
    clock_gettime(CLOCK_MONOTONIC,&start);
    long todo=maxdata;
    while(todo>0){
    if((datasize=send(s,data,min(todo,bufsize),0))<0){
        perror("send error");
        exit(1);
    }
    todo-=datasize;
    }
    if((datasize=recv(s,data,1,0))<0){
        perror("receive error");
        exit(1);
    }
    clock_gettime(CLOCK_MONOTONIC,&end);
    long res=(long)(end.tv_sec-start.tv_sec)*1e9+(end.tv_nsec-start.tv_nsec);
    res=res-rtt;
    free(data);
    long sec = res/(1e9);double ssec =(double)(res-sec*1e9)/(1e9);
    double sendtime=(double)(sec)+ssec;
    double throughput=(double)maxdata/res*1e3;
    printf("%ld %f %f\n",maxdata/8,sendtime,throughput);
    return 0;
}