/* =============================================================================
 * MajesticOS - Kernel Main
 * ============================================================================= */

#include "../include/types.h"
#include "../include/vga.h"
#include "../include/idt.h"
#include "../include/keyboard.h"
#include "../include/shell.h"
#include "../include/kprintf.h"

/* Small busy-wait delay */
static void delay(uint32_t count) {
    volatile uint32_t i;
    for (i = 0; i < count; i++)
        __asm__ volatile("nop");
}

void kernel_main(void) {
    /* 1. Initialize VGA */
    vga_init();

    /* 2. Set up IDT and remap PIC */
    idt_init();

    /* 3. Enable interrupts */
    __asm__ volatile("sti");

    /* 4. Small delay to let hardware settle */
    delay(1000000);

    /* 5. Initialize keyboard */
    keyboard_init();

    /* 6. Launch shell */
    shell_init();
    shell_run();

    for (;;) __asm__ volatile("hlt");
}