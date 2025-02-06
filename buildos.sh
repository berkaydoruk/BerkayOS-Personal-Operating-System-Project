#!/bin/bash
set -e

cd kernel
i686-elf-as keyboard_entry.s -o keyboard_entry.o
i686-elf-as idt_load.s -o idt_load.o
i686-elf-gcc -ffreestanding -c kernel.c -o kernel.o
i686-elf-gcc -ffreestanding -c keyboard.c -o keyboard.o
#i686-elf-gcc -ffreestanding -c idt_load.s -o idt_load.o
i686-elf-ld -T linker.ld -o ../isodir/boot/kernel.bin kernel.o keyboard.o idt_load.o keyboard_entry.o
cd ..
rm myos.iso
grub-mkrescue -o myos.iso isodir
#qemu-system-i386 -cdrom myos.iso -serial stdio
cd isodir/boot
qemu-system-i386 -kernel kernel.bin
cd
cd myos
