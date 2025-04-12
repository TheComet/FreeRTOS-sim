#include "FreeRTOS.h"
#include "Hardware/Interrupts.h"
#include "Simulation/Tick.h"
#include "task.h"
#include <stdio.h>

static struct Tick tick;
int realtimeMode = 0;

void InstructionCallback_EnableRealTimeMode(void) {
  Tick_Config(&tick, 1000);
  realtimeMode = 1;
}

static void SynchronizeWithSystemTime(void) {
  int lag = Tick_Wait(&tick);
  if (lag > 0) {
    fprintf(stderr, "Simulation is lagging! %d ticks behind\n", lag);
  }
  if (lag > 1000 /* 1 second */) {
    Tick_Skip(&tick);
  }
}

static void ProcessPendingInterrupts(void) {
  if (IRQ.SYSTICK_IF) {
    IRQ.SYSTICK_IF = 0;
    vPortTickISR();
  }
}

void InstructionCallback(void) {
  /* Update clock tree */
  static int hclk;
  if (hclk++ == configCPU_CLOCK_HZ / configTICK_RATE_HZ) {
    hclk = 0;
    IRQ.SYSTICK_IF = 1;

    if (realtimeMode) {
      SynchronizeWithSystemTime();
    }
  }

  /* The instruction callback may be called from an interrupt handler, or from
   * within a critical section. For this reason we set the interrupt flags in
   * the clock tree update, and call vPortTickISR() only when interrupts are
   * enabled. */
  if (IRQ.GIE) {
    ProcessPendingInterrupts();
  }
}

#if configUSE_TICKLESS_IDLE == 1
void vPortSleep(TickType_t xExpectedIdleTime) {
  /* Ensure it is still ok to enter the sleep mode. */
  switch (eTaskConfirmSleepModeStatus()) {
  case eAbortSleep: {
    /* A task has been moved out of the Blocked state since this macro was
    executed, or a context switch is being held pending.  Do not enter a
    sleep state. */
    break;
  }

  case eStandardSleep: {
    /* We can fast forward by the number of ticks passed in to this function */
    // TODO: Run simulation here
    if (realtimeMode) {
      Tick_WaitFor(&tick, xExpectedIdleTime);
    }
    vTaskStepTick(xExpectedIdleTime);
    break;
  }

  case eNoTasksWaitingTimeout: {
    /* All tasks are in the blocked state and we can fast forward indefinitely.
     * We only return if an external interrupt occurs. */
    // TODO: Run simulation here until it causes an external interrupt
    break;
  }
  }
}
#endif /* configUSE_TICKLESS_IDLE */
