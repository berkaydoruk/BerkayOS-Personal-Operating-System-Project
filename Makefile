# Toolchain
CC = i686-elf-gcc
AS = i686-elf-as
LD = i686-elf-ld
GRUBMKRESCUE = grub-mkrescue
QEMU = qemu-system-i386

# Directories
SRC_DIR = src
BUILD_DIR = build
ISO_DIR = isodir

# Auto-detect sources
C_SOURCES := $(shell find $(SRC_DIR) -name '*.c')
ASM_SOURCES := $(shell find $(SRC_DIR) -name '*.s')
OBJECTS := $(patsubst $(SRC_DIR)/%, $(BUILD_DIR)/%.o, $(C_SOURCES) $(ASM_SOURCES))

# Flags
CFLAGS = -ffreestanding -Wall -Wextra -Iinclude -Isrc/include -Isrc/drivers/keyboard -Isrc/kernel
ASFLAGS =
LDFLAGS = -T $(SRC_DIR)/kernel/linker.ld -nostdlib

# Targets
KERNEL_BIN = $(ISO_DIR)/boot/kernel.bin

# Build rules
all: $(KERNEL_BIN)

$(BUILD_DIR)/%.c.o: $(SRC_DIR)/%.c
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/%.s.o: $(SRC_DIR)/%.s
	@mkdir -p $(@D)
	$(AS) $(ASFLAGS) $< -o $@

$(KERNEL_BIN): $(OBJECTS)
	@mkdir -p $(@D)
	$(LD) $(LDFLAGS) -o $@ $^

run: $(KERNEL_BIN)
	cd $(ISO_DIR)/boot && $(QEMU) -kernel kernel.bin

clean:
	rm -rf $(BUILD_DIR) $(KERNEL_BIN)

.PHONY: all clean run
