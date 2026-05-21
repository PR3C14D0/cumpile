global gdt_flush
global load_tss

gdt_flush:
    mov eax, [esp + 4]
    lgdt[eax]

    mov ax, 0x10
    mov ds, ax
    mov es, ax
    mov ss, ax
    mov fs, ax
    mov gs, ax

    jmp 0x08:.flush
.flush:
    ret

load_tss:
    mov ax, 0x28 ; GDT index 5 * 8
    ltr ax
    ret