ASM = nasm

SRC = src
BUILD = build

BOOT_ASM = $(SRC)/boot/boot.asm
BOOT_BIN = $(BUILD)/boot.bin

OS_IMAGE = $(BUILD)/cumpile.bin

all: $(OS_IMAGE)

$(BUILD):
	mkdir -p $(BUILD)

$(BOOT_BIN): $(BOOT_ASM) | $(BUILD)
	$(ASM) -f bin $(BOOT_ASM) -o $(BOOT_BIN)

$(OS_IMAGE): $(BOOT_BIN)
	cp $(BOOT_BIN) $(OS_IMAGE)

run: $(OS_IMAGE)
	qemu-system-i386 -drive format=raw,file=$(OS_IMAGE)

clean:
	rm -rf $(BUILD) $(OS_IMAGE)