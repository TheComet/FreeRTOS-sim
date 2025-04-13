#include "App/Init.h"
#include "FreeRTOS.h"
#include "task.h"

portTASK_FUNCTION_PROTO(AppTask1, pvParameters);
portTASK_FUNCTION_PROTO(AppTask2, pvParameters);

void App_Init(void) {
  xTaskCreate(AppTask1, "Task1", configMINIMAL_STACK_SIZE, NULL,
              tskIDLE_PRIORITY + 1, NULL);
  xTaskCreate(AppTask2, "Task2", configMINIMAL_STACK_SIZE, NULL,
              tskIDLE_PRIORITY + 1, NULL);
}

uint8_t ucHeap[configTOTAL_HEAP_SIZE];
void vApplicationStackOverflowHook(TaskHandle_t pxTask, char *pcTaskName) {}

void InstructionCallback(void);
void vApplicationIdleHook(void) { InstructionCallback(); }
