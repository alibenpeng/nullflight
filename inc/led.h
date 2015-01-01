#ifndef _LED_H
#define _LED_H

#define LED0_ON gpio_set(GPIOA, 15)
#define LED0_OFF gpio_clear(GPIOA, 15)
#define LED1_ON gpio_set(GPIOB, 2)
#define LED1_OFF gpio_clear(GPIOB, 2)

void led_blink_lr (int d);
void led_blink_all (int d);
#endif
