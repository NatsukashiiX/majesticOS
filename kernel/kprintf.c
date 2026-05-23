/* =============================================================================
 * MajesticOS - Kernel Printf
 * Lightweight printf for kernel use. Supports: %s %d %u %x %c %%
 * ============================================================================= */

#include "../include/kprintf.h"
#include "../include/vga.h"
#include "../include/string.h"

/* Minimal variadic args without stdarg.h */
typedef __builtin_va_list va_list;
#define va_start(v,l)  __builtin_va_start(v,l)
#define va_end(v)      __builtin_va_end(v)
#define va_arg(v,l)    __builtin_va_arg(v,l)

void kprintf(const char *fmt, ...) {
    va_list args;
    va_start(args, fmt);

    char num_buf[32];

    while (*fmt) {
        if (*fmt != '%') {
            vga_putchar(*fmt++);
            continue;
        }
        fmt++; /* skip '%' */

        switch (*fmt) {
            case 's': {
                const char *s = va_arg(args, const char *);
                if (!s) s = "(null)";
                vga_puts(s);
                break;
            }
            case 'd': {
                int val = va_arg(args, int);
                itoa(val, num_buf, 10);
                vga_puts(num_buf);
                break;
            }
            case 'u': {
                uint32_t val = va_arg(args, uint32_t);
                utoa(val, num_buf, 10);
                vga_puts(num_buf);
                break;
            }
            case 'x': {
                uint32_t val = va_arg(args, uint32_t);
                vga_puts("0x");
                utoa(val, num_buf, 16);
                vga_puts(num_buf);
                break;
            }
            case 'c': {
                char c = (char)va_arg(args, int);
                vga_putchar(c);
                break;
            }
            case '%':
                vga_putchar('%');
                break;
            default:
                vga_putchar('%');
                vga_putchar(*fmt);
                break;
        }
        fmt++;
    }

    va_end(args);
}

void kprintf_color(uint8_t fg, uint8_t bg, const char *fmt, ...) {
    va_list args;
    va_start(args, fmt);
    vga_set_color((vga_color_t)fg, (vga_color_t)bg);
    /* Reuse kprintf logic inline for simplicity */
    char num_buf[32];
    while (*fmt) {
        if (*fmt != '%') { vga_putchar(*fmt++); continue; }
        fmt++;
        switch (*fmt) {
            case 's': { const char *s = va_arg(args, const char *); vga_puts(s ? s : "(null)"); break; }
            case 'd': { int v = va_arg(args, int); itoa(v, num_buf, 10); vga_puts(num_buf); break; }
            case 'u': { uint32_t v = va_arg(args, uint32_t); utoa(v, num_buf, 10); vga_puts(num_buf); break; }
            case 'x': { uint32_t v = va_arg(args, uint32_t); vga_puts("0x"); utoa(v, num_buf, 16); vga_puts(num_buf); break; }
            case 'c': { vga_putchar((char)va_arg(args, int)); break; }
            case '%': vga_putchar('%'); break;
            default:  vga_putchar('%'); vga_putchar(*fmt); break;
        }
        fmt++;
    }
    va_end(args);
    vga_set_color(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK);
}
