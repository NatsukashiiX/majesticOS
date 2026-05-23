#ifndef KPRINTF_H
#define KPRINTF_H

#include "types.h"

/* Kernel printf - supports %s %d %x %c %u */
void kprintf(const char *fmt, ...);
void kprintf_color(uint8_t fg, uint8_t bg, const char *fmt, ...);

#endif /* KPRINTF_H */
