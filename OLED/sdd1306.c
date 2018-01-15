#include "sdd1306.h"

#define	Brightness	0xCF 
#define X_WIDTH 	128
#define Y_WIDTH 	64

void OLED_Fill(uint8_t filldata) {
	uint8_t yChunk, x;
	for(yChunk=0; yChunk<8; yChunk++) {
		OLED_Select_Chunk (0, yChunk);
		
		I2C_Send_Start();
		I2C_Buf = OLED_I2C_ADDR;
		I2C_WriteByte();
		I2C_Buf = 0x40;	//write data
		I2C_WriteByte();
		
#ifdef OLED_FILL_FAST
		for (x=0; x < (uint8_t)128; x++) {
			I2C_Buf = filldata;
			I2C_WriteByte();
		}
#else
		for (x=0; x< 16; x++) {
			I2C_Buf = filldata;
			I2C_WriteByte();
			I2C_Buf = filldata;
			I2C_WriteByte();
			I2C_Buf = filldata;
			I2C_WriteByte();
			I2C_Buf = filldata;
			I2C_WriteByte();
			I2C_Buf = filldata;
			I2C_WriteByte();
			I2C_Buf = filldata;
			I2C_WriteByte();
			I2C_Buf = filldata;
			I2C_WriteByte();
			I2C_Buf = filldata;
			I2C_WriteByte();
		}
#endif
		
		I2C_Send_Stop();
	}
}

void OLED_Init(void) {
	OLED_WrCmd(OLED_INST_DISPLAY_OFF);// --turn off oled panel
	OLED_WrCmd(OLED_INST_SET_LOW_X_ADDR);// ---set low column address
	OLED_WrCmd(OLED_INST_SET_HIGH_X_ADDR);// ---set high column address
	OLED_WrCmd(0x40);// --set start line address  Set Mapping RAM Display Start Line (0x00~0x3F)
	OLED_WrCmd(0x81);// --set contrast control register
	OLED_WrCmd(Brightness); // Set SEG Output Current Brightness
	OLED_WrCmd(0xa1);// --Set SEG/Column Mapping     0xa0 invert left-right 0xa1 normal
	OLED_WrCmd(0xc8);// Set COM/Row Scan Direction   0xc0 invert up-bottom 0xc8 normal
	OLED_WrCmd(0xa6);// --set normal display
	OLED_WrCmd(0xa8);// --set multiplex ratio(1 to 64)
	OLED_WrCmd(0x3f);// --1/64 duty
	OLED_WrCmd(0xd3);// -set display offset	Shift Mapping RAM Counter (0x00~0x3F)
	OLED_WrCmd(0x00);// -not offset
	OLED_WrCmd(0xd5);// --set display clock divide ratio/oscillator frequency
	OLED_WrCmd(0x80);// --set divide ratio, Set Clock as 100 Frames/Sec
	OLED_WrCmd(0xd9);// --set pre-charge period
	OLED_WrCmd(0xf1);// Set Pre-Charge as 15 Clocks & Discharge as 1 Clock
	OLED_WrCmd(0xda);// --set com pins hardware configuration
	OLED_WrCmd(0x12);
	OLED_WrCmd(0xdb);// --set vcomh
	OLED_WrCmd(0x40);// Set VCOM Deselect Level
	OLED_WrCmd(0x20);// -Set Page Addressing Mode (0x00/0x01/0x02)
	OLED_WrCmd(0x02);//
	OLED_WrCmd(0x8d);// --set Charge Pump enable/disable
	OLED_WrCmd(0x14);// --set(0x10) disable
	OLED_WrCmd(0xa4);// Disable Entire Display On (0xa4/0xa5)
	OLED_WrCmd(0xa6);// Disable Inverse Display On (0xa6/a7) 
	OLED_WrCmd(OLED_INST_DISPLAY_ON);// --turn on oled panel
	OLED_Fill(0x00); // Clear the screen
}

void OLED_Select_Chunk (uint8_t xChunk, uint8_t yChunk) { 
	xChunk = xChunk << 3;

	I2C_Send_Start();
	I2C_Buf = OLED_I2C_ADDR;
	I2C_WriteByte();
	
	I2C_Buf = 0x00;	//write command
	I2C_WriteByte();
	I2C_Buf = OLED_INST_SET_Y_ADDR | yChunk;
	I2C_WriteByte();

	I2C_Buf = 0x00;	//write command
	I2C_WriteByte();
	I2C_Buf = OLED_INST_SET_HIGH_X_ADDR | ((xChunk & 0xF0)>>4);
	I2C_WriteByte();

	I2C_Buf = 0x00;	//write command
	I2C_WriteByte();
	I2C_Buf = OLED_INST_SET_LOW_X_ADDR | (xChunk & 0x0F);
	I2C_WriteByte();
	
	I2C_Send_Stop();	
}

void OLED_Program_Chunk (uint8_t* dat) {
	I2C_Send_Start();
	I2C_Buf = OLED_I2C_ADDR;
	I2C_WriteByte();
	I2C_Buf = 0x40;	//write data
	I2C_WriteByte();
	
	I2C_Buf = dat[0];
	I2C_WriteByte();
	I2C_Buf = dat[1];
	I2C_WriteByte();
	I2C_Buf = dat[2];
	I2C_WriteByte();
	I2C_Buf = dat[3];
	I2C_WriteByte();
	I2C_Buf = dat[4];
	I2C_WriteByte();
	I2C_Buf = dat[5];
	I2C_WriteByte();
	I2C_Buf = dat[6];
	I2C_WriteByte();
	I2C_Buf = dat[7];
	I2C_WriteByte();
	
	I2C_Send_Stop();
}

void OLED_WrCmd(uint8_t cmd) {
	I2C_Send_Start();
	I2C_Buf = OLED_I2C_ADDR;
	I2C_WriteByte();
	I2C_Buf = 0x00;	//write command
	I2C_WriteByte();
	I2C_Buf = cmd;
	I2C_WriteByte();
	I2C_Send_Stop();
}

void OLED_WrDat(uint8_t dat) {
	I2C_Send_Start();
	I2C_Buf = OLED_I2C_ADDR;
	I2C_WriteByte();
	I2C_Buf = 0x40;	//write data
	I2C_WriteByte();
	I2C_Buf = dat;
	I2C_WriteByte();
	I2C_Send_Stop();
}

