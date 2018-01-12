#ifndef __USB_DESC_H
#define __USB_DESC_H

#include "types.h"

#define USB_INTERFACES 3

// Common USB Descriptors
#define USB_DESCSIZE_DEVICE 18		// Constant, DO NOT change
#define USB_DESCSIZE_CONFIG_H 0
#define USB_DESCSIZE_CONFIG_L 89	// Actual size of your CfgDesc, set according to your configuration
// Device Descriptor
extern code const uint8_t DevDesc[];
// Configuration Descriptor, Interface Descriptors, Endpoint Descriptors and ...
extern code const uint8_t CfgDesc[];
// String Descriptors
#define USB_STRINGDESC_COUNT 5 		// Number of String Descriptors available
extern code const uint8_t* StringDescs[];

// HID Report Descriptors
#define USB_HIDREPSIZE_VENDORDEF 52
#define USB_HIDREPSIZE_KEYBOARD 62
uint8_t* USB_HID_GetReportDesc(uint8_t interface);
uint8_t USB_HID_GetReportDesc_Length(uint8_t interface);


#endif /* __USB_DESC_H */
