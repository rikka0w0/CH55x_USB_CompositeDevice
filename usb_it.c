#include "ch554_platform.h"
#include "usb_desc.h"
#include "usb_endp.h"

void NOP_Process(void) {}
	
// See CH554 datasheet page 42
code const void (*pEndPoint_OUT_CallBack[])(void) =
 {
		EP0_OUT_Callback,
    EP1_OUT_Callback,
    EP2_OUT_Callback,
    EP3_OUT_Callback,
    EP4_OUT_Callback,
};
	
code const void (*pEndPoint_SOF_CallBack[])(void) =
 {
		EP0_SOF_Callback,
    EP1_SOF_Callback,
    EP2_SOF_Callback,
    EP3_SOF_Callback,
    EP4_SOF_Callback,
};
 
code const void (*pEndPoint_IN_CallBack[])(void) =
 {
		EP0_IN_Callback,
    EP1_IN_Callback,
    EP2_IN_Callback,
    EP3_IN_Callback,
    EP4_IN_Callback,
};
	
code const void (*pEndPoint_SETUP_CallBack[])(void) =
 {
		 EP0_SETUP_Callback,
		 EP1_SETUP_Callback,
		 EP2_SETUP_Callback,
		 EP3_SETUP_Callback,
		 EP4_SETUP_Callback,
};

void USBInterrupt(void) {
	if(UIF_TRANSFER) {
		// Dispatch to service functions
		switch (USB_INT_ST & MASK_UIS_TOKEN) {
			case UIS_TOKEN_OUT:
				(*pEndPoint_OUT_CallBack[USB_INT_ST & MASK_UIS_ENDP])();
				break;
			case UIS_TOKEN_SOF:
				(*pEndPoint_SOF_CallBack[USB_INT_ST & MASK_UIS_ENDP])();
				break;
			case UIS_TOKEN_IN:
				(*pEndPoint_IN_CallBack[USB_INT_ST & MASK_UIS_ENDP])();
				break;
			case UIS_TOKEN_SETUP:
				(*pEndPoint_SETUP_CallBack[USB_INT_ST & MASK_UIS_ENDP])();
				break;
		}
	
		UIF_TRANSFER = 0; 													// Clear interrupt flag
	}
	
	// Device mode USB bus reset
	if(UIF_BUS_RST) {
		UEP0_CTRL = UEP_R_RES_ACK | UEP_T_RES_NAK;
    UEP1_CTRL = bUEP_AUTO_TOG | UEP_R_RES_ACK;
    UEP2_CTRL = bUEP_AUTO_TOG | UEP_R_RES_ACK | UEP_T_RES_NAK;
		UEP3_CTRL = bUEP_AUTO_TOG | UEP_R_RES_ACK | UEP_T_RES_NAK;
    USB_DEV_AD = 0x00;
    UIF_SUSPEND = 0;
    UIF_TRANSFER = 0;
    UIF_BUS_RST = 0;														// Clear interrupt flag
	}
	
	// USB bus suspend / wake up
	if (UIF_SUSPEND) {
		UIF_SUSPEND = 0;
		if ( USB_MIS_ST & bUMS_SUSPEND ) {					// Suspend
        
//			while ( XBUS_AUX & bUART0_TX );					// Wait for Tx
//			SAFE_MOD = 0x55;
//			SAFE_MOD = 0xAA;
//			WAKE_CTRL = bWAK_BY_USB | bWAK_RXD0_LO;	// Wake up by USB or RxD0
//			PCON |= PD;															// Chip sleep
//			SAFE_MOD = 0x55;
//			SAFE_MOD = 0xAA;
//			WAKE_CTRL = 0x00;
        
		} else {	// Unexpected interrupt, not supposed to happen !
			USB_INT_FG = 0xFF;		// Clear interrupt flag
    }
	}
}
