#ifndef MISC_H
#define MISC_H

// types and variables
typedef struct
{
  uint32_t SYSCLK_Frequency;
  uint32_t HCLK_Frequency;
  uint32_t PCLK_Frequency;
  uint32_t ADCCLK_Frequency;
  uint32_t CECCLK_Frequency;
  uint32_t I2C1CLK_Frequency;
  uint32_t USART1CLK_Frequency;
  uint32_t USART2CLK_Frequency; /*!< Only applicable for STM32F072 and STM32F091 devices */
  uint32_t USART3CLK_Frequency; /*!< Only applicable for STM32F091 devices */
  uint32_t USBCLK_Frequency;    /*!< Only applicable for STM32F072 devices */
}RCC_ClocksTypeDef;

// functions and prototypes
//void delay (int a);
void RCC_GetClocksFreq(RCC_ClocksTypeDef* RCC_Clocks);

#endif
