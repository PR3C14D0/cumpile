#include <stdint.h>
#include "vga.h"
#include "idt.h"
#include "gdtc.h"
#include "tss.h"

extern void enter_user_mode();



void _start() {
    vga_clear_screen();
    vga_print("Welcome to cumpile! (Graphics Mode Active)\n");
    vga_print("Width: ");
    vga_print_dec(graphics_width);
    vga_print(" Height: ");
    vga_print_dec(graphics_height);
    vga_print(" BPP: ");
    vga_print_dec(graphics_bpp);
    vga_print(" FB: ");
    vga_print_hex(graphics_framebuffer);
    vga_print("\n");

    gdt_init();
    idt_init();
    tss_init();

    enter_user_mode();

    while(1) {
        __asm__ volatile("hlt");
    }
}