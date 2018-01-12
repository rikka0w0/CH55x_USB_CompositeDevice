#ifndef __USB_HID_KEYBOARD_H
#define __USB_HID_KEYBOARD_H

#include "ch554_platform.h"

#define KBD_Tx_Buf Ep1Buffer
#define KBD_Tx_Enable() {UEP1_CTRL = UEP1_CTRL & ~ MASK_UEP_T_RES | UEP_T_RES_ACK;}

// Modifiers
#define KBD_LCTRL 0x01
#define KBD_LSHIFT 0x02
#define KBD_LALT 0x04
#define KBD_LGUI 0x08
#define KBD_RCTRL 0x10
#define KBD_RSHIFT 0x20
#define KBD_RALT 0x40
#define KBD_RGUI 0x80

// Key State
#define KBD_STATE_IDLE 0
#define KBD_STATE_KEYDOWN 1
#define KBD_STATE_KEYUP 2

void USB_Keyboard_SendKey(uint8_t modifier, uint8_t key);

#endif /* __USB_HID_KEYBOARD_H */
