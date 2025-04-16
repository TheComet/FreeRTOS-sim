.section .note.GNU-stack
.section .text
  .extern InstructionCallback
  .global InstructionCallback_Safe

InstructionCallback_Safe:
  pushal
  pushfl
  movl   %esp, %ebp
  andl   $0xFFFFFFF0, %esp    # Align stack to 16 bytes for C-function call
  call   InstructionCallback
  movl   %ebp, %esp
  popfl
  popal
  ret
