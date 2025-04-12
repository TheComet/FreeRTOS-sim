.section .note.GNU-stack
.section .text
  .extern InstructionCallback
  .global InstructionCallback_Safe

InstructionCallback_Safe:
  pusha
  pushfl
  call   InstructionCallback
  popfl
  popa
  ret
