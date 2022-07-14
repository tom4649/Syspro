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

typedef struct suspended_{//SIGSTPによって中断されたプロセスを管理するための構造体
    pid_t pid;
    struct suspended_* next;
    struct suspended_* pre;
    job * j;
}suspended;

suspended* suspended_list;

suspended* init_suspended(pid_t pid,suspended* pre,job* job){
    suspended* s=malloc(sizeof(suspended));
    s->pid = pid;
    s->next=NULL;
    s->pre=pre;
    s->j=job;
    return s;
}
void destruct_suspended(suspended* b){
    if(b->next!=NULL){
        (b->next)->pre =b->pre;
    }
    if(b->pre!=NULL){
        (b->pre)->next=b->next;
    }
    free(b);
}
void append_suspended(pid_t pid,job*j){
    suspended* c=suspended_list;
    while(c->next!=NULL){c=c->next;}
    c->next = init_suspended(pid,c,j);
    return;
}


void handler_suspended(int signal){
    sigset_t sigset;
    int sig;
    sigemptyset(&sigset);
    sigaddset(&sigset,SIGCONT);
    sigwait(&sigset,&sig);
    return;
}

void set_foreground_signal(pid_t grp){//forefround jobとそのシグナル処理を設定する関数
        struct sigaction act,oldact;
        sigemptyset(&(act.sa_mask));
        act.sa_flags =0;
        act.sa_restorer = NULL;
        act.sa_handler = SIG_IGN;
        sigaction(SIGTTOU,&act,&oldact);
        act.sa_handler=SIG_DFL;
        sigaction(SIGINT,&act,&oldact);//SIGINTを受け取ったときの動作をデフォルトに戻す
        act.sa_handler=handler_suspended;
        sigaction(SIGTSTP,&act,&oldact);
        tcsetpgrp(0,grp);
}
void piperec(int i,process* processlist[],pid_t id){//関数dojob内で作成するプロセスのリストのi番目を実行する
    pid_t pgrp=id;
    if(pgrp == 0){
        pgrp = getpid();
        setpgid(0,pgrp);
        set_foreground_signal(pgrp);
    }else if(pgrp==-1){
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
        if(execvp(pr->program_name,pr->argument_list)<0)exit(0);
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
        pid_t id = ((jb->mode == FOREGROUND) ? 0 : -1);
        piperec(process_num-1,processlist,id);
    }
}

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

void set_ish_signal(){//ishのsignal処理を設定する関数
    struct sigaction act,oldact;
    sigemptyset(&(act.sa_mask));
    act.sa_flags =0;
    act.sa_restorer = NULL;
    act.sa_handler = SIG_IGN;
    sigaction(SIGTTOU,&act,&oldact);
    sigaction(SIGINT,&act,&oldact);
    sigaction(SIGTSTP,&act,&oldact);
    tcsetpgrp(0,getpgrp());
    act.sa_sigaction = sigaction_background;
    act.sa_flags=SA_SIGINFO;
    sigaction(SIGCHLD,&act,&oldact);//background jobのためのシグナル処理
}

void wait_background(pid_t pid,job* j){//background jobをwaitする準備をする関数
    sigset_t sigset;
    sigemptyset(&sigset);
    sigaddset(&sigset,SIGCHLD);
    sigprocmask(SIG_BLOCK,&sigset,NULL);
    append_background(pid,j);//backgroundジョブを追加
    sigprocmask(SIG_UNBLOCK,&sigset,NULL);
}

void do_bgcmd(char* s){//bgコマンドの処理
    job* curr_job = parse_line(s);
    suspended* pos=suspended_list->next;
    if(curr_job->process_list->argument_list[1]){//引数nがあった場合n番目に中断したプロセスを実行。無効な引数の場合は無視
        int n=atoi(curr_job->process_list->argument_list[1]);
        for(int i=0;i<n;i++){if(pos)pos=pos->next;}        
    }
    if(!pos)return;
    //background jobとして実行
    wait_background(pos->pid,pos->j);
    kill(pos->pid,SIGCONT);
    destruct_suspended(pos);
    free_job(curr_job);
}

void do_fgcmd(char* s){//fgコマンドの処理
    job* curr_job = parse_line(s);
    suspended* pos=suspended_list->next;
    if(curr_job->process_list->argument_list[1]){//引数nがあった場合n番目に中断したプロセスを実行。無効な引数の場合は無視
        int n=atoi(curr_job->process_list->argument_list[1]);
        for(int i=0;i<n;i++){if(pos)pos=pos->next;}        
    }
    if(!pos)return;
    free_job(curr_job);
    int status;
    //foreground jobとして実行
    set_foreground_signal(getpgid(pos->pid));
    kill(pos->pid,SIGCONT);
    while(waitpid(pos->pid,&status,WUNTRACED)<0){//プロセスが終了するまで待ち続ける
        //printf("waiting\n");
        if(WIFSTOPPED(status)){//プロセスが中断された場合
            append_suspended(pos->pid,pos->j);
            set_ish_signal();
            return;
        }
        status=0;
    }
    if(WIFSTOPPED(status)){//プロセスが中断された場合
            append_suspended(pos->pid,pos->j);
            set_ish_signal();
            return;
    }
    free_job(pos->j);
    destruct_suspended(pos);
    set_ish_signal();
}

int main(int argc, char *argv[]){
    back_info=init_background(0,NULL,NULL);
    suspended_list = init_suspended(0,NULL,NULL);
    char s[LINELEN];
    set_ish_signal();
    while(get_line(s,LINELEN)){
        set_ish_signal();
        if(!strcmp(s, "exit\n"))break;
        if(!strncmp(s, "bg",2)){//bgコマンドの処理
            do_bgcmd(s);
            continue;
        }
        if(!strncmp(s, "fg",2)){//fgコマンドの処理
            do_fgcmd(s);
            printf("fgfinish\n");
            continue;
        }
        job *curr_job;
        curr_job = parse_line(s);
        if(!(curr_job))continue;
        int pid,status;
        if((pid=fork())==0){
        dojob(curr_job);
        }
        else{
        if(curr_job->mode==FOREGROUND){
            waitpid(pid,&status,WUNTRACED);
            set_ish_signal();
            if(WIFSTOPPED(status)){//プロセスが中断された場合
                append_suspended(pid,curr_job);
            }
            else free_job(curr_job);
        }
        else wait_background(pid,curr_job);
    }
    }
    exit(0);
}
