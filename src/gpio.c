#include "stm32f0xx.h"
#include "common.h"
#include "nf_gpio.h"
/*
void GPIO_PinAFConfig(GPIO_TypeDef* GPIOx, uint16_t GPIO_PinSource, uint8_t GPIO_AF)
{
  uint32_t temp = 0x00;
  uint32_t temp_2 = 0x00;


  temp = ((uint32_t)(GPIO_AF) << ((uint32_t)((uint32_t)GPIO_PinSource & (uint32_t)0x07) * 4));
  GPIOx->AFR[GPIO_PinSource >> 0x03] &= ~((uint32_t)0xF << ((uint32_t)((uint32_t)GPIO_PinSource & (uint32_t)0x07) * 4));
  temp_2 = GPIOx->AFR[GPIO_PinSource >> 0x03] | temp;
  GPIOx->AFR[GPIO_PinSource >> 0x03] = temp_2;
}
*/

void gpio_init(void) {

  /*

  PORT A
  ------

    PA0  - unused
    PA1  - unused
    PA2  - unused
    PA3  - TIM2_CH4  - Motor NW (marked "B")
    PA4  - SPI1_NSS  - BK2423
    PA5  - SPI1_SCK  - BK2423
    PA6  - SPI1_MISO - BK2423
    PA7  - SPI1_MOSI - BK2423
    PA8  - TIM1_CH1  - Motor SW (marked "L")
    PA9  - unused
    PA10 - unused
    PA11 - TIM1_CH4  - Motor SE (marked "R")
    PA12 - unused
    PA13 - SWDAT
    PA14 - SWCLK
    PA15 - LEDs E
    
  PORT B
  ------

    PB0  - unused
    PB1  - unused
    PB2  - LEDs W
    PB3  - unused
    PB4  - unused
    PB5  - unused
    PB6  - I2C1_SCL  - MPU6050
    PB7  - I2C1_SDA  - MPU6050
    PB8  - TIM16_CH1 - Motor NE (marked "F")

  */

  // GPIOA and GPIOB Periph clock enable
	RCC->AHBENR |= RCC_AHBENR_GPIOAEN; 
	RCC->AHBENR |= RCC_AHBENR_GPIOBEN; 

	// Ensure maximum speed setting (even though it is unnecessary)
	GPIOA->OSPEEDR = 0xffffffff;
	GPIOB->OSPEEDR = 0xffffffff;
 
  // Ensure all pull up pull down resistors are disabled
	GPIOA->PUPDR = 0;
	GPIOB->PUPDR = 0;

	GPIOA->OTYPER = 0; // output type = push pull
	GPIOB->OTYPER = 0; // output type = push pull

  // Pin Mode Register settings
	GPIOA->MODER |= (GPIO_MODER_MODER3_1) ; // AF - Timer
	GPIOA->MODER |= (GPIO_MODER_MODER4_0) ; // GP Output - NSS
	GPIOA->MODER |= (GPIO_MODER_MODER5_1) ; // AF - SCK
	GPIOA->MODER |= (GPIO_MODER_MODER6_1) ; // AF - MISO
	GPIOA->MODER |= (GPIO_MODER_MODER7_1) ; // AF - MOSI
	GPIOA->MODER |= (GPIO_MODER_MODER8_1) ; // AF
	GPIOA->MODER |= (GPIO_MODER_MODER11_1) ; // AF
	GPIOA->MODER |= (GPIO_MODER_MODER15_0) ; // GP Output

	GPIOB->MODER |= (GPIO_MODER_MODER2_0) ; // GP Ooutput
	GPIOB->MODER |= (GPIO_MODER_MODER6_1) ; // AF
	GPIOB->MODER |= (GPIO_MODER_MODER7_1) ; // AF
	GPIOB->MODER |= (GPIO_MODER_MODER8_1) ; // AF

  // enable pulldowns for motor pins
  GPIOA->PUPDR |= (2<<3);
  GPIOA->PUPDR |= (2<<8);
  GPIOA->PUPDR |= (2<<11);
  GPIOB->PUPDR |= (2<<8);

  // setup gpio alternate functions

  // SPI
  gpio_set(GPIOA, 4);
  GPIO_PinAFConfig(GPIOA, 5, 0);
  GPIO_PinAFConfig(GPIOA, 6, 0);
  GPIO_PinAFConfig(GPIOA, 7, 0);
  
  // I2C
  GPIO_PinAFConfig(GPIOB, 6, 1);
  GPIO_PinAFConfig(GPIOB, 7, 1);

  // Motor Timers
  GPIO_PinAFConfig(GPIOA, 3, 2);
  GPIO_PinAFConfig(GPIOA, 8, 2);
  GPIO_PinAFConfig(GPIOA, 11, 2);
  GPIO_PinAFConfig(GPIOB, 8, 2);

  /* (1) Timing register value is computed with the AN4235 xls file,
  fast Mode @400kHz with I2CCLK = 48MHz, rise time = 140ns,
  fall time = 40ns */
  /* (2) Periph enable, receive interrupt enable */
  /* (3) Slave address = 0x5A, read transfer, 1 byte to receive, autoend */
  //I2C2->TIMINGR = ( uint32_t)0x00B01A4B; /* (1) */
  //I2C2->CR1 = I2C_CR1_PE | I2C_CR1_RXIE; /* (2) */
  //I2C2->CR2 = I2C_CR2_AUTOEND | (1<<16) | I2C_CR2_RD_WRN | (I2C1_OWN_ADDRESS << 1); /* (3) */
} 


void gpio_set(GPIO_TypeDef* port, u8 pin) {
  port->BSRR = (1<<pin);
}

void gpio_clear(GPIO_TypeDef* port, u8 pin) {
  port->BRR = (1<<pin);
}

void gpio_toggle(GPIO_TypeDef* port, u8 pin) {
  port->ODR ^= (1<<pin);
}

