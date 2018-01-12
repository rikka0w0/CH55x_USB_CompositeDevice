#ifndef __I2C_EEPROM_H
#define __I2C_EEPROM_H

#include "ch554_platform.h"

#define EEPROM_SIZE 65536			//Capacity 64KiB
#define EEPROM_SECTORSIZE 512	//Sector Size 512B

void EEPROM_Read(uint8_t* buf, uint8_t AddrH, uint8_t AddrL, uint8_t length);
void EEPROM_Write(uint8_t* buf, uint8_t AddrH, uint8_t AddrL, uint8_t length);

#endif