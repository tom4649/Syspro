#include "lapic_timer.h"
#include "pm_timer.h"
#define COUNT_MAX 0xffffffff

volatile unsigned int *lvt_timer = (unsigned int *)0xfee00320;
volatile unsigned int *initial_count = (unsigned int *)0xfee00380;
volatile unsigned int *current_count = (unsigned int *)0xfee00390;
volatile unsigned int *divide_config = (unsigned int *)0xfee003e0;

unsigned int lapic_timer_freq_khz = 0;

volatile unsigned int *lapic_eoi = (unsigned int *)0xfee000b0;


void (*reserved_callback)(unsigned long long);

unsigned int measure_lapic_freq_khz() {
  // Initialize LAPIC One-Shot timer
  *divide_config=0xB;
  *lvt_timer=(*lvt_timer&(UINT32)0x00000000);
  *initial_count=COUNT_MAX;
  // Wait 1000ms
  unsigned int start=*current_count;
  pm_timer_wait_millisec(1000);
  // Calculate LAPIC Freq.
  unsigned int end = *current_count;
  lapic_timer_freq_khz=(start-end)/1000;

  return lapic_timer_freq_khz;
}

void lapic_periodic_exec(unsigned int msec, void *callback) {
  if (!lapic_timer_freq_khz) {
    // Comment out me, if needed.
    puts("Call measure_lapic_freq_khz() beforehand\r\n");
    return;
  }
  puts("exec\n");
  reserved_callback = callback;

  // Set LAPIC Periodic Timer
  //
  // Here
  //
  *lvt_timer=(UINT32)0x00020030;
  *initial_count=lapic_timer_freq_khz*msec;
  puts("count_setted\n");
}

void lapic_set_eoi(){
  *lapic_eoi=0;
}

void lapic_intr_handler_internal(unsigned long long sp) {
  // Set End of Interrupt
  puts("internal\n");
  reserved_callback(sp);
  lapic_set_eoi();

}


