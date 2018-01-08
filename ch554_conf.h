#ifndef	__CH554_CONF_H__
#define __CH554_CONF_H__

#define  UART0_BUAD    57600

//#define CLOCK_FREQ_32
#define CLOCK_FREQ_24
//#define CLOCK_FREQ_16
//#define CLOCK_FREQ_12
//#define CLOCK_FREQ_6
//#define CLOCK_FREQ_3
//#define CLOCK_FREQ_P750
//#define CLOCK_FREQ_P1875

#ifdef CLOCK_FREQ_32
#define CLOCK_FREQ 32000000
#endif
#ifdef CLOCK_FREQ_24
#define CLOCK_FREQ 24000000
#endif
#ifdef CLOCK_FREQ_16
#define CLOCK_FREQ 16000000
#endif
#ifdef CLOCK_FREQ_12
#define CLOCK_FREQ 12000000
#endif
#ifdef CLOCK_FREQ_6
#define CLOCK_FREQ 6000000
#endif
#ifdef CLOCK_FREQ_3
#define CLOCK_FREQ 3000000
#endif
#ifdef CLOCK_FREQ_P750
#define CLOCK_FREQ 750000
#endif
#ifdef CLOCK_FREQ_P1875
#define CLOCK_FREQ 187500
#endif


void CH554_Init(void);

#endif
