/**
  ******************************************************************************
  * @file          : minidrv.c
  * @brief         : This code provides the ability to pack / unpack
  *                : and send data via UART and CAN
  ******************************************************************************
*/
/* Includes ------------------------------------------------------------------*/

#include "stm32f1xx_hal.h"
#include "minihdlc.h"

#define UART;		// Для работы с UART - оставить как есть. Для работы с CAN - закомментировать

CAN_HandleTypeDef hcan;
TIM_HandleTypeDef htim1;
UART_HandleTypeDef huart1;
UART_HandleTypeDef huart2;

//extern uint8_t *pDataArray;
//extern uint8_t *pLengthArray;

#ifndef UART

CAN_FilterConfTypeDef*	sFilterConfig;
static CanTxMsgTypeDef	txHeader;
static CanRxMsgTypeDef  rxHeader;
uint8_t txData[8];
uint8_t txMailBox;

/**
 * @brief	configuring CAN to work
 * @param	none
 * @retval	none
 */
void CanConfig(void)
{
	sFilterConfig->FilterNumber = 0;
	sFilterConfig->FilterMode = CAN_FILTERMODE_IDMASK;
	sFilterConfig->FilterScale = CAN_FILTERSCALE_32BIT;
	sFilterConfig->FilterIdHigh = 0x0000;
	sFilterConfig->FilterIdLow = 0x0000;
	sFilterConfig->FilterMaskIdHigh = 0x0000;
	sFilterConfig->FilterMaskIdLow = 0x0000;
	sFilterConfig->FilterFIFOAssignment = 0;
	sFilterConfig->FilterActivation = ENABLE;
	sFilterConfig->BankNumber = 14;

	txHeader.StdId = 0x0122;
	txHeader.ExtId = 0x0000;
	txHeader.RTR = CAN_RTR_DATA;
	txHeader.IDE = CAN_ID_STD;
	txHeader.DLC = 1;											// Количество данных в CAN посылке
	txData[0] = 0xFF;											// Поле данных в CAN посылке
	txData[1] = 0xFE;
	txData[2] = 0xFD;
	txData[3] = 0xFC;
	txData[4] = 0xFB;
	txData[5] = 0xFA;
	txData[6] = 0xF9;
	txData[7] = 0xF8;
}
#endif

/**
 * @brief	sending the extracted data to the terminal
 * @param	*dataArray - pointer to a byte array with data
 * 			lengthArray - length of a byte array with data
 * @retval	none
 */
void MiniDrv_SendUnpackDataInTerminal(uint8_t *dataArray, uint16_t lengthArray)
{
//	*pDataArray = &dataArray[0];
//	*pLengthArray = &lengthArray;
	HAL_UART_Transmit(&huart1, dataArray, lengthArray, 0xFFFF);
	return;
}

#ifdef UART

/**
 * @brief	sends the packed data to UART
 * @param	byte - one byte of data to send UART
 * @retval	none
 */
void MiniDrv_SendPackDataViaUART(uint8_t byte)
{
	HAL_UART_Transmit(&huart2, &byte, sizeof(byte), 0xFFFF);
	return;
}

#else

/**
 * @brief	sends the packed data to CAN
 * @param	byte - one byte of data to send CAN
 * @retval	none
 */
void MiniDrv_SendPackDataViaCAN(uint8_t byte)
{
	txData[7] = byte;
	HAL_CAN_Transmit(&hcan, 10);
	return;
}
#endif

/**
 * @brief	configures the code to work with the specified transmission channel
 * @param	none
 * @retval	none
 */
//void MiniDrv_Init(uint8_t *pData, uint8_t *pLength)
void MiniDrv_Init()
{
#ifdef UART
	minihdlc_init(MiniDrv_SendPackDataViaUART, MiniDrv_SendUnpackDataInTerminal);	// Для UART
#else
	CanConfig();												// подготовка CAN к работе
	minihdlc_init(MiniDrv_SendPackDataViaCAN, MiniDrv_SendUnpackDataInTerminal);	// Для CAN
#endif
	return;
}

/**
 * @brief	packages and transmits a packet to the specified transmission channel
 * @param	*dataArray - pointer to a byte array with data
 * 			lengthArray - length of a byte array with data
 * @retval
 */
void MiniDrv_Send(uint8_t *dataArray, uint8_t lengthArray)
{
	minihdlc_send_frame(dataArray, lengthArray);
	return;
}

/**
 * @brief	accepts the packed data from the specified channel and sends it for unpacking
 * @param	none
 * @retval	none
 */
void MiniDrv_Receive()
{
	uint8_t byte[100];

#ifdef UART
	HAL_UART_Receive(&huart2, &byte, sizeof(byte), 0xFFFF);
#else
	HAL_CAN_Receive(&hcan, CAN_FIFO0, 10);
#endif

	for(int i = 0; i < sizeof(byte); i++)
	{
		minihdlc_char_receiver( byte[i] );
	}

	return;
}



