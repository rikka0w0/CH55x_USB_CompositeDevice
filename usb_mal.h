#ifndef __USB_MAL_H
#define __USB_MAL_H

#include "types.h"

#define MAL_MAX_LUN 0

#define MAL_OK   0
#define MAL_FAIL 1

xdata uint32_t Mass_Block_Size[];
xdata uint32_t Mass_Block_Count[];

uint8_t MAL_GetStatus(uint8_t lun);
uint8_t MAL_Read(uint8_t lun, uint32_t Memory_Offset, uint8_t *Readbuff, uint16_t Transfer_Length);

#endif /* __USB_MAL_H */