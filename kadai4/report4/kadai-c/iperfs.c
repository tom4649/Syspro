#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/ip.h>
#include <arpa/inet.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>

int maxbacklog = 50; 
long maxdata = 8e10;//送受信するデータサイズ
long bufsize=1e9;//バッファのサイズ
long min(long a,long b){
    if(a<b)return a;else return b;
}


int main(int argc,char *argv[]){
    if(argc<2){
        perror("invalid args");
        exit(1);
    }
    int port;
    port = atoi(argv[1]);

    int ss,s=socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);
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
    
    char *rec=malloc(sizeof(char)*bufsize);
    if(rec==NULL){
        perror("malloc error");
        exit(1);
    }

    if((ss=accept(s,(struct sockaddr *)&addr,&size))<0){
        perror("accept error");
        exit(1);
    }
    
    long datasize;
    //RTTの測定
    if((datasize=recv(ss,rec,1,0))<1){
        perror("receive error");
        exit(1);
    }
    if(send(ss,rec,1,0)<1){
        perror("send error");
        exit(1);
    }
    //スループット+RTTの測定
    long todo=maxdata;
    while(todo>0){
    if((datasize=recv(ss,rec,min(todo,bufsize),0))<0){
        perror("receive error");
        exit(1);
    }
    todo-=datasize;
    }
    if((datasize=send(ss,rec,1,0))<0){
        perror("send error");
        exit(1);
    }
    close(ss);free(rec);
}