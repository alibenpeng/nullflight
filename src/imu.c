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

// cleanflight drivers
#include "system.h"
#include "accgyro.h"
#include "accgyro_mpu6050.h"
#include "bus_i2c.h"
#include "flight.h"
#include "imu.h"

gyro_t gyro;
acc_t acc;
u16 gyroLpf;

s16 gyroData[3];
s16 accData[3];

void imu_init(void)
{
  if (mpu6050GyroDetect((mpu6050Config_t *) NULL, &gyro, gyroLpf)) {
    gyro.init();
    gyro.read(gyroData);
  }

  if (mpu6050AccDetect((mpu6050Config_t *) NULL, &acc)) {
    acc.init();
    acc.read(accData);
  }
}

void gyro_read(Axis3f *gyro_out)
{
  gyro.read(gyroData);

  gyro_out->x = (gyroData[0] - GYRO_BIAS_X) * IMU_DEG_PER_LSB_CFG;
  gyro_out->y = (gyroData[1] - GYRO_BIAS_Y) * IMU_DEG_PER_LSB_CFG;
  gyro_out->z = (gyroData[2] - GYRO_BIAS_Z) * IMU_DEG_PER_LSB_CFG;
  
}
