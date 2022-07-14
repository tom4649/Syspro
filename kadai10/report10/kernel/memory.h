#pragma once
#include "sched.h"

void init_virtual_memory();
extern unsigned long long task_cr3s[TASK_NUM];
extern unsigned long long kernel_cr3;
