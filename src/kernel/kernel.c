#include <stdint.h>
#include "vga.h"
#include "idt.h"

#define VGA_MEMORY ((uint16_t*)0xB8000)
#define VGA_WIDTH 80

static void clear_screen() {
    uint16_t* pVga = VGA_MEMORY;
    for(uint32_t i = 0; i < 80 * 25; i++) {
        pVga[i] = (uint16_t)(' ' | (0x0F << 8)); // White on black
    }
}

static void kprintf(const char* str) {
    vga_print(str);
}

const char* hello = "Hello from syscall!\n";
void _start() {
    clear_screen();
    kprintf("Welcome to cumpile!\n");

    idt_init();

    volatile int a = 0;
    volatile int b = 1;
    volatile int c = b/a;

    asm volatile (
        "mov $1, %%eax\n"
        "int $0x80\n"
        :
        : "b"(hello)
        : "eax"
    );

    while(1) {
        __asm__ volatile("hlt");
    }
}