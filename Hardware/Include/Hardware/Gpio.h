#pragma once

#include <stdint.h>

typedef struct {
  uint32_t MODER;
  uint32_t OTYPER;
  uint32_t OSPEEDR;
  uint32_t PUPDR;
  uint32_t IDR;
  uint32_t ODR;
  uint32_t BSRR;
  uint32_t LCK;
  uint32_t AFR[2];
} GPIO_TypeDef;

extern GPIO_TypeDef GPIOA;
extern GPIO_TypeDef GPIOB;
extern GPIO_TypeDef GPIOC;

typedef void (*g_TGpioInterfaceExtiCallback)(uint16_t);
typedef GPIO_TypeDef g_TGpioInterfaceType;
typedef void g_TGpioInterfaceInit;
typedef int g_TGpioInterfacePinState;
typedef int g_TGpioInterfaceStatus;

void GpioInterface_Init(g_TGpioInterfaceType *i_gpioInterfaceHandle,
                        g_TGpioInterfaceInit *i_initHandle);
void GpioInterface_Deinit(g_TGpioInterfaceType *i_gpioInterfaceHandle,
                          uint32_t i_gpioPin);
g_TGpioInterfacePinState
GpioInterface_ReadPin(g_TGpioInterfaceType *i_gpioInterfaceHandle,
                      uint16_t i_gpioPin);
void GpioInterface_WritePin(g_TGpioInterfaceType *i_gpioInterfaceHandle,
                            uint16_t i_gpioPin,
                            g_TGpioInterfacePinState i_gpioPinState);
void GpioInterface_TogglePin(g_TGpioInterfaceType *i_gpioInterfaceHandle,
                             uint16_t GPIOi_gpioPin_Pin);
g_TGpioInterfaceStatus
GpioInterface_LockPin(g_TGpioInterfaceType *i_gpioInterfaceHandle,
                      uint16_t i_gpioPin);
void GpioInterface_RegisterExtiCallback(
    g_TGpioInterfaceExtiCallback i_callback);
