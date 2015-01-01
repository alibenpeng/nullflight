/*
 * This file is part of Cleanflight.
 *
 * Cleanflight is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Cleanflight is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Cleanflight.  If not, see <http://www.gnu.org/licenses/>.
 */

#pragma once

#define STM32F013

#define TARGET_BOARD_IDENTIFIER "CX10" // STM Discovery F3

#define FLASH_PAGE_COUNT 16
#define FLASH_PAGE_SIZE ((uint16_t)0x400)

#define LED0_GPIO   GPIOA
#define LED0_PIN    Pin_15 // Blue and red LEDs right
#define LED0_PERIPHERAL RCC_AHBPeriph_GPIOA
#define LED1_GPIO   GPIOB
#define LED1_PIN    Pin_2 // Blue and red LEDs left
#define LED1_PERIPHERAL RCC_AHBPeriph_GPIOB

#define GYRO
#define USE_GYRO_MPU6050

#define ACC
#define USE_ACC_MPU6050

#define LED0
#define LED1

#define BRUSHED_MOTORS

#define USE_I2C
#define I2C_DEVICE (I2CDEV_1)

#define SENSORS_SET (SENSOR_ACC)

#define SERIAL_PORT_COUNT 0
