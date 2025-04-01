#include <stdint.h>
#include <stdio.h>

typedef uint32_t StackType_t;
struct TaskControlBlock {
  volatile StackType_t *pxTopOfStack;
};

volatile struct TaskControlBlock *volatile pxCurrentTCB;

struct TaskControlBlock tcb1;
struct TaskControlBlock tcb2;
struct TaskControlBlock tcb3;

void vPortTickISR(void);
void vPortYield(void);
void vPortStartScheduler(void);

static void instruction_callback(void) {
  static int mclk;
  static int aclk;
  if (mclk++ % 8 == 0) {
    aclk++;
    vPortTickISR();
  }
}

void vTaskSwitchContext(void) {
  if (pxCurrentTCB == &tcb1)
    pxCurrentTCB = &tcb2;
  else if (pxCurrentTCB == &tcb2)
    pxCurrentTCB = &tcb3;
  else
    pxCurrentTCB = &tcb1;
}

int xTaskIncrementTick(void) { return 1; }

static StackType_t *InitStack(StackType_t *pxStackMem, uint32_t uStackMemSize,
                              void (*pxCode)(void *), void *pvParameters) {
  StackType_t *pxTopOfStack = pxStackMem + uStackMemSize;

  /* Set up so we "return" to the task function */
  *--pxTopOfStack = (StackType_t)pvParameters; /* Task argument 1 */
  *--pxTopOfStack = 0x00000000;                /* return address to scheduler */
  *--pxTopOfStack = (StackType_t)pxCode;       /* return address to task */

  /* pusha instruction stores all GPR's in one go */
  StackType_t *pxStack = pxTopOfStack;
  *--pxTopOfStack = 0xaaaaaaaa;           /* eax */
  *--pxTopOfStack = 0xcccccccc;           /* ecx */
  *--pxTopOfStack = 0xdddddddd;           /* edx */
  *--pxTopOfStack = 0xbbbbbbbb;           /* ebx */
  *--pxTopOfStack = (StackType_t)pxStack; /* esp (stack pointer) */
  *--pxTopOfStack = 0x33333333;           /* ebp */
  *--pxTopOfStack = 0x22222222;           /* esi */
  *--pxTopOfStack = 0x11111111;           /* edi */

  /* eflags */
  *--pxTopOfStack = 0x00000000;

  return pxTopOfStack;
}

static StackType_t task1_stack[1024];
static void Task1(void *pvParameters) {
  int stackvar = 50;
  while (1) {
    instruction_callback();
    printf("Task1: %d\n", stackvar);
    vPortYield();
  }
}

static StackType_t task2_stack[1024];
static void Task2(void *pvParameters) {
  int stackvar = 80;
  while (1) {
    instruction_callback();
    printf("Task2: %d\n", stackvar);
    vPortYield();
  }
}

static StackType_t task3_stack[1024];
static void Task3(void *pvParameters) {
  int stackvar = *(int *)pvParameters;
  while (1) {
    instruction_callback();
    printf("Task3: %d\n", stackvar);
  }
}

int main(int argc, char *argv[]) {
  int value = 42;
  tcb1.pxTopOfStack = InitStack(task1_stack, 1024, Task1, NULL);
  tcb2.pxTopOfStack = InitStack(task2_stack, 1024, Task2, NULL);
  tcb3.pxTopOfStack = InitStack(task3_stack, 1024, Task3, &value);
  pxCurrentTCB = &tcb3;
  vPortStartScheduler();
  return 0;
}
