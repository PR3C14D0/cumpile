#pragma once
#include <stdint.h>

void syscall_init();
void syscall_handler(uint32_t eax, uint32_t ebx);