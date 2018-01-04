#include "types.h"
#include "usb_desc.h"
#include "usb_endp.h"
#include "CH554.h"
#include <string.h>	// For memcpy()

#include "usb_bot.h"

// Used in SET_FEATURE and CLEAR_FEATURE 
#define USB_FEATURE_ENDPOINT_HALT 0						// Endpoint only
#define USB_FEATURE_DEVICE_REMOTE_WAKEUP 1		// Device only

#define USB_ENDP0_SIZE         DEFAULT_ENDP0_SIZE

// The buffer (Tx and Rx) must have an even address, size: 10 (0x0A)
xdata uint8_t  Ep0Buffer[8 > (USB_ENDP0_SIZE + 2) ? 8 : (USB_ENDP0_SIZE + 2)] _at_ 0x0000;

#define UsbSetupBuf ((PUSB_SETUP_REQ)Ep0Buffer)

uint8_t   SetupReq, SetupLen, Ready, UsbConfig;
uint8_t*  pDescr;

// Process SETUP packet
void USB_EP0_SETUP(void) {
	uint8_t len = USB_RX_LEN;
	if (len == (sizeof(USB_SETUP_REQ))) {
		SetupLen = UsbSetupBuf->wLengthL;
		if (UsbSetupBuf->wLengthH || SetupLen > 0x7F)
			SetupLen = 0x7F;	// Limit the total length

		len = 0;	// Assume success -> Tx 0-length packet
		SetupReq = UsbSetupBuf->bRequest;

		if ((UsbSetupBuf->bRequestType & USB_REQ_TYP_MASK) == USB_REQ_TYP_STANDARD) {
			// Standard USB Request
			switch (SetupReq) {
			case USB_GET_DESCRIPTOR:
				switch (UsbSetupBuf->wValueH) {
				case 1:												// Device Descriptor
					pDescr = DevDesc;
					len = USB_DESCSIZE_DEVICE;
					break;
				case 2:												// Configure Descriptor
					pDescr = CfgDesc;
					len = (USB_DESCSIZE_CONFIG_H << 8) | USB_DESCSIZE_CONFIG_L;
					break;
				case 3:												// String Descriptor
					len = UsbSetupBuf->wValueL;	// Index
					if (len < USB_STRINGDESC_COUNT) {
						pDescr = StringDescs[len];
						len = pDescr[0];
					} else {
						len = 0xFF;								// Not supported
					}
					break;
				case 0x22:										// Report Descriptor
					len = UsbSetupBuf->wIndexL;
					if (len < USB_INTERFACES) {
						if (len == USB_INTERFACES - 1)
							Ready = 1;
						
						pDescr = USB_HID_REPDESCS[len];
						len = USB_HID_REPDESCS_SIZE[len];			
					} else {
						len = 0xff;		// The host is trying to config invalid interfaces
					}
					break;
				default:
					len = 0xff;		// Unsupported descriptors
					break;
				}	// switch (UsbSetupBuf->wValueH)
				if (SetupLen > len) {
					SetupLen = len;		// Limit length
				}
				len = SetupLen >= 8 ? 8 : SetupLen;
				memcpy(Ep0Buffer, pDescr, len);
				SetupLen -= len;
				pDescr += len;
				break;
			case USB_SET_ADDRESS:
				SetupLen = UsbSetupBuf->wValueL;	// Save the assigned address
				break;
			case USB_GET_CONFIGURATION:
				Ep0Buffer[0] = UsbConfig;
				if (SetupLen >= 1)
					len = 1;
				break;
			case USB_SET_CONFIGURATION:
				UsbConfig = UsbSetupBuf->wValueL;
				break;
			case 0x0A:
				break;
			case USB_CLEAR_FEATURE:                                            //Clear Feature
				if ((UsbSetupBuf->bRequestType & USB_REQ_RECIP_MASK) == USB_REQ_RECIP_ENDP) {	// Endpoint
					len = USB_EP_HALT_CLEAR(((UINT16)UsbSetupBuf->wIndexH << 8) | UsbSetupBuf->wIndexL);
				} else {
					len = 0xFF;			// Unsupported
				}
				break;
			case USB_SET_FEATURE:                                              /* Set Feature */
				if ((UsbSetupBuf->bRequestType & USB_REQ_RECIP_MASK) == USB_REQ_RECIP_ENDP) { // Endpoint
					if ((((UINT16)UsbSetupBuf->wValueH << 8) | UsbSetupBuf->wValueL) == USB_FEATURE_ENDPOINT_HALT) {
						len = USB_EP_HALT_SET(((UINT16)UsbSetupBuf->wIndexH << 8) | UsbSetupBuf->wIndexL);
					} else {
						len = 0xFF;		// Unsupported
					}	// if ((((UINT16)UsbSetupBuf->wValueH << 8) | UsbSetupBuf->wValueL) == 0x00)
				}	else {
					len = 0xFF;			// Unsupported
				}
				break;
			case USB_GET_STATUS:
				// Device Status Bits:
				// D0 1=Self-Powered 0=Bus-powered
				// D1 1=Support remote wakeup
				// D2-D15 Reserved, must be 0
				//
				// Interface Status Bits:
				// All reserved, must be 0
				// 
				// Endpoint Status Bits:
				// D0 1=Endpoint is halted
				// D1-D15 Reserved, must be 0
				Ep0Buffer[0] = 0x00;
				Ep0Buffer[1] = 0x00;
				if (SetupLen >= 2) {
					len = 2;
				} else {
					len = SetupLen;
				}
				break;
			default:
				len = 0xff;				// Unsupported
				break;
			}	// switch (SetupReq)
		}	// if ((UsbSetupBuf->bRequestType & USB_REQ_TYP_MASK) == USB_REQ_TYP_STANDARD)
		
		else if ((UsbSetupBuf->bRequestType & USB_REQ_TYP_MASK) == USB_REQ_TYP_CLASS) {
			switch( SetupReq ) {
				case 0x01:		// GetReport
					break;
				case 0x02:		// GetIdle
					break;	
				case 0x03:		// GetProtocol
					break;				
				case 0x09:		// SetReport										
					break;
				case 0x0A:		// SetIdle
					break;	
				case 0x0B:		// SetProtocol
					break;
				case 0xFE:		// Get Max LUN
					Ep0Buffer[0] = Bot_Get_Max_Lun();
					len = 1;		// Always 1 byte reply
					break;
				case 0xFF:		// Bulk-Only Mass Storage Reset
					Bot_MSR();
					break;
				default:
					len = 0xFF;
					break;
			}
		}
		
		// Process class requests and vendor requests here (if necessary)
	} else {
		len = 0xff;						// Wrong packet length
	}	// if (len == (sizeof(USB_SETUP_REQ)))
	
	
	//Responce
	if (len == 0xff) {	// Return STALL to host if operation is not supported
		SetupReq = 0xFF;
		UEP0_CTRL = bUEP_R_TOG | bUEP_T_TOG | UEP_R_RES_STALL | UEP_T_RES_STALL;//STALL
	} else {	// Tx data to host or send 0-length packet
		UEP0_T_LEN = len;
		UEP0_CTRL = bUEP_R_TOG | bUEP_T_TOG | UEP_R_RES_ACK | UEP_T_RES_ACK;	// Expect DATA1, Answer ACK
	}
}

