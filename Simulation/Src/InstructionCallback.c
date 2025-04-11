#include "FreeRTOS.h"
#include "Hardware/Interrupts.h"
#include "Simulation/Tick.h"
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
  static int mclk;
  static int aclk;
  if (mclk++ % 8 == 0) {
    if (aclk++ % 8 == 0) {
      IRQ.SYSTICK_IF = 1;
      if (realtimeMode) {
        SynchronizeWithSystemTime();
      }
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
