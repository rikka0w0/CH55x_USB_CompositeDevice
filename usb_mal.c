#include "types.h"
#include "ch554.h"

#include "i2c.h"
#include "eeprom.h"
#include "usb_mal.h"
#include "usb_endp.h"

xdata uint32_t Mass_Block_Size[MAL_MAX_LUN+1];
xdata uint32_t Mass_Block_Count[MAL_MAX_LUN+1];

#define FLASH25_SIZE 65536			//Capacity 64KiB
#define FLASH25_SECTORSIZE 512	//Sector Size 512B
extern uint8_t dead;
uint8_t MAL_GetStatus (uint8_t lun) {
	 switch (lun){
		 case 0:	//SPI Flash
			 Mass_Block_Count[0] = FLASH25_SIZE/FLASH25_SECTORSIZE;
			 Mass_Block_Size[0] =  FLASH25_SECTORSIZE;
			 return dead;
		 default:
			 return MAL_FAIL;
	 }
}