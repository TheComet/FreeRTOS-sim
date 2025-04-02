extern vTaskSwitchContext
extern xTaskIncrementTick
extern pxCurrentTCB

global vPortTickISR
global vPortYield
global xPortStartScheduler

%macro SAVE_CONTEXT 0
    pusha                      ; Save all general-purpose registers, including
                               ; esp (stack pointer)
    pushfd                     ; Save flags register
    mov    eax, [pxCurrentTCB] ; TCB->pxTopOfStack = esp 
    mov    [eax], esp          ; (store stack pointer)
%endmacro

%macro RESTORE_CONTEXT 0
    mov    eax, [pxCurrentTCB] ; esp = TCB->pxTopOfStack
    mov    esp, [eax]          ; (restore stack pointer)
    popfd                      ; Restore flags register
    popa                       ; Restore all general-purpose registers
%endmacro

section .text
align 4
vPortTickISR:
    SAVE_CONTEXT               ; Save context of the current task
    call   xTaskIncrementTick  ; Increment tick count and check for task switch
    test   eax, eax            ; Was a task switch requested?
    je .skip_context_switch
    call   vTaskSwitchContext  ; Call scheduler
.skip_context_switch:
    RESTORE_CONTEXT            ; Restore context of new task
    ret

align 4
vPortYield:
    SAVE_CONTEXT               ; Save context of the current task
    call   vTaskSwitchContext  ; Call scheduler
    RESTORE_CONTEXT            ; Restore context of new task
    ret

align 4
xPortStartScheduler:
    ; TODO: Initialize timer here
    RESTORE_CONTEXT            ; Restore context of the first task
    ret
