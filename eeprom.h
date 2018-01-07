#ifndef __I2C_EEPROM_H
#define __I2C_EEPROM_H

#include "types.h"

void EEPROM_Init(void);
void EEPROM_Read(uint8_t* buf, uint8_t AddrH, uint8_t AddrL, uint8_t length);

#endif