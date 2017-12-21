
/********************************** (C) COPYRIGHT *******************************
* File Name          : CompositeKM.C
* Author             : WCH
* Version            : V1.2 
* Date               : 2017/09/05
* Description        : CH559模拟USB复合设备，键鼠，支持类命令,支持唤醒
*******************************************************************************/

#include "CH554.H"                                                      
#include "Debug.H"
#include <string.h>
#include <stdio.h>

#define THIS_ENDP0_SIZE         DEFAULT_ENDP0_SIZE

UINT8X  Ep0Buffer[8>(THIS_ENDP0_SIZE+2)?8:(THIS_ENDP0_SIZE+2)] _at_ 0x0000;    //端点0 OUT&IN缓冲区，必须是偶地址
UINT8X  Ep1Buffer[64>(MAX_PACKET_SIZE+2)?64:(MAX_PACKET_SIZE+2)] _at_ 0x000a;  //端点1 IN缓冲区,必须是偶地址
UINT8X  Ep2Buffer[128>(2*MAX_PACKET_SIZE+4)?128:(2*MAX_PACKET_SIZE+4)] _at_ 0x0050;  //端点2 IN缓冲区,必须是偶地址


UINT8   SetupReq,SetupLen,Ready,Count,FLAG,UsbConfig;
PUINT8  pDescr;                                                                //USB配置标志
USB_SETUP_REQ   SetupReqBuf;                                                   //暂存Setup包
sbit Ep2InKey = P1^5;



#define UsbSetupBuf     ((PUSB_SETUP_REQ)Ep0Buffer)
#define DEBUG 1
#pragma  NOAREGS


