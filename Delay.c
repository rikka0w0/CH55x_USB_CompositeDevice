/********************************** (C) COPYRIGHT *******************************
* File Name          : Debug.C
* Author             : WCH
* Version            : V1.1
* Date               : 2017/09/05
* Description        : CH554 DEBUG Interface
                     CH554��Ƶ�޸ġ���ʱ��������
                     ����0�ʹ���1��ʼ��
                     ����0�ʹ���1���շ��Ӻ���
                     ���Ź���ʼ��
*******************************************************************************/

#include "Delay.h"

/*******************************************************************************
* Function Name  : mDelayus(UNIT16 n)
* Description    : us��ʱ����
* Input          : UNIT16 n
* Output         : None
* Return         : None
*******************************************************************************/
void	mDelayuS(unsigned short n )  // ��uSΪ��λ��ʱ
{
#ifdef	CLOCK_CFG
#if		CLOCK_CFG <= 6000000
		n >>= 2;
#endif
#if		CLOCK_CFG <= 3000000
		n >>= 2;
#endif
#if		CLOCK_CFG <= 750000
		n >>= 4;
#endif
#endif
	while ( n ) {  // total = 12~13 Fsys cycles, 1uS @Fsys=12MHz
		++ SAFE_MOD;  // 2 Fsys cycles, for higher Fsys, add operation here
#ifdef	CLOCK_CFG
#if		CLOCK_CFG >= 14000000
		++ SAFE_MOD;
#endif
#if		CLOCK_CFG >= 16000000
		++ SAFE_MOD;
#endif
#if		CLOCK_CFG >= 18000000
		++ SAFE_MOD;
#endif
#if		CLOCK_CFG >= 20000000
		++ SAFE_MOD;
#endif
#if		CLOCK_CFG >= 22000000
		++ SAFE_MOD;
#endif
#if		CLOCK_CFG >= 24000000
		++ SAFE_MOD;
#endif
#if		CLOCK_CFG >= 26000000
		++ SAFE_MOD;
#endif
#if		CLOCK_CFG >= 28000000
		++ SAFE_MOD;
#endif
#if		CLOCK_CFG >= 30000000
		++ SAFE_MOD;
#endif
#if		CLOCK_CFG >= 32000000
		++ SAFE_MOD;
#endif
#endif
		-- n;
	}
}

/*******************************************************************************
* Function Name  : mDelayms(UNIT16 n)
* Description    : ms��ʱ����
* Input          : UNIT16 n
* Output         : None
* Return         : None
*******************************************************************************/
void	mDelaymS(unsigned short n )                                                  // ��mSΪ��λ��ʱ
{
	while ( n ) {
#ifdef	DELAY_MS_HW
		while ( ( TKEY_CTRL & bTKC_IF ) == 0 );
		while ( TKEY_CTRL & bTKC_IF );
#else
		mDelayuS( 1000 );
#endif
		-- n;
	}
}
