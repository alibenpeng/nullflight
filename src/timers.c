#include "common.h"
#include "stm32f0xx.h"
#include "misc.h"
#include "led.h"
#include "nf_gpio.h"
#include "timers.h"
#include "spi.h"
#include "iface_nrf24l01.h"
#include "rx_bk2423.h"
#include "flight.h"

/*
  We need the following timers:
  -----------------------------

  TIM2_CH4  - PA03 - Motor NW ("B") - check.
  TIM1_CH1  - PA08 - Motor SW ("L") - check.
  TIM1_CH4  - PA11 - Motor SE ("R") - check.
  TIM16_CH1 - PB08 - Motor NE ("F") - removed broken transistor, so this output can't drive a motor anymore. Let's use this output for the servo!

*/

// Timer prescaler and period
#define TIM_PSC 6
#define TIM_ARR 327
#define TIM_CCR1 0

// The last timer (TIM16_CH1) is used for the servo on tricopters
#ifdef TRI
#define TIM_S_PSC 48       // scale the timer clock to 1MHz
#define TIM_S_ARR 20000    // 20000 cycles @ 1MHz = 50Hz
#define TIM_S_CCR1 1500    // 1500 us is the neutral position
#elif defined TRI_TEST
#define TIM_S_PSC 48       // scale the timer clock to 1MHz
#define TIM_S_ARR 20000    // 20000 cycles @ 1MHz = 50Hz
#define TIM_S_CCR1 1500    // 1500 us is the neutral position
#else
#define TIM_S_PSC TIM_PSC
#define TIM_S_ARR TIM_ARR
#define TIM_S_CCR1 TIM_CCR1
#endif

