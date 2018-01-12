/**
  ******************************************************************************
  * @file    usb_scsi.c
  * @author  MCD Application Team
  * @version V4.0.0
  * @date    21-January-2013
  * @brief   All processing related to the SCSI commands
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

#include "eeprom.h"
#include "ch554_platform.h"
#include "usb_endp.h"

SBIT(ledRW, GPIO1, 1);
/* Private typedef -----------------------------------------------------------*/
#define RW_LED_ON() ledRW=0
#define RW_LED_OFF() ledRW=1
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
xdata uint8_t Sense_Key[MAL_MAX_LUN+1];
xdata uint8_t Sense_Asc[MAL_MAX_LUN+1];
/* External variables --------------------------------------------------------*/
// Defined in usb_bot.c
extern xdata uint8_t Bot_State;
extern xdata Bulk_Only_CBW CBW;
extern xdata uint16_t dataResidue;

/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

// SCSI R/W state machine
#define TXFR_IDLE     0
#define TXFR_ONGOING  1
xdata uint8_t TransferState = TXFR_IDLE;
xdata uint32_t curAddr, endAddr;	

// SCSI Read and Write commands
void SCSI_Read10_Cmd(uint8_t lun , uint32_t LBA , uint32_t BlockNbr) {
	if (Bot_State == BOT_IDLE) {
		if (LUN_GetStatus(lun)) {
			Set_Scsi_Sense_Data(lun, NOT_READY, MEDIUM_NOT_PRESENT);
			Transfer_Failed_ReadWrite();
			return;
		}

		if (!(SCSI_Address_Management(lun, SCSI_READ10, LBA, BlockNbr)))
			return;	// Address out of range

		if ((CBW.bmFlags & 0x80) != 0) {
			Bot_State = BOT_DATA_IN;
		} else {
			Bot_Abort(BOTH_DIR);
			Set_Scsi_Sense_Data(lun, ILLEGAL_REQUEST, INVALID_FIELED_IN_COMMAND);
			Set_CSW (CSW_CMD_FAILED, SEND_CSW_ENABLE);
			return;
		}
	}
	
	if (Bot_State == BOT_DATA_IN) {
		if (TransferState == TXFR_IDLE ) {
			curAddr = LBA * Mass_Block_Size[lun];
			endAddr = curAddr + BlockNbr * Mass_Block_Size[lun];
			TransferState = TXFR_ONGOING;
		}

		if (TransferState == TXFR_ONGOING ) {
			RW_LED_ON();
						
			LUN_Read(lun, curAddr);

			curAddr += BULK_MAX_PACKET_SIZE;
			dataResidue -= BULK_MAX_PACKET_SIZE;

			// SetEPTxCount(ENDP1, BULK_MAX_PACKET_SIZE);
			BOT_EP_Tx_Count(BULK_MAX_PACKET_SIZE);
			// SetEPTxStatus(ENDP1, EP_TX_VALID);
			BOT_EP_Tx_Valid();	// Enable Tx
					
			if (curAddr >= endAddr) {
				RW_LED_OFF();
				curAddr = 0;
				endAddr = 0;
				Bot_State = BOT_DATA_IN_LAST;
				TransferState = TXFR_IDLE;
			}
		} // if (TransferState == TXFR_ONGOING )
	} // if (Bot_State == BOT_DATA_IN)
}

void SCSI_Write10_Cmd(uint8_t lun , uint32_t LBA , uint32_t BlockNbr) {
	if (Bot_State == BOT_IDLE) {
		if (!(SCSI_Address_Management(lun, SCSI_WRITE10 , LBA, BlockNbr)))
			return;	// Address out of range

		if ((CBW.bmFlags & 0x80) == 0) {
			Bot_State = BOT_DATA_OUT;
			
			// SetEPRxStatus(ENDP2, EP_RX_VALID);
			BOT_EP_Rx_Valid();
		} else {
			Bot_Abort(DIR_IN);
			Set_Scsi_Sense_Data(lun, ILLEGAL_REQUEST, INVALID_FIELED_IN_COMMAND);
			Set_CSW (CSW_CMD_FAILED, SEND_CSW_DISABLE);
		}
		
		return;
	} else if (Bot_State == BOT_DATA_OUT) {
		if (TransferState == TXFR_IDLE ) {
			curAddr = LBA * Mass_Block_Size[lun];
			endAddr = curAddr + BlockNbr * Mass_Block_Size[lun];
			TransferState = TXFR_ONGOING;
		}
		
		if (TransferState == TXFR_ONGOING )	{
			RW_LED_ON();
			
			LUN_Write(lun, curAddr);
			
			curAddr += BULK_MAX_PACKET_SIZE;
			dataResidue -= BULK_MAX_PACKET_SIZE;
			
			// SetEPRxStatus(ENDP2, EP_RX_VALID);
			BOT_EP_Rx_Valid();
			
			if (curAddr >= endAddr) {
				RW_LED_OFF();
				curAddr = 0;
				endAddr = 0;
				TransferState = TXFR_IDLE;
				Set_CSW (CSW_CMD_PASSED, SEND_CSW_ENABLE);
			}
		} // if (TransferState == TXFR_ONGOING )
	} // else if (Bot_State == BOT_DATA_OUT)
}

