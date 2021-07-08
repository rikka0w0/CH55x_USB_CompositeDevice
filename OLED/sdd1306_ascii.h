#ifndef __SDD1306_ASCII_H
#define __SDD1306_ASCII_H

#include "../ch554_platform.h"
#include "sdd1306.h"

void OLED_Set_Pos(uint8_t x, uint8_t y);
void OLED_P6x8Str(uint8_t x, uint8_t y, uint8_t ch[]);
void OLED_P8x16Str(uint8_t x, uint8_t y, uint8_t ch[]);

#endif
