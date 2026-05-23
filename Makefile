# =============================================================================
# MajesticOS Makefile
# Builds the bootloader, kernel, and creates a bootable disk image
# =============================================================================

# Toolchain
CC      = gcc
LD      = ld
NASM    = nasm
QEMU    = qemu-system-i386

# Flags
CFLAGS  = -m32 -ffreestanding -fno-stack-protector -fno-builtin \
          -nostdlib -nostdinc -Wall -Wextra -O2 \
          -fno-pie -fno-pic \
          -Iinclude

LDFLAGS = -m elf_i386 -T tools/linker.ld --oformat binary

NASMFLAGS = -f elf32

# Output
BUILD_DIR  = build
OS_IMAGE   = $(BUILD_DIR)/majesticOS.img

# Source files
BOOT_ASM   = boot/boot.asm
ENTRY_ASM  = boot/kernel_entry.asm
IDT_ASM    = kernel/idt_asm.asm

C_SOURCES  = kernel/kernel.c    \
             kernel/string.c    \
             kernel/idt.c       \
             kernel/kprintf.c   \
             drivers/vga.c      \
             drivers/keyboard.c \
             shell/shell.c

# Object files
ENTRY_OBJ  = $(BUILD_DIR)/kernel_entry.o
IDT_OBJ    = $(BUILD_DIR)/idt_asm.o
C_OBJECTS  = $(patsubst %.c,$(BUILD_DIR)/%.o,$(C_SOURCES))

# =============================================================================
# Default target
# =============================================================================
.PHONY: all clean run debug dirs

all: dirs $(OS_IMAGE)

# Create build directories
dirs:
	@mkdir -p $(BUILD_DIR)/kernel
	@mkdir -p $(BUILD_DIR)/drivers
	@mkdir -p $(BUILD_DIR)/shell
	@mkdir -p $(BUILD_DIR)/boot

# =============================================================================
# Bootloader (raw binary, sector 1)
# =============================================================================
$(BUILD_DIR)/boot.bin: $(BOOT_ASM)
	$(NASM) -f bin $< -o $@

# =============================================================================
# Kernel entry stub (assembly -> ELF object)
# =============================================================================
$(ENTRY_OBJ): $(ENTRY_ASM)
	$(NASM) $(NASMFLAGS) $< -o $@

# =============================================================================
# IDT assembly stubs
# =============================================================================
$(IDT_OBJ): $(IDT_ASM)
	$(NASM) $(NASMFLAGS) $< -o $@

# =============================================================================
# C source files -> object files
# =============================================================================
$(BUILD_DIR)/%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# =============================================================================
# Link kernel into raw binary
# =============================================================================
$(BUILD_DIR)/kernel.bin: $(ENTRY_OBJ) $(IDT_OBJ) $(C_OBJECTS)
	$(LD) $(LDFLAGS) -o $@ $^

# =============================================================================
# Combine bootloader + kernel into disk image
# =============================================================================
$(OS_IMAGE): $(BUILD_DIR)/boot.bin $(BUILD_DIR)/kernel.bin
	@echo "  Creating MajesticOS disk image..."
	cat $^ > $@
	@# Pad to 1.44MB floppy image size
	@dd if=/dev/zero bs=1 count=1 seek=1474559 of=$@ 2>/dev/null || true
	@echo ""
	@echo "  ====================================="
	@echo "   MajesticOS image built: $(OS_IMAGE)"
	@echo "  ====================================="

# =============================================================================
# Run in QEMU
# =============================================================================
run: all
	$(QEMU) -drive file=$(OS_IMAGE),format=raw,index=0,if=floppy \
	        -m 32M \
	        -display curses \
	        -name "MajesticOS"

# Run with graphical window
run-gui: all
	$(QEMU) -drive file=$(OS_IMAGE),format=raw,index=0,if=floppy \
	        -m 32M \
	        -name "MajesticOS"

# Debug with GDB
debug: all
	$(QEMU) -drive file=$(OS_IMAGE),format=raw,index=0,if=floppy \
	        -m 32M \
	        -s -S \
	        -name "MajesticOS"

# =============================================================================
# Clean
# =============================================================================
clean:
	rm -rf $(BUILD_DIR)
	@echo "  Build artifacts cleaned."
