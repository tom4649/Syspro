#include <signal.h>
#include <stdio.h>

volatile int count;
void handler(int signal){
    count++;
}

int main(){
    count=0;
    struct sigaction act,oldact;
    act.sa_handler = handler;
    sigemptyset(&(act.sa_mask));
    act.sa_flags =0;
    act.sa_restorer = NULL;
    sigaction(SIGINT,&act,&oldact);
    while(1){
        if(count>=10)break;
    }
    printf("exit\n");
    return 0;
}