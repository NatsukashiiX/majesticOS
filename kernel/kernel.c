/* =============================================================================
 * MajesticOS - Kernel Main
 * Entry point called from kernel_entry.asm after bootloader hands control
 * =============================================================================
 *
 *   __  __    _    _   _____ ____ _____ ___ ____    ___  ____
 *  |  \/  |  / \  | | | ____/ ___|_   _|_ _/ ___|  / _ \/ ___|
 *  | |\/| | / _ \ | | |  _| \___ \ | |  | | |     | | | \___ \
 *  | |  | |/ ___ \| |_| |___ ___) || |  | | |___  | |_| |___) |
 *  |_|  |_/_/   \_\____|_____|____/ |_| |___\____|  \___/|____/
 *
 * ============================================================================= */

#include "../include/types.h"
#include "../include/vga.h"
#include "../include/idt.h"
#include "../include/keyboard.h"
#include "../include/shell.h"
#include "../include/kprintf.h"

/* Kernel entry point */
void kernel_main(void) {
    /* Step 1: Initialize VGA text mode driver */
    vga_init();

    /* Step 2: Set up the Interrupt Descriptor Table */
    idt_init();

    /* Step 3: Initialize the keyboard driver */
    keyboard_init();

    /* Step 4: Launch the shell */
    shell_init();
    shell_run();

    /* Should never reach here */
    for (;;) {
        __asm__ volatile("hlt");
    }
}
