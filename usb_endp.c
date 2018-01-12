#include "usb_endp.h"

#include "ch554_platform.h"
#include "usb_hid_keyboard.h"

#define MAX_PACKET_SIZE 64

// The buffer (Tx and Rx) must have an even address, size: 66 (0x42)
xdatabuf(EP1_ADDR, Ep1Buffer, 64 > (MAX_PACKET_SIZE + 2) ? 64 : (MAX_PACKET_SIZE + 2));
// The buffer (Tx and Rx) must have an even address, size: 132 (0x84)
xdatabuf(EP2_ADDR, Ep2Buffer, 128 > (2 * MAX_PACKET_SIZE + 4) ? 128 : (2 * MAX_PACKET_SIZE + 4));
// The buffer (Tx and Rx) must have an even address, size: 132 (0x84)
xdatabuf(EP3_ADDR, Ep3Buffer, 128 > (2 * MAX_PACKET_SIZE + 4) ? 128 : (2 * MAX_PACKET_SIZE + 4));

uint8_t USB_EP_HALT_SET(uint8_t ep) {
	switch (ep) {
	case 0x82:
		UEP2_CTRL = UEP2_CTRL & (~bUEP_T_TOG) | UEP_T_RES_STALL;
		return 0;
	case 0x02:
		UEP2_CTRL = UEP2_CTRL & (~bUEP_R_TOG) | UEP_R_RES_STALL;
		return 0;
	case 0x83:
		UEP3_CTRL = UEP3_CTRL & (~bUEP_T_TOG) | UEP_T_RES_STALL;
		return 0;
	case 0x03:
		UEP3_CTRL = UEP3_CTRL & (~bUEP_R_TOG) | UEP_R_RES_STALL;
		return 0;	
	case 0x81:
		UEP1_CTRL = UEP1_CTRL & (~bUEP_T_TOG) | UEP_T_RES_STALL;
		return 0;
	default:
		return 0xFF;
	}
}

uint8_t USB_EP_HALT_CLEAR(uint8_t ep) {
	switch (ep)
	{
	case 0x82:
		UEP2_CTRL = UEP2_CTRL & ~(bUEP_T_TOG | MASK_UEP_T_RES) | UEP_T_RES_NAK;
		return 0;
	case 0x02:
		UEP2_CTRL = UEP2_CTRL & ~(bUEP_R_TOG | MASK_UEP_R_RES) | UEP_R_RES_ACK;
		return 0;
	case 0x83:
		UEP3_CTRL = UEP3_CTRL & ~(bUEP_T_TOG | MASK_UEP_T_RES) | UEP_T_RES_NAK;
		return 0;
	case 0x03:
		UEP3_CTRL = UEP3_CTRL & ~(bUEP_R_TOG | MASK_UEP_R_RES) | UEP_R_RES_ACK;
		return 0;	
	case 0x81:
		UEP1_CTRL = UEP1_CTRL & ~(bUEP_T_TOG | MASK_UEP_T_RES) | UEP_T_RES_NAK;
		return 0;
	default:
		return 0xFF;
	}
}

extern uint8_t keyState, kbdModifier, kbdKey;

void USB_EP1_IN(void) {
	UEP1_T_LEN = 0;
	UEP1_CTRL = UEP1_CTRL & ~MASK_UEP_T_RES | UEP_T_RES_NAK;	// No data to send

	if (keyState == KBD_STATE_KEYDOWN)
		keyState = KBD_STATE_IDLE;
}

void USB_EP2_IN(void) {
	UEP2_T_LEN = 0;
	UEP2_CTRL = UEP2_CTRL & ~MASK_UEP_T_RES | UEP_T_RES_NAK;	// No data to send
}


void USB_EP2_OUT(void) {
	uint8_t i;
	if (U_TOG_OK) {	// Discard unsynchronized packets
		for (i = 0; i < USB_RX_LEN; i++)
			EP2_TX_BUF[i] = EP2_RX_BUF[i] ^ 0xFF;		// Invert bits and Tx to host (for validation)
		
		if (EP2_RX_BUF[0]==0xEE) {
			keyState = KBD_STATE_KEYDOWN;
			kbdModifier = EP2_RX_BUF[1];
			kbdKey = EP2_RX_BUF[2];
		}
		
		UEP2_T_LEN = USB_RX_LEN;
		UEP2_CTRL = UEP2_CTRL & ~MASK_UEP_T_RES | UEP_T_RES_ACK;	// Enable Tx
	}
}
