#ifndef _IMU_H
#define _IMU_H

#define GYRO_BIAS_X 40 // Roll (positive value rolls right)
#define GYRO_BIAS_Y 10 // Pitch (positive value pitches forward)
#define GYRO_BIAS_Z 0 // Yaw

#define IMU_UPDATE_FREQ   500
#define IMU_UPDATE_DT     (float)(1.0/IMU_UPDATE_FREQ)


#define MPU6050_DEG_PER_LSB_250  (float)((2 * 250.0) / 65536.0)
#define MPU6050_DEG_PER_LSB_500  (float)((2 * 500.0) / 65536.0)
#define MPU6050_DEG_PER_LSB_1000 (float)((2 * 1000.0) / 65536.0)
#define MPU6050_DEG_PER_LSB_2000 (float)((2 * 2000.0) / 65536.0)

#define IMU_DEG_PER_LSB_CFG   MPU6050_DEG_PER_LSB_2000

extern gyro_t gyro;

void imu_init(void);
void gyro_read(Axis3f *gyro_data);
#endif
