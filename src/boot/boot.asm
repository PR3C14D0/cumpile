BITS 16
org 0x7C00

start:
    cli ; Disable hardware interruptions

    ; Save drive number passed by BIOS in DL
    ; 0x00 = Floppy
    ; 0x80 = First hard disk
    mov [boot_drive], dl

    xor ax, ax ; AX = 0
    mov ds, ax ; DS = 0
    mov es, ax ; ES = 0
    mov ss, ax ; SS = 0
    mov sp, 0x7C00 ; Stack pointer = 0x7C00

    ; Print loading message
    mov si, loading_msg
    call print_string

    ; Loader (LBA read!)
    mov ah, 0x42 ; INT 13 function 42h = Extended LBA Read
    mov dl, [boot_drive] ; Get disk number
    mov si, disk_packet ; SI Points to Disk Address Packet
    int 0x13
    jc disk_error

    ; Print success and jump message
    mov si, jump_msg
    call print_string

    jmp 0x0000:0x8000 ; Far jump to the loader

disk_error:
    mov si, err_msg
    call print_string
.hang:
    jmp .hang

print_string:
    mov ah, 0x0E
.loop:
    lodsb
    or al, al
    jz .done
    int 0x10
    jmp .loop
.done:
    ret

align 4
disk_packet:
    db 0x10         ; Packet size (16 bytes)
    db 0            ; Reserved
    dw 20           ; Read 20 sectors (10240 bytes)
    dw 0x8000       ; Buffer offset (0x8000)
    dw 0x0000       ; Buffer segment (0x0000)
    dq 1            ; Start LBA (Sector 2 = LBA 1)

boot_drive db 0
loading_msg db "Loading loader...", 13, 10, 0
jump_msg db "Jumping to loader...", 13, 10, 0
err_msg db "Disk read failed!", 13, 10, 0

times 510 - ($ - $$) db 0
dw 0xAA55