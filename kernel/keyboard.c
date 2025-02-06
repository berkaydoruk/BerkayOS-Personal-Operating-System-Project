#include "keyboard.h"
#include "kernel.h"
#include <stdint.h>

// PIC portları
#define PIC1_COMMAND 0x20
#define PIC1_DATA 0x21
#define PIC2_COMMAND 0xA0
#define PIC2_DATA 0xA1

// PIC'i yeniden programlama
void pic_remap() {
    outb(PIC1_COMMAND, 0x11);
    outb(PIC2_COMMAND, 0x11);

    outb(PIC1_DATA, 0x20);  // Master PIC için offset (IRQ 0-7 → 0x20-0x27)
    outb(PIC2_DATA, 0x28);  // Slave PIC için offset (IRQ 8-15 → 0x28-0x2F)

    outb(PIC1_DATA, 0x04); // Master PIC, Slave PIC'in IRQ2'de olduğunu bilmeli
    outb(PIC2_DATA, 0x02); // Slave PIC, Master PIC'in IRQ2'sine bağlı

    outb(PIC1_DATA, 0x01);
    outb(PIC2_DATA, 0x01);

    outb(PIC1_DATA, 0x0);  // IRQ'ları aç
    outb(PIC2_DATA, 0x0);

    outb(PIC1_DATA, 0xFD);  // Sadece IRQ1 (klavye) açık
    outb(PIC2_DATA, 0xFF);  // Slave PIC'de tüm IRQ'lar kapalı
}


/* ===================== IDT SETUP ===================== */

/**
 * IDT Entry Structure (8 bytes)
 *
 * Each entry in the Interrupt Descriptor Table (IDT) contains the address
 * of the Interrupt Service Routine (ISR) and its access attributes.
 */
struct IDTEntry {
    unsigned short offset_low;   // Lower 16 bits of ISR address
    unsigned short selector;     // Segment selector (Usually 0x08 - Kernel Code Segment)
    unsigned char zero;          // Must always be zero
    unsigned char type_attr;     // Type and attributes (0x8E = Interrupt Gate)
    unsigned short offset_high;  // Upper 16 bits of ISR address
} __attribute__((packed));

/**
 * IDT Pointer Structure
 *
 * The IDT Pointer is used by the `lidt` instruction to load the IDT.
 */
struct IDTPointer {
    unsigned short limit;   // Size of the IDT table (number of entries * 8 - 1)
    unsigned int base;      // Address of the first IDT entry
} __attribute__((packed));

/* ===================== IDT TABLE ===================== */

// Define the IDT and IDT Pointer
struct IDTEntry idt[256];
struct IDTPointer idt_ptr;

/**
 * Set an entry in the IDT.
 *
 * @param num   Interrupt number (IRQ or exception number)
 * @param base  Address of the ISR function
 * @param sel   Segment selector (should be 0x08 for kernel code segment)
 * @param flags Access control flags (0x8E for interrupt gates)
 */
void set_idt_gate(int num, unsigned int base, unsigned short sel, unsigned char flags) {
    idt[num].offset_low = base & 0xFFFF;
    idt[num].selector = sel;
    idt[num].zero = 0;
    idt[num].type_attr = flags;
    idt[num].offset_high = (base >> 16) & 0xFFFF;
}

/**
 * External Assembly function to load the IDT.
 */
extern void idt_load();

/**
 * Keyboard Interrupt Handler
 *
 * This function will be executed when a keyboard interrupt occurs.
 * Currently, it is empty, but it will be implemented later.
 */
#define KEYBOARD_DATA_PORT 0x60

unsigned int cursor_row = 0;
unsigned int cursor_col = 0;

void update_cursor() {
  	print(" ", cursor_row, cursor_col, 0x0F);

    print("|", cursor_row, cursor_col, 0x0F);
}

extern void keyboard_entry();

void keyboard_interrupt_handler() {
    uint8_t scancode = inb(KEYBOARD_DATA_PORT);

    // EOI gönder (Master PIC'e)
    outb(PIC1_COMMAND, 0x20);

    if (scancode != 0xE0 && !(scancode & 0x80)) {
        if (scancode == 0x1C) {  // Enter tuşu
            print(" ", cursor_row, cursor_col, 0x0F);
            cursor_col = 0;
            cursor_row++;
            if (cursor_row >= 25) {
                cursor_row = 0;
            }
        } else if (scancode == 0x0E) {  // Backspace tuşu
            if (cursor_col > 0) {
              	print(" ", cursor_row, cursor_col, 0x0F);
                cursor_col--;
                print(" ", cursor_row, cursor_col, 0x0F);
            }
        } else if (scancode < sizeof(keymap) && keymap[scancode] != '\0') {
            char str[2] = {keymap[scancode], '\0'};
            print(str, cursor_row, cursor_col, 0x0F);
            cursor_col++;
            if (cursor_col >= 80) {
                cursor_col = 0;
                cursor_row++;
                if (cursor_row >= 25) {
                    cursor_row = 0;
                }
            }
        }
    }

    // **İmleci güncelle**
    update_cursor();
}



const char keymap[128] = {
    '\0', '\0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=', '\b',
    '\t', 'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\n',
    '\0', 'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', '\'', '`',
    '\0', '\\', 'z', 'x', 'c', 'v', 'b', 'n', 'm', ',', '.', '/', '\0', '*',
    '\0', ' ', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0',
    '\0', '\0', '\0', '7', '8', '9', '-', '4', '5', '6', '+', '1', '2', '3', '0', '.'
};



/**
 * Initialize the Interrupt Descriptor Table (IDT).
 *
 * This function sets up the IDT and installs the keyboard interrupt handler (IRQ 1).
 */
void init_idt() {
    pic_remap();
    idt_ptr.limit = (sizeof(struct IDTEntry) * 256) - 1;
    idt_ptr.base = (unsigned int)&idt;

    set_idt_gate(33, (unsigned int)keyboard_entry, 0x08, 0x8E); // IRQ1 → IDT 33. entry

    idt_load();
}




