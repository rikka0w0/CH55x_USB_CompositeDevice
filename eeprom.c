#include "ch554_platform.h"
#include "i2c.h"

// For 24LC512, A0=0, A1=0, A2=0, WP=Floating
#define EEPROM_ADDR 0xA0

void EEPROM_Read(uint8_t* buf, uint8_t AddrH, uint8_t AddrL, uint8_t length) {
	do {
		I2C_Send_Start();	
		I2C_Buf = EEPROM_ADDR;
		I2C_WriteByte();
	} while (I2C_Buf);	// Wait for pending operation to complete
	
	I2C_Buf = AddrH;
	I2C_WriteByte();	// Address H
	
	I2C_Buf = AddrL;
	I2C_WriteByte();	// Address L
	
	I2C_Send_Start();
	I2C_Buf = EEPROM_ADDR|1;	//Read
	I2C_WriteByte();
	
	AddrH = length-1;			// Length
	for (AddrL=0; AddrL<AddrH; AddrL++) {
		I2C_ReadByte();
		buf[AddrL] = I2C_Buf;
		I2C_Send_ACK();
	}
	I2C_ReadByte();
	buf[AddrH] = I2C_Buf;
	I2C_Send_NACK();
	
	I2C_Send_Stop();
}

void EEPROM_Write(uint8_t* buf, uint8_t AddrH, uint8_t AddrL, uint8_t length) {
	do {
		I2C_Send_Start();		
		I2C_Buf = EEPROM_ADDR;
		I2C_WriteByte();
		
	} while (I2C_Buf);	// Wait for pending operation to complete
	
	I2C_Buf = AddrH;
	I2C_WriteByte();	// Address H
	
	I2C_Buf = AddrL;
	I2C_WriteByte();	// Address L
	
	for (AddrL=0; AddrL<length; AddrL++) {
		I2C_Buf = buf[AddrL];
		I2C_WriteByte();	// Data byte
	}
	
	I2C_Send_Stop();
}