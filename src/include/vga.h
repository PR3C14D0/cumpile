#pragma once
#include <stdint.h>
void vga_putc(char c);
void vga_print(const char* pcStr);
void vga_print_hex(uint32_t val);
void vga_print_dec(uint32_t val);