#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <string.h>
#include "./parser/parse.h"
#include <signal.h>
#include <errno.h>


typedef struct background_{//バックグラウンドジョブを管理するための構造体
    pid_t grp_id;
    struct background_* next;
    struct background_* pre;
    job* j;
}background;

background* back_info;

background* init_background(pid_t grp_id,background* pre,job* j){
    background* b=malloc(sizeof(background));
    b->grp_id=grp_id;
    b->next=NULL;
    b->pre = pre;
    b->j=j;
    return b;
}
void destruct_background(background* b){
    if(b->next!=NULL){
        (b->next)->pre =b->pre;
    }
    if(b->pre!=NULL){
        (b->pre)->next=b->next;
    }
    free_job(b->j);
    free(b);
}
void append_background(pid_t grp_id,job * j){
    background* c=back_info;
    while(c->next!=NULL){c=c->next;}
    c->next = init_background(grp_id,c,j);
    return;
}

void piperec(int i,process* processlist[],pid_t id){//関数dojob内で作成するプロセスのリストのi番目を実行する
    pid_t pgrp=id;
    struct sigaction act,oldact;
    sigemptyset(&(act.sa_mask));
    act.sa_flags =0;
    act.sa_restorer = NULL;
    if(id == 0){
        act.sa_handler = SIG_IGN;
        sigaction(SIGTTOU,&act,&oldact);
        act.sa_handler=SIG_DFL;
        sigaction(SIGINT,&act,&oldact);//SIGINTを受け取ったときの動作をデフォルトに戻す
        pgrp = getpid();
        setpgid(0,pgrp);
        tcsetpgrp(0,pgrp);
    }else if(id==-1){
        pgrp = getpid();
        setpgid(0,pgrp);
    }else{
        setpgid(0,pgrp);//job内のプロセスのプロセスグループのidは一致させる
    }

    if(i==0){//一番最初だったら実行する
        process *pr = processlist[i];
        if(pr->input_redirection!=NULL){
            int f = open(pr->input_redirection,O_RDONLY);
            if(f<0){
                perror("open error  - input");
                exit(1);
            }
            dup2(f,0);
        }
        execvp(pr->program_name,pr->argument_list);
    }else{//一番始めのプロセスでなければプロセスを生成して子プロセスで先に実行すべきプロセスを実行したあと、
    //その出力を受け取って親プロセスでi番目のプロセスを実行
        int fp[2];
        if(pipe(fp)<0){
            perror("pipe error");
            exit(1);
        }
        int pid,status;
        if((pid=fork())==0){//子
            close(fp[0]);
            dup2(fp[1],1);
            piperec(i-1,processlist,pgrp);
        }else{//親
            close(fp[1]);
            dup2(fp[0],0);
            process *pr = processlist[i];
            execvp(pr->program_name,pr->argument_list);
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
        process* processlist[process_num];//実行するプロセスのリストを作成。順番は実行する順番。
        int i;
        for(pr = jb->process_list,i=0; pr != NULL; pr = pr->next,i++)processlist[i]=pr;
        if((pr=processlist[process_num-1])->output_redirection!=NULL){
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
        pid_t id = (jb->mode == FOREGROUND ? 0 : -1);
        piperec(process_num-1,processlist,id);
    }
}
void handler_ish_SIGINT(int signal){printf("\n");printf(PROMPT);}
void sigaction_background(int signal,siginfo_t *info,void*ucontext){
    background* b = back_info;
    int status;
    pid_t id = getpgid(info->si_pid);
    if(id<0)return;//すでにwaitされたフォアグラウンドジョブなどの場合
    while(b !=NULL){
        if(b->grp_id ==id){break;}
        b=b->next;
    }
    if(b==NULL) return;
    else{
        destruct_background(b);
        waitpid(info->si_pid,&status,WUNTRACED);
    }
    return;
}

int main(int argc, char *argv[]){
    back_info=init_background(0,NULL,NULL);
    char s[LINELEN];
    struct sigaction act,oldact;
    sigemptyset(&(act.sa_mask));
    act.sa_flags =0;
    act.sa_restorer = NULL;
    act.sa_handler = handler_ish_SIGINT;
    sigaction(SIGINT,&act,&oldact);//ishがSIGINTを受け取った場合の処理
    while(get_line(s,LINELEN)){
        job *curr_job;
        if(!strcmp(s, "exit\n"))break;
        curr_job = parse_line(s);
        int pid,status;
        if((pid=fork())==0){
        dojob(curr_job);
        perror("not executed");
        exit(1);
        }else{
        if(curr_job->mode==FOREGROUND){
            act.sa_handler = SIG_IGN;
            sigaction(SIGTTOU,&act,&oldact);
            waitpid(pid,&status,WUNTRACED);
            pid_t grp = getpgrp();
            tcsetpgrp(0,grp);
            free_job(curr_job);
        }else{
            act.sa_sigaction = sigaction_background;
            act.sa_flags=SA_SIGINFO;
            sigaction(SIGCHLD,&act,&oldact);
            sigset_t sigset;
            sigemptyset(&sigset);
            sigaddset(&sigset,SIGCHLD);
            sigprocmask(SIG_BLOCK,&sigset,NULL);
            append_background(pid,curr_job);//backgroundジョブを追加
            sigprocmask(SIG_UNBLOCK,&sigset,NULL);
        }
    }
    }
}
