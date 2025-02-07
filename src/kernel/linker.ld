ENTRY(kernel_main)    /* Entry point: kernel_main function */

SECTIONS {
    . = 1M;          /* (GRUB bu adrese yükler) Kernel starts from 1MB adress. (GRUB loads it to this adress) */

    /* Multiboot header */
    .multiboot : {
        *(.multiboot)
    }

    /* Codes */
    .text : {
        *(.text)     /* All codes (included kernel_main) */
    }

    /* Started data section */
    .data : {
        *(.data)
    }

    /* Unstarted data (BSS) part */
    .bss : {
        *(COMMON)
        *(.bss)
    }
}