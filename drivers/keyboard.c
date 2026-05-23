/* =============================================================================
 * MajesticOS - PS/2 Keyboard Driver (IRQ + Polling hybrid)
 * ============================================================================= */

#include "../include/keyboard.h"
#include "../include/idt.h"
#include "../include/ports.h"
#include "../include/string.h"

static char kb_buffer[KEYBOARD_BUFFER_SIZE];
static volatile int kb_buf_read  = 0;
static volatile int kb_buf_write = 0;

static const char scancode_map[] = {
    0,    0,   '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=', '\b',
    '\t', 'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\n',
    0,    'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', '\'', '`',
    0,   '\\', 'z', 'x', 'c', 'v', 'b', 'n', 'm', ',', '.', '/', 0,
    '*',  0,   ' '
};

static const char scancode_map_shift[] = {
    0,    0,   '!', '@', '#', '$', '%', '^', '&', '*', '(', ')', '_', '+', '\b',
    '\t', 'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P', '{', '}', '\n',
    0,    'A', 'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L', ':', '"', '~',
    0,   '|', 'Z', 'X', 'C', 'V', 'B', 'N', 'M', '<', '>', '?', 0,
    '*',  0,   ' '
};

static int shift_pressed = 0;
static int caps_lock_on  = 0;
static int extended      = 0;

/* Process a raw scancode into the buffer */
static void process_scancode(uint8_t scancode) {
    if (scancode == 0xE0) { extended = 1; return; }

    if (scancode & 0x80) {
        scancode &= 0x7F;
        if (scancode == 0x2A || scancode == 0x36) shift_pressed = 0;
        extended = 0;
        return;
    }

    if (extended) { extended = 0; return; }

    if (scancode == 0x2A || scancode == 0x36) { shift_pressed = 1; return; }
    if (scancode == 0x3A) { caps_lock_on = !caps_lock_on; return; }

    if (scancode >= sizeof(scancode_map)) return;

    char c = shift_pressed ? scancode_map_shift[scancode] : scancode_map[scancode];
    if (!c) return;

    if (caps_lock_on) {
        if (c >= 'a' && c <= 'z') c -= 32;
        else if (c >= 'A' && c <= 'Z') c += 32;
    }

    int next = (kb_buf_write + 1) % KEYBOARD_BUFFER_SIZE;
    if (next != kb_buf_read) {
        kb_buffer[kb_buf_write] = c;
        kb_buf_write = next;
    }
}

/* IRQ1 handler */
static void keyboard_irq_handler(registers_t *regs) {
    (void)regs;
    uint8_t scancode = port_byte_in(KEYBOARD_DATA_PORT);
    process_scancode(scancode);
}

void keyboard_init(void) {
    /* Flush output buffer */
    while (port_byte_in(KEYBOARD_STATUS_PORT) & 0x01)
        port_byte_in(KEYBOARD_DATA_PORT);

    /* Enable keyboard via controller command */
    /* Wait for input buffer to be clear */
    while (port_byte_in(KEYBOARD_STATUS_PORT) & 0x02);
    port_byte_out(KEYBOARD_STATUS_PORT, 0xAE); /* Enable first PS/2 port */

    /* Send enable scanning command to keyboard */
    while (port_byte_in(KEYBOARD_STATUS_PORT) & 0x02);
    port_byte_out(KEYBOARD_DATA_PORT, 0xF4);

    /* Wait for ACK */
    while (!(port_byte_in(KEYBOARD_STATUS_PORT) & 0x01));
    port_byte_in(KEYBOARD_DATA_PORT);

    /* Install IRQ handler and enable interrupts */
    irq_install_handler(1, keyboard_irq_handler);
    __asm__ volatile("sti");
}

int keyboard_available(void) {
    /* Also poll directly in case IRQ missed */
    if (port_byte_in(KEYBOARD_STATUS_PORT) & 0x01) {
        uint8_t scancode = port_byte_in(KEYBOARD_DATA_PORT);
        process_scancode(scancode);
    }
    return kb_buf_read != kb_buf_write;
}

char keyboard_getchar(void) {
    while (!keyboard_available());
    char c = kb_buffer[kb_buf_read];
    kb_buf_read = (kb_buf_read + 1) % KEYBOARD_BUFFER_SIZE;
    return c;
}