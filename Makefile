ASM = nasm
CC = i686-elf-gcc
LD = i686-elf-ld

SRC = src
BUILD = build

BOOT_ASM = $(SRC)/boot/boot.asm
LOADER_ASM = $(SRC)/boot/loader.asm

KERNEL_SRC = $(SRC)/kernel/kernel.c
ELF_SRC = $(SRC)/kernel/elf.c
DISK_ASM = $(SRC)/kernel/disk.asm

VGA_SRC = $(SRC)/kernel/vga.c
SYSCALL_SRC = $(SRC)/kernel/syscall.c
IDT_SRC = $(SRC)/kernel/idt.c
ISR_ASM = $(SRC)/kernel/isr.asm

BOOT_BIN = $(BUILD)/boot.bin
LOADER_BIN = $(BUILD)/loader.bin
KERNEL_BIN = $(BUILD)/kernel.bin
OS_IMAGE = $(BUILD)/cumpile.bin

CFLAGS = -ffreestanding -O2 -Wall -Wextra -m32 -Isrc/include
LDFLAGS = -T linker.ld -nostdlib -m elf_i386

all: $(OS_IMAGE)

$(BUILD):
	mkdir -p $(BUILD)

# BOOT & LOADER
$(BOOT_BIN): $(BOOT_ASM) | $(BUILD)
	$(ASM) -f bin $(BOOT_ASM) -o $(BOOT_BIN)

$(BUILD)/loader.o: $(LOADER_ASM) | $(BUILD)
	$(ASM) -f elf32 $(LOADER_ASM) -o $(BUILD)/loader.o

$(BUILD)/elf.o: $(ELF_SRC) | $(BUILD)
	$(CC) $(CFLAGS) -c $(ELF_SRC) -o $(BUILD)/elf.o

$(BUILD)/disk.o: $(DISK_ASM) | $(BUILD)
	$(ASM) -f elf32 $(DISK_ASM) -o $(BUILD)/disk.o

$(LOADER_BIN): $(BUILD)/loader.o $(BUILD)/elf.o $(BUILD)/disk.o
	$(LD) -Ttext 0x8000 -e start -nostdlib -m elf_i386 --oformat binary $(BUILD)/loader.o $(BUILD)/elf.o $(BUILD)/disk.o -o $(LOADER_BIN)
	python3 -c "import os; f=open('$(LOADER_BIN)', 'ab'); f.write(b'\x00' * (10240 - os.path.getsize('$(LOADER_BIN)'))); f.close()"

# KERNEL
$(BUILD)/vga.o: $(VGA_SRC) | $(BUILD)
	$(CC) $(CFLAGS) -c $(VGA_SRC) -o $(BUILD)/vga.o

$(BUILD)/syscall.o: $(SYSCALL_SRC) | $(BUILD)
	$(CC) $(CFLAGS) -c $(SYSCALL_SRC) -o $(BUILD)/syscall.o

$(BUILD)/idt.o: $(IDT_SRC) | $(BUILD)
	$(CC) $(CFLAGS) -c $(IDT_SRC) -o $(BUILD)/idt.o

$(BUILD)/isr.o: $(ISR_ASM) | $(BUILD)
	$(ASM) -f elf32 $(ISR_ASM) -o $(BUILD)/isr.o

$(BUILD)/kernel.o: $(KERNEL_SRC) | $(BUILD)
	$(CC) $(CFLAGS) -c $(KERNEL_SRC) -o $(BUILD)/kernel.o

$(KERNEL_BIN): $(BUILD)/kernel.o $(BUILD)/vga.o $(BUILD)/syscall.o $(BUILD)/isr.o $(BUILD)/idt.o linker.ld
	$(LD) $(LDFLAGS) $(BUILD)/kernel.o $(BUILD)/vga.o $(BUILD)/isr.o $(BUILD)/idt.o $(BUILD)/syscall.o -o $(KERNEL_BIN)

# FINAL IMAGE
$(OS_IMAGE): $(BOOT_BIN) $(LOADER_BIN) $(KERNEL_BIN)
	cat $(BOOT_BIN) $(LOADER_BIN) $(KERNEL_BIN) > $(OS_IMAGE)
	python3 -c "import os; f=open('$(OS_IMAGE)', 'ab'); f.write(b'\x00' * (10485760 - os.path.getsize('$(OS_IMAGE)'))); f.close()"

# RUN
run: $(OS_IMAGE)
	qemu-system-i386 -drive format=raw,file=$(OS_IMAGE)

# CLEAN
clean:
	rm -rf $(BUILD) $(OS_IMAGE)