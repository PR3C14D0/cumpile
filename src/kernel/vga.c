#include "vga.h"
#include <stdint.h>

#define VGA_WIDTH 80
#define VGA_HEIGHT 25
#define VGA_SIZE (VGA_WIDTH * VGA_HEIGHT)
#define VGA_ATTRIB 0x0F

static uint16_t* const pcVGA = (uint16_t*)0xB8000;
static uint32_t nCursor = 0;

static void vga_scroll() {
    for (uint32_t i = 0; i < VGA_WIDTH * (VGA_HEIGHT - 1); i++) {
        pcVGA[i] = pcVGA[i + VGA_WIDTH];
    }
    for (uint32_t i = VGA_WIDTH * (VGA_HEIGHT - 1); i < VGA_SIZE; i++) {
        pcVGA[i] = (VGA_ATTRIB << 8) | ' ';
    }
    nCursor = VGA_WIDTH * (VGA_HEIGHT - 1);
}

void vga_putc(char c) {
    if (c == '\n') {
        nCursor = (nCursor / VGA_WIDTH + 1) * VGA_WIDTH;
    } else if (c == '\r') {
        nCursor = (nCursor / VGA_WIDTH) * VGA_WIDTH;
    } else if (c == '\t') {
        nCursor = (nCursor + 8) & ~7;
    } else {
        pcVGA[nCursor++] = (VGA_ATTRIB << 8) | c;
    }

    if (nCursor >= VGA_SIZE) {
        vga_scroll();
    }
}

void vga_print(const char* pcStr) {
    while (*pcStr) {
        vga_putc(*pcStr++);
    }
}

void vga_print_hex(uint32_t val) {
    char szHex[] = "0123456789ABCDEF";
    vga_print("0x");
    for (int i = 28; i >= 0; i -= 4) {
        vga_putc(szHex[(val >> i) & 0x0F]);
    }
}

void vga_print_dec(uint32_t val) {
    if (val == 0) {
        vga_putc('0');
        return;
    }
    char szBuf[12];
    int i = 10;
    szBuf[11] = '\0';
    while (val > 0) {
        szBuf[--i] = '0' + (val % 10);
        val /= 10;
    }
    vga_print(&szBuf[i]);
}