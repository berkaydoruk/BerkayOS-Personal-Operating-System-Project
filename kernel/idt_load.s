.section .text
.global idt_load  # Export the function for use in C

idt_load:
    lidt (idt_ptr)  # Load the IDT pointer
    ret           # Return to caller
