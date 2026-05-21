BITS 32
global enter_user_mode

extern user_main

enter_user_mode:
    cli

    ; user stack pointer
    mov eax, 0x80000

    ; build iret stack frame
    push 0x20 | 3       ; user SS
    push eax            ; user ESP

    pushfd              ; 32-bit EFLAGS
    or dword [esp], 0x200 ; enable interrupts in user mode (IF = 0x200)

    push 0x18 | 3       ; user CS
    push user_main      ; user EIP

    ; load user data segments
    mov ax, 0x20 | 3
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax

    iret