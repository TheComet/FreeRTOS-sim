extern InstructionCallback
global InstructionCallback_Safe

section .text
align 4
InstructionCallback_Safe:
    pusha
    pushfd
    call   InstructionCallback
    popfd
    popa
    ret
