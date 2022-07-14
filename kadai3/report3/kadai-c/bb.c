#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;

typedef struct bounded_buffer{
    int buf[10];
    int num;
}bounded_buffer;;

bounded_buffer *bb_create(){
    bounded_buffer *bb = malloc(sizeof(bounded_buffer));
    for(int i=0;i<10;i++){
        (bb->buf)[i]=0;
    }
    bb->num =0;
    return bb;
}
int bb_get(bounded_buffer *bb){
    pthread_mutex_lock(&mutex);
    if(bb->num >0){
        bb->num--;
        int ans=bb->buf[0];
        for(int i=0;i<bb->num;i++)(bb->buf)[i]=(bb->buf)[i+1];
        pthread_cond_signal(&cond);
        pthread_mutex_unlock(&mutex);
        return ans;
    }
    sched_yield();
    while(bb->num == 0){
        pthread_cond_wait(&cond,&mutex);
    }
    bb->num--;
    int ans=bb->buf[0];
    for(int i=0;i<bb->num;i++)(bb->buf)[i]=(bb->buf)[i+1];
    pthread_cond_signal(&cond);
    pthread_mutex_unlock(&mutex);
    return ans;
}

void bb_put(bounded_buffer *bb, int n){
    pthread_mutex_lock(&mutex);
    if(bb->num <10){
        bb->num++;
        (bb->buf)[bb->num-1]=n;
        pthread_cond_signal(&cond);
        pthread_mutex_unlock(&mutex);
        return;
    }
    while(bb->num==10)pthread_cond_wait(&cond,&mutex);
    bb->num++;
    (bb->buf)[bb->num-1]=n;
    pthread_cond_signal(&cond);
    pthread_mutex_unlock(&mutex);
    return;
}

void bb_destroy(bounded_buffer *bb){
    free(bb);
    return;
}