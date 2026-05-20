#include "vga.h"
#include <stdint.h>

static uint16_t* const pcVGA = (uint16_t*)0xB8000;
static uint32_t nCursor = 0;

void vga_putc(char c) {
    pcVGA[nCursor++] = (0x0F << 8) | c;
}

void vga_print(const char* pcStr) {
    while (*pcStr) {
        vga_putc(*pcStr++);
    }
}