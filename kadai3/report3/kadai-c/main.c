#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <assert.h>
#include "bb.h"

int N=10000;

void *put1(void *b){
    bounded_buffer *bb= (bounded_buffer*)b;
    for(int i=0;i<N;i++)if(i%3==0)bb_put(bb,1);
    pthread_exit(NULL);
}
void *put2(void *b){
    bounded_buffer *bb= (bounded_buffer*)b;
    for(int i=0;i<N;i++)if(i%3==1)bb_put(bb,1);
    pthread_exit(NULL);
}
void *put3(void *b){
    bounded_buffer *bb= (bounded_buffer*)b;
    for(int i=0;i<N;i++)if(i%3==2)bb_put(bb,1);
    pthread_exit(NULL);
}

void *get1(void *b){
    bounded_buffer *bb= (bounded_buffer*)b;
    for(int i=0;i<N;i++)if(i%3==0){
        int n;n=bb_get(bb);
        assert(n==1);
        printf("%d\n",n);
    }
    pthread_exit(NULL);
}
void *get2(void *b){
    bounded_buffer *bb= (bounded_buffer*)b;
    for(int i=0;i<N;i++)if(i%3==1){
        int n;n=bb_get(bb);
        assert(n==1);
        printf("%d\n",n);
    }
    pthread_exit(NULL);
}
void *get3(void *b){
    bounded_buffer *bb= (bounded_buffer*)b;
    for(int i=0;i<N;i++)if(i%3==2){
        int n;n=bb_get(bb);
        assert(n==1);
        printf("%d\n",n);
    }
    pthread_exit(NULL);
}



int main(){
    bounded_buffer *bb=bb_create();
    /*複数の要素が挿入した順番で全て取り出せること*/
    for(int i=0;i<N;i++){
        if(i%20<10)bb_put(bb,i%20);
        else assert(bb_get(bb)==i%10);
    }

    /*要素を10000回挿入して10000回取り出せること*/
    pthread_t thread1,thread2,thread3,thread4,thread5,thread6;
    if(pthread_create(&thread1,NULL,put1,bb)!=0){
    printf("error: pthread_create\n");
    exit(1);
    }
    if(pthread_create(&thread2,NULL,get1,bb)!=0){
    printf("error: pthread_create\n");
    exit(1);
    }
    if(pthread_create(&thread3,NULL,put2,bb)!=0){
    printf("error: pthread_create\n");
    exit(1);
    }
    if(pthread_create(&thread4,NULL,get2,bb)!=0){
    printf("error: pthread_create\n");
    exit(1);
    }
    if(pthread_create(&thread5,NULL,put3,bb)!=0){
    printf("error: pthread_create\n");
    exit(1);
    }
    if(pthread_create(&thread6,NULL,get3,bb)!=0){
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
    if(pthread_join(thread3,NULL)!=0){
    printf("error: pthread_join\n");
    exit(1);
    }
    if(pthread_join(thread4,NULL)!=0){
    printf("error: pthread_join\n");
    exit(1);
    }
    if(pthread_join(thread5,NULL)!=0){
    printf("error: pthread_join\n");
    exit(1);
    }
    if(pthread_join(thread6,NULL)!=0){
    printf("error: pthread_join\n");
    exit(1);
    }
    bb_destroy(bb);
    return 0;

}