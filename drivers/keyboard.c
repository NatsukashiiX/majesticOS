/* =============================================================================
 * MajesticOS - PS/2 Keyboard Driver
 * Uses polling mode for maximum QEMU compatibility
 * ============================================================================= */

#include "../include/keyboard.h"
#include "../include/idt.h"
#include "../include/ports.h"
#include "../include/string.h"

/* US QWERTY scancode to ASCII (normal) */
static const char scancode_map[] = {
    0,    0,   '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=', '\b',
    '\t', 'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\n',
    0,    'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', '\'', '`',
    0,   '\\', 'z', 'x', 'c', 'v', 'b', 'n', 'm', ',', '.', '/', 0,
    '*',  0,   ' '
};

/* US QWERTY scancode to ASCII (shifted) */
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

void keyboard_init(void) {
    /* Flush any pending keystrokes */
    while (port_byte_in(KEYBOARD_STATUS_PORT) & 0x01)
        port_byte_in(KEYBOARD_DATA_PORT);
}

/* Poll keyboard status port - returns 1 if a key is ready */
int keyboard_available(void) {
    return port_byte_in(KEYBOARD_STATUS_PORT) & 0x01;
}

/* Blocking poll: waits for a key press and returns ASCII character */
char keyboard_getchar(void) {
    uint8_t scancode;

    while (1) {
        /* Wait for data to be available */
        while (!keyboard_available());

        scancode = port_byte_in(KEYBOARD_DATA_PORT);

        /* Extended key prefix - skip next scancode */
        if (scancode == 0xE0) {
            extended = 1;
            continue;
        }

        /* Key release (bit 7 set) */
        if (scancode & 0x80) {
            scancode &= 0x7F;
            if (scancode == 0x2A || scancode == 0x36)
                shift_pressed = 0;
            extended = 0;
            continue;
        }

        /* Extended key - ignore arrows etc for now */
        if (extended) {
            extended = 0;
            continue;
        }

        /* Modifier keys */
        if (scancode == 0x2A || scancode == 0x36) {
            shift_pressed = 1;
            continue;
        }
        if (scancode == 0x3A) {
            caps_lock_on = !caps_lock_on;
            continue;
        }

        /* Map scancode to ASCII */
        if (scancode >= sizeof(scancode_map))
            continue;

        char c = shift_pressed ? scancode_map_shift[scancode]
                               : scancode_map[scancode];

        if (!c) continue;

        /* Apply caps lock to letters only */
        if (caps_lock_on) {
            if (c >= 'a' && c <= 'z') c -= 32;
            else if (c >= 'A' && c <= 'Z') c += 32;
        }

        return c;
    }
}