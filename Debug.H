/* 调试 */
/* 提供printf子程序和延时函数 */
/* CH554主频修改、延时函数定义
   串口0和串口1初始化
   串口0和串口1的收发子函数
   看门狗初始化	
*/

#ifndef	__DEBUG_H__
#define __DEBUG_H__

//定义函数返回值
#ifndef  SUCCESS
#define  SUCCESS  0
#endif
#ifndef  FAIL
#define  FAIL    0xFF
#endif

//定义定时器起始
#ifndef  START
#define  START  1
#endif
#ifndef  STOP
#define  STOP    0
#endif

#ifndef  DE_PRINTF
#define  DE_PRINTF     0
#endif
#define	 FREQ_SYS	     12000000	         //系统主频12MHz
#ifndef  UART0_BUAD
#define  UART0_BUAD    57600
#define  UART1_BUAD    57600
#endif

void	mDelayuS( UINT16 n );              // 以uS为单位延时
void	mDelaymS( UINT16 n );              // 以mS为单位延时
void	mInitSTDIO( );                      //T1作为波特率发生器
#endif
