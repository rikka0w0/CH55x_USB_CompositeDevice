/**
  ******************************************************************************
  * @file    usb_bot.c
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


/* Includes ------------------------------------------------------------------*/
#include "usb_scsi.h"
#include "usb_bot.h"
#include "usb_mal.h"

#include "usb_endp.h"
#include "types.h"
#include "ch554.h"
#include <String.h>


/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
uint8_t Bot_State = BOT_IDLE;
uint16_t Data_Len;
xdata Bulk_Only_CBW CBW;
xdata Bulk_Only_CSW CSW;
uint32_t SCSI_LBA;
uint16_t SCSI_BlkLen;
/* Extern variables ----------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Extern function prototypes ------------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/*******************************************************************************
* Function Name  : Mass_Storage_In
* Description    : Mass Storage IN transfer.
* Input          : None.
* Output         : None.
* Return         : None.
*******************************************************************************/
void Mass_Storage_In (void)
{
  switch (Bot_State)
  {
    case BOT_CSW_Send:
    case BOT_ERROR:
      Bot_State = BOT_IDLE;
      //SetEPRxStatus(ENDP2, EP_RX_VALID);/* enable the Endpoint to receive the next cmd*/	
			UEP3_CTRL = UEP3_CTRL & ~MASK_UEP_R_RES | UEP_R_RES_ACK;
      
      break;
    case BOT_DATA_IN:
			if (CBW.CB[0] == SCSI_READ10) {
        SCSI_Read10_Cmd(CBW.bLUN , SCSI_LBA , SCSI_BlkLen);
			}
      break;
    case BOT_DATA_IN_LAST:
      Set_CSW (CSW_CMD_PASSED, SEND_CSW_ENABLE);

      // SetEPRxStatus(ENDP2, EP_RX_VALID);
			UEP3_CTRL = UEP3_CTRL & ~MASK_UEP_R_RES | UEP_R_RES_ACK;
      break;
    default:
      break;
  }
}

/*******************************************************************************
* Function Name  : Mass_Storage_Out
* Description    : Mass Storage OUT transfer.
* Input          : None.
* Output         : None.
* Return         : None.
*******************************************************************************/
void Mass_Storage_Out (void)
{
  uint8_t CMD;

	Data_Len = USB_RX_LEN;	// EP3_RX_BUF
	
  switch (Bot_State)
  {
    case BOT_IDLE:
      CBW_Decode();
      break;
    case BOT_DATA_OUT:
      if (CBW.CB[0] == SCSI_WRITE10) {
        SCSI_Write10_Cmd(CBW.bLUN , SCSI_LBA , SCSI_BlkLen);
      } else {
				Bot_Abort(DIR_OUT);
				Set_Scsi_Sense_Data(CBW.bLUN, ILLEGAL_REQUEST, INVALID_FIELED_IN_COMMAND);
				Set_CSW (CSW_PHASE_ERROR, SEND_CSW_DISABLE);				
			}

      break;
    default:
      Bot_Abort(BOTH_DIR);
      Set_Scsi_Sense_Data(CBW.bLUN, ILLEGAL_REQUEST, INVALID_FIELED_IN_COMMAND);
      Set_CSW (CSW_PHASE_ERROR, SEND_CSW_DISABLE);
      break;
  }
}

void L2B32(void* dest, const void* src, uint8_t offset) {
	((uint8_t*)dest)[3] = ((uint8_t*)src)[offset++];
	((uint8_t*)dest)[2] = ((uint8_t*)src)[offset++];
	((uint8_t*)dest)[1] = ((uint8_t*)src)[offset++];
	((uint8_t*)dest)[0] = ((uint8_t*)src)[offset];
}

