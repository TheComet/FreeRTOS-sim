extern instruction_callback
global instruction_callback_invisible

section .text

align 4
instruction_callback_invisible:
    pusha
    pushfd
    call instruction_callback
    popfd
    popa
    ret

