#include "common.h"
#include "stm32f0xx.h"
#include "misc.h"
#include "led.h"
#include "nf_gpio.h"
#include "pid.h"

// cleanflight drivers
#include "system.h"
#include "accgyro.h"
#include "accgyro_mpu6050.h"
#include "bus_i2c.h"
#include "flight.h"
#include "imu.h"

#define FC_FREQ_HZ 500

// taken from crazyflie
#define TRUNCATE_SINT16(out, in) (out = (in<INT16_MIN)?INT16_MIN:((in>INT16_MAX)?INT16_MAX:in) )
#define SATURATE_SINT16(in) ( (in<INT16_MIN)?INT16_MIN:((in>INT16_MAX)?INT16_MAX:in) )


// Frac 16.16
#define FRAC_MANTISSA 16
#define FRAC_SCALE (1 << FRAC_MANTISSA)

#define CHAN_MULTIPLIER 100
#define CHAN_MAX_VALUE (100 * CHAN_MULTIPLIER)
#define CHAN_MIN_VALUE (-100 * CHAN_MULTIPLIER)


static u32 elapsed_micros;

static Axis3f gyro_data; // Gyro axis data in deg/s

static float rollRateDesired;
static float pitchRateDesired;
static float yawRateDesired;

static PidObject pidRollRate;
static PidObject pidPitchRate;
static PidObject pidYawRate;

static s16 rollOutput;
static s16 pitchOutput;
static s16 yawOutput;

static u16 actuatorThrust, m0_val, m1_val, m2_val, m3_val; //  yaw, pitch, roll,
//volatile static u8 yaw_trim, pitch_trim, roll_trim;

// private functions
static uint16_t limitServo(s16 value);
static uint16_t limitThrust(int32_t value);
static void distributePower(const uint16_t thrust, const int16_t roll,
                            const int16_t pitch, const int16_t yaw);


u32 map(u32 x, u32 in_min, u32 in_max, u32 out_min, u32 out_max)
{
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}


void controllerInit(void)
{

  pidInit(&pidRollRate, 0, PID_ROLL_RATE_KP, PID_ROLL_RATE_KI, PID_ROLL_RATE_KD, IMU_UPDATE_DT);
  pidInit(&pidPitchRate, 0, PID_PITCH_RATE_KP, PID_PITCH_RATE_KI, PID_PITCH_RATE_KD, IMU_UPDATE_DT);
  pidInit(&pidYawRate, 0, PID_YAW_RATE_KP, PID_YAW_RATE_KI, PID_YAW_RATE_KD, IMU_UPDATE_DT);
  pidSetIntegralLimit(&pidRollRate, PID_ROLL_RATE_INTEGRATION_LIMIT);
  pidSetIntegralLimit(&pidPitchRate, PID_PITCH_RATE_INTEGRATION_LIMIT);
  pidSetIntegralLimit(&pidYawRate, PID_YAW_RATE_INTEGRATION_LIMIT);

}



void controllerCorrectRatePID(
       float rollRateActual, float pitchRateActual, float yawRateActual,
       float rollRateDesired, float pitchRateDesired, float yawRateDesired)
{
  pidSetDesired(&pidRollRate, rollRateDesired);
  TRUNCATE_SINT16(rollOutput, pidUpdate(&pidRollRate, rollRateActual, TRUE));

  pidSetDesired(&pidPitchRate, pitchRateDesired);
  TRUNCATE_SINT16(pitchOutput, pidUpdate(&pidPitchRate, pitchRateActual, TRUE));

  pidSetDesired(&pidYawRate, yawRateDesired);
  TRUNCATE_SINT16(yawOutput, pidUpdate(&pidYawRate, yawRateActual, TRUE));
}


// Convert fractional 16.16 to float32
static void frac2float(s32 n, float* res)
{
    if (n == 0) {
        *res = 0.0;
        return;
    }
    u32 m = n < 0 ? -n : n;
    int i;
    for (i = (31-FRAC_MANTISSA); (m & 0x80000000) == 0; i--, m <<= 1) ;
    m <<= 1; // Clear implicit leftmost 1
    m >>= 9;
    u32 e = 127 + i;
    if (n < 0) m |= 0x80000000;
    m |= e << 23;
    *((u32 *) res) = m;
}

