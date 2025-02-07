#include "kernel.h"
#include "keyboard.h"
#include <stdint.h>

struct IDTEntry idt[256];
struct IDTPointer idt_ptr;

// Multiboot Header (Mandatory for GRUB)
__attribute__((section(".multiboot")))
const unsigned int multiboot_header[] = {
    0x1BADB002,    // Magic number (Fixed number for GRUB to know kernel)
    0x00,          // Flags (No extra feature. For eaxmple for memory map 0x01 is used.)
    -(0x1BADB002 + 0x00) // Checksum (magic + flags + checksum = 0)
};

// VGA Buffer starts at memory address 0xB8000
volatile char *vga_buffer = (volatile char*)0xB8000; // volatile: Tells to the compliler to optimize this part of memory area.
#define WHITE_ON_BLACK 0x0F
/**
 * Function to print a message on the screen using VGA text mode.
 *
 * @param message - The string to be printed.
 * @param row - The row position (0 to 24).
 * @param col - The column position (0 to 79).
 * @param color - The color of the text (foreground and background).
 *
 * The function iterates through the message string and writes each
 * character along with its color attribute to the VGA memory buffer.
 * If a newline ('\n') is encountered, the text moves to the next row.
 */
void print(const char *message, unsigned int row, unsigned int col, unsigned char color) {
    // Calculate the starting index in the VGA buffer based on row and column
    unsigned int index = (row * 80 + col) * 2;

    // Iterate through the message string character by character
    for (int i = 0; message[i] != '\0'; i++) {
        if (message[i] == '\n') {
            // If a newline character is encountered, move to the next row
            row++;
            col = 0; // Reset column position
            index = (row * 80 + col) * 2; // Update buffer index
            continue;
        }
        vga_buffer[index] = message[i];   // Store character in VGA memory
        vga_buffer[index + 1] = color;    // Store color attribute
        index += 2;  // Move to the next character slot
    }
}
struct IDTPointer idt_ptr;

void clear_screen() {
    for (int row = 0; row < 25; row++) {
        for (int col = 0; col < 80; col++) {
            print(" ", row, col, 0x0F);  // Ekranı boşluklarla doldur
        }
    }
    cursor_row = 0;  // İmleci başa al
    cursor_col = 0;
    update_cursor();
}

/**
 * Kernel main function.
 *
 * This function is the entry point of the OS kernel. It initializes
 * the screen by printing a welcome message using the `print_to_screen` function.
 * Once the message is displayed, the kernel enters an infinite loop.
 */

void kernel_main() {

  	clear_screen();

    print("Kernel is starting...\n", 0, 0, 0x0F);

    init_idt();

    asm volatile ("sti");


    while (1);
}

