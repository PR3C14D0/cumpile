#include <stdint.h>

#define VGA_MEMORY ((uint16_t*)0xB8000)
#define VGA_WIDTH 80

static void clear_screen() {
    uint16_t* pVga = VGA_MEMORY;
    for(uint32_t i = 0; i < 80 * 25; i++) {
        pVga[i] = (uint16_t)(' ' | (0x0F << 8)); // White on black
    }
}

static void kprintf(const char* str) {
    uint16_t* pVga = VGA_MEMORY;

    uint32_t i = 0;
    uint32_t pos = 0;

    while(str[i]) {
        pVga[pos++] = (uint16_t)(str[i] | (0x0F << 8));
        i++;
    }
}

void _start() {
    clear_screen();
    kprintf("Welcome to cumpile!");
    while(1) {
        __asm__ volatile("hlt");
    }
}