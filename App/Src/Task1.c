#include "FreeRTOS.h"
#include "task.h"
#include <stdio.h>

int x;

portTASK_FUNCTION(AppTask1, pvParameters) {
  while (1) {
    x++;
    printf("Task1\n");
  }
}
