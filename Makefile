ASM = nasm
CC = i686-elf-gcc
LD = i686-elf-ld

SRC = src
BUILD = build

BOOT = $(SRC)/boot
KERNEL = $(SRC)/kernel
USER = $(SRC)/user

CFLAGS = -ffreestanding -O2 -Wall -Wextra -m32 -Isrc/include
LDFLAGS = -T linker.ld -nostdlib -m elf_i386

KERNEL_OBJS = \
$(BUILD)/kernel.o \
$(BUILD)/vga.o \
$(BUILD)/syscall.o \
$(BUILD)/idt.o \
$(BUILD)/isr.o \
$(BUILD)/gdt.o \
$(BUILD)/gdtc.o \
$(BUILD)/font.o \
$(BUILD)/libuser.o \
$(BUILD)/main.o \
$(BUILD)/usermode.o \
$(BUILD)/tss.o

all: $(BUILD)/cumpile.img

$(BUILD):
	mkdir -p $(BUILD)

# Build bootloader
$(BUILD)/boot.bin: $(BOOT)/boot.asm | $(BUILD)
	$(ASM) -f bin $(BOOT)/boot.asm -o $(BUILD)/boot.bin

# Build disk.o
$(BUILD)/disk.o: $(BOOT)/disk.asm | $(BUILD)
	$(ASM) -f elf32 $(BOOT)/disk.asm -o $(BUILD)/disk.o

# Build elf.o
$(BUILD)/elf.o: $(BOOT)/elf.c | $(BUILD)
	$(CC) $(CFLAGS) -c $(BOOT)/elf.c -o $(BUILD)/elf.o

# Build loader.o
$(BUILD)/loader.o: $(BOOT)/loader.asm | $(BUILD)
	$(ASM) -f elf32 $(BOOT)/loader.asm -o $@

# Link loader
$(BUILD)/loader.bin: $(BUILD)/loader.o $(BUILD)/elf.o $(BUILD)/disk.o | $(BUILD)
	$(LD) -Ttext 0x8000 -e start -nostdlib -m elf_i386 --oformat binary \
	$(BUILD)/loader.o $(BUILD)/elf.o $(BUILD)/disk.o -o $(BUILD)/loader.bin
	python3 -c "import os; f=open('$(BUILD)/loader.bin', 'ab');f.write(b'\x00' * (10240 - os.path.getsize('$(BUILD)/loader.bin')));f.close()"

# KERNEL OBJECTS
$(BUILD)/%.o: $(KERNEL)/%.c | $(BUILD)
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD)/%.o: $(KERNEL)/%.asm | $(BUILD)
	$(ASM) -f elf32 $< -o $@

# USER OBJECTS
$(BUILD)/%.o: $(USER)/%.c | $(BUILD)
	$(CC) $(CFLAGS) -c $< -o $@

# # LINK
$(BUILD)/kernel.bin: $(KERNEL_OBJS) | $(BUILD)
	$(LD) $(LDFLAGS) $^ -o $@

# IMAGE
$(BUILD)/cumpile.img: $(BUILD)/boot.bin $(BUILD)/loader.bin $(BUILD)/kernel.bin | $(BUILD)
	cat $^ > $@
	python3 -c "import os; f=open('$@','ab'); f.write(b'\x00'*(10*1024*1024-os.path.getsize('$@')))"

# RUN
run: $(BUILD)/cumpile.img
	qemu-system-i386 -drive format=raw,file=$<

clean:
	rm -rf $(BUILD)