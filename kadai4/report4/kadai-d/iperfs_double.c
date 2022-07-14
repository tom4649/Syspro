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
long maxdata = 4e10;//送受信するデータサイズ
long bufsize=1e9;//バッファのサイズ
int flag =0;//送信終了を表すフラグ
long min(long a,long b){
    if(a<b)return a;else return b;
}

void *server (void * arg){//スレッドが実行する関数
    int s= *(int *) arg;
    long todo=maxdata,datasize;
    char *rec=malloc(sizeof(char)*bufsize);
    while(todo>0){
    if((datasize=recv(s,rec,min(todo,bufsize),0))<0){
        perror("receive error");
        exit(1);
    }
    todo-=datasize;
    }
    flag=1;
    send(s,rec,1,0);//エラー処理は行わない
    free(rec);
    pthread_exit(NULL);
}

void *server_sub (void * arg){//スレッドが実行する関数
    int s= *(int *) arg;
    char *rec=malloc(sizeof(char)*bufsize);
    while(flag==0){
    if((send(s,rec,bufsize,0))<0){
        if(flag==1)break;
        perror("send error/sub");
        exit(1);
    }
    }
    free(rec);
    pthread_exit(NULL);
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
    if((ss=accept(s,(struct sockaddr *)&addr,&size))<0){
        perror("accept error");
        exit(1);
    }    
    
    char rec[1];
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

//双方向通信のスループット測定
    pthread_t thread1,thread2;
    if(pthread_create(&(thread1),NULL,server,&ss)!=0){
        printf("error: pthread_create\n");
        exit(1);
    }
    if(pthread_create(&(thread2),NULL,server_sub,&ss)!=0){
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
    close(s);close(ss);
}