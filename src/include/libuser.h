#pragma once
#include <stdint.h>

/* Userspace System Calls */
void sys_print(char* s);

/* Userspace GPU Calls */
void gpu_flip(void);

/* Userspace standard library functions */
int printf(const char* format, ...);
