#include "FreeRTOS.h"
#include "task.h"
#include <stdio.h>
#include <string.h>

int x;

static int GetStrLen(const char *s) { return strlen(s) + 5; }

static void DoWork(void *param, const char *s) { int l = GetStrLen(s); }

portTASK_FUNCTION(AppTask1, pvParameters) {
  while (1) {
    x++;
    DoWork((void *)x, "hahahah");
    printf("Task1\n");
  }
}
