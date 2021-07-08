#include "Delay.h"
#include "ch554_platform.h"
#include "ch554_conf.h"
#include "usb_desc.h"
#include "usb_endp.h"
#include "usb_mal.h"
#include "i2c.h"



void ConfigSysClock() {
//SAFE_MOD = 0x55;
//SAFE_MOD = 0xAA;
//CLOCK_CFG |= bOSC_EN_XT;    // Enable External Crystal
//CLOCK_CFG &= ~bOSC_EN_INT;	// Disable External Crystal

	// Enters `safe mode` for next 13 to 23 CPU clock cycles
	SAFE_MOD = 0x55;
	SAFE_MOD = 0xAA;

#if defined CLOCK_FREQ_32
	CLOCK_CFG = CLOCK_CFG & ~ MASK_SYS_CK_SEL | 0x07;  // 32MHz
#elif defined CLOCK_FREQ_24
	CLOCK_CFG = CLOCK_CFG & ~ MASK_SYS_CK_SEL | 0x06;  // 24MHz
#elif defined CLOCK_FREQ_16
	CLOCK_CFG = CLOCK_CFG & ~ MASK_SYS_CK_SEL | 0x05;  // 16MHz
#elif defined CLOCK_FREQ_12
	CLOCK_CFG = CLOCK_CFG & ~ MASK_SYS_CK_SEL | 0x04;  // 12MHz
#elif defined CLOCK_FREQ_6
	CLOCK_CFG = CLOCK_CFG & ~ MASK_SYS_CK_SEL | 0x03;  // 6MHz
#elif defined CLOCK_FREQ_3
	CLOCK_CFG = CLOCK_CFG & ~ MASK_SYS_CK_SEL | 0x02;  // 3MHz
#elif defined CLOCK_FREQ_P750
	CLOCK_CFG = CLOCK_CFG & ~ MASK_SYS_CK_SEL | 0x01;  // 750KHz
#elif defined CLOCK_FREQ_P1875
	CLOCK_CFG = CLOCK_CFG & ~ MASK_SYS_CK_SEL | 0x00;  // 187.5KHz
#else
	#error "Clock frequency is not configured!"
#endif

	SAFE_MOD = 0x00;
}

void USBDevice_Init() {
	IE_USB = 0;					// Disable USB interrupt (for setup)
	USB_CTRL = 0x00;			// Device mode, Full speed(12Mbps), Disable pull up(Invisible to host), Reset stuff
	//UDEV_CTRL = bUD_PD_DIS;		//Disable DP/DN pull down resistor

	//Configure Endpoint 0
	UEP0_DMA_L = (uint8_t) Ep0Buffer; // Address of Endpoint 0 buffer
	// bUEP4_RX_EN and bUEP4_TX_EN controls both Endpoint 0 and 4, refer to table 16.3.2
	UEP4_1_MOD &= ~(bUEP4_RX_EN | bUEP4_TX_EN);		// EP0 64-byte buffer
	UEP0_CTRL = UEP_R_RES_ACK | UEP_T_RES_NAK;		// SETUP/OUT -> ACK, IN -> NAK

	//Configure Endpoint 1
	UEP1_DMA_L = (uint8_t) Ep1Buffer; // Address of Endpoint 1 buffer
    //UEP4_1_MOD = UEP4_1_MOD & ~bUEP1_BUF_MOD | bUEP1_TX_EN;
	UEP4_1_MOD &=~bUEP1_BUF_MOD;	//See table 16.3.3
	UEP4_1_MOD |= bUEP1_TX_EN;		// EP1 Tx only, 64 bytes of buffer
	// Enable DATA0/DATA1 toggling, Reply NAK for IN transactions
    UEP1_CTRL = bUEP_AUTO_TOG | UEP_T_RES_NAK;

	//Configure Endpoint 2
    UEP2_DMA_L = (uint8_t) Ep2Buffer;	// Address of Endpoint 2 buffer
    //UEP2_3_MOD = UEP2_3_MOD & ~bUEP2_BUF_MOD | bUEP2_TX_EN;
	UEP2_3_MOD &=~bUEP2_BUF_MOD;	// Disable Endpoint2 buffer mode
	UEP2_3_MOD |= bUEP2_TX_EN;		// Enable Endpoint2 Tx
	UEP2_3_MOD |= bUEP2_RX_EN;		// Enable Endpoint2 Rx
	// Enable DATA0/DATA1 toggling, Reply NAK for IN transactions, ACK for OUT transactions
    UEP2_CTRL = bUEP_AUTO_TOG | UEP_T_RES_NAK | UEP_R_RES_ACK;


	//Configure Endpoint 3
    UEP3_DMA_L = (uint16_t) Ep3Buffer;	// Address of Endpoint 2 buffer
    //UEP2_3_MOD = UEP2_3_MOD & ~bUEP2_BUF_MOD | bUEP2_TX_EN;
	UEP2_3_MOD &=~bUEP3_BUF_MOD;	// Disable Endpoint3 buffer mode
	UEP2_3_MOD |= bUEP3_TX_EN;		// Enable Endpoint3 Tx
	UEP2_3_MOD |= bUEP3_RX_EN;		// Enable Endpoint3 Rx
    UEP3_CTRL = bUEP_AUTO_TOG | UEP_T_RES_NAK | UEP_R_RES_ACK;

	// Setup interrupts
    USB_INT_FG = 0xFF;			 // Clear interrupt flags
	// Generate a interrupt when USB bus suspend | transfer complete | bus reset
    USB_INT_EN = bUIE_SUSPEND | bUIE_TRANSFER | bUIE_BUS_RST;
    IE_USB = 1;	//Enable USB interrupt


	// Get ready for enumeration
    USB_DEV_AD = 0x00;	//Set USB address to 0, get ready for enumeration
    // Connect pull up resistor | While handling interrup, reply NAK t if flag is set | Enable DMA
    USB_CTRL = bUC_DEV_PU_EN | bUC_INT_BUSY | bUC_DMA_EN;
    UDEV_CTRL |= bUD_PORT_EN;	// Enable USB Port

	// Reset all tx length to 0
    UEP1_T_LEN = 0;
    UEP2_T_LEN = 0;
    UEP3_T_LEN = 0;
}

void UART0_Init(void) {
	uint32_t x;
	uint8_t x2;

	SM0 = 0;
	SM1 = 1;
	SM2 = 0;	// UART0 Mode 0

	// Use Timer1 as the baud rate generator
	RCLK = 0;
	TCLK = 0;
	PCON |= SMOD;
	x = 10 * FREQ_SYS / UART0_BUAD / 16;
	x2 = x % 10;
	x /= 10;
	if ( x2 >= 5 ) x ++;	// Round

	TMOD = TMOD & ~ bT1_GATE & ~ bT1_CT & ~ MASK_T1_MOD | bT1_M1;	// Timer1: 8-bit auto reload
	T2MOD = T2MOD | bTMR_CLK | bT1_CLK;	// Select input clock for Timer1
	TH1 = 0-x;
	TR1 = 1;  // Start Timer1
	TI = 1;
	REN = 1;	// Enable UART1
}

void CH554_Init(void) {
	ConfigSysClock();
	mDelaymS(5);	// Wait until the internal crystal becomes stable

	UART0_Init();
	I2C_Init();
	LUN_Init();

	USBDevice_Init();

	EA = 1;
}

void putchar(char c) {
    while (!TI) /* assumes UART is initialized */
    ;
    TI = 0;
    SBUF = c;
}

char getchar() {
    while(!RI); /* assumes UART is initialized */
    RI = 0;
    return SBUF;
}