/*******************************************************************************
* Function Name  : CBW_Decode
* Description    : Decode the received CBW and call the related SCSI command
*                 routine.
* Input          : None.
* Output         : None.
* Return         : None.
*******************************************************************************/
void CBW_Decode(void)
{	
	uint8_t i;

	// Copy CBW from Endpoint Rx Buffer
	for (i = 0; i < USB_RX_LEN; i++)
		((uint8_t*)(&CBW))[i] = EP3_RX_BUF[i];
	
	L2B32(&(CBW.dDataLength), EP3_RX_BUF, 8);
	
	CSW.dTag = CBW.dTag;
  CSW.dDataResidue = CBW.dDataLength;
	
  if (Data_Len != BOT_CBW_PACKET_LENGTH)
  {
    Bot_Abort(BOTH_DIR);
    /* reset the CBW.dSignature to disable the clear feature until receiving a Mass storage reset*/
    CBW.dSignature = 0;
    Set_Scsi_Sense_Data(CBW.bLUN, ILLEGAL_REQUEST, PARAMETER_LIST_LENGTH_ERROR);
    Set_CSW (CSW_CMD_FAILED, SEND_CSW_DISABLE);
		
    return;
  }
	
   if ((CBW.CB[0] == SCSI_READ10 ) || (CBW.CB[0] == SCSI_WRITE10 ))
   {
     /* Calculate Logical Block Address */
		// SCSI_LBA = (CBW.CB[2] << 24) | (CBW.CB[3] << 16) | (CBW.CB[4] <<  8) | CBW.CB[5];
     ((uint8_t*)&SCSI_LBA)[0] = CBW.CB[2];
 		((uint8_t*)&SCSI_LBA)[1] = CBW.CB[3];
 		((uint8_t*)&SCSI_LBA)[2] = CBW.CB[4];
 		((uint8_t*)&SCSI_LBA)[3] = CBW.CB[5];
 		
     /* Calculate the Number of Blocks to transfer */
		// SCSI_BlkLen = (CBW.CB[7] <<  8) | CBW.CB[8];
		((uint8_t*)&SCSI_BlkLen)[0] = CBW.CB[7];
		((uint8_t*)&SCSI_BlkLen)[1] = CBW.CB[8];
   }
	
  if (CBW.dSignature == BOT_CBW_SIGNATURE)
  {
    /* Valid CBW */
    if ((CBW.bLUN > MAL_MAX_LUN) || (CBW.bCBLength < 1) || (CBW.bCBLength > 16))
    {
      Bot_Abort(BOTH_DIR);
      Set_Scsi_Sense_Data(CBW.bLUN, ILLEGAL_REQUEST, INVALID_FIELED_IN_COMMAND);
      Set_CSW (CSW_CMD_FAILED, SEND_CSW_DISABLE);
    }
    else
    {
      switch (CBW.CB[0])
      {
        case SCSI_REQUEST_SENSE:
          SCSI_RequestSense_Cmd (CBW.bLUN);
          break;
        case SCSI_INQUIRY:
          SCSI_Inquiry_Cmd(CBW.bLUN);
          break;
        case SCSI_START_STOP_UNIT:
          SCSI_Start_Stop_Unit_Cmd(CBW.bLUN);
          break;
        case SCSI_ALLOW_MEDIUM_REMOVAL:
          SCSI_Start_Stop_Unit_Cmd(CBW.bLUN);
          break;
        case SCSI_MODE_SENSE6:
          SCSI_ModeSense6_Cmd (CBW.bLUN);
          break;
        case SCSI_MODE_SENSE10:
          SCSI_ModeSense10_Cmd (CBW.bLUN);
          break;
        case SCSI_READ_FORMAT_CAPACITIES:
          SCSI_ReadFormatCapacity_Cmd(CBW.bLUN);
          break;
        case SCSI_READ_CAPACITY10:
          SCSI_ReadCapacity10_Cmd(CBW.bLUN);
          break;
        case SCSI_TEST_UNIT_READY:
          SCSI_TestUnitReady_Cmd(CBW.bLUN);
          break;
        case SCSI_READ10:
          SCSI_Read10_Cmd(CBW.bLUN, SCSI_LBA , SCSI_BlkLen);
          break;
        case SCSI_WRITE10:
          SCSI_Invalid_Cmd(CBW.bLUN);//SCSI_Write10_Cmd(CBW.bLUN, SCSI_LBA , SCSI_BlkLen);
          break;
        case SCSI_VERIFY10:
          SCSI_Invalid_Cmd(CBW.bLUN);//SCSI_Verify10_Cmd(CBW.bLUN);
          break;
        case SCSI_FORMAT_UNIT:
          SCSI_Invalid_Cmd(CBW.bLUN);//SCSI_Format_Cmd(CBW.bLUN);
          break;
          /*Unsupported command*/
        case SCSI_MODE_SELECT10:
          SCSI_Mode_Select10_Cmd(CBW.bLUN);
          break;
        case SCSI_MODE_SELECT6:
          SCSI_Mode_Select6_Cmd(CBW.bLUN);
          break;

        case SCSI_SEND_DIAGNOSTIC:
          SCSI_Send_Diagnostic_Cmd(CBW.bLUN);
          break;
        case SCSI_READ6:
          SCSI_Read6_Cmd(CBW.bLUN);
          break;
        case SCSI_READ12:
          SCSI_Read12_Cmd(CBW.bLUN);
          break;
        case SCSI_READ16:
          SCSI_Read16_Cmd(CBW.bLUN);
          break;
        case SCSI_READ_CAPACITY16:
          SCSI_READ_CAPACITY16_Cmd(CBW.bLUN);
          break;
        case SCSI_WRITE6:
          SCSI_Write6_Cmd(CBW.bLUN);
          break;
        case SCSI_WRITE12:
          SCSI_Write12_Cmd(CBW.bLUN);
          break;
        case SCSI_WRITE16:
          SCSI_Write16_Cmd(CBW.bLUN);
          break;
        case SCSI_VERIFY12:
          SCSI_Verify12_Cmd(CBW.bLUN);
          break;
        case SCSI_VERIFY16:
          SCSI_Verify16_Cmd(CBW.bLUN);
          break;

        default:
        {
          Bot_Abort(BOTH_DIR);
          Set_Scsi_Sense_Data(CBW.bLUN, ILLEGAL_REQUEST, INVALID_COMMAND);
          Set_CSW (CSW_CMD_FAILED, SEND_CSW_DISABLE);
        }
      }
    }
  }
  else
  {
    /* Invalid CBW */
    Bot_Abort(BOTH_DIR);
    Set_Scsi_Sense_Data(CBW.bLUN, ILLEGAL_REQUEST, INVALID_COMMAND);
    Set_CSW (CSW_CMD_FAILED, SEND_CSW_DISABLE);
  }
}

