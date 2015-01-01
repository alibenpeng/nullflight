
#include "common.h"
#include "stm32f0xx.h"

void spi_init(void) {

  /* What it should be:

    MSB first - default
    8 Bits packet length - cr2 ds bits 0-2
    CPOL=0 (clock low when inactive) - default
    CPHA=0 (data valid on leading edge) - default
    Enable is active low - default

  */

  // enable the clock for the spi peripheral
  RCC->APB2ENR |= RCC_APB2ENR_SPI1EN;

  // reset both config registers
  SPI1->CR1 = 0x0000;
  SPI1->CR2 = 0x0000;

  SPI1->CR1 = SPI_CR1_MSTR
            | SPI_CR1_BR_2;

  SPI1->CR2 = SPI_CR2_SSOE
            | SPI_CR2_FRXTH
            | SPI_CR2_DS_2
            | SPI_CR2_DS_1
            | SPI_CR2_DS_0;
  
/*
*/

  // SPI enable
  SPI1->CR1 |= SPI_CR1_SPE;
}

/*---------------------------------------------------------------------------*/
/** @brief SPI Data Write.

Data is written to the SPI interface.

@param[in] spi Unsigned int32. SPI peripheral identifier @ref spi_reg_base.
@param[in] data Unsigned int16. 8 or 16 bit data to be written.
*/

void spi_write(u8 data)
{
  uint32_t spixbase = 0x00;

  spixbase = (uint32_t)SPI1; 
  spixbase += 0x0C;
  
  *(__IO uint8_t *) spixbase = data;
}

/*---------------------------------------------------------------------------*/
/** @brief SPI Data Read.

Data is read from the SPI interface after the incoming transfer has finished.

@param[in] spi Unsigned int32. SPI peripheral identifier @ref spi_reg_base.
@returns data Unsigned int16. 8 or 16 bit data.
*/

u8 spi_read(void)
{
  uint32_t spixbase = 0x00;
  
  spixbase = (uint32_t)SPI1; 
  spixbase += 0x0C;
  
	//while (!(SPI1->SR & SPI_SR_RXNE));

  return *(__IO uint8_t *) spixbase;
}

/*---------------------------------------------------------------------------*/
/** @brief SPI Data Write and Read Exchange.

Data is written to the SPI interface, then a read is done after the incoming
transfer has finished.

@param[in] spi Unsigned int32. SPI peripheral identifier @ref spi_reg_base.
@param[in] data Unsigned int16. 8 or 16 bit data to be written.
@returns data Unsigned int16. 8 or 16 bit data.
*/

u8 spi_xfer(u8 data)
{
  spi_write(data);

  /* Wait for transfer finished. */
  while (!(SPI1->SR & SPI_SR_RXNE));

  return spi_read();
}

