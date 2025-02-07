#ifndef KERNEL_H
#define KERNEL_H

#include <stdint.h>
#include "idt.h"

// **IDT Pointer Yapısı Tanımı Burada!**
//struct IDTPointer {
//    uint16_t limit;   // IDT boyutu (entry * 8 - 1)
//    uint32_t base;    // IDT'nin hafızadaki adresi
//} __attribute__((packed));

// **IDT Pointer'ın Bildirimi**
extern struct IDTPointer idt_ptr;

// **Kernel Fonksiyon Bildirimleri**
void print(const char *message, unsigned int row, unsigned int col, unsigned char color);

#endif // KERNEL_H
