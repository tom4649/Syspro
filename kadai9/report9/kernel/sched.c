#pragma GCC diagnostic ignored "-Wunused-parameter"
#include "pm_timer.h"
#define STACK_SIZE 1024
#define TASK_NUM 3

char stack1[STACK_SIZE];
char stack2[STACK_SIZE];
char stack3[STACK_SIZE];

void task1(){
  while(1){
    puts("hello from task1\n");
    volatile int i = 100000000;
    while(i--);
    }
}

void task2(){
  while(1){
    puts("hello from task2\n");
    volatile int i = 100000000;
    while(i--);
  }
}

void task3(){
  while(1){
    puts("hello from task3\n");
    volatile int i = 100000000;
    while(i--);
  }
}

struct Task{
  unsigned long long sp;
};

unsigned int current_task;


struct Task tasks[TASK_NUM];

static void init_task(int idx,unsigned char* stack_bottom,unsigned long long rip){
  unsigned long long* sp=(unsigned long long*)stack_bottom;

  //ss
  unsigned long long ss;
  asm volatile ("mov %%ss,%0":"=r"(ss));

  puts("task called\n");
  sp--;
  *sp=ss;

  //rsp
  sp--;
  *sp=(unsigned long long)stack_bottom ;

  UINT64 rflags=0x0;
   asm volatile (//rflags
       "mov %%rsp, %0\n"
       "mov %1, %%rsp\n"
       "pushfq\n":"=r"(rflags):"m"(sp));
  sp--;
  asm volatile ("mov %0,%%rsp"::"m"(rflags));

  //cs
  UINT16 cs;
  asm volatile ("mov %%cs,%0":"=r"(cs));
  sp--;
  *sp=(unsigned long long)cs;

    //rip
  sp--;
  *sp=rip;

  //汎用レジスタ
  for(int i=0;i<7;i++){
    sp--;
    *sp=0x00;
  }
tasks[idx].sp=sp;
}

void init_tasks(){
  puts("init_tasks() called\n");
  init_task(1, stack2+STACK_SIZE,(unsigned long long)task2);
  init_task(2, stack3+STACK_SIZE,(unsigned long long) task3);
  current_task=0;
  task1();
}

void schedule(unsigned long long sp) {
  puts("schedule() called\r\n");
  tasks[current_task].sp=sp;
  current_task=(current_task+1)%TASK_NUM;
  lapic_set_eoi();
  unsigned long long next_sp=tasks[current_task].sp;
  asm volatile ("mov %0,%%rsp\n"::"a"(next_sp));
  asm volatile (
  "pop %rbp\n"
  "pop %rsi\n"
  "pop %rdi\n"
  "pop %rdx\n"
  "pop %rcx\n"
  "pop %rbx\n"
  "pop %rax\n"
  "iretq\n");
}
