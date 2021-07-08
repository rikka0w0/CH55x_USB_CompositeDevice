#include "Delay.h"
#include "usb_endp.h"
#include "usb_hid_keyboard.h"
#include "ch554_conf.h"
#include <stdio.h>
#include "ch554_platform.h"

uint8_t keyState, kbdModifier, kbdKey;

void main(void) {
    CH554_Init();

    printf("start ...\n");

    keyState = KBD_STATE_IDLE;

    while(1) {
    	while(keyState == KBD_STATE_IDLE);
    	keyState = KBD_STATE_KEYDOWN;
    	USB_Keyboard_SendKey(kbdModifier, kbdKey);
    	while(keyState == KBD_STATE_KEYDOWN);
    	USB_Keyboard_SendKey(0, 0);
    }
}

/*
 * According to SDCC specification, interrupt handlers MUST be placed within the file which contains
 * the void main(void) function, otherwise SDCC won't be able to recognize it. It's not a bug but a feature.
 * If you know how to fix this, please let me know.
 */
void USBInterruptEntry(void) interrupt INT_NO_USB {
	USBInterrupt();
}
