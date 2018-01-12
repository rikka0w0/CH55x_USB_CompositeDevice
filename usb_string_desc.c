#include "ch554_platform.h"
#include "usb_desc.h"

// Language Descriptor
code const uint8_t LangDesc[] = {
	4, 0x03,		// Length = 4 bytes, String Descriptor (0x03) 
	0x09, 0x04	// 0x0409 English - United States
};

// String Descriptors:

code const uint8_t DevName1[] = {
	34, 0x03, 	// Length = 34 bytes, String Descriptor (0x03) 
	'C', 0, 'H', 0, '5', 0, '5', 0, '4', 0, ' ', 0, 'C', 0, 'u', 0, 's', 0, 't', 0, 'o', 0, 'm', 0, ' ', 0, 'H', 0, 'I', 0, 'D', 0
};

code const uint8_t DevName2[] = {
	30, 0x03, 	// Length = 30 bytes, String Descriptor (0x03) 
	'C', 0, 'H', 0, '5', 0, '5', 0, '4', 0, ' ', 0, 'K', 0, 'e', 0, 'y', 0, 'b', 0, 'o', 0, 'a', 0, 'r', 0, 'd', 0
};

code const uint8_t ProductName[] = {
	38, 0x03, 	// Length = 30 bytes, String Descriptor (0x03) 
	'R', 0, 'i', 0, 'k', 0, 'k', 0, 'a', 0, '`', 0, 's', 0, ' ', 0, 'C', 0, 'H', 0, '5', 0, '5', 0, '4', 0, ' ', 0, 'D', 0, 'e', 0, 'm', 0, 'o', 0
};

code const uint8_t ManuName[] = {
	18, 0x03, 	// Length = 30 bytes, String Descriptor (0x03) 
	'R', 0, 'i', 0, 'k', 0, 'k', 0, 'a', 0, '0', 0, 'w', 0, '0', 0
};

code const uint8_t* StringDescs[USB_STRINGDESC_COUNT] = {	
	LangDesc,			// 0 (If you want to support string descriptors, you must have this!)
	DevName1,			// 1
	DevName2,			// 2
	ProductName,	// 3
	ManuName			// 4
};