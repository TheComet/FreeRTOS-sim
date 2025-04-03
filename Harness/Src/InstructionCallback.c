#include "FreeRTOS.h"
#include "Hardware/Hardware.h"

static void ProcessPendingInterrupts(void) {
  if (IRQ.SYSTICK_IF) {
    IRQ.SYSTICK_IF = 0;
    vPortTickISR();
  }
}

void InstructionCallback(void) {
  static int mclk;
  static int aclk;
  if (mclk++ % 8 == 0) {
    if (aclk++ % 8 == 0) {
      IRQ.SYSTICK_IF = 1;
    }
  }

  if (IRQ.GIE) {
    ProcessPendingInterrupts();
  }
}
