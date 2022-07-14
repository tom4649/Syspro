#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdlib.h>


int main(int argc, char *argv[],char *envp[]){
    int pid,status;
    char* name = argv[1];
    argv[argc]=NULL;
    if((pid=fork())==0){
        execve(name,argv+1,envp);
        perror("not executed");
        exit(1);
    }else{
        waitpid(pid,&status,WUNTRACED);
    }
}