#include "ch554.h"
#include "i2c.h"

xdata uint8_t I2C_Buf;

sbit sda_pin = P1^5;
sbit scl_pin = P1^4;

void I2C_DELAY(void) {
	uint8_t i;
	for (i=0; i<7; i++);
}

void I2C_Init(void) {
	I2C_SDA_H();
	I2C_DELAY();
	I2C_SCL_H();
	I2C_DELAY();
}

void I2C_Send_Start(void) {
	// I2C_SCL = H, I2C_SDA = H
	I2C_SDA_H();
	I2C_DELAY();
	I2C_SCL_H();
	I2C_DELAY();
	I2C_SDA_L();
	I2C_DELAY();
	I2C_SCL_L();
	I2C_DELAY();	
	// I2C_SCL = L, I2C_SDA = L
}

void I2C_Send_Stop(void) {
	// I2C_SCL = L, I2C_SDA = H
	I2C_SDA_L();
	I2C_DELAY();
	I2C_SCL_H();
	I2C_DELAY();
	I2C_SDA_H(); //Master release the bus
	I2C_DELAY();
}

void I2C_Send_ACK(void) {
	// I2C_SCL = L	
	I2C_SDA_L();
	I2C_DELAY();
	I2C_SCL_H();
	I2C_DELAY();
	I2C_SCL_L();
	I2C_DELAY();	
}

void I2C_Send_NACK(void) {
	// I2C_SCL = L
	I2C_SDA_H();
	I2C_DELAY();
	I2C_SCL_H();
	I2C_DELAY();
	I2C_SCL_L();
	I2C_DELAY();	
}

void I2C_WriteByte(void) {
	// I2C_SCL = L
	if (I2C_Buf & 0x80) {I2C_SDA_H();} else {I2C_SDA_L();}
	I2C_DELAY();
	I2C_SCL_H();
	I2C_DELAY();
	I2C_SCL_L();
	I2C_DELAY();
	
	if (I2C_Buf & 0x40) {I2C_SDA_H();} else {I2C_SDA_L();}
	I2C_DELAY();
	I2C_SCL_H();
	I2C_DELAY();
	I2C_SCL_L();
	I2C_DELAY();
	
	if (I2C_Buf & 0x20) {I2C_SDA_H();} else {I2C_SDA_L();}
	I2C_DELAY();
	I2C_SCL_H();
	I2C_DELAY();
	I2C_SCL_L();
	I2C_DELAY();
	
	if (I2C_Buf & 0x10) {I2C_SDA_H();} else {I2C_SDA_L();}
	I2C_DELAY();
	I2C_SCL_H();
	I2C_DELAY();
	I2C_SCL_L();
	I2C_DELAY();
	
	if (I2C_Buf & 0x08) {I2C_SDA_H();} else {I2C_SDA_L();}
	I2C_DELAY();
	I2C_SCL_H();
	I2C_DELAY();
	I2C_SCL_L();
	I2C_DELAY();

	if (I2C_Buf & 0x04) {I2C_SDA_H();} else {I2C_SDA_L();}
	I2C_DELAY();
	I2C_SCL_H();
	I2C_DELAY();
	I2C_SCL_L();
	I2C_DELAY();

	if (I2C_Buf & 0x02) {I2C_SDA_H();} else {I2C_SDA_L();}
	I2C_DELAY();
	I2C_SCL_H();
	I2C_DELAY();
	I2C_SCL_L();
	I2C_DELAY();

	if (I2C_Buf & 0x01) {I2C_SDA_H();} else {I2C_SDA_L();}
	I2C_DELAY();
	I2C_SCL_H();
	I2C_DELAY();
	I2C_SCL_L();
	I2C_DELAY();
	
	I2C_SDA_H();
	I2C_DELAY();
}

void I2C_ReadByte(void) {
	I2C_Buf = 0;
	
	// I2C_SCL = L
	I2C_SDA_H(); //Master release the bus
	I2C_DELAY();	

	I2C_SCL_H();
	I2C_DELAY();
	if (I2C_SDA())
		I2C_Buf |= 0x80;
	I2C_SCL_L();
	I2C_DELAY();
	

	I2C_SCL_H();
	I2C_DELAY();
	if (I2C_SDA())
		I2C_Buf |= 0x40;
	I2C_SCL_L();
	I2C_DELAY();

	I2C_SCL_H();
	I2C_DELAY();
	if (I2C_SDA())
		I2C_Buf |= 0x20;
	I2C_SCL_L();
	I2C_DELAY();

	I2C_SCL_H();
	I2C_DELAY();
	if (I2C_SDA())
		I2C_Buf |= 0x10;
	I2C_SCL_L();
	I2C_DELAY();

	I2C_SCL_H();
	I2C_DELAY();
	if (I2C_SDA())
		I2C_Buf |= 0x08;
	I2C_SCL_L();
	I2C_DELAY();

	I2C_SCL_H();
	I2C_DELAY();
	if (I2C_SDA())
		I2C_Buf |= 0x04;
	I2C_SCL_L();
	I2C_DELAY();

	I2C_SCL_H();
	I2C_DELAY();
	if (I2C_SDA())
		I2C_Buf |= 0x02;
	I2C_SCL_L();
	I2C_DELAY();

	I2C_SCL_H();
	I2C_DELAY();
	if (I2C_SDA())
		I2C_Buf |= 0x01;
	I2C_SCL_L();
	I2C_DELAY();
}

void I2C_WaitForACK(void){
	// I2C_SCL = L
	I2C_SCL_H();
	I2C_DELAY();
	while (I2C_SDA())
		I2C_DELAY();
	I2C_SCL_L();
	I2C_DELAY();
}