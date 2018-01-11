#include "ch554.h"

#include "eeprom.h"
#include "usb_mal.h"
#include "usb_bot.h"

xdata uint32_t Mass_Block_Size[MAL_MAX_LUN+1];
xdata uint32_t Mass_Block_Count[MAL_MAX_LUN+1];

xdata uint8_t EEPROM_Status = MAL_OK;

code const uint8_t EEPROM_Inquiry_Data[] =
  {
    0x00,          /* Direct Access Device */
    0x80,          /* RMB = 1: Removable Medium */
    0x04,          /* Version: No conformance claim to standard */
    0x02,

    0x1f,          /* Additional Length */
    0x00,          /* SCCS = 1: Storage Controller Component */
    0x00,
    0x00,
    /* Vendor Identification */
    'R', 'i', 'k', 'k', 'a', ' ', ' ', ' ',				/* Manufacturer: 8 bytes */
    /* Product Identification */
    'C', 'H', '5', '5', '4', ' ', 'U', 'S',				/* Product : 16 Bytes */
		'B', ' ', 'D', 'r', 'i', 'v', 'e', ' ',
    /* Product Revision Level */
    '1', '.', '0', ' '														/* Version : 4 Bytes */
  };

void LUN_Init (void) {
	Mass_Block_Count[0] = EEPROM_SIZE/EEPROM_SECTORSIZE;
	Mass_Block_Size[0] =  EEPROM_SECTORSIZE;
}

uint8_t LUN_GetStatus (uint8_t lun) {
	if (lun == 0) {
		return EEPROM_Status;
	}
	return MAL_FAIL;
}

uint8_t* LUN_GetInquiryData (uint8_t lun) {
	if (lun == 0) {
		return EEPROM_Inquiry_Data;
	}
	return 0;
}

// Read BULK_MAX_PACKET_SIZE bytes of data from device to BOT_Tx_Buf
void LUN_Read (uint8_t lun, uint32_t curAddr) {
	if (lun == 0) {
		EEPROM_Read(BOT_Tx_Buf, U32B1(curAddr), U32B0(curAddr), BULK_MAX_PACKET_SIZE);
	}
}

// Write BULK_MAX_PACKET_SIZE bytes of data from BOT_Rx_Buf to device
void LUN_Write (uint8_t lun, uint32_t curAddr) {
	if (lun == 0) {
		EEPROM_Write(BOT_Rx_Buf, U32B1(curAddr), U32B0(curAddr), BULK_MAX_PACKET_SIZE);
	}
}

SBIT(ledEject,	0x90, 0);
void LUN_Eject (uint8_t lun) {
	if (lun == 0) {
		EEPROM_Status = MAL_FAIL;
		ledEject = 0;
	}
}
