#include "syscall.h"
#include "vga.h"

#define SYS_WRITE 1

#define GPU_FLIP 1

void syscall_handler(uint32_t eax, uint32_t ebx) {
    switch(eax) {
        case SYS_WRITE:
            vga_print((char*)ebx);
            break;
        default:
            vga_print("Unknown syscall\n");
            break;
    }
}

void gpucall_handler(uint32_t eax, uint32_t ebx) {
    switch(eax) {
        case GPU_FLIP:
            vga_print("Swapchain flip called\n");
            vga_flip();
            break;
        default:
            vga_print("Unknown gpucall\n");
            break;
    }
}

void syscall_init() {
    vga_print("Syscalls ready\n");
}