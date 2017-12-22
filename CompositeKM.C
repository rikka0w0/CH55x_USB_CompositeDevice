#include "types.h"
#include "CH554.H"                                                      
#include "Debug.H"
#include "usb_desc.h"
#include "usb_conf.h"
#include <string.h>
#include <stdio.h>

extern xdata uint8_t Ep0Buffer[];

xdata uint8_t  Ep1Buffer[64>(MAX_PACKET_SIZE+2)?64:(MAX_PACKET_SIZE+2)] _at_ 0x000a;  //端点1 IN缓冲区,必须是偶地址
//0x42
xdata uint8_t  Ep2Buffer[128>(2*MAX_PACKET_SIZE+4)?128:(2*MAX_PACKET_SIZE+4)] _at_ 0x0050;  //端点2 IN缓冲区,必须是偶地址
//0x84

uint8_t Count,FLAG;
extern UINT8 Ready;

sbit Ep2InKey = P1^5;


#define DEBUG 1
#pragma  NOAREGS





UINT8C CustomHID_StringSerial[26] =
  {
    26,           // Length of the descriptor
    0x03,					// Type: String Descriptor
    'S', 0, 'T', 0, 'M', 0,'3', 0,'2', 0
  };


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

void EP2Out(uint8_t len) {
		UINT8 i;
	                for ( i = 0; i < len; i ++ )
                {
                    Ep2Buffer[MAX_PACKET_SIZE+i] = Ep2Buffer[i] ^ 0xFF;         // OUT数据取反到IN由计算机验证
                }
                UEP2_T_LEN = len;
                UEP2_CTRL = UEP2_CTRL & ~ MASK_UEP_T_RES | UEP_T_RES_ACK;       // 允许上传
}

void USB_EP2_OUT(void) {
	            if ( U_TOG_OK )                                                     // 不同步的数据包将丢弃
            {
								EP2Out(USB_RX_LEN);
            }
}

void USB_EP1_IN(void) {
  UEP1_T_LEN = 0;                                                     //预使用发送长度一定要清空
//UEP2_CTRL ^= bUEP_T_TOG;                                          //如果不设置自动翻转则需要手动翻转
  UEP1_CTRL = UEP1_CTRL & ~ MASK_UEP_T_RES | UEP_T_RES_NAK;           //默认应答NAK
  FLAG = 1;   	
}

void USB_EP2_IN(void) {
  UEP2_T_LEN = 0;                                                     //预使用发送长度一定要清空
//UEP1_CTRL ^= bUEP_T_TOG;                                          //如果不设置自动翻转则需要手动翻转
  UEP2_CTRL = UEP2_CTRL & ~ MASK_UEP_T_RES | UEP_T_RES_NAK;           //默认应答NAK	
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
            HIDKey[2] = 0x04;                                             //按键开始
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
