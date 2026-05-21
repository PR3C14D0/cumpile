BITS 32
global isr_0x80

; Macro for exceptions that do not push an error code
%macro ISR_NOERRCODE 1
global isr_%1
isr_%1:
    cli             ; Disable interrupts
    push 0          ; Push dummy error code
    push %1         ; Push interrupt number
    jmp isr_common_stub
%endmacro

; Macro for exceptions that push an error code
%macro ISR_ERRCODE 1
global isr_%1
isr_%1:
    cli             ; Disable interrupts
    push %1         ; Push interrupt number
    jmp isr_common_stub
%endmacro

; CPU Exceptions (0-31)
ISR_NOERRCODE 0     ; 0: Divide By Zero
ISR_NOERRCODE 1     ; 1: Debug
ISR_NOERRCODE 2     ; 2: Non-Maskable Interrupt
ISR_NOERRCODE 3     ; 3: Breakpoint
ISR_NOERRCODE 4     ; 4: Into Detected Overflow
ISR_NOERRCODE 5     ; 5: Out of Bounds
ISR_NOERRCODE 6     ; 6: Invalid Opcode
ISR_NOERRCODE 7     ; 7: No Coprocessor

ISR_ERRCODE   8     ; 8: Double Fault

ISR_NOERRCODE 9     ; 9: Coprocessor Segment Overrun

ISR_ERRCODE 10      ; 10: Bad TSS
ISR_ERRCODE 11      ; 11: Segment Not Present
ISR_ERRCODE 12      ; 12: Stack Fault
ISR_ERRCODE 13      ; 13: General Protection Fault
ISR_ERRCODE 14      ; 14: Page Fault

ISR_NOERRCODE 15    ; 15: Unknown Interrupt
ISR_NOERRCODE 16    ; 16: Coprocessor Fault

ISR_ERRCODE 17      ; 17: Alignment Check

ISR_NOERRCODE 18    ; 18: Machine Check
ISR_NOERRCODE 19    ; 19: SIMD Floating-Point Exception
ISR_NOERRCODE 20    ; 20: Virtualization Exception
ISR_NOERRCODE 21    ; 21: Control Protection Exception
ISR_NOERRCODE 22    ; 22: Reserved
ISR_NOERRCODE 23    ; 23: Reserved
ISR_NOERRCODE 24    ; 24: Reserved
ISR_NOERRCODE 25    ; 25: Reserved
ISR_NOERRCODE 26    ; 26: Reserved
ISR_NOERRCODE 27    ; 27: Reserved
ISR_NOERRCODE 28    ; 28: Hypervisor Injection Exception
ISR_NOERRCODE 29    ; 29: VMM Communication Exception

ISR_ERRCODE 30      ; 30: Security Exception

ISR_NOERRCODE 31    ; 31: Reserved

extern exception_handler

; Common entry point for all CPU exceptions
isr_common_stub:
    pusha           ; Save EDI, ESI, EBP, ESP, EBX, EDX, ECX, EAX

    push esp        ; Pass pointer to cpu_state struct
    call exception_handler
    add esp, 4      ; Clean up stack

    popa            ; Restore general registers
    add esp, 8      ; Clean up error code and int number
    iret            ; Return to interrupted code

extern syscall_handler
; System call entry (int 0x80)
; Stack on entry (from ring 3 iret frame): eip, cs, eflags, esp, ss
isr_0x80:
    pusha               ; Save all GP regs: edi esi ebp esp ebx edx ecx eax
    ; After pusha, esp points to saved regs. pusha layout (top to bottom):
    ;   [esp+0]=edi [esp+4]=esi [esp+8]=ebp [esp+12]=esp_saved
    ;   [esp+16]=ebx [esp+20]=edx [esp+24]=ecx [esp+28]=eax
    mov eax, [esp + 28] ; restore actual eax (syscall number)
    mov ebx, [esp + 16] ; restore actual ebx (argument)
    push ebx            ; arg1: string ptr
    push eax            ; arg0: syscall number
    call syscall_handler
    add esp, 8          ; clean args
    popa                ; restore all regs
    iret                ; return to userspace