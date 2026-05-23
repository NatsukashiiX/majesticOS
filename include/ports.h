#ifndef PORTS_H
#define PORTS_H

#include "types.h"

/* Read a byte from the specified I/O port */
static inline uint8_t port_byte_in(uint16_t port) {
    uint8_t result;
    __asm__ volatile("in %%dx, %%al" : "=a"(result) : "d"(port));
    return result;
}

/* Write a byte to the specified I/O port */
static inline void port_byte_out(uint16_t port, uint8_t data) {
    __asm__ volatile("out %%al, %%dx" : : "a"(data), "d"(port));
}

/* Read a word from the specified I/O port */
static inline uint16_t port_word_in(uint16_t port) {
    uint16_t result;
    __asm__ volatile("in %%dx, %%ax" : "=a"(result) : "d"(port));
    return result;
}

/* Write a word to the specified I/O port */
static inline void port_word_out(uint16_t port, uint16_t data) {
    __asm__ volatile("out %%ax, %%dx" : : "a"(data), "d"(port));
}

/* Small I/O delay */
static inline void io_wait(void) {
    port_byte_out(0x80, 0);
}

#endif /* PORTS_H */