// SCSI Inquiry Command routine
void SCSI_Inquiry_Cmd(uint8_t lun) {
	uint8_t* Inquiry_Data;
	uint16_t Inquiry_Data_Length;

	if (CBW.CB[1] & 0x01) {/*Evpd is set*/
		Inquiry_Data = (uint8_t*) Page00_Inquiry_Data;
		Inquiry_Data_Length = PAGE00_INQUIRY_DATA_LEN;
	} else {
		Inquiry_Data = LUN_GetInquiryData(lun);
		
		if (CBW.CB[4] <= STANDARD_INQUIRY_DATA_LEN)
			Inquiry_Data_Length = CBW.CB[4];
		else
			Inquiry_Data_Length = STANDARD_INQUIRY_DATA_LEN;
	}
	Transfer_Data_Request(Inquiry_Data, Inquiry_Data_Length);
}

// SCSI read capacity commands
void SCSI_ReadCapacity10_Cmd(uint8_t lun) {	
	if (LUN_GetStatus(lun)) {
		Set_Scsi_Sense_Data(lun, NOT_READY, MEDIUM_NOT_PRESENT);
		Transfer_Failed_ReadWrite();
		return;
	}
	
	// Index of the last block,
	BOT_Tx_Buf[0] = (uint8_t)((Mass_Block_Count[lun] - 1) >> 24);
	BOT_Tx_Buf[1] = (uint8_t)((Mass_Block_Count[lun] - 1) >> 16);
	BOT_Tx_Buf[2] = (uint8_t)((Mass_Block_Count[lun] - 1) >>  8);
	BOT_Tx_Buf[3] = (uint8_t)(Mass_Block_Count[lun] - 1);
	
	// Block Length
	BOT_Tx_Buf[4] = (uint8_t)(Mass_Block_Size[lun] >>  24);
	BOT_Tx_Buf[5] = (uint8_t)(Mass_Block_Size[lun] >>  16);
	BOT_Tx_Buf[6] = (uint8_t)(Mass_Block_Size[lun] >>  8);
	BOT_Tx_Buf[7] = (uint8_t)(Mass_Block_Size[lun]);
	Reply_Request(READ_CAPACITY10_DATA_LEN);		
}

void SCSI_ReadFormatCapacity_Cmd(uint8_t lun) {
	if (LUN_GetStatus(lun) != 0 ) {
		Set_Scsi_Sense_Data(lun, NOT_READY, MEDIUM_NOT_PRESENT);
		Transfer_Failed_ReadWrite();
		return;
	}
	
	BOT_Tx_Buf[0] = 0x00;
	BOT_Tx_Buf[1] = 0x00;
	BOT_Tx_Buf[2] = 0x00;
	BOT_Tx_Buf[3] = 0x08;	// Capacity List Length
	
	// Block Count
	BOT_Tx_Buf[4] = (uint8_t)(Mass_Block_Count[lun] >> 24);
	BOT_Tx_Buf[5] = (uint8_t)(Mass_Block_Count[lun] >> 16);
	BOT_Tx_Buf[6] = (uint8_t)(Mass_Block_Count[lun] >>  8);
	BOT_Tx_Buf[7] = (uint8_t)(Mass_Block_Count[lun]);
	
	// Block Length
	BOT_Tx_Buf[8] = 0x02;	// Descriptor Code: Formatted Media
	BOT_Tx_Buf[9] = (uint8_t)(Mass_Block_Size[lun] >>  16);
	BOT_Tx_Buf[10] = (uint8_t)(Mass_Block_Size[lun] >>  8);
	BOT_Tx_Buf[11] = (uint8_t)(Mass_Block_Size[lun]);
	Reply_Request(READ_FORMAT_CAPACITY_DATA_LEN);
}

// SCSI Mode Sense Command
void SCSI_ModeSense6_Cmd (uint8_t lun) {
	Transfer_Data_Request(Mode_Sense6_data, MODE_SENSE6_DATA_LEN);
}

void SCSI_ModeSense10_Cmd (uint8_t lun) {
	Transfer_Data_Request(Mode_Sense10_data, MODE_SENSE10_DATA_LEN);
}


// Request Sense Command Routine and Set Sense Data
void SCSI_RequestSense_Cmd (uint8_t lun) {
	uint8_t i;

	for (i=0; i<REQUEST_SENSE_DATA_LEN; i++)
		BOT_Tx_Buf[i] = Scsi_Sense_Data[i];
	
	BOT_Tx_Buf[2] = Sense_Key[lun];
	BOT_Tx_Buf[12] = Sense_Asc[lun];
	
	if (CBW.CB[4] <= REQUEST_SENSE_DATA_LEN) {
		Reply_Request(CBW.CB[4]);		
	} else {
		Reply_Request(REQUEST_SENSE_DATA_LEN);
	}
}

void Set_Scsi_Sense_Data(uint8_t lun, uint8_t Sens_Key, uint8_t Asc) {
	Sense_Key[lun] = Sens_Key;
	Sense_Asc[lun] = Asc;
}

