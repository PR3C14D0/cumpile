BITS 16
section .text

global start
extern load_kernel
extern kernel_entry

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

    lgdt [gdt_descriptor]

    ; Enable protected mode
    mov eax, cr0
    or eax, 1
    mov cr0, eax

    jmp CODE_SEG:pm_entry
    

enable_a20:
    in al, 0x92
    test al, 2
    jnz .done
    or al, 2
    and al, 0xFE    ; Prevent CPU fast reset (bit 0)
    out 0x92, al
.done:
    ret

gdt_start:
    dq 0x0000000000000000 ; Null descriptor
    ; Code segment
    ; - Base: 0x00000000
    ; - Limit: 0xFFFF
    ; - Type: 0x9A -> Executable + Readable, kernel
    ; - Present: 1
    ; - 32 bit segment (D = 1)

    dq 0x00CF9A000000FFFF 

    ; Data segment
    ; - Base: 0x00000000
    ; - Limit: 0xFFFF
    ; - Type: 0x92 -> Executable + Readable, data segment
    ; - Present: 1
    ; - 32 bit segment (D = 1)
    dq 0x00CF92000000FFFF

    ; User code segment
    dq 0x00CFFA000000FFFF

    ; User data segment
    dq 0x00CFF2000000FFFF
gdt_end:

gdt_descriptor:
    dw gdt_end - gdt_start - 1
    dd gdt_start

; Selectors
CODE_SEG equ 0x08
DATA_SEG equ 0x10

; Protected mode entry
BITS 32
pm_entry:
    mov ax, DATA_SEG ;  Load data segment
    mov ds, ax ; DS = Data segment (general data)
    mov es, ax ; ES = Data segment (generic use)
    mov ss, ax ; SS = Stack segment (Stack in protected mode)
    mov fs, ax ; FS = Extra segment (Kernel free use)
    mov gs, ax ; GS = Other extra segment

    ; Define stack pointer in high memory
    mov esp, 0x90000

    call load_kernel 
    jmp [kernel_entry]
.hang:
    jmp .hang