#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

const size_t bufsize =8192;//読み込むバイト数
void die(const char *s){
    perror(s);
    exit(1);
}

void iterate_write(FILE *f_t,void *buf,size_t t){
    int flag =fwrite(buf,1,t,f_t);
    if(flag<0)die("write error2\n");
    if(flag<t)iterate_write(f_t,buf,t-flag);
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
    FILE *f_s,*f_t;
    f_s = fopen(source,"r");
    if(!f_s)die(source);
    f_t = fopen(target,"w");
    if(!f_t)die(target);
    char buf[bufsize];
    int r=0;
    while(1){
    while((r=fread(buf,1,bufsize,f_s))==bufsize){
        int flag = fwrite(buf,1,bufsize,f_t);
        if(flag<0)die("write error\n");
        iterate_write(f_t,buf,bufsize-flag);
    }
    if(r<0)die("read error\n");
    iterate_write(f_t,buf,r);
    if(r==0||fread(buf,1,bufsize,f_s)==0){
        if(fclose(f_s)<0)die(source);
        if(fclose(f_t)<0)die(target);
        break;//fileの終わり
    }
}
    clock_gettime(CLOCK_MONOTONIC,&end);
    printf("%ldバイト:%ld ns\n",bufsize,end.tv_nsec-start.tv_nsec);
    return 0;
}



