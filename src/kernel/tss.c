#include "tss.h"
#include "gdtc.h"

struct tss_t tss;

extern uint64_t gdt[];

void gdt_set_tss(int i, uint32_t base, uint32_t limit) {
    uint64_t descriptor = 0;

    descriptor  = (uint64_t)(limit & 0x0000FFFF);          // bits  0-15: limit_low
    descriptor |= (uint64_t)(base  & 0x0000FFFF) << 16;    // bits 16-31: base_low
    descriptor |= (uint64_t)((base >> 16) & 0xFF) << 32;   // bits 32-39: base_mid
    descriptor |= (uint64_t)0x89 << 40;                    // bits 40-47: access (present, ring0, TSS32)
    descriptor |= (uint64_t)((limit >> 16) & 0x0F) << 48;  // bits 48-51: limit_high
    descriptor |= (uint64_t)((base >> 24) & 0xFF) << 56;   // bits 56-63: base_high

    gdt[i] = descriptor;
}

extern void load_tss();

void tss_init() {
    tss.esp0 = 0x90000;
    tss.ss0 = 0x10;

    for(uint32_t i = 0; i < sizeof(tss) / 4; i++) {
        ((uint32_t*)&tss)[i] = 0;
    }

    tss.esp0 = 0x90000;
    tss.ss0 = 0x10;
    tss.iomap_base = sizeof(tss);

    gdt_set_tss(5, (uint32_t)&tss, sizeof(tss));

    load_tss();
}