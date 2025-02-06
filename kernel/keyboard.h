#include <stdint.h>
#ifndef KERNEL_H
#define KERNEL_H

extern unsigned int cursor_row;
extern unsigned int cursor_col;

// Function to initialize the IDT
void init_idt();

// Declare the keyboard interrupt handler (defined in keyboard.c or assembly)
extern void keyboard_interrupt_handler();

// keyboard.h'e ekleyin
static inline uint8_t inb(uint16_t port) {
    uint8_t ret;
    asm volatile ("inb %1, %0" : "=a"(ret) : "Nd"(port));
    return ret;
}

static inline void outb(uint16_t port, uint8_t val) {
    asm volatile ("outb %0, %1" : : "a"(val), "Nd"(port));
}

extern const char keymap[128];

void update_cursor();

#endif // KEYBOARD_H
