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

/* To work with UART -> uncomment the string: "#define UART;"
 * To work with CAN  -> comment the string:   "#define UART;"       */
#define UART

#define UART_TIMEOUT 0xFF

CAN_HandleTypeDef hcan;
TIM_HandleTypeDef htim1;
UART_HandleTypeDef huart1;
UART_HandleTypeDef huart2;

#ifndef UART

CAN_FilterTypeDef		sFilterConfig;
CAN_TxHeaderTypeDef		txHeader;
CAN_RxHeaderTypeDef		rxHeader;

/**
 * @brief	configuring CAN to work
 * @param	none
 * @retval	none
 */
void CanConfig(void)
{
	sFilterConfig.FilterBank 			= 0;
	sFilterConfig.FilterMode 			= CAN_FILTERMODE_IDMASK;
	sFilterConfig.FilterScale 			= CAN_FILTERSCALE_32BIT;
	sFilterConfig.FilterIdHigh 			= 0x0000;
	sFilterConfig.FilterIdLow 			= 0x0000;
	sFilterConfig.FilterMaskIdHigh 		= 0x0000;
	sFilterConfig.FilterMaskIdLow 		= 0x0000;
	sFilterConfig.FilterFIFOAssignment 	= CAN_RX_FIFO0;
	sFilterConfig.FilterActivation 		= ENABLE;
	sFilterConfig.SlaveStartFilterBank 	= 14;

	HAL_CAN_ConfigFilter(&hcan, &sFilterConfig);

	HAL_CAN_Start(&hcan);

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
	HAL_UART_Transmit(&huart1, dataArray, lengthArray, UART_TIMEOUT);
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
	HAL_UART_Transmit(&huart2, &byte, sizeof(byte), UART_TIMEOUT);
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
	txHeader.StdId 				= 0x0000;
	txHeader.ExtId 				= 0x0000;
	txHeader.RTR 				= CAN_RTR_DATA;
	txHeader.IDE 				= CAN_ID_STD;
	txHeader.DLC 				= 1;
	txHeader.TransmitGlobalTime = DISABLE;

	uint32_t mailBoxNum = 0;

	HAL_CAN_AddTxMessage(&hcan, &txHeader, byte, &mailBoxNum);

	return;
}

#endif

/**
 * @brief	configures the code to work with the specified transmission channel
 * @param	none
 * @retval	none
 */
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


#ifdef UART
	uint8_t byte[1024];

	HAL_UART_Receive(&huart2, &byte[0], sizeof(byte), UART_TIMEOUT);

	for(int i = 0; i < sizeof(byte); i++)
	{
		minihdlc_char_receiver( byte[i] );
	}
#else
	uint8_t msgData[8];
	rxHeader.StdId 	= 0x0000;
	rxHeader.ExtId 	= 0x0000;
	rxHeader.RTR 	= CAN_RTR_DATA;
	rxHeader.IDE 	= CAN_ID_STD;
	rxHeader.DLC 	= 1;

	HAL_CAN_GetRxMessage(&hcan, CAN_RX_FIFO0, &rxHeader, msgData);

	for(int i = 0; i < sizeof(msgData); i++)
	{
		minihdlc_char_receiver( msgData[i] );
	}
#endif

	return;
}