/* 设备描述符 */
UINT8C DevDesc[18] = {	
	18,		// Total length
	0x01,	// Type: Device Descriptor
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
UINT8C CfgDesc[] =
{
	// Configuration Descriptor
	9,					// Length of the descriptor
	0x02,				// Type: Configuration Descriptor
	66, 0x00,		// Total length of this and following descriptors
	0x02,				// Number of interfaces
	0x01, 			// Value used to select this configuration
	0x00,				// Index of corresponding string descriptor
	0xA0,				// Attributes, D7 must be 1, D6 Self-powered, D5 Remote Wakeup, D4-D0=0
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
	0x3e, 0x00,	// wItemLength: Total length of Report descriptor
	
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
	
	// HID descriptor (Mouse)
	9,				// Length of the descriptor
	0x21,				// HID: Interface Descriptor
	0x10, 0x01,	// bcdHID: HID Class Spec release number
	0x00,				// bCountryCode: Hardware target country
	0x01,				// bNumDescriptors: Number of HID class descriptors to follow
	0x22,				// bDescriptorType
	0x34, 0x00,	// wItemLength: Total length of Report descriptor
	
	// Endpoint descriptor (Mouse)	// EP2, IN
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
	
	// Endpoint descriptor (Mouse)	// EP2, OUT
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

/*字符串描述符*/
/*HID类报表描述符*/
UINT8C KeyRepDesc[62] =
{
    0x05,0x01,0x09,0x06,0xA1,0x01,0x05,0x07,
    0x19,0xe0,0x29,0xe7,0x15,0x00,0x25,0x01,
    0x75,0x01,0x95,0x08,0x81,0x02,0x95,0x01,
    0x75,0x08,0x81,0x01,0x95,0x03,0x75,0x01,
    0x05,0x08,0x19,0x01,0x29,0x03,0x91,0x02,
    0x95,0x05,0x75,0x01,0x91,0x01,0x95,0x06,
    0x75,0x08,0x26,0xff,0x00,0x05,0x07,0x19,
    0x00,0x29,0x91,0x81,0x00,0xC0
};

//gliethttp
UINT8C MouseRepDesc[52] =
{
  0x06,	0xA0,	0xFF,	// Usage Page(FFA0h, vendor defined)
  0x09, 0x01,				// Usage(vendor defined)
  0xA1, 0x01,				// Collection(Application)
  0x09, 0x02 ,			// Usage(vendor defined)
		0xA1, 0x00,				// Collection(Physical)
		0x06,0xA1,0xFF,		// Usage Page(vendor defined)

		// Input Report
		0x09, 0x03 ,	// Usage(vendor defined)
		0x09, 0x04,		// Usage(vendor defined)
		0x15, 0x80,		// Logical Minimum(0x80 or -128)
		0x25, 0x7F,		// Logical Maximum(0x7F or 127)
		0x35, 0x00,		// Physical Minimum(0)
		0x45,	0xFF,		// Physical Maximum(255)
		0x75, 0x08,		// Report size(8 Bits, 1 Byte)
		0x95, 0x40,		// Report count(64 fields)
		0x81, 0x02,		// Input(data, variable, absolute)
		// Output Report
		0x09, 0x05,		// Usage(vendor defined)
		0x09, 0x06,		// Usage(vendor defined)
		0x15, 0x80,		// Logical Minimum(0x80 or -128)
		0x25, 0x7F,		// Logical Maximum(0x7F or 127)
		0x35, 0x00,		// Physical Minimum(0)
		0x45,0xFF,		// Physical Maximum(255)
		0x75,0x08,		// Report size(8 Bits, 1 Byte)
		0x95, 0x40,		// Report count(64 fields)
		0x91, 0x02,		// Output(data, variable, absolute)
		0xC0,					//End Collection(Physical)
  0xC0				//End Collection(Application)
};

UINT8C CustomHID_StringSerial[26] =
  {
    26,           // Length of the descriptor
    0x03,					// Type: String Descriptor
    'S', 0, 'T', 0, 'M', 0,'3', 0,'2', 0
  };

/*鼠标数据*/
//UINT8 HIDMouse[4] = {0x0,0x0,0x0,0x0};
/*键盘数据*/
UINT8 HIDKey[8] = {0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0};

/*******************************************************************************
* Function Name  : CH554SoftReset()
* Description    : CH554软复位
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void CH554SoftReset( )
{
    SAFE_MOD = 0x55;
    SAFE_MOD = 0xAA;
    GLOBAL_CFG	|=bSW_RESET;
}

/*******************************************************************************
* Function Name  : CH554USBDevWakeup()
* Description    : CH554设备模式唤醒主机，发送K信号
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void CH554USBDevWakeup( )
{
  UDEV_CTRL |= bUD_LOW_SPEED;
  mDelaymS(2);
  UDEV_CTRL &= ~bUD_LOW_SPEED;	
}

/*******************************************************************************
* Function Name  : USBDeviceInit()
* Description    : USB设备模式配置,设备模式启动，收发端点配置，中断开启
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void USBDeviceInit()
{
	  IE_USB = 0;						// Disable USB interrupt (for setup)
	  USB_CTRL = 0x00;			// Device mode, Full speed(12Mbps), Disable pull up(Invisible to host), Reset stuff
		//UDEV_CTRL = bUD_PD_DIS;		//Disable DP/DN pull down resistor
	
		//Configure Endpoint 0
    UEP0_DMA = Ep0Buffer; // Address of Endpoint 2 buffer
		// bUEP4_RX_EN and bUEP4_TX_EN controls both Endpoint 0 and 4, refer to table 16.3.2
		UEP4_1_MOD &= ~(bUEP4_RX_EN | bUEP4_TX_EN);		// EP0 64-byte buffer
		UEP0_CTRL = UEP_R_RES_ACK | UEP_T_RES_NAK;		// SETUP/OUT -> ACK, IN -> NAK
    
		//Configure Endpoint 1
		UEP1_DMA = Ep1Buffer; // Address of Endpoint 1 buffer
    //UEP4_1_MOD = UEP4_1_MOD & ~bUEP1_BUF_MOD | bUEP1_TX_EN;
		UEP4_1_MOD &=~bUEP1_BUF_MOD;	//See table 16.3.3
		UEP4_1_MOD |= bUEP1_TX_EN;		// EP1 Tx only 																端点1发送使能 64字节缓冲区
    UEP1_CTRL = bUEP_AUTO_TOG | UEP_T_RES_NAK;	                                //端点1自动翻转同步标志位，IN事务返回NAK		
	
		//Configure Endpoint 2
    UEP2_DMA = Ep2Buffer;	// Address of Endpoint 2 buffer
    //UEP2_3_MOD = UEP2_3_MOD & ~bUEP2_BUF_MOD | bUEP2_TX_EN;
		UEP2_3_MOD &=~bUEP2_BUF_MOD;	// Disable Endpoint2 buffer mode 
		UEP2_3_MOD |= bUEP2_TX_EN;		// Enable Endpoint2 Tx
		UEP2_3_MOD |= bUEP2_RX_EN;		// Enable Endpoint2 Rx
    UEP2_CTRL = bUEP_AUTO_TOG | UEP_T_RES_NAK | UEP_R_RES_ACK;                                 //端点2自动翻转同步标志位，IN事务返回NAK
	

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
}
/*******************************************************************************
* Function Name  : Enp1IntIn()
* Description    : USB设备模式端点1的中断上传
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void Enp1IntIn( )
{
    memcpy( Ep1Buffer, HIDKey, sizeof(HIDKey));                              //加载上传数据
    UEP1_T_LEN = sizeof(HIDKey);                                             //上传数据长度
    UEP1_CTRL = UEP1_CTRL & ~ MASK_UEP_T_RES | UEP_T_RES_ACK;                //有数据时上传数据并应答ACK
}
/*******************************************************************************
* Function Name  : Enp2IntIn()
* Description    : USB设备模式端点2的中断上传
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void Enp2IntIn( )
{
    //memcpy( Ep2Buffer, HIDMouse, sizeof(HIDMouse));                              //加载上传数据
    //UEP2_T_LEN = sizeof(HIDMouse);                                              //上传数据长度
    //UEP2_CTRL = UEP2_CTRL & ~ MASK_UEP_T_RES | UEP_T_RES_ACK;                  //有数据时上传数据并应答ACK
}
/*******************************************************************************
* Function Name  : DeviceInterrupt()
* Description    : CH559USB中断处理函数
*******************************************************************************/
void    DeviceInterrupt( void ) interrupt INT_NO_USB using 1                      //USB中断服务程序,使用寄存器组1
{
    UINT8 len = 0;
		UINT8 i;
    if(UIF_TRANSFER)                                                            //USB传输完成标志
    {
        switch (USB_INT_ST & (MASK_UIS_TOKEN | MASK_UIS_ENDP))
        {
        case UIS_TOKEN_IN | 2:                                                  //endpoint 2# 中断端点上传
            UEP2_T_LEN = 0;                                                     //预使用发送长度一定要清空
//            UEP1_CTRL ^= bUEP_T_TOG;                                          //如果不设置自动翻转则需要手动翻转
            UEP2_CTRL = UEP2_CTRL & ~ MASK_UEP_T_RES | UEP_T_RES_NAK;           //默认应答NAK
            break;
				
        case UIS_TOKEN_OUT | 2:                                                 //endpoint 2# 端点批量下传
            if ( U_TOG_OK )                                                     // 不同步的数据包将丢弃
            {
                len = USB_RX_LEN;                                               //接收数据长度，数据从Ep2Buffer首地址开始存放
                for ( i = 0; i < len; i ++ )
                {
                    Ep2Buffer[MAX_PACKET_SIZE+i] = Ep2Buffer[i] ^ 0xFF;         // OUT数据取反到IN由计算机验证
                }
                UEP2_T_LEN = len;
                UEP2_CTRL = UEP2_CTRL & ~ MASK_UEP_T_RES | UEP_T_RES_ACK;       // 允许上传
            }
            break;
        case UIS_TOKEN_IN | 1:                                                  //endpoint 1# 中断端点上传
            UEP1_T_LEN = 0;                                                     //预使用发送长度一定要清空
//            UEP2_CTRL ^= bUEP_T_TOG;                                          //如果不设置自动翻转则需要手动翻转
            UEP1_CTRL = UEP1_CTRL & ~ MASK_UEP_T_RES | UEP_T_RES_NAK;           //默认应答NAK
            FLAG = 1;                                                           /*传输完成标志*/
            break;
        case UIS_TOKEN_SETUP | 0:                                                //SETUP事务
            len = USB_RX_LEN;
            if(len == (sizeof(USB_SETUP_REQ)))
            {
                SetupLen = UsbSetupBuf->wLengthL;
                if(UsbSetupBuf->wLengthH || SetupLen > 0x7F )
                {
                    SetupLen = 0x7F;    // 限制总长度
                }
                len = 0;                                                        // 默认为成功并且上传0长度
                SetupReq = UsbSetupBuf->bRequest;								
                if ( ( UsbSetupBuf->bRequestType & USB_REQ_TYP_MASK ) != USB_REQ_TYP_STANDARD )/* HID类命令 */
                {
									switch( SetupReq ) 
									{
										case 0x01: // GetReport
												 break;
										case 0x02: // GetIdle
												 break;	
										case 0x03: // GetProtocol
												 break;				
										case 0x09: // SetReport										
												 break;
										case 0x0A: // SetIdle
												 break;	
										case 0x0B: // SetProtocol
												 break;
										default:
												 len = 0xFF;  								 					            /*命令不支持*/					
												 break;
								  }	
                }
                else
                { // 标准请求
                    switch(SetupReq)                                        //请求码
                    {
                    case USB_GET_DESCRIPTOR:
                        switch(UsbSetupBuf->wValueH)
                        {
                        case 1:                                             //设备描述符
                            pDescr = DevDesc;                               //把设备描述符送到要发送的缓冲区
                            len = sizeof(DevDesc);
                            break;
                        case 2:                                             //配置描述符
                            pDescr = CfgDesc;                               //把设备描述符送到要发送的缓冲区
                            len = sizeof(CfgDesc);
                            break;
                        case 0x22:                                          //报表描述符
                            if(UsbSetupBuf->wIndexL == 0)                   //接口0报表描述符
                            {
															  pDescr = MouseRepDesc;                      //数据准备上传
                                len = sizeof(MouseRepDesc);

                            }
                            else if(UsbSetupBuf->wIndexL == 1)              //接口1报表描述符
                            {
                                pDescr = KeyRepDesc;                        //数据准备上传
                                len = sizeof(KeyRepDesc);
                                Ready = 1;                                  //如果有更多接口，该标准位应该在最后一个接口配置完成后有效
                            }
                            else
                            {
                                len = 0xff;                                 //本程序只有2个接口，这句话正常不可能执行
                            }
                            break;
                        default:
                            len = 0xff;                                     //不支持的命令或者出错
                            break;
                        }
                        if ( SetupLen > len )
                        {
                            SetupLen = len;    // 限制总长度
                        }
                        len = SetupLen >= 8 ? 8 : SetupLen;                  //本次传输长度
                        memcpy(Ep0Buffer,pDescr,len);                        //加载上传数据
                        SetupLen -= len;
                        pDescr += len;
                        break;
                    case USB_SET_ADDRESS:
                        SetupLen = UsbSetupBuf->wValueL;                     //暂存USB设备地址
                        break;
                    case USB_GET_CONFIGURATION:
                        Ep0Buffer[0] = UsbConfig;
                        if ( SetupLen >= 1 )
                        {
                            len = 1;
                        }
                        break;
                    case USB_SET_CONFIGURATION:
                        UsbConfig = UsbSetupBuf->wValueL;
                        break;
                    case 0x0A:
                        break;
                    case USB_CLEAR_FEATURE:                                            //Clear Feature
                        if ( ( UsbSetupBuf->bRequestType & USB_REQ_RECIP_MASK ) == USB_REQ_RECIP_ENDP )// 端点
                        {
                            switch( UsbSetupBuf->wIndexL )
                            {
                            case 0x82:
                                UEP2_CTRL = UEP2_CTRL & ~ ( bUEP_T_TOG | MASK_UEP_T_RES ) | UEP_T_RES_NAK;
                                break;
                            case 0x81:
                                UEP1_CTRL = UEP1_CTRL & ~ ( bUEP_T_TOG | MASK_UEP_T_RES ) | UEP_T_RES_NAK;
                                break;
                            case 0x02:
                                UEP2_CTRL = UEP2_CTRL & ~ ( bUEP_R_TOG | MASK_UEP_R_RES ) | UEP_R_RES_ACK;
                                break;
                            default:
                                len = 0xFF;                                            // 不支持的端点
                                break;
                            }
                        }
                        if ( ( UsbSetupBuf->bRequestType & USB_REQ_RECIP_MASK ) == USB_REQ_RECIP_DEVICE )// 设备
                        {
													break;
                        }													
                        else
                        {
                            len = 0xFF;                                                // 不是端点不支持
                        }
                        break;
                    case USB_SET_FEATURE:                                              /* Set Feature */
                        if( ( UsbSetupBuf->bRequestType & 0x1F ) == 0x00 )             /* 设置设备 */
                        {
                            if( ( ( ( UINT16 )UsbSetupBuf->wValueH << 8 ) | UsbSetupBuf->wValueL ) == 0x01 )
                            {
                                if( CfgDesc[ 7 ] & 0x20 )
                                {
                                    /* 设置唤醒使能标志 */
                                }
                                else
                                {
                                    len = 0xFF;                                        /* 操作失败 */
                                }
                            }
                            else
                            {
                                len = 0xFF;                                            /* 操作失败 */
                            }
                        }
                        else if( ( UsbSetupBuf->bRequestType & 0x1F ) == 0x02 )        /* 设置端点 */
                        {
                            if( ( ( ( UINT16 )UsbSetupBuf->wValueH << 8 ) | UsbSetupBuf->wValueL ) == 0x00 )
                            {
                                switch( ( ( UINT16 )UsbSetupBuf->wIndexH << 8 ) | UsbSetupBuf->wIndexL )
                                {
                                case 0x82:
                                    UEP2_CTRL = UEP2_CTRL & (~bUEP_T_TOG) | UEP_T_RES_STALL;/* 设置端点2 IN STALL */
                                    break;
                                case 0x02:
                                    UEP2_CTRL = UEP2_CTRL & (~bUEP_R_TOG) | UEP_R_RES_STALL;/* 设置端点2 OUT Stall */
                                    break;
                                case 0x81:
                                    UEP1_CTRL = UEP1_CTRL & (~bUEP_T_TOG) | UEP_T_RES_STALL;/* 设置端点1 IN STALL */
                                    break;
                                default:
                                    len = 0xFF;                               //操作失败
                                    break;
                                }
                            }
                            else
                            {
                                len = 0xFF;                                   //操作失败
                            }
                        }
                        else
                        {
                            len = 0xFF;                                      //操作失败
                        }
                        break;
                    case USB_GET_STATUS:
                        Ep0Buffer[0] = 0x00;
                        Ep0Buffer[1] = 0x00;
                        if ( SetupLen >= 2 )
                        {
                            len = 2;
                        }
                        else
                        {
                            len = SetupLen;
                        }
                        break;
                    default:
                        len = 0xff;                                           //操作失败
                        break;
                    }
                }
            }
            else
            {
                len = 0xff;                                                   //包长度错误
            }
            if(len == 0xff)
            {
                SetupReq = 0xFF;
                UEP0_CTRL = bUEP_R_TOG | bUEP_T_TOG | UEP_R_RES_STALL | UEP_T_RES_STALL;//STALL
            }
            else if(len)                                                //上传数据或者状态阶段返回0长度包
            {
                UEP0_T_LEN = len;
                UEP0_CTRL = bUEP_R_TOG | bUEP_T_TOG | UEP_R_RES_ACK | UEP_T_RES_ACK;//默认数据包是DATA1，返回应答ACK
            }
            else
            {
                UEP0_T_LEN = 0;  //虽然尚未到状态阶段，但是提前预置上传0长度数据包以防主机提前进入状态阶段
                UEP0_CTRL = bUEP_R_TOG | bUEP_T_TOG | UEP_R_RES_ACK | UEP_T_RES_ACK;//默认数据包是DATA1,返回应答ACK
            }
            break;
        case UIS_TOKEN_IN | 0:                                               //endpoint0 IN
            switch(SetupReq)
            {
            case USB_GET_DESCRIPTOR:
                len = SetupLen >= 8 ? 8 : SetupLen;                          //本次传输长度
                memcpy( Ep0Buffer, pDescr, len );                            //加载上传数据
                SetupLen -= len;
                pDescr += len;
                UEP0_T_LEN = len;
                UEP0_CTRL ^= bUEP_T_TOG;                                     //同步标志位翻转
                break;
            case USB_SET_ADDRESS:
                USB_DEV_AD = bUDA_GP_BIT | SetupLen;
                UEP0_CTRL = UEP_R_RES_ACK | UEP_T_RES_NAK;
                break;
            default:
                UEP0_T_LEN = 0;                                              //状态阶段完成中断或者是强制上传0长度数据包结束控制传输
                UEP0_CTRL = UEP_R_RES_ACK | UEP_T_RES_NAK;
                break;
            }
            break;
        case UIS_TOKEN_OUT | 0:  // endpoint0 OUT
            len = USB_RX_LEN;
            if(SetupReq == 0x09)
            {
                if(Ep0Buffer[0])
                {
                    printf("Light on Num Lock LED!\n");
                }
                else if(Ep0Buffer[0] == 0)
                {
                    printf("Light off Num Lock LED!\n");
                }				
            }
            UEP0_CTRL ^= bUEP_R_TOG;                                     //同步标志位翻转						
            break;
        default:
            break;
        }
        UIF_TRANSFER = 0;                                                 //写0清空中断
    }
    if(UIF_BUS_RST)                                                       //设备模式USB总线复位中断
    {
        UEP0_CTRL = UEP_R_RES_ACK | UEP_T_RES_NAK;
        UEP1_CTRL = bUEP_AUTO_TOG | UEP_R_RES_ACK;
        UEP2_CTRL = bUEP_AUTO_TOG | UEP_R_RES_ACK | UEP_T_RES_NAK;
        USB_DEV_AD = 0x00;
        UIF_SUSPEND = 0;
        UIF_TRANSFER = 0;
        UIF_BUS_RST = 0;                                                 //清中断标志
    }
    if (UIF_SUSPEND)                                                     //USB总线挂起/唤醒完成
    {
        UIF_SUSPEND = 0;
        if ( USB_MIS_ST & bUMS_SUSPEND )                                 //挂起
        {
#if DEBUG
            printf( "zz" );                                              //睡眠状态
#endif
//             while ( XBUS_AUX & bUART0_TX );                              //等待发送完成
//             SAFE_MOD = 0x55;
//             SAFE_MOD = 0xAA;
//             WAKE_CTRL = bWAK_BY_USB | bWAK_RXD0_LO;                      //USB或者RXD0有信号时可被唤醒
//             PCON |= PD;                                                  //睡眠
//             SAFE_MOD = 0x55;
//             SAFE_MOD = 0xAA;
//             WAKE_CTRL = 0x00;
        }
    }
    else {                                                               //意外的中断,不可能发生的情况
        USB_INT_FG = 0xFF;                                               //清中断标志
//      printf("UnknownInt  N");
    }
}
void HIDValueHandle()
{
    UINT8 i;
        i = getkey( );
        printf( "%c", (UINT8)i );
        switch(i)
        {
//键盘数据上传示例
        case 'A':                                                         //A键
            FLAG = 0;
            HIDKey[2] = 0xf8;                                             //按键开始
            Enp1IntIn();
            HIDKey[2] = 0;        	                                       //按键结束
            while(FLAG == 0)
            {
                ;    /*等待上一包传输完成*/
            }
            Enp1IntIn();
            break;
        case 'P':                                                         //P键
            FLAG = 0;
            HIDKey[2] = 0x59;
            Enp1IntIn();
            HIDKey[2] = 0;                                                //按键结束
            while(FLAG == 0)
            {
                ;    /*等待上一包传输完成*/
            }
            Enp1IntIn();
            break;
        case 'Q':                                                         //Num Lock键
            FLAG = 0;
            HIDKey[2] = 0x53;
            Enp1IntIn();
            HIDKey[2] = 0;                                                //按键结束
            while(FLAG == 0)
            {
                ;    /*等待上一包传输完成*/
            }
            Enp1IntIn();
            break;
        default:                                                          //其他
            UEP1_CTRL = UEP1_CTRL & ~ MASK_UEP_T_RES | UEP_T_RES_NAK;     //默认应答NAK
            UEP2_CTRL = UEP2_CTRL & ~ MASK_UEP_T_RES | UEP_T_RES_NAK;     //默认应答NAK
            break;
        }
}

main()
{
    CfgFsys( );                                                           //CH559时钟选择配置
    mDelaymS(5);                                                          //修改主频等待内部晶振稳定,必加	
    mInitSTDIO( );                                                        //串口0初始化
//#ifdef DE_PRINTF
    printf("start ...\n");
//#endif	
    USBDeviceInit();                                                      //USB设备模式初始化
    EA = 1;                                                               //允许单片机中断
    UEP1_T_LEN = 0;                                                       //预使用发送长度一定要清空
    UEP2_T_LEN = 0;                                                       //预使用发送长度一定要清空
    FLAG = 0;
    Ready = 0;
	
    while(1)
    {
        if(Ready)
        {
            HIDValueHandle();
        }
        if(Ready&&(Ep2InKey == 0)){                                       
//#ifdef DE_PRINTF                                                          //读取芯片ID号
          printf("ID0 = %02x %02x \n",(UINT16)*(PUINT8C)(0x3FFA),(UINT16)*(PUINT8C)(0x3FFB));
          printf("ID1 = %02x %02x \n",(UINT16)*(PUINT8C)(0x3FFC),(UINT16)*(PUINT8C)(0x3FFD));
          printf("ID2 = %02x %02x \n",(UINT16)*(PUINT8C)(0x3FFE),(UINT16)*(PUINT8C)(0x3FFF));
//#endif	   							
          CH554USBDevWakeup();                                           //	
          mDelaymS( 10 ); 					
          Enp1IntIn();				
        }
        mDelaymS( 100 );                                                   //模拟单片机做其它事
    }
}
