#ifndef _SPI_H
#define _SPI_H

#include "common.h"

void spi_init(void);
void spi_write(u16 data);
u16 spi_read(void);
u16 spi_xfer(u16 data);
#endif
