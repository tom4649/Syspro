#include "syscall.h"
#include "util.h"

unsigned long long syscall_puts(char* str) {
  // Do puts
  //
  // Here (10-A)
  //
puts(str);

  return 0;
}

unsigned long long syscall_handler_internal(unsigned long long syscall_id,
    unsigned long long arg1, unsigned long long arg2, unsigned long long arg3) {
  unsigned long long ret;

  switch (syscall_id) {
    case 0:
      ret = syscall_puts((char *)arg1);
      break;
    default:
      break;
  }

  return ret;
}
