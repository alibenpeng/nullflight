#ifndef _SPIPROTO_H_
#define _SPIPROTO_H_

#include "stm32f0xx.h"
#include "nf_gpio.h"
#include "spi.h"

#define PROTOSPI_pin_set(io) gpio_set((GPIO_TypeDef*)io.port,io.pin)
#define PROTOSPI_pin_clear(io) gpio_clear((GPIO_TypeDef*)io.port,io.pin)
#define PROTOSPI_xfer(byte) spi_xfer(byte)
#define _NOP()  asm volatile ("nop")

#endif // _SPIPROTO_H_
