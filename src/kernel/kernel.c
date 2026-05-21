#include <stdint.h>
#include "vga.h"
#include "idt.h"
#include "gdtc.h"
#include "tss.h"

#define VGA_MEMORY ((uint16_t*)0xB8000)
#define VGA_WIDTH 80

extern void enter_user_mode();

static void clear_screen() {
    uint16_t* pVga = VGA_MEMORY;
    for(uint32_t i = 0; i < 80 * 25; i++) {
        pVga[i] = (uint16_t)(' ' | (0x0F << 8)); // White on black
    }
}

static void kprintf(const char* str) {
    vga_print(str);
}

void _start() {
    clear_screen();
    kprintf("Welcome to cumpile!\n");

    gdt_init();
    idt_init();
    tss_init();

    enter_user_mode();

    while(1) {
        __asm__ volatile("hlt");
    }
}