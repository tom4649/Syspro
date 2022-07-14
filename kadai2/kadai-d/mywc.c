#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

const int bufsize =512;//読み込むバイト数
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
    if(argc!=2){
        perror("invalid args");
        exit(1);
    }
    char *source=argv[1];
    int fd_s,nline=0,nword=0,nbite=0;
    fd_s = open(source,O_RDONLY);
    if(fd_s<0)die(source);
    char buf[bufsize];
    int r=0,flag=0;//flag:単語をたどっているとき1
    while(1){
    while((r=read(fd_s,buf,bufsize))==bufsize){
        nbite+=bufsize;
        for(int i=0;i<bufsize;i++){
            if(buf[i]!=' '&&buf[i]!=9&&buf[i]!='\n'){
                if(flag==0){flag=1;nword++;}
                continue;
            }
            if(buf[i]=='\n')nline++;
            flag=0;
        }
    }
    if(r<0)die("read error\n");
    nbite+=r;
    for(int i=0;i<r;i++){
            if(buf[i]!=' '&&buf[i]!=9&&buf[i]!='\n'){
                if(flag==0){flag=1;nword++;}
                continue;
            }
            if(buf[i]=='\n')nline++;
            flag=0;
        }
    if(r==0||read(fd_s,buf,bufsize)==0){
        if(close(fd_s)<0)die(source);
         break;//fileの終わり
    }
}
printf("%d %d %d %s\n",nline,nword,nbite,source);
return 0;
}

