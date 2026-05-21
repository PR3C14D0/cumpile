BITS 16
section .text

global start
extern load_kernel
extern kernel_entry

start:
    ; Clear segment registers
    xor ax, ax
    mov ds, ax
    mov es, ax
    mov ss, ax
    mov sp, 0x7C00 ; Stack grows down from boot sector

    ; Get VBE Mode Info for 1024x768x32 (mode 0x118)
    mov ax, 0x4F01
    mov cx, 0x118
    mov di, 0x9000      ; Store Mode Info block at 0x9000
    int 0x10
    cmp ax, 0x004F
    jne .no_vbe

    ; Save VBE information to fixed memory address 0x6000
    mov eax, [0x9000 + 40] ; Physical base address of linear framebuffer (LFB)
    mov [0x6000], eax

    mov ax, [0x9000 + 18]  ; X resolution (width)
    mov [0x6004], ax

    mov ax, [0x9000 + 20]  ; Y resolution (height)
    mov [0x6006], ax

    mov al, [0x9000 + 25]  ; Bits per pixel (depth)
    mov [0x6008], al

    ; Set VBE Mode 1024x768x32 with Linear Frame Buffer (LFB) bit set (bx = 0x118 | 0x4000 = 0x4118)
    mov ax, 0x4F02
    mov bx, 0x4118
    int 0x10
    cmp ax, 0x004F
    jne .no_vbe
    jmp .switch_pm

.no_vbe:
    ; Fallback: print red ERR on VGA text screen and hang
    mov ax, 0xB800
    mov es, ax
    mov word [es:0], 0x0C45 ; Red 'E'
    mov word [es:2], 0x0C52 ; Red 'R'
    mov word [es:4], 0x0C52 ; Red 'R'
.hang_vbe:
    hlt
    jmp .hang_vbe

.switch_pm:
    cli ; Disable interrupts before protected mode transition

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
    mov ds, ax ; DS = Data segment
    mov es, ax ; ES = Data segment
    mov ss, ax ; SS = Stack segment
    mov fs, ax ; FS = Extra segment
    mov gs, ax ; GS = Other extra segment

    ; Define stack pointer in high memory
    mov esp, 0x90000

    call load_kernel 
    jmp [kernel_entry]
.hang:
    jmp .hang