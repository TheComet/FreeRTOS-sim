extern vTaskSwitchContext
extern xTaskIncrementTick
extern pxCurrentTCB
extern usCriticalNesting
extern IRQ

global vPortTickISR
global vPortYield
global xPortStartScheduler

%macro SAVE_CONTEXT 0
    ; Save all general-purpose registers. Note that the stack pointer is also
    ; included in "pusha"
    pusha   ; eax, ecx, edx, ebx, esp, ebp, esi, edi
    pushfd  ; eflags register

    ; Save critical section state. This includes interrupt flags and nesting counter
    mov    eax, [IRQ]               ; Save Interrupt flags
    push   eax
    mov    eax, [usCriticalNesting] ; Save nesting counter
    push   eax

    ; Update TCB with our stack pointer: *(*TCB).pxTopOfStack = esp
    mov    eax, [pxCurrentTCB]
    mov    [eax], esp
%endmacro

%macro RESTORE_CONTEXT 0
    ; Load our stack pointer from TCB: esp = *(*TCB).pxTopOfStack
    mov    eax, [pxCurrentTCB]
    mov    esp, [eax]

    ; Restore critical section state. This includes interrupt flags and nesting counter
    pop    eax
    mov    [usCriticalNesting], eax ; Restore nesting counter
    pop    eax
    mov    [IRQ], eax          ; Restore interrupt flags

    ; Restore all general-purpose registers. Note that the stack pointer is also
    ; included in "popa"
    popfd   ; eax, ecx, edx, ebx, esp, ebp, esi, edi
    popa    ; eflags register
%endmacro

section .text

align 4
vPortTickISR:
    call   xTaskIncrementTick  ; Increment tick count and check for task switch
    test   eax, eax            ; Was a task switch requested?
    jne    vPortYield
    ret

align 4
vPortYield:
    SAVE_CONTEXT               ; Save context of the current task
    call   vTaskSwitchContext  ; Call scheduler
    RESTORE_CONTEXT            ; Restore context of new task
    ret                        ; "Return" to the task function

align 4
xPortStartScheduler:
    RESTORE_CONTEXT            ; Restore context of the first task
    ret                        ; "Return" to the task function
