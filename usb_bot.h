/**
  ******************************************************************************
  * @file    usb_bot.h
  * @author  MCD Application Team
  * @version V4.0.0
  * @date    21-January-2013
  * @brief   BOT State Machine management
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT 2013 STMicroelectronics</center></h2>
  *
  * Licensed under MCD-ST Liberty SW License Agreement V2, (the "License");
  * You may not use this file except in compliance with the License.
  * You may obtain a copy of the License at:
  *
  *        http://www.st.com/software_license_agreement_liberty_v2
  *
  * Unless required by applicable law or agreed to in writing, software 
  * distributed under the License is distributed on an "AS IS" BASIS, 
  * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  * See the License for the specific language governing permissions and
  * limitations under the License.
  *
  ******************************************************************************
  */


/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __USB_BOT_H
#define __USB_BOT_H

/* Includes ------------------------------------------------------------------*/
#include "types.h"

// BOT_USB Config
#include "usb_endp.h"
#define BOT_Rx_Buf EP3_RX_BUF
#define BOT_Tx_Buf EP3_TX_BUF
#define BULK_MAX_PACKET_SIZE EP3_SIZE
#define BOT_EP_Tx_RTOG()	{UEP3_CTRL &= ~bUEP_T_TOG;}
#define BOT_EP_Rx_RTOG()	{UEP3_CTRL &= ~bUEP_R_TOG;}
#define BOT_EP_Tx_Valid()	{UEP3_CTRL = UEP3_CTRL & ~MASK_UEP_T_RES | UEP_T_RES_ACK;}
#define BOT_EP_Rx_Valid()	{UEP3_CTRL = UEP3_CTRL & ~MASK_UEP_R_RES | UEP_R_RES_ACK;}
#define BOT_EP_Tx_Stall()	{UEP3_CTRL = UEP3_CTRL & (~bUEP_T_TOG) | UEP_T_RES_STALL;}
#define BOT_EP_Rx_Stall()	{UEP3_CTRL = UEP3_CTRL & (~bUEP_R_TOG) | UEP_R_RES_STALL;}
#define BOT_EP_Tx_Count(val) {UEP3_T_LEN = val;}
#define BOT_EP_Rx_Length	USB_RX_LEN

/* Exported types ------------------------------------------------------------*/
/* Bulk-only Command Block Wrapper */

typedef struct _Bulk_Only_CBW {
	uint8_t dSignature0;
	uint8_t dSignature1;
	uint8_t dSignature2;
	uint8_t dSignature3;

	uint8_t dTag0;
	uint8_t dTag1;
	uint8_t dTag2;
	uint8_t dTag3;

	uint32_t dDataLength;
	uint8_t  bmFlags;
	uint8_t  bLUN;
	uint8_t  bCBLength;
	uint8_t  CB[16];
}
Bulk_Only_CBW;

/* Exported constants --------------------------------------------------------*/

/*****************************************************************************/
/*********************** Bulk-Only Transfer State machine ********************/
/*****************************************************************************/
#define BOT_IDLE                      0		// Idle state
#define BOT_DATA_OUT                  1		// Data Out state
#define BOT_DATA_IN                   2		// Data In state
#define BOT_DATA_IN_LAST              3		// Last Data In Last
#define BOT_CSW_Send                  4		// Command Status Wrapper
#define BOT_ERROR                     5		// Error state
#define BOT_DATA_IN_LAST_FAIL         6		// Send a Check_Status to host

// BOT CBW & CSW Signatures
#define BOT_CBW_PACKET_LENGTH         31
#define BOT_CBW_SIGNATURE0						0x55
#define BOT_CBW_SIGNATURE1						0x53
#define BOT_CBW_SIGNATURE2						0x42
#define BOT_CBW_SIGNATURE3						0x43

#define CSW_DATA_LENGTH               13
#define BOT_CSW_SIGNATURE0						0x55
#define BOT_CSW_SIGNATURE1						0x53
#define BOT_CSW_SIGNATURE2						0x42
#define BOT_CSW_SIGNATURE3						0x53

/* CSW Status Definitions */
#define CSW_CMD_PASSED                0x00
#define CSW_CMD_FAILED                0x01
#define CSW_PHASE_ERROR               0x02

#define SEND_CSW_DISABLE              0
#define SEND_CSW_ENABLE               1

#define DIR_IN                        0
#define DIR_OUT                       1
#define BOTH_DIR                      2

/* Exported macro ------------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */
void Bot_MSR(void);
uint8_t Bot_Get_Max_Lun(void);

void Mass_Storage_In (void);
void Mass_Storage_Out (void);
void CBW_Decode(void);
void Transfer_Data_Request(uint8_t* Data_Pointer, uint8_t Data_Len);
void Reply_Request(uint8_t Data_Len);
void Transfer_Failed_ReadWrite(void);
void Set_CSW (uint8_t CSW_Status, uint8_t Send_Permission);
void Bot_Abort(uint8_t Direction);

#endif /* __USB_BOT_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/

