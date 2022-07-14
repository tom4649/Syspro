.global lapic_intr_handler
lapic_intr_handler:
  push %rax
  push %rbx
  push %rcx
  push %rdx
  push %rdi
  push %rsi
  push %rbp
  mov %rsp, %rdi
  call lapic_intr_handler_internal
  pop %rbp
  pop %rsi
  pop %rdi
  pop %rdx
  pop %rcx
  pop %rbx
  pop %rax
  iretq

.global syscall_handler
syscall_handler:
  // Save Registers
  //
  // Here (10-A)
  //
  push %rax
  push %rbx
  push %rcx
  push %rdx
  push %rdi
  push %rsi
  push %rbp

  call syscall_handler_internal
  //
  // Here (10-A)
  //
  pop %rbp
  pop %rsi
  pop %rdi
  pop %rdx
  pop %rcx
  pop %rbx
  pop %rbx

  // Resume
  //
  // Here (10-A)
  //

  iretq
