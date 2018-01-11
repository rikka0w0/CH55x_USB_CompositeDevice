#include "ch554.h"
#include "types.h"

#include "ch554_conf.H"                                                      
#include "Delay.H"
#include "usb_endp.h"

#include <string.h>
#include <stdio.h>

xdata uint8_t FLAG;
extern uint8_t Ready;

void main(void) {
    CH554_Init();                                                           //CH559时钟选择配置

    //printf("start ...\n");
                                                         //允许单片机中断
    FLAG = 0;
    Ready = 0;
	
    while(1);
}

extern void DeviceInterrupt(void);
void USB(void) interrupt INT_NO_USB {
	DeviceInterrupt();
}
