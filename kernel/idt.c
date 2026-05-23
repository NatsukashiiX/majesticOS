/* =============================================================================
 * MajesticOS - Interrupt Descriptor Table (IDT) & IRQ Handler
 * Sets up interrupt handling for hardware and software interrupts
 * ============================================================================= */

#include "../include/idt.h"
#include "../include/ports.h"
#include "../include/vga.h"
#include "../include/kprintf.h"
#include "../include/string.h"

#define IDT_ENTRIES 256

static idt_entry_t idt[IDT_ENTRIES];
static idt_ptr_t   idt_ptr_struct;
static isr_handler_t irq_handlers[16];

/* Exception messages */
static const char *exception_msgs[] = {
    "Division By Zero",
    "Debug",
    "Non Maskable Interrupt",
    "Breakpoint",
    "Into Detected Overflow",
    "Out of Bounds",
    "Invalid Opcode",
    "No Coprocessor",
    "Double Fault",
    "Coprocessor Segment Overrun",
    "Bad TSS",
    "Segment Not Present",
    "Stack Fault",
    "General Protection Fault",
    "Page Fault",
    "Unknown Interrupt",
    "Coprocessor Fault",
    "Alignment Check",
    "Machine Check",
    "SIMD Floating Point Exception"
};

/* External ISR/IRQ stubs defined in idt_asm.asm */
extern void isr0(void);  extern void isr1(void);  extern void isr2(void);
extern void isr3(void);  extern void isr4(void);  extern void isr5(void);
extern void isr6(void);  extern void isr7(void);  extern void isr8(void);
extern void isr9(void);  extern void isr10(void); extern void isr11(void);
extern void isr12(void); extern void isr13(void); extern void isr14(void);
extern void isr15(void); extern void isr16(void); extern void isr17(void);
extern void isr18(void); extern void isr19(void); extern void isr20(void);
extern void isr21(void); extern void isr22(void); extern void isr23(void);
extern void isr24(void); extern void isr25(void); extern void isr26(void);
extern void isr27(void); extern void isr28(void); extern void isr29(void);
extern void isr30(void); extern void isr31(void);

extern void irq0(void);  extern void irq1(void);  extern void irq2(void);
extern void irq3(void);  extern void irq4(void);  extern void irq5(void);
extern void irq6(void);  extern void irq7(void);  extern void irq8(void);
extern void irq9(void);  extern void irq10(void); extern void irq11(void);
extern void irq12(void); extern void irq13(void); extern void irq14(void);
extern void irq15(void);

extern void idt_flush(uint32_t);

void idt_set_gate(uint8_t num, uint32_t base, uint16_t sel, uint8_t flags) {
    idt[num].base_low  = base & 0xFFFF;
    idt[num].base_high = (base >> 16) & 0xFFFF;
    idt[num].selector  = sel;
    idt[num].zero      = 0;
    idt[num].flags     = flags;
}

/* Remap PIC so IRQs don't conflict with CPU exceptions */
void pic_remap(void) {
    port_byte_out(PIC1_COMMAND, 0x11); io_wait();
    port_byte_out(PIC2_COMMAND, 0x11); io_wait();
    port_byte_out(PIC1_DATA,    0x20); io_wait(); /* IRQ 0-7  -> INT 0x20-0x27 */
    port_byte_out(PIC2_DATA,    0x28); io_wait(); /* IRQ 8-15 -> INT 0x28-0x2F */
    port_byte_out(PIC1_DATA,    0x04); io_wait();
    port_byte_out(PIC2_DATA,    0x02); io_wait();
    port_byte_out(PIC1_DATA,    0x01); io_wait();
    port_byte_out(PIC2_DATA,    0x01); io_wait();
    port_byte_out(PIC1_DATA,    0x00);
    port_byte_out(PIC2_DATA,    0x00);
}

