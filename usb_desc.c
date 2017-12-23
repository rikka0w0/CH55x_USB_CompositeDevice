#include "usb_desc.h"
#include "types.h"

// Device Descriptor
code const uint8_t DevDesc[] = {	
	USB_DESCSIZE_DEVICE,	// Total length
	0x01,				// Type: Device Descriptor
	0x10, 0x01,	// USB Spec., 0x0110 -> USB 1.1
	0x00,				// Class code, 00 - each interface defines its own class
	0x00,				// Device Subclass
	0x00,				// Device Protocol
	8,					// Max packet size
	0x3d, 0x41,	// VID	0x413d
	0x07, 0x21,	// PID	0x2107
	0x00, 0x00,	// Device release number in BCD
	0x00,				// Manufactor, index of string descriptor, 0 - NO
	0x00,				// Product string descriptor ID
	0x00,				// Serial number (String descriptor ID) 
	0x01				// Number of possible configurations
};


// Configuration Descriptor and Interface Descriptor
code const uint8_t CfgDesc[] =
{
	// Configuration Descriptor
	9,					// Length of the descriptor
	0x02,				// Type: Configuration Descriptor
	// Total length of this and following descriptors
	USB_DESCSIZE_CONFIG_L, USB_DESCSIZE_CONFIG_H,	
	USB_INTERFACES,		// Number of interfaces
	0x01, 			// Value used to select this configuration
	0x00,				// Index of corresponding string descriptor
	0x80,				// Attributes, D7 must be 1, D6 Self-powered, D5 Remote Wakeup, D4-D0=0
	0x32,				// Max current drawn by device, in units of 2mA
	
	
	
	// Interface descriptor (Keyboard)
	9,					// Length of the descriptor
	0x04,				// Type: Interface Descriptor
	0x01,				// Interface ID
	0x00,				// Alternate setting
	0x01,				// Number of Endpoints
	0x03,				// Interface class code
	0x01,				// Subclass code 0=No subclass, 1=Boot Interface subclass
	0x01,				// Protocol code 0=None, 1=Keyboard, 2=Mouse
	0x00,				// Index of corresponding string descriptor
	
	// HID descriptor (Keyboard)
	9,					// Length of the descriptor
	0x21,				// HID: Interface Descriptor
	0x11, 0x01,	// bcdHID: HID Class Spec release number
	0x00,				// bCountryCode: Hardware target country
	0x01,				// bNumDescriptors: Number of HID class descriptors to follow
	0x22,				// bDescriptorType
	// wItemLength: Total length of Report descriptor
	USB_HIDREPSIZE_KEYBOARD, 0,
	
	// Endpoint descriptor (Keyboard)
	7,				// Length of the descriptor
	0x05,				// Type: Interface Descriptor
	0x81, 			// Endpoint: D7: 0-Out 1-In, D6-D4=0, D3-D0 Endpoint number
	0x03,				// Attributes:
							// D1:0 Transfer type: 00 = Control 01 = Isochronous 10 = Bulk 11 = Interrupt
							// 			The following only apply to isochronous endpoints. Else set to 0.
							// D3:2 Synchronisation Type: 00 = No Synchronisation 01 = Asynchronous 10 = Adaptive 11 = Synchronous
							// D5:4	Usage Type: 00 = Data endpoint 01 = Feedback endpoint 10 = Implicit feedback Data endpoint 11 = Reserved
							// D7:6 = 0
	0x08,	0x00,	// Maximum packet size can be handled
	0x0a,				// Interval for polling, in units of 1 ms for low/full speed 
	
	
	
	// Interface descriptor (Mouse)
	9,					// Length of the descriptor
	0x04,				// Type: Interface Descriptor
	0x00,				// Interface ID
	0x00,				// Alternate setting
	0x02,				// Number of Endpoints
	0x03,				// Interface class code
	0x01,				// Subclass code 0=No subclass, 1=Boot Interface subclass
	0x00,				// Protocol code 0=None, 1=Keyboard, 2=Mouse
	0x00,				// Index of corresponding string descriptor
	
	// HID descriptor (Vendor-defined)
	9,				// Length of the descriptor
	0x21,				// HID: Interface Descriptor
	0x10, 0x01,	// bcdHID: HID Class Spec release number
	0x00,				// bCountryCode: Hardware target country
	0x01,				// bNumDescriptors: Number of HID class descriptors to follow
	0x22,				// bDescriptorType
	// wItemLength: Total length of Report descriptor
	USB_HIDREPSIZE_VENDORDEF, 0,	
	
	// Endpoint descriptor (Vendor-defined)	// EP2, IN
  7,					// Length of the descriptor
	0x05,				// Type: Interface Descriptor
	0x82,				// Endpoint: D7: 0-Out 1-In, D6-D4=0, D3-D0 Endpoint number
	0x03,				// Attributes:
							// D1:0 Transfer type: 00 = Control 01 = Isochronous 10 = Bulk 11 = Interrupt
							// 			The following only apply to isochronous endpoints. Else set to 0.
							// D3:2 Synchronisation Type: 00 = No Synchronisation 01 = Asynchronous 10 = Adaptive 11 = Synchronous
							// D5:4	Usage Type: 00 = Data endpoint 01 = Feedback endpoint 10 = Implicit feedback Data endpoint 11 = Reserved
							// D7:6 = 0
	0x40,0x00,	// Maximum packet size can be handled
	0x18,				// Interval for polling, in units of 1 ms for low/full speed
	
	// Endpoint descriptor (Vendor-defined)	// EP2, OUT
  7,					// Length of the descriptor
	0x05,				// Type: Interface Descriptor
	0x02,				// Endpoint: D7: 0-Out 1-In, D6-D4=0, D3-D0 Endpoint number
	0x03,				// Attributes:
							// D1:0 Transfer type: 00 = Control 01 = Isochronous 10 = Bulk 11 = Interrupt
							// 			The following only apply to isochronous endpoints. Else set to 0.
							// D3:2 Synchronisation Type: 00 = No Synchronisation 01 = Asynchronous 10 = Adaptive 11 = Synchronous
							// D5:4	Usage Type: 00 = Data endpoint 01 = Feedback endpoint 10 = Implicit feedback Data endpoint 11 = Reserved
							// D7:6 = 0
	0x40,0x00,	// Maximum packet size can be handled
	0x18	      // Interval for polling, in units of 1 ms for low/full speed 
};