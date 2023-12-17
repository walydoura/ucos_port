#ifndef __AT24C02_H
#define __AT24C02_H

#include "includes.h"

#define AT24C02_DEVICE_ADDR   0xA0

uint8_t	AT24C02_ReadBytes(uint8_t addr, uint8_t len, uint8_t* recv_buf);
uint8_t	AT24C02_WriteBytes(uint8_t addr, uint8_t len, uint8_t* tx_buf);

#endif
