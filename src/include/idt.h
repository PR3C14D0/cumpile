#pragma once
#include <stdint.h>

struct idt_entry {
    uint16_t low;
    uint16_t sel;
    uint8_t zero;
    uint8_t flags;
    uint16_t high;
} __attribute((packed));

struct idt_ptr {
    uint16_t limit;
    uint32_t base;
} __attribute((packed));

void idt_init();