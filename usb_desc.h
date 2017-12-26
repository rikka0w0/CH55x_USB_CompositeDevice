#ifndef __USB_DESC_H
#define __USB_DESC_H

#include "types.h"

#define USB_INTERFACES 2

// Common USB Descriptors
#define USB_DESCSIZE_DEVICE 18		// Constant, DO NOT change
#define USB_DESCSIZE_CONFIG_H 0
#define USB_DESCSIZE_CONFIG_L 66	// Actual size of your CfgDesc, set according to your configuration
#define USB_STRINGDESC_COUNT 5 		// Number of String Descriptors available
// Device Descriptor
code const uint8_t DevDesc[];
// Configuration Descriptor, Interface Descriptors, Endpoint Descriptors and ...
code const uint8_t CfgDesc[];	
// String Descriptors
code const uint8_t* StringDescs[];

// HID Report Descriptors
#define USB_HIDREPSIZE_KEYBOARD 62
#define USB_HIDREPSIZE_VENDORDEF 52
code const uint8_t* USB_HID_REPDESCS[];
code const uint8_t USB_HID_REPDESCS_SIZE[];


#endif /* __USB_DESC_H */