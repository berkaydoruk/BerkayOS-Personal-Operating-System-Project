#include "kernel.h"
#include "keyboard.h"
#include "idt.h"
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

/**
 * Set an entry in the IDT.
 *
 * @param num   Interrupt number (IRQ or exception number)
 * @param base  Address of the ISR function
 * @param sel   Segment selector (should be 0x08 for kernel code segment)
 * @param flags Access control flags (0x8E for interrupt gates)
 */
void set_idt_gate(int num, uint32_t base, uint16_t sel, uint8_t flags) {
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

/* ===================== KEYBOARD INTERRUPT HANDLER ===================== */

#define KEYBOARD_DATA_PORT 0x60
#define VGA_WIDTH 80

unsigned int cursor_row = 0;
unsigned int cursor_col = 0;

void move_cursor(uint16_t pos) {
    outb(0x3D4, 0x0F);
    outb(0x3D5, (uint8_t)(pos & 0xFF));
    outb(0x3D4, 0x0E);
    outb(0x3D5, (uint8_t)((pos >> 8) & 0xFF));
}

void update_cursor() {
    uint16_t pos = cursor_row * VGA_WIDTH + cursor_col;
    move_cursor(pos);
}

extern void keyboard_entry();

void keyboard_interrupt_handler() {
    uint8_t scancode = inb(KEYBOARD_DATA_PORT);

    // EOI gönder (Master PIC'e)
    outb(PIC1_COMMAND, 0x20);

    if (scancode != 0xE0 && !(scancode & 0x80)) {
        if (scancode == 0x1C) {  // Enter tuşu
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
    idt_ptr.base = (uint32_t)&idt;

    set_idt_gate(33, (uint32_t)keyboard_entry, 0x08, 0x8E); // IRQ1 → IDT 33. entry

    idt_load();
}
