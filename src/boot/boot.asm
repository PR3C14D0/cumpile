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

    ; Enable A20
    call enable_a20

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

enable_a20:
; Wait input buffer empty (bit 1 = 0)
.wait_input:
    in al, 0x64
    test al, 00000010b
    jnz .wait_input

    ; disable keyboard
    mov al, 0xAD
    out 0x64, al
; Wait controller ready again
.wait_input_again:
    in al, 0x64
    test al, 00000010b
    jnz .wait_input_again

    ; Request output port read
    mov al, 0xD0
    out 0x64, al

.wait_output:
    in al, 0x64
    test al, 00000001b
    jz .wait_output

    ; Read output port
    in al, 0x60

    ; Enable A20
    or al, 00000010b
    mov bl, al

; Wait. Safe to send write command
.wait_input_write:
    in al, 0x64
    test al, 00000010b
    jnz .wait_input_write

    ; Write output port command
    mov al, 0xD1
    out 0x64, al

; Wait. Safe to send data
.wait_input_data:
    in al, 0x64
    test al, 00000010b
    jnz .wait_input_data
    
    ; Write modified output port (A20 enabled)
    mov al, bl
    out 0x60, al

; WAIT!!!!! Safe before re enabling keyboard
.wait_input_finish:
    in al, 0x64
    test al, 00000010b
    jnz .wait_input_finish

    ; Re enable keyboard
    mov al, 0xAE
    out 0x64, al

    ret

msg db "Welcome to cumpile!", 0

; Padding + boot signature
times 510 - ($ - $$) db 0
dw 0xAA55