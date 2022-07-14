#include "memory.h"
#include "sched.h"

struct Entry {
  unsigned int present : 1;
  unsigned int writable : 1;
  unsigned int user_accessible : 1;
  unsigned int write_through_caching : 1;
  unsigned int disable_cache : 1;
  unsigned int accessed : 1;
  unsigned int dirty : 1;
  unsigned int huge_page : 1;
  unsigned int global : 1;
  unsigned int available1 : 3;
  unsigned long long physical_address : 40;
  unsigned int available2 : 11;
  unsigned int no_execute : 1;
} __attribute__((packed));

unsigned long long task_cr3s[TASK_NUM];
unsigned long long kernel_cr3;

unsigned long long app_address[3]={0x104000,0x105000,0x106000};

struct Entry PML4s[TASK_NUM][512]__attribute__((aligned(4096)));
struct Entry PDPs[TASK_NUM][512]__attribute__((aligned(4096)));
struct Entry PDs[TASK_NUM][512]__attribute__((aligned(4096)));
struct Entry PTs[TASK_NUM][8][512]__attribute__((aligned(4096)));

struct Entry kernel_PD[512]__attribute__((aligned(4096)));
struct Entry kernel_PTs[8][512]__attribute__((aligned(4096)));

struct Entry io_PD[512]__attribute__((aligned(4096)));
struct Entry fb_PT[512]__attribute__((aligned(4096)));
struct Entry lapic_PT[512]__attribute__((aligned(4096)));

void init_virtual_memory() {
  // Save kernel cr3 register value
  asm volatile ("mov %%cr3, %0" : "=r"(kernel_cr3));

  // Page mapping
  //
  // Here 10
  //
  unsigned long long mod =4096;
  for(unsigned int i=0;i<TASK_NUM;i++){
    task_cr3s[i]=PML4s[i];
    PML4s[i][0].physical_address= (unsigned long long)(PDPs[i])/mod;
    PDPs[i][1].physical_address= (unsigned long long)(PDs[i])/mod;
    PDPs[i][3].physical_address= (unsigned long long)(io_PD)/mod;
    PDPs[i][4].physical_address= (unsigned long long)(kernel_PD)/mod;
    for(int k=0;k<8;k++){
        PDs[i][k].physical_address= (unsigned long long)(PTs[i][k])/mod;
    }


    for(unsigned int j=0;j<512;j++){
      PML4s[i][j].huge_page=0;

      PDs[i][j].physical_address= (unsigned long long)(PTs[i][0])/mod;
      
      PML4s[i][j].present=1;
      PML4s[i][j].writable=1;
      PDPs[i][j].present= 1;
      PDPs[i][j].writable= 1;
      PDs[i][j].present= 1;
      PDs[i][j].writable= 1;

      for(int k=0;k<8;k++){
        PTs[i][k][j].huge_page=0;
        PTs[i][k][j].physical_address=app_address[i]+k*512+j;

        PTs[i][k][j].present=1;
        PTs[i][k][j].writable=1;
      }
    } 
  }

  for(unsigned int i=0;i<512; i++){
    for(int k=0;k<8;k++){
      kernel_PD[k].physical_address= (unsigned long long)(kernel_PTs[k])/mod;
    }
      kernel_PD[i].present=1;
      kernel_PD[i].writable=1;
    for(unsigned int j=0;j<8; j++){
      kernel_PTs[j][i].huge_page=0;
      kernel_PTs[j][i].physical_address=0x100000+j*512+i;
      kernel_PTs[j][i].present=1;
      kernel_PTs[j][i].writable=1;
    }
  }
  io_PD[0].physical_address=(unsigned long long)(fb_PT)/mod;
  io_PD[503].physical_address=(unsigned long long)(lapic_PT)/mod;
  for(unsigned int i=0;i<512; i++){
    fb_PT[i].huge_page=0;
    lapic_PT[i].huge_page=0;

    fb_PT[i].physical_address=0x0c0000+i;
    lapic_PT[i].physical_address=0x0fee00+i;

    io_PD[i].present=1;
    io_PD[i].writable=1;
    fb_PT[i].present=1;
    fb_PT[i].writable=1;
    lapic_PT[i].present=1;
    lapic_PT[i].writable=1;
  
  }

}
