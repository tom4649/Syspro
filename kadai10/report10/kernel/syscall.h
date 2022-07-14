typedef enum SYSCALL{
  SYSCALL_PUTS,
}SYSCALL;
unsigned long long syscall_handler_internal(unsigned long long syscall_id,
unsigned long long arg1, unsigned long long arg2, unsigned long long arg3) ;