/*******************************************************************************
* Function Name  : Transfer_Data_Request
* Description    : Send the request response to the PC HOST.
* Input          : uint8_t* Data_Address : point to the data to transfer.
*                  uint16_t Data_Length : the number of Bytes to transfer.
* Output         : None.
* Return         : None.
*******************************************************************************/
void Transfer_Data_Request(uint8_t* Data_Pointer, uint16_t Data_Len)
{	
	uint16_t dDataResidue = CSW.dDataResidue;
  // USB_SIL_Write(EP1_IN, Data_Pointer, Data_Len);
	memcpy(EP3_TX_BUF, Data_Pointer, Data_Len);
	UEP3_T_LEN = Data_Len;
	// SetEPTxStatus(ENDP1, EP_TX_VALID);
	UEP3_CTRL = UEP3_CTRL & ~MASK_UEP_T_RES | UEP_T_RES_ACK;	// Enable Tx
	
  Bot_State = BOT_DATA_IN_LAST;

  // CSW.dDataResidue = ((uint16_t)CSW.dDataResidue) - Data_Len;
	dDataResidue -= Data_Len;
	((uint8_t*)(&CSW.dDataResidue))[0] = ((uint8_t*)&dDataResidue)[1];
	((uint8_t*)(&CSW.dDataResidue))[1] = ((uint8_t*)&dDataResidue)[0];
	((uint8_t*)(&CSW.dDataResidue))[2] = 0;
	((uint8_t*)(&CSW.dDataResidue))[3] = 0;
  CSW.bStatus = CSW_CMD_PASSED;
}

/*******************************************************************************
* Function Name  : Set_CSW
* Description    : Set the SCW with the needed fields.
* Input          : uint8_t CSW_Status this filed can be CSW_CMD_PASSED,CSW_CMD_FAILED,
*                  or CSW_PHASE_ERROR.
* Output         : None.
* Return         : None.
*******************************************************************************/
void Set_CSW (uint8_t CSW_Status, uint8_t Send_Permission)
{
	uint8_t i;
	CSW.bStatus = CSW_Status;
  CSW.dSignature = BOT_CSW_SIGNATURE;
	
  // USB_SIL_Write(EP1_IN, ((uint8_t *)& CSW), CSW_DATA_LENGTH);
	//memcpy(EP3_TX_BUF, ((uint8_t *)& CSW), CSW_DATA_LENGTH);
	for (i = 0; i < CSW_DATA_LENGTH; i++) 
		(EP3_TX_BUF)[i] = ((uint8_t *)&CSW)[i];
	UEP3_T_LEN = CSW_DATA_LENGTH;
	
	
  Bot_State = BOT_ERROR;
  if (Send_Permission)
  {
    Bot_State = BOT_CSW_Send;
    // SetEPTxStatus(ENDP1, EP_TX_VALID);
		UEP3_CTRL = UEP3_CTRL & ~MASK_UEP_T_RES | UEP_T_RES_ACK;	// Enable Tx
  }
}

/*******************************************************************************
* Function Name  : Bot_Abort
* Description    : Stall the needed Endpoint according to the selected direction.
* Input          : Endpoint direction IN, OUT or both directions
* Output         : None.
* Return         : None.
*******************************************************************************/
void Bot_Abort(uint8_t Direction)
{
  switch (Direction)
  {
    case DIR_IN :
      // SetEPTxStatus(ENDP1, EP_TX_STALL);
			UEP3_CTRL = UEP3_CTRL & (~bUEP_T_TOG) | UEP_T_RES_STALL;
      break;
    case DIR_OUT :
      // SetEPRxStatus(ENDP2, EP_RX_STALL);
			UEP3_CTRL = UEP3_CTRL & (~bUEP_R_TOG) | UEP_R_RES_STALL;
      break;
    case BOTH_DIR :
      // SetEPTxStatus(ENDP1, EP_TX_STALL);
			UEP3_CTRL = UEP3_CTRL & (~bUEP_T_TOG) | UEP_T_RES_STALL;
      // SetEPRxStatus(ENDP2, EP_RX_STALL);
			UEP3_CTRL = UEP3_CTRL & (~bUEP_R_TOG) | UEP_R_RES_STALL;
      break;
    default:
      break;
  }
}

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
