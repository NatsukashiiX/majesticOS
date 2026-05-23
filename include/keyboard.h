#ifndef KEYBOARD_H
#define KEYBOARD_H

#include "types.h"

#define KEYBOARD_DATA_PORT    0x60
#define KEYBOARD_STATUS_PORT  0x64
#define KEYBOARD_BUFFER_SIZE  256

/* Special keys */
#define KEY_ENTER     '\n'
#define KEY_BACKSPACE  0x08
#define KEY_TAB        0x09
#define KEY_ESCAPE     0x1B
#define KEY_ARROW_UP   0xE0
#define KEY_ARROW_DOWN 0xE1

void keyboard_init(void);
char keyboard_getchar(void);
void keyboard_handler(void);
int  keyboard_available(void);

#endif /* KEYBOARD_H */
