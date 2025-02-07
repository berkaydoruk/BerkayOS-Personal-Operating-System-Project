// idt.h
#ifndef IDT_H
#define IDT_H

#include <stdint.h>

struct IDTEntry {
    uint16_t offset_low;
    uint16_t selector;
    uint8_t zero;
    uint8_t type_attr;
    uint16_t offset_high;
} __attribute__((packed));

struct IDTPointer {
    uint16_t limit;
    uint32_t base;
} __attribute__((packed));

// **Extern Deklarasyonlar Eklendi!**
extern struct IDTEntry idt[256];
extern struct IDTPointer idt_ptr;

#endif // IDT_H