#include <stdlib.h>
#include <pthread.h>
#include <stdio.h>

void *fun(void *s){
    //int *t = (int *)s;
    //printf("%d\n",*t);
    pthread_exit(NULL);
}

int main(){
    int N=50;
    pthread_t threads[N];
    for(int i=0;i<N;i++){
        if(pthread_create(&(threads[i]),NULL,fun,&i)!=0){
            printf("error pthread_create\n");
            exit(1);
        }
    }
    for(int i=0;i<N;i++){
        if(pthread_join(threads[i],NULL)!=0){
            printf("error pthread_join\n");
            exit(1);
        }
    }

}

