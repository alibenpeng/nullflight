#ifndef _GPIO_H_
#define _GPIO_H_

#include "stm32f0xx.h"
#include "stm32f0xx_gpio.h"

#define MMIO32(addr)                (*(volatile uint32_t *)(addr))
#define GPIO_BSRR(port)      MMIO32(port + 0x18)
#define GPIO_BRR(port)      MMIO32(port + 0x28)


void gpio_init(void);
void gpio_set(GPIO_TypeDef* port, u8 pin);
void gpio_clear(GPIO_TypeDef* port, u8 pin);
#endif

