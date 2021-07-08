#ifndef __SDD1306_H
#define __SDD1306_H

#include "../i2c.h"
#include "../ch554_platform.h"

#define OLED_I2C_ADDR 0x78
// Uncomment the following macro to enable fast fill. but the code is larger
#define OLED_FILL_FAST 1

#define OLED_INST_SET_LOW_X_ADDR 0x00
#define OLED_INST_SET_HIGH_X_ADDR 0x10
#define OLED_INST_SET_Y_ADDR 0xB0
#define OLED_INST_DISPLAY_OFF 0xAE
#define OLED_INST_DISPLAY_ON 0xAF

// Initialize the SDD1306 Oled screen
void OLED_Init(void);

// SDD1306 OLED Screen (128*64):
// . - - - - - x = 1 2 8 - - - - -
// |
// y
// =
// 6
// 4
// |

// Each chunk is 8*8 bit, which is represented by a 8 byte integer
// xChunk = [0,15]
// yChunk = [0,7]

// Format of a chunk:
// B0 means the first byte, dat[0], B1 means the second byte, dat[1]...
// b0 = LSB, b7 = MSB
// (left top)
//   B0 | B1 | B2 | B3 | B4 | B5 | B6 | B7
//   b0 | b0 | b0 | b0 | b0 | b0 | b0 | b0
//   b1 | b1 | b1 | b1 | b1 | b1 | b1 | b1
//   b2 | b2 | b2 | b2 | b2 | b2 | b2 | b2
//   b3 | b3 | b3 | b3 | b3 | b3 | b3 | b3
//   b4 | b4 | b4 | b4 | b4 | b4 | b4 | b4
//   b5 | b5 | b5 | b5 | b5 | b5 | b5 | b5
//   b6 | b6 | b6 | b6 | b6 | b6 | b6 | b6
//   b7 | b7 | b7 | b7 | b7 | b7 | b7 | b7 (right bottom)
// Chunk programming is faster than segment programming
void OLED_Select_Chunk(unsigned char xChunk, unsigned char yChunk);
void OLED_Program_Chunk(uint8_t* dat);


// Fill the entire OLED screen with dat, 0x00 - clear, 0xFF - fill
void OLED_Fill(uint8_t dat);



void OLED_WrCmd(uint8_t cmd);
void OLED_WrDat(uint8_t dat);
#endif
