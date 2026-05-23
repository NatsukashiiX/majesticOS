#ifndef IDT_H
#define IDT_H

#include "types.h"

/* IDT entry */
typedef struct {
    uint16_t base_low;      /* Lower 16 bits of handler address */
    uint16_t selector;      /* Kernel segment selector */
    uint8_t  zero;          /* Always zero */
    uint8_t  flags;         /* Type and attribute flags */
    uint16_t base_high;     /* Upper 16 bits of handler address */
} __attribute__((packed)) idt_entry_t;

/* IDT pointer */
typedef struct {
    uint16_t limit;
    uint32_t base;
} __attribute__((packed)) idt_ptr_t;

/* Registers structure passed to interrupt handlers */
typedef struct {
    uint32_t ds;
    uint32_t edi, esi, ebp, esp, ebx, edx, ecx, eax;
    uint32_t int_no, err_code;
    uint32_t eip, cs, eflags, useresp, ss;
} registers_t;

typedef void (*isr_handler_t)(registers_t *);

void idt_init(void);
void idt_set_gate(uint8_t num, uint32_t base, uint16_t sel, uint8_t flags);
void irq_install_handler(int irq, isr_handler_t handler);
void irq_uninstall_handler(int irq);

/* PIC ports */
#define PIC1_COMMAND 0x20
#define PIC1_DATA    0x21
#define PIC2_COMMAND 0xA0
#define PIC2_DATA    0xA1

void pic_remap(void);
void irq_handler(registers_t *regs);
void isr_handler(registers_t *regs);

#endif /* IDT_H */
