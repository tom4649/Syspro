#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

const int bufsize =512;//読み込むバイト数
void die(const char *s){//エラーを出力して終了する関数
    perror(s);
    exit(1);
}

void iterate_write(int fd_t,void *buf,int t){
    int flag =write(fd_t,buf,t);
    if(flag<0)die("write error\0");
    if(flag<t)iterate_write(fd_t,buf+flag,t-flag);
    return;
}//bufの中身を全てfd_tが指すファイルにtバイト書き込む関数

int main(int argc,char *argv[]){
    if(argc!=3){
        perror("invalid args\0");
        exit(1);
    }
    char *source=argv[1], *target=argv[2];
    if(strcmp(source,target)==0){//コピー元とコピー先が同じ時の挙動
        char message[100]="mycp: '";
        strcat(message,source);
        strcat(message,"' と '");
        strcat(message,source);
        strcat(message,"' は同じファイルです\n");
        int flag=write(2,message,sizeof(message));
        if(flag<0)die("read error\0");
        exit(1);
    }
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
        if(flag<0)die("write error\0");
        iterate_write(fd_t,buf,bufsize-flag);
    }
    if(r<0)die("read error\0");
    iterate_write(fd_t,buf,r);
    if(r==0||read(fd_s,buf,bufsize)==0){
        if(close(fd_s)<0)die(source);
        if(close(fd_t)<0)die(target);
        return 0;//fileの終わり
    }
}

}

