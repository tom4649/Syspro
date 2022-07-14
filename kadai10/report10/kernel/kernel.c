#include "hardware.h"
#include "segmentation.h"
#include "sched.h"
#include "interrupt.h"
#include "memory.h"
#include "util.h"
#include "pm_timer.h"
#include "lapic_timer.h"
#include "syscall.h"
#pragma GCC diagnostic ignored "-Wunused-parameter" // For l6 (kernel_param_dummy)

void start(void *SystemTable __attribute__ ((unused)), struct HardwareInfo *_hardware_info, unsigned long long kernel_param_dummy) {
  // From here - Put this part at the top of start() function
  // Do not use _hardware_info directry since this data is located in UEFI-app space
  hardware_info = *_hardware_info;
  init_segmentation();
  init_virtual_memory();
  init_frame_buffer(&hardware_info.fb);
  init_acpi_pm_timer(hardware_info.rsdp);
  measure_lapic_freq_khz();


   init_intr();

 puts("ret\n");
unsigned long long ret;
char *str="sywer\t\n";
asm volatile (
  "mov %[id],%%rdi\n"
  "mov %[str],%%rsi\n"
  "int $0x80\n"
  "mov %%rax,%[ret]\n"
  :[ret]"=r"(ret)
  :[id]"r"((unsigned long long)SYSCALL_PUTS),
  [str]"m"((unsigned long long) str)
);
void* handler;
asm volatile ("lea schedule(%%rip), %[handler]":[handler]"=r"(handler));
lapic_periodic_exec(1000,handler);

init_tasks();
  // Do not delete it!
  while (1);
}
