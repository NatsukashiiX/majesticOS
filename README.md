# MajesticOS

```
  __  __    _    _   _____ ____ _____ ___ ____    ___  ____
 |  \/  |  / \  | | | ____/ ___|_   _|_ _/ ___|  / _ \/ ___|
 | |\/| | / _ \ | | |  _| \___ \ | |  | | |     | | | \___ \
 | |  | |/ ___ \| |_| |___ ___) || |  | | |___  | |_| |___) |
 |_|  |_/_/   \_\____|_____|____/ |_| |___\____|  \___/|____/
```

A retro-style x86 hobby operating system written from scratch in C and x86 Assembly.

---

## Features

- Custom x86 bootloader (16-bit real mode → 32-bit protected mode)
- 32-bit protected mode kernel written in C
- Full Interrupt Descriptor Table (IDT) with exception handling
- PS/2 keyboard driver with circular input buffer
- VGA text mode driver (80×25, 16 colors) with hardware cursor
- Retro interactive shell with command history
- Zero external dependencies — no libc, no GRUB

## Shell Commands

| Command         | Description                           |
|----------------|---------------------------------------|
| `help`          | Show all available commands           |
| `clear`         | Clear the screen                      |
| `about`         | About MajesticOS                      |
| `version`       | Show OS version info                  |
| `echo [msg]`    | Print a message                       |
| `color`         | Show the 16-color VGA palette         |
| `banner`        | Redisplay the ASCII art banner        |
| `uname`         | System information                    |
| `art`           | ASCII art gallery                     |
| `calc [expr]`   | Simple calculator (e.g. `calc 7 + 3`) |
| `history`       | Show command history                  |
| `reboot`        | Reboot the system                     |
| `halt`          | Halt the system                       |

---

## Project Structure

```
majesticOS/
├── boot/
│   ├── boot.asm           # Stage 1 bootloader (512 bytes, MBR)
│   └── kernel_entry.asm   # Assembly bridge to C kernel
├── kernel/
│   ├── kernel.c           # Kernel main entry point
│   ├── idt.c              # Interrupt Descriptor Table
│   ├── idt_asm.asm        # ISR/IRQ assembly stubs
│   ├── kprintf.c          # Kernel printf implementation
│   └── string.c           # String utilities (no libc)
├── drivers/
│   ├── vga.c              # VGA text mode driver
│   └── keyboard.c         # PS/2 keyboard driver
├── shell/
│   └── shell.c            # Interactive retro shell
├── include/
│   ├── types.h            # Fixed-width integer types
│   ├── vga.h
│   ├── keyboard.h
│   ├── idt.h
│   ├── shell.h
│   ├── kprintf.h
│   ├── ports.h            # x86 I/O port macros
│   └── string.h
├── tools/
│   └── linker.ld          # Kernel linker script
└── Makefile
```

---

## Building & Running

### Prerequisites

```bash
# Ubuntu / Debian
sudo apt-get install build-essential nasm qemu-system-x86 gcc-multilib

# Arch Linux
sudo pacman -S base-devel nasm qemu gcc

# macOS (with Homebrew + cross-compiler)
brew install nasm qemu i386-elf-gcc i386-elf-binutils
# Edit Makefile: CC=i386-elf-gcc  LD=i386-elf-ld
```

### Build

```bash
make
```

### Run in QEMU (terminal/curses mode)

```bash
make run
```

### Run in QEMU (graphical window)

```bash
make run-gui
```

### Debug with GDB

```bash
# Terminal 1
make debug

# Terminal 2
gdb
(gdb) target remote :1234
(gdb) set architecture i386
(gdb) continue
```

### Clean

```bash
make clean
```

---

## Boot Process

```
Power On
  │
  ▼
BIOS loads boot.asm at 0x7C00 (MBR)
  │
  ▼
Bootloader sets up segments, loads kernel from disk
  │
  ▼
Switch to 32-bit Protected Mode via GDT
  │
  ▼
Jump to kernel_entry.asm → kernel_main() in C
  │
  ▼
VGA init → IDT init → Keyboard init → Shell
```

---

## Writing to Real Hardware

You can write MajesticOS to a USB drive (WARNING: this will erase it):

```bash
# Find your USB device
lsblk

# Write (replace /dev/sdX with your USB device!)
sudo dd if=build/majesticOS.img of=/dev/sdX bs=512 && sync
```

Then boot from the USB drive in your BIOS/UEFI settings.

---

## Extending MajesticOS

Want to add features? Here are some ideas:

- **Memory Manager** — implement a simple bitmap allocator in `kernel/mm.c`
- **Filesystem** — add a simple FAT12 reader for the floppy image
- **Timer** — hook IRQ0 (PIT) for a system clock
- **More shell commands** — add to `shell/shell.c`'s dispatcher
- **Serial output** — add a COM1 driver for debugging

---

## License

MIT License — free to use, modify, and distribute.

---

*Built with love for the retro hacker spirit.*
