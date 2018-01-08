#include "ch554.h"
#include "types.h"

#include "ch554_conf.H"                                                      
#include "Delay.H"
#include "usb_endp.h"

#include <string.h>
#include <stdio.h>

uint8_t Count,FLAG;
extern UINT8 Ready;

sbit Ep2InKey = P1^5;


/*键盘数据*/
xdata unsigned char HIDKey[8] = {0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0};


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

void HIDValueHandle()
{
    UINT8 i;
        i = getkey( );
        //printf( "%c", (UINT8)i );
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
    CH554_Init();                                                           //CH559时钟选择配置

    printf("start ...\n");
                                                         //允许单片机中断
    FLAG = 0;
    Ready = 0;
	
    while(1)
    {
        if(Ready)
        {
            HIDValueHandle();
        }
        if(Ready&&(Ep2InKey == 0)){                                       
          printf("ID0 = %02x %02x \n",(UINT16)*(PUINT8C)(0x3FFA),(UINT16)*(PUINT8C)(0x3FFB));
          printf("ID1 = %02x %02x \n",(UINT16)*(PUINT8C)(0x3FFC),(UINT16)*(PUINT8C)(0x3FFD));
          printf("ID2 = %02x %02x \n",(UINT16)*(PUINT8C)(0x3FFE),(UINT16)*(PUINT8C)(0x3FFF));
          mDelaymS( 10 ); 					
          Enp1IntIn();				
        }
        mDelaymS( 100 );                                                   //模拟单片机做其它事
    }
}
