#include "FreeRTOS.h"
#include "task.h"

portTASK_FUNCTION_PROTO(AppTask1, pvParameters);
portTASK_FUNCTION_PROTO(AppTask2, pvParameters);

int main(void) {
  xTaskCreate(AppTask1, "Task1", configMINIMAL_STACK_SIZE, NULL,
              tskIDLE_PRIORITY + 1, NULL);
  xTaskCreate(AppTask2, "Task2", configMINIMAL_STACK_SIZE, NULL,
              tskIDLE_PRIORITY + 1, NULL);

  vTaskStartScheduler();
}

uint8_t ucHeap[configTOTAL_HEAP_SIZE];
void vApplicationStackOverflowHook(TaskHandle_t pxTask, char *pcTaskName) {}

__attribute__((no_caller_saved_registers)) void instruction_callback(void);
void vApplicationIdleHook(void) { instruction_callback(); }
