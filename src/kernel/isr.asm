BITS 32
global isr_0x80

%macro ISR_NOERRCODE 1
global isr_%1
isr_%1:
    cli
    push 0 
    push %1
    jmp isr_common_stub
%endmacro

%macro ISR_ERRCODE 1
global isr_%1
isr_%1:
    cli
    push %1 
    jmp isr_common_stub
%endmacro

ISR_NOERRCODE 0
ISR_NOERRCODE 1
ISR_NOERRCODE 2
ISR_NOERRCODE 3
ISR_NOERRCODE 4
ISR_NOERRCODE 5
ISR_NOERRCODE 6
ISR_NOERRCODE 7

ISR_ERRCODE   8

ISR_NOERRCODE 9

ISR_ERRCODE 10
ISR_ERRCODE 11
ISR_ERRCODE 12
ISR_ERRCODE 13
ISR_ERRCODE 14

ISR_NOERRCODE 15
ISR_NOERRCODE 16

ISR_ERRCODE 17

ISR_NOERRCODE 18
ISR_NOERRCODE 19
ISR_NOERRCODE 20
ISR_NOERRCODE 21
ISR_NOERRCODE 22
ISR_NOERRCODE 23
ISR_NOERRCODE 24
ISR_NOERRCODE 25
ISR_NOERRCODE 26
ISR_NOERRCODE 27
ISR_NOERRCODE 28
ISR_NOERRCODE 29

ISR_ERRCODE 30

ISR_NOERRCODE 31

extern exception_handler

isr_common_stub:
    pusha

    push esp
    call exception_handler
    add esp, 4 

    popa
    add esp, 8
    iret

extern syscall_handler
isr_0x80:
    pusha
    push ebx
    push eax
    call syscall_handler
    add esp, 8
    popa
    iret