.section .note.GNU-stack

.section .data
  main_stack: .long 0

.section .text
  .extern vTaskSwitchContext
  .extern xTaskIncrementTick
  .extern vPortCreateEndSchedulerTask
  .extern vTaskResetState
  .extern vTimerResetState
  .extern vPortHeapResetState
  
  .global vPortTickISR
  .global vPortYield
  .global xPortStartScheduler
  .global vPortEndScheduler
  .global vPortCallOnMainStack

.section .data
  .extern pxCurrentTCB
  .extern SR


.macro SAVE_CONTEXT
  pusha                       # eax, ecx, edx, ebx, esp, ebp, esi, edi
  pushfl                      # eflags register

  movl   SR@GOT(%ebx), %eax   # Load address of status register from GOT
  pushl  (%eax)               # Push the current value of SR onto the stack

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

  popl   %eax                 # Pop the interrupt flags from the stack
  movl   SR@GOT(%ebx), %edx   # Load address of status register from GOT
  movl   %eax, (%edx)         # Store the popped value in SR

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
  RESTORE_CONTEXT            # Restore context of new task
  ret                        # "Return" to the task function

xPortStartScheduler:
  # See port.c
  call   vPortCreateEndSchedulerTask

  # Save context of the main thread so we can restore it in vPortEndScheduler
  pusha
  pushfl
  movl   main_stack@GOT(%ebx), %eax  # Load address of main_stack from GOT
  movl   %esp, (%eax)

  # Restore context of the first task and "return" to the task function
  RESTORE_CONTEXT
  ret

vPortEndScheduler:
  # Clean up FreeRTOS global state
  call   vTaskResetState
  call   vTimerResetState
  #call   vCoRoutineResetState
  call   vPortHeapResetState

  # Restore context of the main thread
  movl  main_stack@GOT(%ebx), %eax
  movl  (%eax), %esp
  popfl
  popa
  ret

vPortCallOnMainStack:
  # Currently on the stack:
  #   0: function to call using said stack
  #   1: return address
  movl   main_stack@GOT(%ebx), %ecx  # Load alternate stack pointer
  movl   (%ecx), %ecx
  movl   4(%esp), %edx       # Load function to call
  movl   %esp, -4(%ecx)      # Save our stack pointer on the alternate stack
  leal   -4(%ecx), %esp      # Switch stacks
  call   *%edx               # Call function on alternate stack
  popl   %esp                # Restore original stack pointer
  ret
