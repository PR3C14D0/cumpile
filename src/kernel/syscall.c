#include "syscall.h"
#include "vga.h"

#define SYS_WRITE 1

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

void syscall_init() {
    vga_print("Syscalls ready\n");
}