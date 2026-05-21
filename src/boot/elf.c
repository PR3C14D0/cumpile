#include "elf.h"
#include <stdint.h>

extern void read_disk(uint32_t lba, void* pBuffer);

/* Global variable where we store kernel entry point */
uint32_t kernel_entry;

void load_kernel() {
    uint8_t* pBuffer = (uint8_t*)0x10000;

    // Load 100 sectors of the kernel starting at sector 21 to ensure the whole ELF fits in memory.
    for (uint32_t i = 0; i < 100; i++) {
        // LBA 21 = Kernel start in disk
        read_disk(21 + i, pBuffer + (i * 512));
    }

    ElfHeader* pElf = (ElfHeader*)pBuffer;

    volatile uint16_t* pVga = (volatile uint16_t*)0xB8000;

    // Check ELF Magic
    if (pElf->ident[0] != 0x7F || pElf->ident[1] != 'E' || pElf->ident[2] != 'L' || pElf->ident[3] != 'F') {
        // If ELF is not valid, write a red ERR on screen
        pVga[0] = (uint16_t)('E' | (0x0C << 8)); // Red 'E'
        pVga[1] = (uint16_t)('R' | (0x0C << 8)); // Red 'R'
        pVga[2] = (uint16_t)('R' | (0x0C << 8)); // Red 'R'
        return;
    }

    pVga[0] = (uint16_t)('O' | (0x0A << 8)); // Green 'O'
    pVga[1] = (uint16_t)('K' | (0x0A << 8)); // Green 'K'

    /* Parse program header */
    ProgramHeader* pPh = (ProgramHeader*)(pBuffer + pElf->phoff);
    // pElf->phoff = Offset inside the ELF where Program header table is

    /* Load kernel sectors */

    /* Iterate between all program headers */
    for(uint32_t i = 0; i < pElf->phnum; i++) {
        if(pPh[i].type != 1) continue; // PT_LOAD = 1

        /*
            Source: where ELF data lives in RAM
            Destination: Virtual address where segment must live
        */
        uint8_t* pSrc = pBuffer + pPh[i].offset;
        uint8_t* pDst = (uint8_t*)pPh[i].vaddr;

        // Copy segment data
        for(uint32_t j = 0; j < pPh[i].filesz; j++) {
            pDst[j] = pSrc[j];
        }
        
        // Zero out the remaining memsz space (for BSS)
        for(uint32_t j = pPh[i].filesz; j < pPh[i].memsz; j++) {
            pDst[j] = 0;
        }
    }
    kernel_entry = pElf->entry;
}