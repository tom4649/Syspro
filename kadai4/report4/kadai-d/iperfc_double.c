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
long maxdata = 4e8;//送受信するデータサイズ
long bufsize=1e9;//バッファのサイズ
int flag =0;//送信終了を表すフラグ
long res;
long min(long a,long b){
    if(a<b)return a;else return b;
}

void *server (void * arg){//スレッドが実行する関数
    int s= *(int *) arg;
    long todo=maxdata,datasize;
    char *rec=malloc(sizeof(char)*bufsize);
    for(int i=0;i<bufsize;i++)rec[i]='a';
    struct timespec start,end;
    clock_gettime(CLOCK_MONOTONIC,&start);
    while(todo>0){
    if((datasize=send(s,rec,min(todo,bufsize),0))<0){
        perror("send error");
        exit(1);
    }
    todo-=datasize;
    }
    flag=1;
    if((datasize=recv(s,rec,1,0))<0){
        perror("receive error");
        exit(1);
    }
    clock_gettime(CLOCK_MONOTONIC,&end);
    res=(long)(end.tv_sec-start.tv_sec)*1e9+(end.tv_nsec-start.tv_nsec);
    free(rec);
    pthread_exit(NULL);
}

void *server_sub (void * arg){//スレッドが実行する関数
    int s= *(int *) arg;
    char *rec=malloc(sizeof(char)*bufsize);
    while(flag==0){
    if((recv(s,rec,bufsize,0))<0){
        if(flag==1)break;
        perror("receive error/sub");
        exit(1);
    }
    }
    free(rec);
    pthread_exit(NULL);
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

    if(connect(s,(struct sockaddr *)&addr, size)<0){
        perror("connect error");
        exit(1);
    }
    
    char rec[1];rec[0]='a';
    long datasize;
    struct timespec start,end;
    //RTTの測定
    clock_gettime(CLOCK_MONOTONIC,&start);
    if((datasize=send(s,rec,1,0))<1){
        perror("send error");
        exit(1);
    }
    if(recv(s,rec,1,0)<1){
        perror("receive error");
        exit(1);
    }
    clock_gettime(CLOCK_MONOTONIC,&end);
    long rtt=(long)(end.tv_sec-start.tv_sec)*1e9+(end.tv_nsec-start.tv_nsec);

//双方向通信のスループット測定
    pthread_t thread1,thread2;
    if(pthread_create(&(thread1),NULL,server,&s)!=0){
        printf("error: pthread_create\n");
        exit(1);
    }
    if(pthread_create(&(thread2),NULL,server_sub,&s)!=0){
        printf("error: pthread_create\n");
        exit(1);
    }
    if(pthread_join(thread1,NULL)!=0){
        printf("error: pthread_join\n");
        exit(1);
    }
    if(pthread_join(thread2,NULL)!=0){
        printf("error: pthread_join\n");
        exit(1);
    }
    res=res-rtt;
    long sec = res/(1e9);double ssec =(double)(res-sec*1e9)/(1e9);
    double sendtime=(double)(sec)+ssec;
    double throughput=(double)maxdata/res*1e3;
    printf("%ld %f %f\n",maxdata/8,sendtime,throughput);
    close(s);
}