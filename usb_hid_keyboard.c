#include "usb_hid_keyboard.h"
#include "usb_endp.h"

// Send a USB HID keyboard report to host
// For detailed format, check this out: https://docs.mbed.com/docs/ble-hid/en/latest/api/md_doc_HID.html
void USB_Keyboard_SendKey(uint8_t modifier, uint8_t key) {
	KBD_Tx_Buf[0] = modifier;
	KBD_Tx_Buf[1] = 0;		// reserved
	KBD_Tx_Buf[2] = key;	// key0

	KBD_Tx_Buf[3] = 0;		// key1
	KBD_Tx_Buf[4] = 0;		// key2
	KBD_Tx_Buf[5] = 0;		// key3
	KBD_Tx_Buf[6] = 0;		// key4
	KBD_Tx_Buf[7] = 0;		// key5
	UEP1_T_LEN = 8;

	KBD_Tx_Enable();
}