// Process IN packet
void USB_EP0_IN(void) {
	uint8_t len = 0;
	switch (SetupReq)
	{
	case USB_GET_DESCRIPTOR:
		len = SetupLen >= 8 ? 8 : SetupLen;		// Fix length
		memcpy(Ep0Buffer, pDescr, len);				// Copy data to Ep0Buffer, ready to Tx
		SetupLen -= len;
		pDescr += len;
		UEP0_T_LEN = len;
		UEP0_CTRL ^= bUEP_T_TOG;							// Switch between DATA0 and DATA1	
		break;
	case USB_SET_ADDRESS:
		USB_DEV_AD = bUDA_GP_BIT | SetupLen;
		UEP0_CTRL = UEP_R_RES_ACK | UEP_T_RES_NAK;
		break;
	default:
		UEP0_T_LEN = 0;												// End of transaction
		UEP0_CTRL = UEP_R_RES_ACK | UEP_T_RES_NAK;
		break;
	}
}

// Process OUT packet
void USB_EP0_OUT(void) {
	uint8_t len = USB_RX_LEN;
	if (SetupReq == 0x09)
	{
		if (Ep0Buffer[0])
		{
			//printf("Light on Num Lock LED!\n");
		}
		else if (Ep0Buffer[0] == 0)
		{
			//printf("Light off Num Lock LED!\n");
		}
	}
	UEP0_CTRL ^= bUEP_R_TOG;	// Switch between DATA0 and DATA1	
}

