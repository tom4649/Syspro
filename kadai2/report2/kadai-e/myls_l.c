#include <dirent.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <time.h>
#include <stdlib.h>
#include <regex.h>
#include <string.h>

void itoa(int num,char *buf){
    int i=0;
    while(num>0){
        buf[i]=num%10 + '0';
        num/=10;i++;
    }
    buf[i]='\0';
    int j;char temp;
    for(i=0,j=strlen(buf)-1;i<j;i++,j--){
        temp=buf[i];buf[i]=buf[j];buf[j]=temp;
    }
    return;
}

void search_name(uid_t num,char *name,int type){//idからnameを取得する関数(type 0:user,1,group)
    regex_t pat;
    int err;
    char cnum[512];itoa(num,cnum);
    char exp[1024]=".*:.*:";
    strcat(exp,cnum);
    if(type==0)strcat(exp,":.*:.*:.*:.*");
    if(type==1)strcat(exp,":.*");
    err=regcomp(&pat,exp, REG_EXTENDED | REG_NOSUB | REG_NEWLINE);
    if(err!=0){//error処理
        char buf[1024];
        regerror(err,&pat,buf,sizeof buf);
        perror(buf);
        exit(1);
    }
    FILE *f;
    if(type==0)f=fopen("/etc/passwd","r");
    else f=fopen("/etc/group","r");
    if(!f){perror("open error");exit(1);}
    char buf[4096];
    int flag=0;
    while(fgets(buf,sizeof buf,f)){
        if(regexec(&pat,buf,0,NULL,0)==0){
            int i=0;flag=1;
            while(buf[i]!=':'){
                name[i]=buf[i];
                i++;
            }
        name[i]='\0';
        }
    }
    fclose(f);
    regfree(&pat);
    if(flag==0){
        perror("no user");exit(1);
    }
    return;

}

void p_int_to_ch(const mode_t num,char res[10]){//数字表記のpermissionをアルファベットに変換
    int n=num;
    int list[9]={0,0,0,0,0,0,0,0,0};
    char *clist="rwxrwxrwx";
    for(int i=0;i<9;i++){
        if(n%2==1)list[8-i]=1;
        n=n/2;
    }
    for(int i=0;i<9;i++){
        if(list[i]==1)res[i]=clist[i];
        else res[i]='-';
    }
    res[9]='\0';
    return;
}

int main(){
    DIR *dp=opendir("./");
    if(dp==0){
        perror("open error");
        exit(1);
    }
    struct dirent *d;
    struct stat st;
    blkcnt_t sum=0;
    while((d=readdir(dp))!=0){
        if(d->d_name[0]=='.')continue;
        if(d->d_type==DT_DIR)printf("d");
        else if(d->d_type==DT_LNK)printf("l");
        else if(d->d_type==DT_BLK)printf("b");
        else if(d->d_type==DT_CHR)printf("c");
        else printf("-");//ファイルタイプの出力
        if(lstat(d->d_name,&st)<0){
            perror("stat");
            exit(1);
        };
        char perm[10]="rwxrwxrwx";p_int_to_ch(st.st_mode&~S_IFMT,perm);
        char user[1024],group[1024];
        search_name(st.st_uid,user,0);
        search_name(st.st_gid,group,1);
        sum+=st.st_blocks;
        struct tm *tmp;tmp=localtime(&(st.st_mtim.tv_sec));
        printf("%s %ld %s %s %5ld %2d月 %2d %02d:%02d %s",perm,st.st_nlink,user,group,st.st_size,tmp->tm_mon+1,tmp->tm_mday, tmp->tm_hour,tmp->tm_min,d->d_name);
    //順にパーミッション、ハードリンクの数、オーナー名、グループ名、バイトサイズ、タイムスタンプ、ファイル名を出力
    //以下シンボリックリンクの場合に対応
        if(d->d_type==DT_LNK){
            char linkname[128];
            if(readlink(d->d_name, linkname, 128)<0){
                perror("read link error");
                exit(1);
            }
            linkname[127]='\0';
            printf(" -> %s\n",linkname);
            continue;
        }
        printf("\n");
    }
    printf("合計 %ld\n",sum/2);//割り当てられたブロック数の合計
    
}

