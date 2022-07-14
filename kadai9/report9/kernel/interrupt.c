#include "interrupt.h"
#include "lapic_timer.h"

struct InterruptDescriptor {
  unsigned short offset_lo;
  unsigned short segment;
  unsigned short attribute;
  unsigned short offset_mid;
  unsigned int offset_hi;
  unsigned int reserved;
} __attribute__((packed));

struct InterruptDescriptor IDT[256];

void lapic_intr_handler();

void syscall_handler();

static void load_idt_to_idtr() {
  // Set idtr
  //
  // Here (9-A)
  //
  puts("load\n");
  char data[10];
  UINT16 tablelimit=sizeof(IDT)-1;
  UINT64 baseaddress=(void *)IDT;
  for(int i=0;i<2;i++){
    data[i]=(tablelimit>>(i*8))&(0xFFFF);
  }
  for(int i=2;i<10;i++){
    data[i]=(baseaddress>>((i-2)*8))&(0xFFFF);
  }

  asm volatile("lidt %0"::"m"(data));
}

static void register_intr_handler(unsigned char index,unsigned long long offset, unsigned short segment,unsigned short attribute){
  IDT[index].reserved=0;
  IDT[index].offset_lo=offset&(0xFFFF);
  IDT[index].offset_mid=(offset>>16)&(0xFFFF);
  IDT[index].offset_hi=(offset>>32)&(0xFFFFFFFF);
  IDT[index].segment=segment;
  IDT[index].attribute=attribute;

}

void init_intr() {
  // Get segment register value
  unsigned short cs;
  asm volatile ("mov %%cs, %0" : "=r"(cs));

  void* lapic_intr_handler_addr;
  asm volatile ("lea lapic_intr_handler(%%rip), %0" : "=r"(lapic_intr_handler_addr));

  void* syscall_handler_addr;
  asm volatile ("lea syscall_handler(%%rip), %[handler]" : [handler]"=r"(syscall_handler_addr));

  // Register Local APIC handler
  //
  // Here (9-A)
  //
  unsigned char index =48;
  unsigned short attribute=0xee00;
  register_intr_handler(index,lapic_intr_handler_addr,cs,attribute);
  // Register Sycall handler
  //
  // Here (10-A)
  //
  puts("init\n");

  // Tell CPU the location of IDT
  load_idt_to_idtr();

  // Set IF bit in RFLAGS register
  asm volatile ("sti");
}
