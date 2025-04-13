#include "FreeRTOS.h"
#include "Hardware/Gpio.h"
#include "task.h"
#include "timers.h"
#include <stdio.h>

void MyTimer(TimerHandle_t xTimer) {
  static int counter;
  printf("Timer %d\n", counter++);
}

static int DoWork(int x) {
  int y = x * 2;
  y = y % 50;
  return y * 5;
}

portTASK_FUNCTION(AppTask1, pvParameters) {
  TimerHandle_t handle =
      xTimerCreate("MyTimer", pdMS_TO_TICKS(500), pdTRUE, NULL, MyTimer);
  xTimerStart(handle, portMAX_DELAY);

  int x = 0;
  while (1) {
    x++;
    DoWork(x);
    printf("Task1 %d\n", x);
    vTaskDelay(pdMS_TO_TICKS(1000));
  }
}
