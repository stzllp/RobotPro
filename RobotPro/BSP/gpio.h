#ifndef __GPIO_H__
#define __GPIO_H__

#include "stm32f2xx.h"

void GPIO_PpUpOut(GPIO_TypeDef *gpio, uint32_t gpiopin);
void GPIO_AfPpUpOut(GPIO_TypeDef *gpio, uint32_t gpiopin);
void GPIO_UpIn(GPIO_TypeDef *gpio, uint32_t gpiopin);
void GPIO_OdUpOut(GPIO_TypeDef *gpio, uint32_t gpiopin);

#endif
