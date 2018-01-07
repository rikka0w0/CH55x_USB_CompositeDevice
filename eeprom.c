#include "types.h"

//#define EEPROM_ADDR 0xA0

//sbit led2=P1^0;

//xdata uint8_t buff[5];
//code uint8_t check[] = {0x3c, 0x90, 0x6d, 0x6b, 0x66};
//code uint8_t check2[] = {0x40,0,0,0,0};

//void EEPROM_Read(uint8_t* buf, uint8_t AddrH, uint8_t AddrL, uint8_t length) {
//	I2C_Send_Start();
//	I2C_WriteByte(EEPROM_ADDR);
//	I2C_WaitForACK();
//	
//	I2C_WriteByte(AddrH);	// Address H
//	I2C_WaitForACK();
//	
//	I2C_WriteByte(AddrL);	// Address L
//	I2C_WaitForACK();
//	
//	I2C_Send_Start();
//	I2C_WriteByte(EEPROM_ADDR|1); //Read
//	I2C_WaitForACK();
//	
//	AddrH = length-1;			// Length
//	for (AddrL=0; AddrL<AddrH; AddrL++) {
//		buf[AddrL] = I2C_ReadByte();
//		I2C_Send_ACK();
//	}
//	buf[AddrH] = I2C_ReadByte();
//	I2C_Send_NACK();

//	I2C_Send_Stop();	
//}

void EEPROM_Init(void) {
//	unsigned char i;
//	I2C_Init();
//	for (i=0; i<100; i++);
//	EEPROM_Read(buff, 0, 1, 5);
//	for (i=0; i<5; i++)
//		if (buff[i] != check[i])
//			led2=0;
//	EEPROM_Read(buff, 0, 0x1A, 5);
//	for (i=0; i<5; i++)
//		if (buff[i] != check2[i])
//			led2=0;
}