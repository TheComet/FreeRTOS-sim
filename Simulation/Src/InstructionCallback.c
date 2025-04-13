#include "Simulation/InstructionCallback.h"
#include "Hardware/Interrupts.h"
#include "Simulation/StackPivot.h"
#include "Simulation/Tick.h"

/* FreeRTOS */
#include "FreeRTOS.h"
#include "queue.h"
#include "task.h"

static struct Tick tick;
static int realtimeMode = 0;

static int ProcessPendingInterrupts(void) {
  int interruptHasOccurred = IRQ.SYSTICK_IF | IRQ.ENDSCHED_IF;
  if (IRQ.SYSTICK_IF) {
    IRQ.SYSTICK_IF = 0;
    vPortTickISR();
  }
  if (IRQ.ENDSCHED_IF) {
    IRQ.ENDSCHED_IF = 0;
    vPortEndSchedulerFromISR();
  }
  return interruptHasOccurred;
}

void EndSimulation(void) { IRQ.ENDSCHED_IF = 1; }

static void StepSimulation(void) {
  if (realtimeMode) {
    Tick_Wait(&tick);
  }
}

void InstructionCallback(void) {
  /* Update clock tree */
  static int hclk;
  if (hclk++ == configCPU_CLOCK_HZ / configTICK_RATE_HZ) {
    hclk = 0;
    IRQ.SYSTICK_IF = 1;
    StackPivot_Call(StepSimulation);
  }

  /* The instruction callback may be called from an interrupt handler, or from
   * within a critical section. For this reason we set the interrupt flags in
   * the clock tree update, and call vPortTickISR() only when interrupts are
   * enabled. */
  if (SR.GIE) {
    ProcessPendingInterrupts();
  }
}

void InstructionCallback_EnableRealTimeMode(void) {
  Tick_Config(&tick, 1000);
  realtimeMode = 1;
}

#if configUSE_TICKLESS_IDLE == 1
void vPortSleep(TickType_t xExpectedIdleTime) {
  switch (eTaskConfirmSleepModeStatus()) {
  case eAbortSleep: {
    /* A task has been moved out of the Blocked state since this macro was
    executed, or a context switch is being held pending.  Do not enter a
    sleep state. */
    break;
  }

    /* All tasks are in the blocked state and we can fast forward indefinitely.
     * We only return if an external interrupt occurs. */
  case eNoTasksWaitingTimeout:
    xExpectedIdleTime = portMAX_DELAY;
    /* fallthrough */
  case eStandardSleep: {
    /* We can fast forward up to the number of ticks passed in to this function.
     * We may have to stop earlier if the simulation causes an external
     * interrupt. */
    TickType_t i;
    for (i = 0; !ProcessPendingInterrupts() && i < xExpectedIdleTime; i++) {
      StackPivot_Call(StepSimulation);
    }
    vTaskStepTick(i);
    break;
  }
  }
}
#endif /* configUSE_TICKLESS_IDLE */
