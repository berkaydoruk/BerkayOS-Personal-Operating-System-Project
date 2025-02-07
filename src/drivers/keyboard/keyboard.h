#ifndef KEYBOARD_H  // Corrected from KERNEL_H
#define KEYBOARD_H

#include <stdint.h>
#include "io.h"
#include "kernel.h"

extern unsigned int cursor_row;
extern unsigned int cursor_col;

void update_cursor();

// Function to initialize the IDT
void init_idt();

// Declare the keyboard interrupt handler (defined in keyboard.c or assembly)
extern void keyboard_interrupt_handler();

extern const char keymap[128];

#endif
