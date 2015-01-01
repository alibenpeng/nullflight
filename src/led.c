#include "stm32f0xx.h"
#include "common.h"
#include "nf_gpio.h"
#include "misc.h"

#if 0
void led_blink_all (int d) {
  // Set PB2
  GPIOB->BSRR = (1<<2);
  // Set PA15
  GPIOA->BSRR = (1<<15);
  delay(d);
  // Reset PB2
  GPIOB->BRR = (1<<2);
  // Reset PA15
  GPIOA->BRR = (1<<15);
  delay(d);
}


void led_blink_lr (int d) {
  // Set PB2
  GPIOB->BSRR = (1<<2);
  // Reset PA15
  GPIOA->BRR = (1<<15);
  delay(d);
  // Reset PB2
  GPIOB->BRR = (1<<2);
  // Set PA15
  GPIOA->BSRR = (1<<15);
  delay(d);
}


void led_blink_lr (int d) {
  // Set PB2
  gpio_set(GPIOB, 2);
  // Reset PA15
  gpio_clear(GPIOA, 15);
  delay(d);
  // Reset PB2
  gpio_clear(GPIOB, 2);
  // Set PA15
  gpio_set(GPIOA, 15);
  delay(d);
}

#else
#endif
