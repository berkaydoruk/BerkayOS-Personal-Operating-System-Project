#ifndef IO_H
#define IO_H

#include <stdint.h>

/**
 * `inb` - Port'tan 8-bit veri oku
 *
 * @param port  Okunacak I/O port numarası
 * @return      Porttan okunan 8-bit değer
 */
static inline uint8_t inb(uint16_t port) {
    uint8_t ret;
    asm volatile ("inb %1, %0" : "=a"(ret) : "Nd"(port));
    return ret;
}

/**
 * `outb` - Port'a 8-bit veri yaz
 *
 * @param port  Yazılacak I/O port numarası
 * @param val   Yazılacak değer
 */
static inline void outb(uint16_t port, uint8_t val) {
    asm volatile ("outb %0, %1" : : "a"(val), "Nd"(port));
}

/**
 * `inw` - Port'tan 16-bit veri oku
 */
static inline uint16_t inw(uint16_t port) {
    uint16_t ret;
    asm volatile ("inw %1, %0" : "=a"(ret) : "Nd"(port));
    return ret;
}

/**
 * `outw` - Port'a 16-bit veri yaz
 */
static inline void outw(uint16_t port, uint16_t val) {
    asm volatile ("outw %0, %1" : : "a"(val), "Nd"(port));
}

/**
 * `inl` - Port'tan 32-bit veri oku
 */
static inline uint32_t inl(uint16_t port) {
    uint32_t ret;
    asm volatile ("inl %1, %0" : "=a"(ret) : "Nd"(port));
    return ret;
}

/**
 * `outl` - Port'a 32-bit veri yaz
 */
static inline void outl(uint16_t port, uint32_t val) {
    asm volatile ("outl %0, %1" : : "a"(val), "Nd"(port));
}

#endif // IO_H
