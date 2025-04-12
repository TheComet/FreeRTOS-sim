#include "FreeRTOS.h"
#include "task.h"
#include <stdio.h>

int y;

portTASK_FUNCTION(AppTask2, pvParameters) {
  while (1) {
    y++;
    printf("Task2 %d\n", y);
    vTaskDelay(pdMS_TO_TICKS(1000));
  }
}
