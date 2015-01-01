// includes
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


#include "imu.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/

u8 payload[];

/**
  * @brief  Main program.
  * @param  None
  * @retval None
  */
int main(void)
{
  /*!< At this stage the microcontroller clock setting is already configured, 
       this is done through SystemInit() function which is called from startup
       file (startup_stm32f0xx.s) before to branch to application main.
       To reconfigure the default setting of SystemInit() function, refer to
       system_stm32f0xx.c file
     */

  systemInit();
  gpio_init();
  timers_init();

  spi_init();
  rx_init();
  rx_bind();
  rx_search_channel();

  i2cInit();
  delay(100);
  imu_init();
  controllerInit();

/* what's missing here?
  FIXME FIRST: Battery level detection!!!

  FIXME: acc+gyro calibration
  FIXME: autolevel code missing

*/


  while (1) {
    rx_read_payload();
    flightControl();
  }

	return 0;
}

