BITS 32
global read_disk

; void read_disk(uint32_t lba, void* pBuffer)
read_disk:
    push ebp ; Save last base pointer
    mov ebp, esp ; Create standard stack frame

    ; Save register states
    push edi
    push ecx
    push edx
    push eax

    ; [ebp + 8]  -> lba (First argument)
    ; [ebp + 12] -> pBuffer (Second argument)

    ; Wait for drive to not be busy before writing registers
    mov dx, 0x1F7 ; ATA status port

.wait_ready:
    in al, dx ; Read disk state
    test al, 0x80 ; Bit 7 = BSY
    jnz .wait_ready ; If busy, wait

    ; Send the sector count (1 sector)
    mov dx, 0x1F2 ; Sector count register
    mov al, 1 ; Read 1 sector
    out dx, al

    ; Send LBA (Low byte 0-7 bits)
    mov eax, [ebp + 8] ; Load LBA
    mov dx, 0x1F3 ; LBA low byte register
    out dx, al ; Send AL

    ; LBA (Mid byte 8-15 bits)
    mov eax, [ebp + 8]
    shr eax, 8 ; Displace 8 bits
    mov dx, 0x1F4 ; LBA mid register
    out dx, al

    ; LBA (High byte 16-23 bits)
    mov eax, [ebp + 8]
    shr eax, 16 ; Displace 16 bits
    mov dx, 0x1F5 ; LBA high register
    out dx, al

    ; Send Drive and LBA bits 24-27
    mov eax, [ebp + 8]
    shr eax, 24 ; LBA 24-27 bits
    and al, 0x0F ; Only 4 valid bits

    ; 0xE0 means LBA mode, Master drive
    ; 1110 0000
    or al, 0xE0     

    mov dx, 0x1F6 ; Drive/Head register
    out dx, al

    ; Send Command (Read Sectors: 0x20)
    mov dx, 0x1F7 ; Command register
    mov al, 0x20 ; 0x20 = READ SECTORS (PIO)
    out dx, al

; Wait until disk is ready for transferring data
.wait_bsy:
    in al, dx
    test al, 0x80   ; BSY bit
    jnz .wait_bsy

.wait_drq:
    in al, dx
    test al, 0x08   ; DRQ bit (DRQ = Data request)
    jz .wait_drq

    ; Read sector data (256 words = 512 bytes)
    mov ecx, 256
    mov edi, [ebp + 12] ; pBuffer
    mov dx, 0x1F0 ; Disk data port

.read_loop:
    in ax, dx ; Reads 16 bits from disk
    mov [edi], ax ; It stores in memory
    add edi, 2 ; 2 bits forward
    loop .read_loop ; Loop 256 times

    ; Restore registers
    pop eax
    pop edx
    pop ecx
    pop edi

    pop ebp
    ret