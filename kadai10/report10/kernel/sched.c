#pragma GCC diagnostic ignored "-Wunused-parameter"
#include "pm_timer.h"
#include "sched.h"
#include "memory.h"
//#define STACK_SIZE 1024
#define APP1_START 0x104000000

// char stack1[STACK_SIZE];
// char stack2[STACK_SIZE];
// char stack3[STACK_SIZE];

// void task1(){
//   while(1){
//     puts("hello from task1\n");
//     volatile int i = 100000000;
//     while(i--);
//     }
// }

// void task2(){
//   while(1){
//     puts("hello from task2\n");
//     volatile int i = 100000000;
//     while(i--);
//   }
// }

// void task3(){
//   while(1){
//     puts("hello from task3\n");
//     volatile int i = 100000000;
//     while(i--);
//   }
// }

struct Task{
  unsigned long long sp;
  unsigned long long cr3;
};

unsigned int current_task;


struct Task tasks[TASK_NUM];

static void init_task(int idx,unsigned char* stack_bottom,unsigned long long rip){
  unsigned long long cr3=tasks[idx].cr3;
  asm volatile ("mov %0,%%cr3" ::"r"(cr3));

  unsigned long long* sp=(unsigned long long*)stack_bottom;

  //ss
  unsigned long long ss;
  asm volatile ("mov %%ss,%0":"=r"(ss));

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
  asm volatile ("mov %0,%%rsp"::"r"(rflags));

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

asm volatile ("mov %0,%%cr3" ::"r"(kernel_cr3));
}



// void init_tasks(){
//   puts("init_tasks() called\n");
//   init_task(1, stack2+STACK_SIZE,(unsigned long long)task2);
//   init_task(2, stack3+STACK_SIZE,(unsigned long long) task3);
//   current_task=0;
//   task1();
// }

void init_tasks(){
  puts("init_tasks() called\n");
  for(int i=0;i<3;i++){
    tasks[i].cr3=task_cr3s[i];
  }
  init_task(1, 0x41000000,(unsigned long long)0x40000000);
  init_task(2, 0x41000000,(unsigned long long)0x40000000);
  current_task=0;
  unsigned long long rip = 0x40000000;
  unsigned long long cr3=tasks[0].cr3;
  asm volatile ("mov %0,%%cr3" ::"r"(cr3));
asm volatile ("jmp %0" :: "m"(rip));
}

void schedule(unsigned long long sp) {
  puts("schedule() called\r\n");
  tasks[current_task].sp=sp;
  current_task=(current_task+1)%TASK_NUM;
  lapic_set_eoi();
  unsigned long long next_sp=tasks[current_task].sp;
  asm volatile ("mov %0,%%rsp\n"::"a"(next_sp));
  unsigned long long cr3=tasks[current_task].cr3;
  asm volatile ("mov %0,%%cr3" ::"r"(cr3));
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