// Removable device ejection
void SCSI_Start_Stop_Unit_Cmd(uint8_t lun) {	
	Set_CSW (CSW_CMD_PASSED, SEND_CSW_ENABLE);
	
	if ((CBW.CB[4] & 3) == 2)	// LOEJ = 1 and Start = 0
		LUN_Eject(lun);
}

void SCSI_Allow_Medium_Removal_Cmd(uint8_t lun) {
	if (CBW.CB[4] & 1) {
		Set_Scsi_Sense_Data(lun, ILLEGAL_REQUEST, INVALID_COMMAND);
		Set_CSW (CSW_CMD_FAILED, SEND_CSW_ENABLE);
	} else {
		Set_CSW (CSW_CMD_PASSED, SEND_CSW_ENABLE);
	}
}







/*******************************************************************************
* Function Name  : SCSI_Verify10_Cmd
* Description    : SCSI Verify10 Command routine.
* Input          : None.
* Output         : None.
* Return         : None.
*******************************************************************************/
void SCSI_Verify10_Cmd(uint8_t lun) {
	if ((CBW.dDataLength == 0) && !(CBW.CB[1] & BLKVFY)) { /* BLKVFY not set*/
		Set_CSW (CSW_CMD_PASSED, SEND_CSW_ENABLE);
	} else {
		Bot_Abort(BOTH_DIR);
		Set_Scsi_Sense_Data(lun, ILLEGAL_REQUEST, INVALID_FIELED_IN_COMMAND);
		Set_CSW (CSW_CMD_FAILED, SEND_CSW_DISABLE);
	}
}

/*******************************************************************************
* Function Name  : SCSI_Valid_Cmd
* Description    : Valid Commands routine.
* Input          : None.
* Output         : None.
* Return         : None.
*******************************************************************************/
void SCSI_TestUnitReady_Cmd(uint8_t lun) {
	if (LUN_GetStatus(lun)) {
		Set_Scsi_Sense_Data(lun, NOT_READY, MEDIUM_NOT_PRESENT);
		Set_CSW (CSW_CMD_FAILED, SEND_CSW_ENABLE);
		// Bot_Abort(DIR_IN);
	} else {
		Set_CSW (CSW_CMD_PASSED, SEND_CSW_ENABLE);
	}
}
/*******************************************************************************
* Function Name  : SCSI_Format_Cmd
* Description    : Format Commands routine.
* Input          : None.
* Output         : None.
* Return         : None.
*******************************************************************************/
void SCSI_Format_Cmd(uint8_t lun) {
	if (LUN_GetStatus(lun)) {
		Set_Scsi_Sense_Data(lun, NOT_READY, MEDIUM_NOT_PRESENT);
		Set_CSW (CSW_CMD_FAILED, SEND_CSW_ENABLE);
		Bot_Abort(DIR_IN);
	}
}
/*******************************************************************************
* Function Name  : SCSI_Invalid_Cmd
* Description    : Invalid Commands routine
* Input          : None.
* Output         : None.
* Return         : None.
*******************************************************************************/
void SCSI_Invalid_Cmd(uint8_t lun) {
	if (CBW.dDataLength == 0) {
		Bot_Abort(DIR_IN);
	} else {
		if ((CBW.bmFlags & 0x80) != 0) {
			Bot_Abort(DIR_IN);
		} else {
			Bot_Abort(BOTH_DIR);
		}
	}
	
	Set_Scsi_Sense_Data(lun, ILLEGAL_REQUEST, INVALID_COMMAND);
	Set_CSW (CSW_CMD_FAILED, SEND_CSW_DISABLE);
}

/*******************************************************************************
* Function Name  : SCSI_Address_Management
* Description    : Test the received address.
* Input          : uint8_t Cmd : the command can be SCSI_READ10 or SCSI_WRITE10.
* Output         : None.
* Return         : Read\Write status (bool).
*******************************************************************************/
uint8_t SCSI_Address_Management(uint8_t lun , uint8_t Cmd , uint32_t LBA , uint32_t BlockNbr) {
	if ((LBA + BlockNbr) > Mass_Block_Count[lun]) {
		if (Cmd == SCSI_WRITE10) {
			Bot_Abort(BOTH_DIR);
		}
		Bot_Abort(DIR_IN);
		Set_Scsi_Sense_Data(lun, ILLEGAL_REQUEST, ADDRESS_OUT_OF_RANGE);
		Set_CSW (CSW_CMD_FAILED, SEND_CSW_DISABLE);
		return 0;
	}

	if (CBW.dDataLength != BlockNbr * Mass_Block_Size[lun]) {
		if (Cmd == SCSI_WRITE10) {
			Bot_Abort(BOTH_DIR);
		} else {
			Bot_Abort(DIR_IN);
		}
		Set_Scsi_Sense_Data(lun, ILLEGAL_REQUEST, INVALID_FIELED_IN_COMMAND);
		Set_CSW (CSW_CMD_FAILED, SEND_CSW_DISABLE);
		return 0;
	}
	return 1;
}
/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
