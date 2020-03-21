#ifndef _SOFT_SPI_H
#define _SOFT_SPI_H

#include "sys.h"

extern void SOFTSPI_Init(void);
extern uint8_t SOFTSPI_ReadWriteByte(uint8_t TxData);

#endif
