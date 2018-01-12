#ifndef __I2C_H
#define __I2C_H

#include "ch554_platform.h"

extern xdata uint8_t I2C_Buf;

void I2C_Init(void);

// Generate a I2C start sequence
void I2C_Send_Start(void);
// Write a byte (stored in I2C_Buf) to I2C bus and return the ACK status in I2C_Buf
// xxxx xxx(ACK bit)
// ACK bit = 0 -> ACK received
// ACK bit = 1 -> NACK received
void I2C_WriteByte();

// Read a byte from I2C bus and store the result into I2C_Buf
void I2C_ReadByte(void);
// Send a ACK to I2C bus
void I2C_Send_ACK(void);
// Send a NACK to I2C bus
void I2C_Send_NACK(void);

// Generate a I2C stop condition, release the bus
void I2C_Send_Stop(void);

#endif