void timers_init(void) {
#if 1

  RCC->APB1ENR |= RCC_APB1ENR_TIM2EN;                     // Enable clock for TIM2
  RCC->APB2ENR |= RCC_APB2ENR_TIM1EN;                     // Enable clock for TIM1
  RCC->APB2ENR |= RCC_APB2ENR_TIM16EN;                    // Enable clock for TIM16

  //  Timer 1 config
  TIM1->PSC = TIM_PSC;                                    // Set prescaler
  TIM1->ARR = TIM_ARR;                                    // Set reload value (=TimerPeriod in the stdperiph example)
  TIM1->CCMR1 |= TIM_CCMR1_OC1M_1 | TIM_CCMR1_OC1M_2;     // OC1M = 110 for PWM Mode 1 output on ch1
  TIM1->CCMR2 |= TIM_CCMR2_OC4M_1 | TIM_CCMR2_OC4M_2;     // OC4M = 110 for PWM Mode 1 output on ch4
  TIM1->CCMR1 |= TIM_CCMR1_OC1PE;                         // Output 1 preload enable
  TIM1->CCMR2 |= TIM_CCMR2_OC4PE;                         // Output 4 preload enable
  TIM1->CR1 |= TIM_CR1_ARPE;                              // Auto-reload preload enable
  TIM1->CCER |= TIM_CCER_CC1E;                            // Enable output for ch1
  TIM1->CCER |= TIM_CCER_CC4E;                            // Enable output for ch4
  TIM1->CCR1 = TIM_CCR1;
  TIM1->EGR |= TIM_EGR_UG;                                // Force update
  TIM1->SR &= ~TIM_SR_UIF;                                // Clear the update flag
  TIM1->CR1 |= TIM_CR1_CEN;                               // Enable counter
  TIM1->BDTR |= TIM_BDTR_MOE;                             // Enable main output

  TIM2->PSC = TIM_PSC;                                    // Set prescaler
  TIM2->ARR = TIM_ARR;                                    // Set reload value (=TimerPeriod in the stdperiph example)
  TIM2->CCMR2 |= TIM_CCMR2_OC4M_1 | TIM_CCMR2_OC4M_2;     // OC4M = 110 for PWM Mode 1 output on ch4
  TIM2->CCMR2 |= TIM_CCMR2_OC4PE;                         // Output 4 preload enable
  TIM2->CR1 |= TIM_CR1_ARPE;                              // Auto-reload preload enable
  TIM2->CCER |= TIM_CCER_CC4E;                            // Enable output for ch4
  TIM2->CCR1 = TIM_CCR1;
  TIM2->EGR |= TIM_EGR_UG;                                // Force update
  TIM2->SR &= ~TIM_SR_UIF;                                // Clear the update flag
  TIM2->CR1 |= TIM_CR1_CEN;                               // Enable counter
  TIM2->BDTR |= TIM_BDTR_MOE;                             // Enable main output

  TIM16->PSC = TIM_S_PSC;                                    // Set prescaler
  TIM16->ARR = TIM_S_ARR;                                    // Set reload value (=TimerPeriod in the stdperiph example)
  TIM16->CCMR1 |= TIM_CCMR1_OC1M_1 | TIM_CCMR1_OC1M_2;     // OC1M = 110 for PWM Mode 1 output on ch1
  TIM16->CCMR1 |= TIM_CCMR1_OC1PE;                         // Output 1 preload enable
  TIM16->CR1 |= TIM_CR1_ARPE;                              // Auto-reload preload enable
  TIM16->CCER |= TIM_CCER_CC1E;                            // Enable output for ch1
  TIM16->CCR1 = TIM_S_CCR1;
  TIM16->EGR |= TIM_EGR_UG;                                // Force update
  TIM16->SR &= ~TIM_SR_UIF;                                // Clear the update flag
  TIM16->CR1 |= TIM_CR1_CEN;                               // Enable counter
  TIM16->BDTR |= TIM_BDTR_MOE;                             // Enable main output

#else

  /* Compute the value to be set in ARR regiter to generate signal frequency at 17.57 Khz */
  int TimerPeriod = (SystemCoreClock / 17570 ) - 1;

  /* Compute CCR1 value to generate a duty cycle at 50% for channel 1 and 1N */
  Channel1Pulse = (uint16_t) (((uint32_t) 5 * (TimerPeriod - 1)) / 10);
  /* Compute CCR2 value to generate a duty cycle at 37.5%  for channel 2 and 2N */
  Channel2Pulse = (uint16_t) (((uint32_t) 375 * (TimerPeriod - 1)) / 1000);
  /* Compute CCR3 value to generate a duty cycle at 25%  for channel 3 and 3N */
  Channel3Pulse = (uint16_t) (((uint32_t) 25 * (TimerPeriod - 1)) / 100);
  /* Compute CCR4 value to generate a duty cycle at 12.5%  for channel 4 */
  Channel4Pulse = (uint16_t) (((uint32_t) 125 * (TimerPeriod- 1)) / 1000);

  /* Time Base configuration */
  TIM_TimeBaseStructure.TIM_Prescaler = 0;
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
  TIM_TimeBaseStructure.TIM_Period = TimerPeriod;
  TIM_TimeBaseStructure.TIM_ClockDivision = 0;
  TIM_TimeBaseStructure.TIM_RepetitionCounter = 0;

  TIM_TimeBaseInit(TIM1, &TIM_TimeBaseStructure);

  /* Channel 1, 2,3 and 4 Configuration in PWM mode */
  TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM2;
  TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
  TIM_OCInitStructure.TIM_OutputNState = TIM_OutputNState_Enable;
  TIM_OCInitStructure.TIM_Pulse = Channel1Pulse;
  TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_Low;
  TIM_OCInitStructure.TIM_OCNPolarity = TIM_OCNPolarity_High;
  TIM_OCInitStructure.TIM_OCIdleState = TIM_OCIdleState_Set;
  TIM_OCInitStructure.TIM_OCNIdleState = TIM_OCIdleState_Reset;

  TIM_OC1Init(TIM1, &TIM_OCInitStructure);

  TIM_OCInitStructure.TIM_Pulse = Channel2Pulse;
  TIM_OC2Init(TIM1, &TIM_OCInitStructure);

  TIM_OCInitStructure.TIM_Pulse = Channel3Pulse;
  TIM_OC3Init(TIM1, &TIM_OCInitStructure);

  TIM_OCInitStructure.TIM_Pulse = Channel4Pulse;
  TIM_OC4Init(TIM1, &TIM_OCInitStructure);

  /* TIM1 counter enable */
  TIM_Cmd(TIM1, ENABLE);

  /* TIM1 Main Output Enable */
  TIM_CtrlPWMOutputs(TIM1, ENABLE);
#endif

}
