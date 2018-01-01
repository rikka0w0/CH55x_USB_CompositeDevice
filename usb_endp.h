#ifndef __USB_ENDP_H
#define __USB_ENDP_H

#include "types.h"
uint8_t USB_EP_HALT_SET(uint8_t ep);
uint8_t USB_EP_HALT_CLEAR(uint8_t ep);

// EP0
xdata uint8_t Ep0Buffer[];
void USB_EP0_SETUP(void);
void USB_EP0_IN(void);
void USB_EP0_OUT(void);

// EP1
xdata uint8_t Ep1Buffer[];
void USB_EP1_IN(void);

// EP2
xdata uint8_t Ep2Buffer[];
#define EP2_SIZE 64
#define EP2_RX_BUF (Ep2Buffer)
#define EP2_TX_BUF (Ep2Buffer + EP2_SIZE)
void USB_EP2_IN(void);
void USB_EP2_OUT(void);

// EP3
xdata uint8_t Ep3Buffer[];
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