#include "Hardware/Gpio.h"

GPIO_TypeDef GPIOA;
GPIO_TypeDef GPIOB;
GPIO_TypeDef GPIOC;

void GpioInterface_Init(g_TGpioInterfaceType *i_gpioInterfaceHandle,
                        g_TGpioInterfaceInit *i_initHandle) {}
void GpioInterface_Deinit(g_TGpioInterfaceType *i_gpioInterfaceHandle,
                          uint32_t i_gpioPin) {}
g_TGpioInterfacePinState
GpioInterface_ReadPin(g_TGpioInterfaceType *i_gpioInterfaceHandle,
                      uint16_t i_gpioPin) {}
void GpioInterface_WritePin(g_TGpioInterfaceType *i_gpioInterfaceHandle,
                            uint16_t i_gpioPin,
                            g_TGpioInterfacePinState i_gpioPinState) {}
void GpioInterface_TogglePin(g_TGpioInterfaceType *i_gpioInterfaceHandle,
                             uint16_t GPIOi_gpioPin_Pin) {}
g_TGpioInterfaceStatus
GpioInterface_LockPin(g_TGpioInterfaceType *i_gpioInterfaceHandle,
                      uint16_t i_gpioPin);
void GpioInterface_RegisterExtiCallback(
    g_TGpioInterfaceExtiCallback i_callback) {}
