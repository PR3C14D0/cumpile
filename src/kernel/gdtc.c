#include "gdtc.h"
#include <stdint.h>

struct gdt_entry {
    uint16_t limit_low;
    uint16_t base_low;
    uint8_t base_mid;
    uint8_t access;
    uint8_t granularity;
    uint8_t base_high;
} __attribute__((packed));

struct gdt_ptr {
    uint16_t limit;
    uint32_t base;
} __attribute__((packed));

struct gdt_entry gdt[6];
struct gdt_ptr pGDT;

extern void gdt_flush(uint32_t);

void set_gate(int i, uint32_t base, uint32_t limit, uint8_t access, uint8_t gran) {
    gdt[i].base_low = base;
    gdt[i].base_mid = base >> 16;
    gdt[i].base_high = base >> 24;

    gdt[i].limit_low = limit;
    gdt[i].granularity = (limit >> 16) & 0x0F;
    gdt[i].granularity |= gran & 0xF0;
    gdt[i].access = access;
}

void gdt_init() {
    pGDT.limit = sizeof(gdt) - 1;
    pGDT.base = (uint32_t)&gdt;

    set_gate(0, 0, 0, 0, 0);
    set_gate(1, 0, 0xFFFFFFFF, 0x9A, 0xCF);
    set_gate(2, 0, 0xFFFFFFFF, 0x92, 0xCF);
    set_gate(3, 0, 0xFFFFFFFF, 0xFA, 0xCF);
    set_gate(4, 0, 0xFFFFFFFF, 0xF2, 0xCF);

    set_gate(5, 0, 0, 0, 0); // TSS placeholder

    gdt_flush((uint32_t)&pGDT);
}