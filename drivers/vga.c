/* =============================================================================
 * MajesticOS - VGA Text Mode Driver
 * Handles all screen output in 80x25 text mode
 * ============================================================================= */

#include "../include/vga.h"
#include "../include/types.h"
#include "../include/ports.h"

#define KEY_BACKSPACE_CHAR 0x08

/* VGA state */
static uint16_t *vga_buffer = (uint16_t *)VGA_ADDRESS;
static int cursor_row = 0;
static int cursor_col = 0;
static uint8_t current_color = 0;

/* Build a VGA entry (character + color byte) */
static inline uint16_t vga_entry(char c, uint8_t color) {
    return (uint16_t)c | ((uint16_t)color << 8);
}

/* Build a color byte from fg/bg */
static inline uint8_t vga_color_byte(vga_color_t fg, vga_color_t bg) {
    return (uint8_t)fg | ((uint8_t)bg << 4);
}

/* Update hardware cursor position */
static void update_cursor(void) {
    uint16_t pos = cursor_row * VGA_WIDTH + cursor_col;
    port_byte_out(0x3D4, 0x0F);
    port_byte_out(0x3D5, (uint8_t)(pos & 0xFF));
    port_byte_out(0x3D4, 0x0E);
    port_byte_out(0x3D5, (uint8_t)((pos >> 8) & 0xFF));
}

/* Initialize VGA driver */
void vga_init(void) {
    current_color = vga_color_byte(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK);
    cursor_row = 0;
    cursor_col = 0;
    vga_clear();
}

/* Clear the screen */
void vga_clear(void) {
    for (int i = 0; i < VGA_SIZE; i++) {
        vga_buffer[i] = vga_entry(' ', current_color);
    }
    cursor_row = 0;
    cursor_col = 0;
    update_cursor();
}

/* Set current text color */
void vga_set_color(vga_color_t fg, vga_color_t bg) {
    current_color = vga_color_byte(fg, bg);
}

/* Scroll screen up one line */
void vga_scroll(void) {
    /* Move all lines up by one */
    for (int row = 1; row < VGA_HEIGHT; row++) {
        for (int col = 0; col < VGA_WIDTH; col++) {
            vga_buffer[(row - 1) * VGA_WIDTH + col] =
                vga_buffer[row * VGA_WIDTH + col];
        }
    }
    /* Clear the last line */
    for (int col = 0; col < VGA_WIDTH; col++) {
        vga_buffer[(VGA_HEIGHT - 1) * VGA_WIDTH + col] =
            vga_entry(' ', current_color);
    }
    cursor_row = VGA_HEIGHT - 1;
}

/* Put a single character at the current cursor position */
void vga_putchar(char c) {
    if (c == '\n') {
        cursor_col = 0;
        cursor_row++;
    } else if (c == '\r') {
        cursor_col = 0;
    } else if (c == '\t') {
        cursor_col = (cursor_col + 8) & ~7;
    } else if (c == KEY_BACKSPACE_CHAR) {
        if (cursor_col > 0) {
            cursor_col--;
            vga_buffer[cursor_row * VGA_WIDTH + cursor_col] =
                vga_entry(' ', current_color);
        }
    } else {
        vga_buffer[cursor_row * VGA_WIDTH + cursor_col] =
            vga_entry(c, current_color);
        cursor_col++;
    }

    /* Handle line wrap */
    if (cursor_col >= VGA_WIDTH) {
        cursor_col = 0;
        cursor_row++;
    }

    /* Handle scroll */
    if (cursor_row >= VGA_HEIGHT) {
        vga_scroll();
    }

    update_cursor();
}

/* Write a null-terminated string */
void vga_puts(const char *str) {
    while (*str) {
        vga_putchar(*str++);
    }
}

/* Write a string with specific colors */
void vga_puts_color(const char *str, vga_color_t fg, vga_color_t bg) {
    uint8_t saved = current_color;
    vga_set_color(fg, bg);
    vga_puts(str);
    current_color = saved;
}

/* Put a character at a specific position */
void vga_putchar_at(char c, vga_color_t fg, vga_color_t bg, int x, int y) {
    vga_buffer[y * VGA_WIDTH + x] = vga_entry(c, vga_color_byte(fg, bg));
}

/* Move cursor to specific position */
void vga_set_cursor(int x, int y) {
    cursor_col = x;
    cursor_row = y;
    update_cursor();
}

int vga_get_row(void) { return cursor_row; }
int vga_get_col(void) { return cursor_col; }


