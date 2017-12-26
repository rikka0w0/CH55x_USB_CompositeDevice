#include "types.h"
#include "usb_endp.h"
#include "CH554.h"

// The buffer (Tx and Rx) must have an even address, size: 66 (0x42)
xdata uint8_t Ep1Buffer[64 > (MAX_PACKET_SIZE + 2) ? 64 : (MAX_PACKET_SIZE + 2)] _at_ 0x000a;
// The buffer (Tx and Rx) must have an even address, size: 132 (0x84)
xdata uint8_t Ep2Buffer[128 > (2 * MAX_PACKET_SIZE + 4) ? 128 : (2 * MAX_PACKET_SIZE + 4)] _at_ 0x0050;


uint8_t USB_EP_HALT_SET(uint8_t ep) {
	switch (ep) {
	case 0x82:
		UEP2_CTRL = UEP2_CTRL & (~bUEP_T_TOG) | UEP_T_RES_STALL;
		return 0;
	case 0x02:
		UEP2_CTRL = UEP2_CTRL & (~bUEP_R_TOG) | UEP_R_RES_STALL;
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
	case 0x81:
		UEP1_CTRL = UEP1_CTRL & ~(bUEP_T_TOG | MASK_UEP_T_RES) | UEP_T_RES_NAK;
		return 0;
	default:
		return 0xFF;
	}
}

extern uint8_t FLAG;

void USB_EP1_IN(void) {
	UEP1_T_LEN = 0;                                                     //预使用发送长度一定要清空
  //UEP2_CTRL ^= bUEP_T_TOG;                                          //如果不设置自动翻转则需要手动翻转
	UEP1_CTRL = UEP1_CTRL & ~MASK_UEP_T_RES | UEP_T_RES_NAK;           //默认应答NAK

	FLAG = 1;
}

void USB_EP2_IN(void) {
	UEP2_T_LEN = 0;                                                     //预使用发送长度一定要清空
  //UEP1_CTRL ^= bUEP_T_TOG;                                          //如果不设置自动翻转则需要手动翻转
	UEP2_CTRL = UEP2_CTRL & ~MASK_UEP_T_RES | UEP_T_RES_NAK;           //默认应答NAK	
}



void USB_EP2_OUT(void) {
	uint8_t i;
	if (U_TOG_OK) {	// Discard unsynchronized packets
		for (i = 0; i < USB_RX_LEN; i++)
			Ep2Buffer[MAX_PACKET_SIZE + i] = Ep2Buffer[i] ^ 0xFF;		// Invert bits and Tx to host (for validation)
		
		UEP2_T_LEN = USB_RX_LEN;
		UEP2_CTRL = UEP2_CTRL & ~MASK_UEP_T_RES | UEP_T_RES_ACK;	// Enable Tx
	}
}