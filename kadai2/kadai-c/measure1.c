#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

const int bufsize =8192;//読み込むバイト数
void die(const char *s){
    perror(s);
    exit(1);
}

void iterate_write(int fd_t,void *buf,int t){
    int flag =write(fd_t,buf,t);
    if(flag<0)die("write error2\n");
    if(flag<t)iterate_write(fd_t,buf,t-flag);
    return;
}

int main(int argc,char *argv[]){
    struct timespec start,end;
    clock_gettime(CLOCK_MONOTONIC,&start);
    if(argc!=3){
        perror("invalid args");
        exit(1);
    }
    char *source=argv[1], *target=argv[2];
    int fd_s,fd_t;
    fd_s = open(source,O_RDONLY);
    if(fd_s<0)die(source);
    fd_t = open(target,O_WRONLY | O_CREAT,0666);
    if(fd_t<0)die(target);
    char buf[bufsize];
    int r=0;
    while(1){
    while((r=read(fd_s,buf,bufsize))==bufsize){
        int flag = write(fd_t,buf,bufsize);
        if(flag<0)die("write error\n");
        iterate_write(fd_t,buf,bufsize-flag);
    }
    if(r<0)die("read error\n");
    iterate_write(fd_t,buf,r);
    if(r==0||read(fd_s,buf,bufsize)==0){
        if(close(fd_s)<0)die(source);
        if(close(fd_t)<0)die(target);
        break;//fileの終わり
    }
}
    clock_gettime(CLOCK_MONOTONIC,&end);
    printf("%dバイト:%ld ns\n",bufsize,end.tv_nsec-start.tv_nsec);
    return 0;
}



