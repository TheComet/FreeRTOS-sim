#include "FreeRTOS.h"
#include "task.h"
#include <stdio.h>

int y;

portTASK_FUNCTION(AppTask2, pvParameters) {
  while (1) {
    y++;
  }
}
