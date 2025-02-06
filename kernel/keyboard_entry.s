.global keyboard_entry
.extern keyboard_interrupt_handler

keyboard_entry:
    pusha
    call keyboard_interrupt_handler
    popa
    iret
