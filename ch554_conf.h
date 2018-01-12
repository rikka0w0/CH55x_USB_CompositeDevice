#ifndef	__CH554_CONF_H__
#define __CH554_CONF_H__

#define  UART0_BUAD    57600

//#define CLOCK_FREQ_32
//#define CLOCK_FREQ_24
//#define CLOCK_FREQ_16
//#define CLOCK_FREQ_12
//#define CLOCK_FREQ_6
//#define CLOCK_FREQ_3
//#define CLOCK_FREQ_P750
//#define CLOCK_FREQ_P1875

#ifdef CLOCK_FREQ_32
#define FREQ_SYS 32000000
#endif
#ifdef CLOCK_FREQ_24
#define FREQ_SYS 24000000
#endif
#ifdef CLOCK_FREQ_16
#define FREQ_SYS 16000000
#endif
#ifdef CLOCK_FREQ_12
#define FREQ_SYS 12000000
#endif
#ifdef CLOCK_FREQ_6
#define FREQ_SYS 6000000
#endif
#ifdef CLOCK_FREQ_3
#define FREQ_SYS 3000000
#endif
#ifdef CLOCK_FREQ_P750
#define FREQ_SYS 750000
#endif
#ifdef CLOCK_FREQ_P1875
#define FREQ_SYS 187500
#endif


void CH554_Init(void);

// Interrupt Handlers
void USBInterrupt(void);

#endif
