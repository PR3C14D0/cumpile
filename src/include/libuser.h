#pragma once
#include <stdint.h>

/* Userspace System Calls */
void sys_print(char* s);

/* Userspace standard library functions */
int printf(const char* format, ...);
