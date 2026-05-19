BITS 16
ORG 0x7C00

start:
    cli ; Disable interrupts

    ; Clear registers
    xor ax, ax
    mov ds, ax
    mov es, ax
    mov ss, ax

    mov sp, 0x7C00 ; Stack grows down from boot sector

    sti ; Enable interrupts

    mov si, msg

.print:
    lodsb
    or al, al
    jz .hang

    mov ah, 0x0E
    int 0x10
    jmp .print

.hang:
    jmp .hang

msg db "Welcome to cumpile!", 0

; Padding + boot signature
times 510 - ($ - $$) db 0
dw 0xAA55