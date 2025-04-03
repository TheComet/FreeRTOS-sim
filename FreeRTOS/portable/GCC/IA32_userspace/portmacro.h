#ifndef PORTMACRO_H
#define PORTMACRO_H

#include "Hardware/Hardware.h"
#include <stdint.h>

/*-----------------------------------------------------------
 * Port specific definitions.
 *
 * The settings in this file configure FreeRTOS correctly for the
 * given hardware and compiler.
 *
 * These settings should not be altered.
 *-----------------------------------------------------------
 */

/* Type definitions. */
#define portCHAR                    char
#define portFLOAT                   float
#define portDOUBLE                  double
#define portLONG                    long
#define portSHORT                   short
#define portSTACK_TYPE              uint32_t
#define portBASE_TYPE               long

#define portSTACK_GROWTH            ( -1 )
#define portBYTE_ALIGNMENT          32
#define portPOINTER_SIZE_TYPE       size_t
typedef portSTACK_TYPE              StackType_t;

typedef int32_t                     BaseType_t;
typedef uint32_t                    UBaseType_t;
typedef uint32_t                    TickType_t;

#define portTICK_PERIOD_MS          ((TickType_t) 1000 / configTICK_RATE_HZ )
#define portMAX_DELAY               ((TickType_t)0xffffffffUL)

#define portDISABLE_INTERRUPTS()    IRQ.GIE = 0
#define portENABLE_INTERRUPTS()     IRQ.GIE = 1

extern volatile uint16_t usCriticalNesting;

#define portENTER_CRITICAL()       \
  do {                             \
    portDISABLE_INTERRUPTS();      \
    usCriticalNesting++;           \
  } while (0)

#define portEXIT_CRITICAL()        \
  do {                             \
    if(--usCriticalNesting == 0) { \
      portENABLE_INTERRUPTS();     \
    }                              \
  } while (0)


void vPortYield();
void vPortTickISR();

#define portYIELD()                 vPortYield();

/* Task function macros as described on the FreeRTOS.org WEB site. */
#define portTASK_FUNCTION_PROTO( vFunction, pvParameters ) \
    void vFunction( void * pvParameters ) __attribute__((noreturn))
#define portTASK_FUNCTION( vFunction, pvParameters ) \
    void vFunction( void * pvParameters )

#endif

