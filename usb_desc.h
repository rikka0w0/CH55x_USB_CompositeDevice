#ifndef __USB_DESC_H
#define __USB_DESC_H

#include "types.h"


// Common USB Descriptors
#define USB_DESCSIZE_DEVICE 18		// Constant, DO NOT change
#define USB_DESCSIZE_CONFIG_H 0
#define USB_DESCSIZE_CONFIG_L 66	// Actual size of your CfgDesc, set according to your configuration
// Device Descriptor
extern code const uint8_t DevDesc[];
// Configuration Descriptor, Interface Descriptors, Endpoint Descriptors and ...
extern code const uint8_t CfgDesc[];	





// HID Report Descriptors
#define USB_HIDREPSIZE_KEYBOARD_H 0
#define USB_HIDREPSIZE_KEYBOARD_L 62
#define USB_HIDREPSIZE_VENDORDEF_H 0
#define USB_HIDREPSIZE_VENDORDEF_L 52
extern code const uint8_t KeyRepDesc[];
extern code const uint8_t VendorDefDesc[];



#endif /* __USB_DESC_H */