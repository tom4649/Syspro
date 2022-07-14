#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <string.h>
#include "./parser/parse.h"

void piperec(int i,process processlist[]){//関数dojob内で作成するプロセスのリストのi番目を実行する
    if(i==0){//一番最初だったら実行する
        process pr = processlist[i];
        if(pr.input_redirection!=NULL){
            int f = open(pr.input_redirection,O_RDONLY);
            if(f<0){
                perror("open error  - input");
                exit(1);
            }
            dup2(f,0);
        }
        execvp(pr.program_name,pr.argument_list);
    }else{//一番始めのプロセスでなければプロセスを生成して子プロセスで先に実行すべきプロセスを実行したあと、
    //その出力を受け取って親プロセスでi番目のプロセスを実行
        int fp[2];
        if(pipe(fp)<0){
            perror("pipe error");
            exit(1);
        }
        int pid,status;//flagは子プロセスが成功したかどうかを表す。0:成功
        if((pid=fork())==0){//子
            close(fp[0]);
            dup2(fp[1],1);
            piperec(i-1,processlist);
        }else{//親
            close(fp[1]);
            dup2(fp[0],0);
            process pr = processlist[i];
            execvp(pr.program_name,pr.argument_list);
            waitpid(pid,&status,WUNTRACED);
            exit(0);
        }
    }
}

void dojob(job* curr_job){//一連のジョブを実行する関数
    int index=0;
    job* jb;
    for(index = 0, jb = curr_job; jb != NULL; jb = jb->next, ++index) {
        process* pr;
        int process_num=0;
        for(pr = jb->process_list; pr != NULL; pr = pr->next,process_num++);
        process* processlist=malloc(sizeof(process)*process_num);//実行するプロセスのリストを作成。順番は実行する順番。
        int i;
        for(pr = jb->process_list,i=0; pr != NULL; pr = pr->next,i++)processlist[i]=*pr;
        if((pr=processlist+process_num-1)->output_redirection!=NULL){
            int f;
            if(pr->output_option==TRUNC){if((f=open(pr->output_redirection,O_TRUNC|O_CREAT|O_WRONLY,S_IWRITE|S_IREAD))<0){
                perror("open error - output");
                exit(1);
            }}
            else {if((f=open(pr->output_redirection,O_APPEND|O_WRONLY|O_CREAT,S_IWRITE|S_IREAD))<0){
                perror("open error - output");
                exit(1);
            }}
            dup2(f,1);
        }
        piperec(process_num-1,processlist);
        free(processlist);
    }
}

int main(int argc, char *argv[]){
    char s[LINELEN];
    job *curr_job;

    while(get_line(s,LINELEN)){
        if(!strcmp(s, "exit\n"))break;
        curr_job = parse_line(s);
        int pid,status;
        if((pid=fork())==0){
        dojob(curr_job);
        perror("not executed");
        exit(1);
        }else{
        waitpid(pid,&status,WUNTRACED);
        free_job(curr_job);
    }

    }
}
