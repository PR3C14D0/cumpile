#include <stdint.h>
#include "vga.h"
#include "idt.h"
#include "gdtc.h"
#include "tss.h"

extern void enter_user_mode();



static void clear_screen() {
    uint32_t* fb = (uint32_t*)graphics_framebuffer;
    uint32_t total_pixels = (uint32_t)graphics_width * graphics_height;
    
    /* Fill entire graphical screen with premium slate color */
    for (uint32_t i = 0; i < total_pixels; i++) {
        fb[i] = 0x1E1E2E;
    }
}

static void kprintf(const char* str) {
    vga_print(str);
}

void _start() {
    clear_screen();
    kprintf("Welcome to cumpile! (Graphics Mode Active)\n");

    gdt_init();
    idt_init();
    tss_init();

    enter_user_mode();

    while(1) {
        __asm__ volatile("hlt");
    }
}