void idt_init(void) {
    idt_ptr_struct.limit = sizeof(idt_entry_t) * IDT_ENTRIES - 1;
    idt_ptr_struct.base  = (uint32_t)&idt;

    memset(&idt, 0, sizeof(idt_entry_t) * IDT_ENTRIES);
    memset(irq_handlers, 0, sizeof(irq_handlers));

    /* Install ISRs (CPU exceptions 0-31) */
    idt_set_gate(0,  (uint32_t)isr0,  0x08, 0x8E);
    idt_set_gate(1,  (uint32_t)isr1,  0x08, 0x8E);
    idt_set_gate(2,  (uint32_t)isr2,  0x08, 0x8E);
    idt_set_gate(3,  (uint32_t)isr3,  0x08, 0x8E);
    idt_set_gate(4,  (uint32_t)isr4,  0x08, 0x8E);
    idt_set_gate(5,  (uint32_t)isr5,  0x08, 0x8E);
    idt_set_gate(6,  (uint32_t)isr6,  0x08, 0x8E);
    idt_set_gate(7,  (uint32_t)isr7,  0x08, 0x8E);
    idt_set_gate(8,  (uint32_t)isr8,  0x08, 0x8E);
    idt_set_gate(9,  (uint32_t)isr9,  0x08, 0x8E);
    idt_set_gate(10, (uint32_t)isr10, 0x08, 0x8E);
    idt_set_gate(11, (uint32_t)isr11, 0x08, 0x8E);
    idt_set_gate(12, (uint32_t)isr12, 0x08, 0x8E);
    idt_set_gate(13, (uint32_t)isr13, 0x08, 0x8E);
    idt_set_gate(14, (uint32_t)isr14, 0x08, 0x8E);
    idt_set_gate(15, (uint32_t)isr15, 0x08, 0x8E);
    idt_set_gate(16, (uint32_t)isr16, 0x08, 0x8E);
    idt_set_gate(17, (uint32_t)isr17, 0x08, 0x8E);
    idt_set_gate(18, (uint32_t)isr18, 0x08, 0x8E);
    idt_set_gate(19, (uint32_t)isr19, 0x08, 0x8E);
    idt_set_gate(20, (uint32_t)isr20, 0x08, 0x8E);
    idt_set_gate(21, (uint32_t)isr21, 0x08, 0x8E);
    idt_set_gate(22, (uint32_t)isr22, 0x08, 0x8E);
    idt_set_gate(23, (uint32_t)isr23, 0x08, 0x8E);
    idt_set_gate(24, (uint32_t)isr24, 0x08, 0x8E);
    idt_set_gate(25, (uint32_t)isr25, 0x08, 0x8E);
    idt_set_gate(26, (uint32_t)isr26, 0x08, 0x8E);
    idt_set_gate(27, (uint32_t)isr27, 0x08, 0x8E);
    idt_set_gate(28, (uint32_t)isr28, 0x08, 0x8E);
    idt_set_gate(29, (uint32_t)isr29, 0x08, 0x8E);
    idt_set_gate(30, (uint32_t)isr30, 0x08, 0x8E);
    idt_set_gate(31, (uint32_t)isr31, 0x08, 0x8E);

    pic_remap();

    /* Install IRQs (hardware interrupts 32-47) */
    idt_set_gate(32, (uint32_t)irq0,  0x08, 0x8E);
    idt_set_gate(33, (uint32_t)irq1,  0x08, 0x8E);
    idt_set_gate(34, (uint32_t)irq2,  0x08, 0x8E);
    idt_set_gate(35, (uint32_t)irq3,  0x08, 0x8E);
    idt_set_gate(36, (uint32_t)irq4,  0x08, 0x8E);
    idt_set_gate(37, (uint32_t)irq5,  0x08, 0x8E);
    idt_set_gate(38, (uint32_t)irq6,  0x08, 0x8E);
    idt_set_gate(39, (uint32_t)irq7,  0x08, 0x8E);
    idt_set_gate(40, (uint32_t)irq8,  0x08, 0x8E);
    idt_set_gate(41, (uint32_t)irq9,  0x08, 0x8E);
    idt_set_gate(42, (uint32_t)irq10, 0x08, 0x8E);
    idt_set_gate(43, (uint32_t)irq11, 0x08, 0x8E);
    idt_set_gate(44, (uint32_t)irq12, 0x08, 0x8E);
    idt_set_gate(45, (uint32_t)irq13, 0x08, 0x8E);
    idt_set_gate(46, (uint32_t)irq14, 0x08, 0x8E);
    idt_set_gate(47, (uint32_t)irq15, 0x08, 0x8E);

    idt_flush((uint32_t)&idt_ptr_struct);
}

/* CPU Exception handler */
void isr_handler(registers_t *regs) {
    vga_puts_color("\n[MAJESTIC KERNEL PANIC] ", VGA_COLOR_LIGHT_RED, VGA_COLOR_BLACK);
    if (regs->int_no < 20) {
        vga_puts_color(exception_msgs[regs->int_no], VGA_COLOR_YELLOW, VGA_COLOR_BLACK);
    } else {
        vga_puts_color("Unknown Exception", VGA_COLOR_YELLOW, VGA_COLOR_BLACK);
    }
    kprintf("\nInterrupt: %d  Error Code: %d\n", regs->int_no, regs->err_code);
    kprintf("EIP: 0x%x  CS: 0x%x  EFLAGS: 0x%x\n", regs->eip, regs->cs, regs->eflags);
    kprintf("\nSystem halted.\n");
    for (;;) { __asm__ volatile("hlt"); }
}

/* Hardware IRQ handler */
void irq_handler(registers_t *regs) {
    /* Send End-Of-Interrupt to PIC */
    if (regs->int_no >= 40)
        port_byte_out(PIC2_COMMAND, 0x20);
    port_byte_out(PIC1_COMMAND, 0x20);

    /* Call registered handler if any */
    int irq = regs->int_no - 32;
    if (irq >= 0 && irq < 16 && irq_handlers[irq])
        irq_handlers[irq](regs);
}

void irq_install_handler(int irq, isr_handler_t handler) {
    if (irq >= 0 && irq < 16)
        irq_handlers[irq] = handler;
}

void irq_uninstall_handler(int irq) {
    if (irq >= 0 && irq < 16)
        irq_handlers[irq] = NULL;
}
