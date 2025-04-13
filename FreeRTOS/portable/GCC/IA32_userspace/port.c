#include "FreeRTOS.h"
#include "portable.h"
#include "queue.h"
#include "task.h"

StackType_t *pxPortInitialiseStack(StackType_t *pxTopOfStack,
                                   TaskFunction_t pxCode, void *pvParameters) {
  /* TaskFunc(void* pvParameters) */
  *--pxTopOfStack = (StackType_t)pvParameters; /* Task argument 1 */
  *--pxTopOfStack = 0x00000000;                /* return address to scheduler */

  /* Set up so we "return" to the task function when restoring context */
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

  /* Need to store interrupt state because FreeRTOS can yield in critical
   * sections */
  struct SR initialSR = {.GIE = 1};
  *--pxTopOfStack = *(uint32_t *)&initialSR;

  return pxTopOfStack;
}

/* There's no nicer way to end the scheduler from the simulation.
 * vTaskEndScheduler() must be called from a task context, and it cannot be
 * called from the idle task either... */
static QueueHandle_t endSchedulerQueue;
portTASK_FUNCTION(EndSchedulerTask, pvParameters) {
  char buf;
  (void)pvParameters;
  endSchedulerQueue = xQueueCreate(1, sizeof(buf));
  xQueueReceive(endSchedulerQueue, &buf, portMAX_DELAY);
  vQueueDelete(endSchedulerQueue);
  endSchedulerQueue = NULL;
  vTaskEndScheduler();
  configASSERT(0);
}

void vPortEndSchedulerFromISR(void) {
  char buf;
  BaseType_t xHigherPriorityTaskWoken = pdFALSE;
  xQueueSendFromISR(endSchedulerQueue, &buf, &xHigherPriorityTaskWoken);
  portYIELD_FROM_ISR();
}

void vPortCreateEndSchedulerTask(void) {
  xTaskCreate(EndSchedulerTask, "EndSched", configMINIMAL_STACK_SIZE, NULL,
              configMAX_PRIORITIES - 1, NULL);
}
