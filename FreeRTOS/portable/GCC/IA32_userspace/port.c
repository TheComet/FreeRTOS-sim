#include "FreeRTOS.h"
#include "portable.h"

volatile uint16_t usCriticalNesting = 0;

StackType_t *pxPortInitialiseStack(StackType_t *pxTopOfStack,
                                   TaskFunction_t pxCode, void *pvParameters) {
  /* TaskFunc(void* pvParameters) */
  *--pxTopOfStack = (StackType_t)pvParameters; /* Task argument 1 */
  *--pxTopOfStack = 0x00000000;                /* return address to scheduler */

  /* Set up so we "return" to the task function */
  *--pxTopOfStack = (StackType_t)pxCode; /* return address to task */

  /* pusha instruction stores all GPR's in one go */
  StackType_t *pxStack = pxTopOfStack;
  *--pxTopOfStack = 0xaaaaaaaa;           /* eax */
  *--pxTopOfStack = 0xcccccccc;           /* ecx */
  *--pxTopOfStack = 0xdddddddd;           /* edx */
  *--pxTopOfStack = 0xbbbbbbbb;           /* ebx */
  *--pxTopOfStack = (StackType_t)pxStack; /* esp (stack pointer) */
  *--pxTopOfStack = 0x33333333;           /* ebp */
  *--pxTopOfStack = 0x22222222;           /* esi */
  *--pxTopOfStack = 0x11111111;           /* edi */

  /* eflags */
  *--pxTopOfStack = 0x00000000;

  /* Initial IRQ and nesting counters -- Interrupts are expected to be enabled
   * as soon as the task begins, this is why we set the GIE=1 flag here. */
  struct IRQ initialIRQ = {.GIE = 1};
  uint16_t initialCriticalNesting = 0;
  *--pxTopOfStack = *(uint32_t *)&initialIRQ;
  *--pxTopOfStack = initialCriticalNesting;

  return pxTopOfStack;
}

#include <unistd.h>
void vPortEndScheduler(void) { _exit(1); }
