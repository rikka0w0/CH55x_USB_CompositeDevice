#ifndef __I2C_H
#define __I2C_H

#include "types.h"


#define I2C_SDA()	(sda_pin==1)
#define I2C_SDA_H()	{sda_pin = 1;}
#define I2C_SDA_L()	{sda_pin = 0;}
#define I2C_SCL_H()	{scl_pin = 1;}
#define I2C_SCL_L()	{scl_pin = 0;}
//#define I2C_DELAY() {}

void I2C_DELAY(void);
void I2C_Init(void);
void I2C_Send_Start(void);
void I2C_Send_Stop(void);
void I2C_Send_ACK(void);
void I2C_Send_NACK(void);
void I2C_WriteByte();
void I2C_ReadByte(void);
void I2C_CheckACK(void);

#endif