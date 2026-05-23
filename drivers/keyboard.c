/* =============================================================================
 * MajesticOS - PS/2 Keyboard Driver
 * Handles keyboard input via IRQ1 with a circular buffer
 * ============================================================================= */

#include "../include/keyboard.h"
#include "../include/idt.h"
#include "../include/ports.h"
#include "../include/string.h"

/* Circular keyboard buffer */
static char kb_buffer[KEYBOARD_BUFFER_SIZE];
static int  kb_buf_read  = 0;
static int  kb_buf_write = 0;

/* US QWERTY scancode to ASCII (normal, no shift) */
static const char scancode_map[] = {
    0,    0,   '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=', '\b',
    '\t', 'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\n',
    0,    'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', '\'', '`',
    0,   '\\', 'z', 'x', 'c', 'v', 'b', 'n', 'm', ',', '.', '/', 0,
    '*',  0,   ' '
};

/* US QWERTY scancode to ASCII (with shift) */
static const char scancode_map_shift[] = {
    0,    0,   '!', '@', '#', '$', '%', '^', '&', '*', '(', ')', '_', '+', '\b',
    '\t', 'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P', '{', '}', '\n',
    0,    'A', 'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L', ':', '"', '~',
    0,   '|', 'Z', 'X', 'C', 'V', 'B', 'N', 'M', '<', '>', '?', 0,
    '*',  0,   ' '
};

static int shift_pressed   = 0;
static int caps_lock_on    = 0;
static int extended        = 0;

/* IRQ1 keyboard interrupt handler */
static void keyboard_irq_handler(registers_t *regs) {
    (void)regs;

    uint8_t scancode = port_byte_in(KEYBOARD_DATA_PORT);

    /* Extended key prefix */
    if (scancode == 0xE0) {
        extended = 1;
        return;
    }

    /* Key release (bit 7 set) */
    if (scancode & 0x80) {
        scancode &= 0x7F;
        if (scancode == 0x2A || scancode == 0x36)
            shift_pressed = 0;
        extended = 0;
        return;
    }

    /* Modifier keys */
    if (scancode == 0x2A || scancode == 0x36) { /* Left/Right Shift */
        shift_pressed = 1;
        return;
    }
    if (scancode == 0x3A) { /* Caps Lock */
        caps_lock_on = !caps_lock_on;
        return;
    }

    /* Extended keys (arrows, etc.) - skip for now */
    if (extended) {
        extended = 0;
        return;
    }

    /* Map to ASCII */
    char c = 0;
    if (scancode < sizeof(scancode_map)) {
        if (shift_pressed)
            c = scancode_map_shift[scancode];
        else
            c = scancode_map[scancode];

        /* Apply caps lock to letters */
        if (caps_lock_on && c >= 'a' && c <= 'z') c -= 32;
        else if (caps_lock_on && c >= 'A' && c <= 'Z') c += 32;
    }

    if (!c) return;

    /* Add to circular buffer */
    int next = (kb_buf_write + 1) % KEYBOARD_BUFFER_SIZE;
    if (next != kb_buf_read) {
        kb_buffer[kb_buf_write] = c;
        kb_buf_write = next;
    }
}

void keyboard_init(void) {
    irq_install_handler(1, keyboard_irq_handler);
    /* Enable IRQ1 */
    __asm__ volatile("sti");
}

/* Returns 1 if a key is available in the buffer */
int keyboard_available(void) {
    return kb_buf_read != kb_buf_write;
}

/* Blocking read: waits for a key and returns it */
char keyboard_getchar(void) {
    while (!keyboard_available());
    char c = kb_buffer[kb_buf_read];
    kb_buf_read = (kb_buf_read + 1) % KEYBOARD_BUFFER_SIZE;
    return c;
}
