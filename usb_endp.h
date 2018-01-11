#ifndef __USB_ENDP_H
#define __USB_ENDP_H

#include "types.h"
uint8_t USB_EP_HALT_SET(uint8_t ep);
uint8_t USB_EP_HALT_CLEAR(uint8_t ep);

// EP0
#define EP0_ADDR 0x0000
extern xdatabuf(EP0_ADDR, Ep0Buffer, );
void USB_EP0_SETUP(void);
void USB_EP0_IN(void);
void USB_EP0_OUT(void);

// EP1
#define EP1_ADDR 0x000a
extern xdatabuf(EP1_ADDR, Ep1Buffer, );
void USB_EP1_IN(void);

// EP2
#define EP2_ADDR 0x0050
extern xdatabuf(EP2_ADDR, Ep2Buffer, );
#define EP2_SIZE 64
#define EP2_RX_BUF (Ep2Buffer)
#define EP2_TX_BUF (Ep2Buffer + EP2_SIZE)
void USB_EP2_IN(void);
void USB_EP2_OUT(void);

// EP3
#define EP3_ADDR 0x00D4
extern xdatabuf(EP3_ADDR, Ep3Buffer, );
#define EP3_SIZE 64
#define EP3_RX_BUF (Ep3Buffer)
#define EP3_TX_BUF (Ep3Buffer + EP3_SIZE)
void Mass_Storage_In(void);
void Mass_Storage_Out(void);

void NOP_Process(void);

// Out
#define EP0_OUT_Callback USB_EP0_OUT
#define EP1_OUT_Callback NOP_Process
#define EP2_OUT_Callback USB_EP2_OUT
#define EP3_OUT_Callback Mass_Storage_Out
#define EP4_OUT_Callback NOP_Process
		
// SOF
#define EP0_SOF_Callback NOP_Process
#define EP1_SOF_Callback NOP_Process
#define EP2_SOF_Callback NOP_Process
#define EP3_SOF_Callback NOP_Process
#define EP4_SOF_Callback NOP_Process
		
// IN
#define EP0_IN_Callback USB_EP0_IN
#define EP1_IN_Callback USB_EP1_IN
#define EP2_IN_Callback USB_EP2_IN
#define EP3_IN_Callback Mass_Storage_In
#define EP4_IN_Callback NOP_Process
		
// SETUP
#define EP0_SETUP_Callback USB_EP0_SETUP
#define EP1_SETUP_Callback NOP_Process
#define EP2_SETUP_Callback NOP_Process
#define EP3_SETUP_Callback NOP_Process
#define EP4_SETUP_Callback NOP_Process

#endif /*__USB_ENDP_H*/