void flightControl(void) {



  // this should run at 500Hz
  if ((micros() - elapsed_micros) >= (2000)) {
    elapsed_micros = micros();

    // flicker the leds to measure correct speed
    GPIOA->ODR ^= 1<<15;
    GPIOB->ODR ^= 1<<2;

    // read gyro
    gyro_read(&gyro_data);

    // controls
/*
    throttle   = payload[0] * 0xff;
    yaw        = payload[1] * 0xff;
    pitch      = payload[3] * 0xff;
    roll       = payload[4] * 0xff;

    // trims
    yaw_trim   = payload[2];
    pitch_trim = payload[5];
    roll_trim  = payload[6];
*/


    // convert channels for crazyflie PID:
#if 0
    rollRateDesired = (float) (payload[4] - 128.0) * 256;
    pitchRateDesired = (float) (payload[3] - 128.0) * 256;
    yawRateDesired = (float) (payload[1] - 128.0) * 256;

#else
    actuatorThrust = payload[0] * 256;
    // Roll, aka aileron, float +- 50.0 in degrees
    s32 f_roll = (payload[4] - 0x80) * 0x50 * FRAC_SCALE / (10000 / 400);
    frac2float(f_roll, &rollRateDesired);

    // Pitch, aka elevator, float +- 50.0 degrees
    s32 f_pitch = (payload[3] - 0x80) * 0x50 * FRAC_SCALE / (10000 / 400);
    frac2float(f_pitch, &pitchRateDesired);

    // Thrust, aka throttle 0..65535, working range 5535..65535
    // No space for overshoot here, hard limit Channel3 by -10000..10000
/*
    s32 ch = payload[0] * 0xff;
    if (ch < CHAN_MIN_VALUE) {
        ch = CHAN_MIN_VALUE;
    } else if (ch > CHAN_MAX_VALUE) {
        ch = CHAN_MAX_VALUE;
    }
    actuatorThrust  = ch*3L + 35535L;
*/

    // Yaw, aka rudder, float +- 400.0 deg/s
    s32 f_yaw = (payload[1] - 0x80) * 0x50 * FRAC_SCALE / (10000 / 400);
    frac2float(f_yaw, &yawRateDesired);
#endif

    // gyro.* == *rateActual == Data measured by IMU
    // *rateDesired == Data from RX
    controllerCorrectRatePID(-gyro_data.x, gyro_data.y, -gyro_data.z, rollRateDesired, pitchRateDesired, yawRateDesired);

//#define TUNE_ROLL

    if (actuatorThrust > 0)
    {
#if defined(TUNE_ROLL)
      distributePower(actuatorThrust, rollOutput, 0, 0);
#elif defined(TUNE_PITCH)
      distributePower(actuatorThrust, 0, pitchOutput, 0);
#elif defined(TUNE_YAW)
      distributePower(actuatorThrust, 0, 0, -yawOutput);
#else
      distributePower(actuatorThrust, rollOutput, pitchOutput, -yawOutput);
#endif
    }
    else
    {
      distributePower(0, 0, 0, 0);
      pidReset(&pidRollRate);
      pidReset(&pidPitchRate);
      pidReset(&pidYawRate);
    }


  }
#if 0
    m0_val = actuatorThrust;
    m1_val = actuatorThrust;
    m2_val = actuatorThrust;
    m3_val = ((yawOutput * 1000) / 0xffff) + 1000;
    TIM2->CCR4  = m0_val; // Motor "B"
    TIM1->CCR1  = m1_val; // Motor "L"
    TIM1->CCR4  = m2_val; // Motor "R"
    TIM16->CCR1 = m3_val; // Motor "F"
#endif
}

static void distributePower(const uint16_t thrust, const int16_t roll,
                            const int16_t pitch, const int16_t yaw)
{
#if defined QUAD_X
  roll = roll >> 1;
  pitch = pitch >> 1;
  m0_val = limitThrust(thrust - roll + pitch + yaw);
  m1_val = limitThrust(thrust - roll - pitch - yaw);
  m2_val =  limitThrust(thrust + roll - pitch + yaw);
  m3_val =  limitThrust(thrust + roll + pitch - yaw);
#elif defined QUAD_PLUS
  m0_val = limitThrust(thrust + pitch + yaw);
  m1_val = limitThrust(thrust - roll - yaw);
  m2_val =  limitThrust(thrust - pitch + yaw);
  m3_val =  limitThrust(thrust + roll - yaw);
#elif defined broken_QUAD_X
  m0_val = thrust + roll + pitch + yaw;
  m1_val = thrust - roll + pitch - yaw;
  m2_val = thrust + roll - pitch - yaw;
  m3_val = thrust - roll - pitch + yaw;
#elif defined broken_QUAD_PLUS
  m0_val = thrust + pitch + yaw;
  m1_val = thrust - roll - yaw;
  m2_val = thrust + roll - yaw;
  m3_val = thrust - pitch + yaw;
#elif defined QUAD_TEST
  m0_val = thrust;
  m1_val = thrust;
  m2_val = thrust;
  m3_val = thrust;
#elif defined TRI
  m0_val = limitThrust(thrust + pitch * 1.333333f)        / 200;  // Rear
  m1_val = limitThrust(thrust - roll - pitch * 0.666667f) / 200; // Front right
  m2_val = limitThrust(thrust + roll - pitch * 0.666667f) / 200; // Front left
  m3_val = limitServo(map(yaw, -20000, 20000, 1000, 2000));      // Servo
#elif defined TRI_TEST
  m0_val = thrust;
  m1_val = thrust;
  m2_val = thrust;
  m3_val = map(yaw, -32768, 32767, 1000, 2000);
#endif


  // write new values to CC registers
  TIM2->CCR4  = m0_val; // Motor "B"
  TIM1->CCR1  = m1_val; // Motor "L"
  TIM1->CCR4  = m2_val; // Motor "R"
  TIM16->CCR1 = m3_val; // Motor "F" or Servo
}

static uint16_t limitServo(s16 value)
{
  if(value > 2000)
  {
    value = 2000;
  }
  else if(value < 1000)
  {
    value = 1000;
  }

  return (uint16_t)value;
}

static uint16_t limitThrust(int32_t value)
{
  if(value > UINT16_MAX)
  {
    value = UINT16_MAX;
  }
  else if(value < 0)
  {
    value = 0;
  }

  return (uint16_t)value;
}



