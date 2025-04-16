# Some notes:
#   - Floating point registers are not saved/restored
#     + The code under test currently doesn't use floats so this is not a
#       problem.
#   - SIMD registers are not saved/restored
#     + The code under test *may* be auto-vectorized by GCC. If problems occur,
#       either disable this feature with -fno-tree-vectorize or adjust the
#       SAVE_CONTEXT and RESTORE_CONTEXT macros.
#   - When calling C functions, the stack pointer must always be aligned to 16
#     bytes prior to the call. Any C code calling assembly routines will have
#     aligned the stack pointer to 16 bytes as well. This makes it straight
#     forward to adjust the stack pointer. On function entry, the stack pointer
#     will be -4 bytes due to the return address.
#   - The stack used by main() is stored into main_stack so it can be restored
#     in vPortEndScheduler.
#   - The main stack is additionally (ab)used to pivot function calls that
#     require more stack space. The FreeRTOS task stacks are quite small because
#     they were designed for embedded use. The simulation is updated using this
#     technique.

.section .note.GNU-stack

.section .data
  .align 16
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
  pushal                      # eax, ecx, edx, ebx, esp, ebp, esi, edi
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
  popal                       # eax, ecx, edx, ebx, esp, ebp, esi, edi
.endm


.section .text
vPortTickISR:
  leal   -12(%esp), %esp     # return address(4) + 12 = 16
  call   xTaskIncrementTick  # Increment tick count and check for task switch
  leal   12(%esp), %esp      # We may jump to vPortYield
  testl  %eax, %eax          # Was a task switch requested?
  jne    vPortYield          # Yes? Switch tasks
  ret

vPortYield:
  SAVE_CONTEXT               # Save context of the current task
  leal   -4(%esp), %esp      # return address(4) + context(8) + 4 = 16
  call   vTaskSwitchContext  # Call scheduler
  RESTORE_CONTEXT            # Restore context of new task
  ret                        # "Return" to the task function

xPortStartScheduler:
  # See port.c
  leal   -12(%esp), %esp     # return address(4) + 12 = 16
  call   vPortCreateEndSchedulerTask
  leal   12(%esp), %esp

  # Save context of the main thread so we can restore it in vPortEndScheduler
  pushal
  pushfl
  movl   main_stack@GOT(%ebx), %eax  # Load address of main_stack from GOT
  movl   %esp, (%eax)

  # Restore context of the first task and "return" to the task function
  RESTORE_CONTEXT
  ret

vPortEndScheduler:
  # Clean up FreeRTOS global state
  leal   -12(%esp), %esp     # return address(4) + 12 = 16
  call   vTaskResetState
  call   vTimerResetState
  #call   vCoRoutineResetState
  call   vPortHeapResetState

  # Restore context of the main thread
  movl  main_stack@GOT(%ebx), %eax
  movl  (%eax), %esp
  popfl
  popal
  ret

vPortCallOnMainStack:
  # Currently on the stack:
  #   0: function to call using said stack
  #   1: return address

  # NOTE: main_stack is not aligned

  movl   main_stack@GOT(%ebx), %ecx  # Load alternate stack pointer
  movl   (%ecx), %ecx
  movl   4(%esp), %edx       # Load function to call
  movl   %esp, -8(%ecx)      # Save our stack pointer on the alternate stack
  leal   -8(%ecx), %esp      # Switch stacks
  call   *%edx               # Call function on alternate stack
  popl   %esp                # Restore original stack pointer
  ret
