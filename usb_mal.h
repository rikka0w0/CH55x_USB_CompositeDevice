#ifndef __USB_MAL_H
#define __USB_MAL_H

#include "types.h"

#define MAL_MAX_LUN 0

#define MAL_OK   0
#define MAL_FAIL 1

extern xdata uint32_t Mass_Block_Size[];
extern xdata uint32_t Mass_Block_Count[];

void LUN_Init(void);
uint8_t LUN_GetStatus(uint8_t lun);
uint8_t* LUN_GetInquiryData (uint8_t lun);
void LUN_Eject (uint8_t lun);
// Read BULK_MAX_PACKET_SIZE bytes of data from device to BOT_Tx_Buf
void LUN_Read (uint8_t lun, uint32_t curAddr);
// Write BULK_MAX_PACKET_SIZE bytes of data from BOT_Rx_Buf to device
void LUN_Write (uint8_t lun, uint32_t curAddr);

#endif /* __USB_MAL_H */
