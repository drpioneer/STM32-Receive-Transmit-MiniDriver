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

/*    Для работы с UART -> раскомментировать строку: '#define UART;'
 *    для работы с CAN  -> закомментировать строку:  '#define UART;'           */
#define UART;

CAN_HandleTypeDef hcan;
TIM_HandleTypeDef htim1;
UART_HandleTypeDef huart1;
UART_HandleTypeDef huart2;

#ifndef UART

CAN_FilterConfTypeDef	sFilterConfig;
CanTxMsgTypeDef			txHeader;
CanRxMsgTypeDef			rxHeader;

/**
 * @brief	configuring CAN to work
 * @param	none
 * @retval	none
 */
void CanConfig(void)
{
	sFilterConfig.FilterNumber = 1;
	sFilterConfig.FilterMode = CAN_FILTERMODE_IDMASK;
	sFilterConfig.FilterScale = CAN_FILTERSCALE_32BIT;
	sFilterConfig.FilterIdHigh = 0x0000;
	sFilterConfig.FilterIdLow = 0x0000;
	sFilterConfig.FilterMaskIdHigh = 0x0000;
	sFilterConfig.FilterMaskIdLow = 0x0000;
	sFilterConfig.FilterFIFOAssignment = CAN_FIFO0;
	sFilterConfig.FilterActivation = ENABLE;
	sFilterConfig.BankNumber = 14;

	HAL_CAN_ConfigFilter(&hcan, &sFilterConfig);

	txHeader.StdId = 0x321;
	txHeader.ExtId = 0x000;
	txHeader.RTR = CAN_RTR_DATA;
	txHeader.IDE = CAN_ID_STD;
	txHeader.DLC = 1;											// Количество данных в CAN посылке
	txHeader.Data[0] = 0xFF;									// Данные в CAN посылке
	txHeader.Data[1] = 0xFE;
	txHeader.Data[2] = 0xFD;
	txHeader.Data[3] = 0xFC;
	txHeader.Data[4] = 0xFB;
	txHeader.Data[5] = 0xFA;
	txHeader.Data[6] = 0xF9;
	txHeader.Data[7] = 0xF8;

	// обнуление буфера для сообщения
	rxHeader.FIFONumber = CAN_FIFO0;
	rxHeader.StdId = 0x00;
	rxHeader.ExtId = 0x00;
	rxHeader.RTR =   0x00;
	rxHeader.IDE =   0x00;
	rxHeader.DLC =   0x00;
	rxHeader.Data[0] = 0x00;
	rxHeader.Data[1] = 0x00;
	rxHeader.Data[2] = 0x00;
	rxHeader.Data[3] = 0x00;
	rxHeader.Data[4] = 0x00;
	rxHeader.Data[5] = 0x00;
	rxHeader.Data[6] = 0x00;
	rxHeader.Data[7] = 0x00;

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
	txHeader.Data[7] = byte;
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

	minihdlc_init(MiniDrv_SendPackDataViaUART, MiniDrv_SendUnpackDataInTerminal);

#else

	CanConfig();
	minihdlc_init(MiniDrv_SendPackDataViaCAN, MiniDrv_SendUnpackDataInTerminal);

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
//	uint8_t data;

#ifdef UART
	HAL_UART_Receive(&huart2, &byte, sizeof(byte), 10);
#else
	HAL_CAN_Receive(&hcan, CAN_FIFO0, 0xFFFF);
//	data = rxHeader.Data[7];
#endif

	for(int i = 0; i < sizeof(byte); i++)
	{
		minihdlc_char_receiver( byte[i] );
//		minihdlc_char_receiver( data );
	}

	return;
}



