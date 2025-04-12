.section .note.GNU-stack
.section .text
  .extern vTaskSwitchContext
  .extern xTaskIncrementTick
  
  .global vPortTickISR
  .global vPortYield
  .global xPortStartScheduler

.section .data
  .extern pxCurrentTCB
  .extern IRQ


.macro SAVE_CONTEXT
  # Save all general-purpose registers. Note that the stack pointer is also
  # included in "pusha"
  pusha                       # eax, ecx, edx, ebx, esp, ebp, esi, edi
  pushfl                      # eflags register

  # Save interrupt flags
  movl   IRQ@GOT(%ebx), %eax  # Load address of IRQ from GOT
  pushl  (%eax)               # Push the current value of IRQ onto the stack

  # Update TCB with our stack pointer: *(*TCB).pxTopOfStack = esp
  movl   pxCurrentTCB@GOT(%ebx), %eax  # Load address of pxCurrentTCB from GOT
  movl   (%eax), %eax         # Load first field of TCB struct (pxTopOfStack)
  movl   %esp, (%eax)         # Store the current stack pointer in pxTopOfStack
.endm

.macro RESTORE_CONTEXT
  # Load our stack pointer from TCB: esp = *(*TCB).pxTopOfStack
  movl   pxCurrentTCB@GOT(%ebx), %edx  # Load address of pxCurrentTCB from GOT
  movl   (%edx), %edx         # Load first field of TCB struct (pxTopOfStack)
  movl   (%edx), %esp         # Store value of pxTopOfStack in stack pointer

  # Restore interrupt flags
  popl   %eax                 # Pop the interrupt flags from the stack
  movl   IRQ@GOT(%ebx), %edx  # Load address of IRQ from GOT
  movl   %eax, (%edx)         # Store the popped value in IRQ

  # Restore all general-purpose registers. Note that the stack pointer is also
  # included in "popa"
  popfl                       # eflags register
  popa                        # eax, ecx, edx, ebx, esp, ebp, esi, edi
.endm


.section .text
vPortTickISR:
  call   xTaskIncrementTick  # Increment tick count and check for task switch
  testl  %eax, %eax          # Was a task switch requested?
  jne    vPortYield
  ret
vPortYield:
  SAVE_CONTEXT               # Save context of the current task
  call   vTaskSwitchContext  # Call scheduler
xPortStartScheduler:
  RESTORE_CONTEXT            # Restore context of new task
  ret                        # "Return" to the task function
