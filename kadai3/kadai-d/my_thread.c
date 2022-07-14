#define _GNU_SOURCE
#include <sched.h>
#include <sys/mman.h>
#include <stdlib.h>
#include <sys/syscall.h>
#include <sys/wait.h>
#include <stdio.h>
#include <unistd.h>

static int test(void* a){
    printf("ok\n");
    return 0;
}

int main(){
    char *stack, *stackTop;
    pid_t pid;
    int STACK_SIZE =1024*1024;
    stack =malloc(STACK_SIZE);
    if(stack == NULL){
        perror("stack error");
        exit(1);
    }
    stackTop = stack + STACK_SIZE;
    pid = clone(test,stackTop,CLONE_THREAD|CLONE_SIGHAND|CLONE_VM,NULL);
    if(pid == -1){
        perror("clone error");
        exit(1);
    }
    sleep(1);
    return 0;
}