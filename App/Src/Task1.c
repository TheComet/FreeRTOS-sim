#include "FreeRTOS.h"
#include "Hardware/Gpio.h"
#include "task.h"
#include "timers.h"
#include <stdio.h>

void MyTimer(TimerHandle_t xTimer) { printf("Timer\n"); }

static int DoWork(int x) {
  int y = x * 2;
  y = y % 50;
  return y * 5;
}

portTASK_FUNCTION(AppTask1, pvParameters) {
  TimerHandle_t handle = xTimerCreate("MyTimer", 1, pdTRUE, NULL, MyTimer);
  xTimerStart(handle, portMAX_DELAY);

  static int x;
  while (1) {
    x++;
    DoWork(x);
    printf("Task 1: %d\n", x);
  }
}
