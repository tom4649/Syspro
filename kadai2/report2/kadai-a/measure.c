#include <sys/types.h>
#include <unistd.h>
#include <time.h>
#include<stdio.h>

int main(){
    struct timespec start,end;
    clock_gettime(CLOCK_MONOTONIC,&start);
    getpid();
    clock_gettime(CLOCK_MONOTONIC,&end);
    long res1=(long)(end.tv_sec-start.tv_sec)*1e9+(end.tv_nsec-start.tv_nsec);
    printf("1回:%ld ns\n",res1);
    clock_gettime(CLOCK_MONOTONIC,&start);
    for(int i=0;i<1000;i++){
    getpid();
    }
    clock_gettime(CLOCK_MONOTONIC,&end);
    long res2=(long)(end.tv_sec-start.tv_sec)*1e9+(end.tv_nsec-start.tv_nsec);
    printf("1000回:%ld ns\n",res2);
    return 0;
}