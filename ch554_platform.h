#ifndef __PLATFORM
#define __PLATFORM

#if defined (SDCC) || defined (__SDCC)
	#ifndef __SDCC
		#define __SDCC
	#endif

	// Use GCC defined standard types
	#include <stdint.h>
	// SDCC is little-endian
	#define __LITTLE_ENDIAN

	// Define Keil C51 specified derivatives
	#define xdata __xdata
	#define code __code
	#define interrupt __interrupt

	#define xdatabuf(addr, name, sz) __xdata __at(addr) uint8_t name[sz]
	#define extern_xdatabuf(addr, name) extern __xdata __at(addr) uint8_t name[]
#elif defined (__C51__) || defined (__CX51__)
	#ifndef __C51__
		#define __C51__
	#endif

	// Define standard types
	#ifndef bool
	typedef bit bool;
	#endif

	#ifndef uint8_t
	typedef unsigned char uint8_t;
	#endif

	#ifndef int8_t
	typedef signed char int8_t;
	#endif

	#ifndef uint16_t
	typedef unsigned short uint16_t;
	#endif

	#ifndef int16_t
	typedef signed short int16_t;
	#endif

	#ifndef uint32_t
	typedef unsigned long uint32_t;
	#endif

	#ifndef int32_t
	typedef signed long int32_t;
	#endif

	// Keil C51 is big-endian
	#define __BIG_ENDIAN

	// SDCC style SBIT
	#define SBIT(var, port, pin) sbit var = port^pin

	#define xdatabuf(addr, name, sz) xdata uint8_t name[sz] _at_ addr
	#define extern_xdatabuf(addr, name) extern xdata uint8_t name[]
#else
	#error "Unsupported compiler!"
#endif

#if defined __BIG_ENDIAN
	// uint16_t
	#define U16B0(u) (((uint8_t*)&u)[1])
	#define U16B1(u) (((uint8_t*)&u)[0])
	// uint32_t
	#define U32B0(u) (((uint8_t*)&u)[3])
	#define U32B1(u) (((uint8_t*)&u)[2])
	#define U32B2(u) (((uint8_t*)&u)[1])
	#define U32B3(u) (((uint8_t*)&u)[0])

	#define U16_XBig(dstAddr, srcAddr) {	\
			((uint8_t*)dstAddr)[0] = ((uint8_t*)srcAddr)[0];	\
			((uint8_t*)dstAddr)[1] = ((uint8_t*)srcAddr)[1];	\
	}

	#define U16_XLittle(dstAddr, srcAddr) {	\
			((uint8_t*)dstAddr)[0] = ((uint8_t*)srcAddr)[1];	\
			((uint8_t*)dstAddr)[1] = ((uint8_t*)srcAddr)[0];	\
	}

	#define U32_XBig(dstAddr, srcAddr) {	\
			((uint8_t*)dstAddr)[0] = ((uint8_t*)srcAddr)[0];	\
			((uint8_t*)dstAddr)[1] = ((uint8_t*)srcAddr)[1];	\
			((uint8_t*)dstAddr)[2] = ((uint8_t*)srcAddr)[2];	\
			((uint8_t*)dstAddr)[3] = ((uint8_t*)srcAddr)[3];	\
	}

	#define U32_XLittle(dstAddr, srcAddr) {	\
			((uint8_t*)dstAddr)[0] = ((uint8_t*)srcAddr)[3];	\
			((uint8_t*)dstAddr)[1] = ((uint8_t*)srcAddr)[2];	\
			((uint8_t*)dstAddr)[2] = ((uint8_t*)srcAddr)[1];	\
			((uint8_t*)dstAddr)[3] = ((uint8_t*)srcAddr)[0];	\
	}
#elif defined __LITTLE_ENDIAN
	// uint16_t
	#define U16B0(u) (((uint8_t*)&u)[0])
	#define U16B1(u) (((uint8_t*)&u)[1])
	// uint32_t
	#define U32B0(u) (((uint8_t*)&u)[0])
	#define U32B1(u) (((uint8_t*)&u)[1])
	#define U32B2(u) (((uint8_t*)&u)[2])
	#define U32B3(u) (((uint8_t*)&u)[3])

	#define U16_XBig(dstAddr, srcAddr) {	\
			((uint8_t*)dstAddr)[0] = ((uint8_t*)srcAddr)[1];	\
			((uint8_t*)dstAddr)[1] = ((uint8_t*)srcAddr)[0];	\
	}

	#define U16_XLittle(dstAddr, srcAddr) {	\
			((uint8_t*)dstAddr)[0] = ((uint8_t*)srcAddr)[0];	\
			((uint8_t*)dstAddr)[1] = ((uint8_t*)srcAddr)[1];	\
	}

	#define U32_XBig(dstAddr, srcAddr) {	\
			((uint8_t*)dstAddr)[0] = ((uint8_t*)srcAddr)[3];	\
			((uint8_t*)dstAddr)[1] = ((uint8_t*)srcAddr)[2];	\
			((uint8_t*)dstAddr)[2] = ((uint8_t*)srcAddr)[1];	\
			((uint8_t*)dstAddr)[3] = ((uint8_t*)srcAddr)[0];	\
	}

	#define U32_XLittle(dstAddr, srcAddr) {	\
			((uint8_t*)dstAddr)[0] = ((uint8_t*)srcAddr)[0];	\
			((uint8_t*)dstAddr)[1] = ((uint8_t*)srcAddr)[1];	\
			((uint8_t*)dstAddr)[2] = ((uint8_t*)srcAddr)[2];	\
			((uint8_t*)dstAddr)[3] = ((uint8_t*)srcAddr)[3];	\
	}
#else
	#error "QAQ"
#endif


#endif	// __PLATFORM



#define GPIO1 0x90
#define GPIO2 0xA0
#define GPIO3 0xB0




#ifdef __SDCC
	#include "include/ch554.h"
	#include "include/ch554_usb.h"
#endif

#ifdef __C51__
	#include "keilc51/ch554.h"
#endif
