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
#include "main.h"

/* To work with UART -> uncomment the string: "#define UART;"
 * To work with CAN  -> comment the string:   "#define UART;"       */
//#define UART

#define UART_TIMEOUT 	0xFF

CAN_HandleTypeDef 		hcan;
TIM_HandleTypeDef 		htim1;
UART_HandleTypeDef 		huart1;
UART_HandleTypeDef 		huart2;

#ifndef UART

CAN_FilterTypeDef		sFilterConfig;
CAN_TxHeaderTypeDef		txHeader;
CAN_RxHeaderTypeDef		rxHeader;
uint8_t               	txData[8];
uint8_t               	rxData[8];
uint32_t              	txMailbox;

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

	if ( HAL_CAN_ConfigFilter(&hcan, &sFilterConfig) != HAL_OK )
	{
		Error_Handler();
	}

	if ( HAL_CAN_Start(&hcan) != HAL_OK )
	{
		Error_Handler();
	}
}
#endif

/**
 * @brief	sending the extracted data to the terminal
 * @param	*dataArray - pointer to a byte array with data
 * 			lengthArray - length of a byte array with data
 * @retval	none
 */
void SendUnpackDataInTerm(uint8_t *dataArray, uint16_t lengthArray)
{
	if ( HAL_UART_Transmit(&huart1, dataArray, lengthArray, UART_TIMEOUT) != HAL_OK )
	{
		Error_Handler();
	}
	return;
}

#ifdef UART

/**
 * @brief	sends the packed data to UART
 * @param	byte - one byte of data to send UART
 * @retval	none
 */
void SendPackDataViaUART(uint8_t byte)
{
	if ( HAL_UART_Transmit(&huart2, &byte, sizeof(byte), UART_TIMEOUT) != HAL_OK )
	{
		Error_Handler();
	}
	return;
}

#else

/**
 * @brief	sends the packed data to CAN
 * @param	byte - one byte of data to send via CAN
 * @retval	none
 */
void SendPackDataViaCAN(uint8_t byte)
{
	txHeader.StdId 	= 0x11;
	txHeader.RTR 	= CAN_RTR_DATA;
	txHeader.IDE 	= CAN_ID_STD;
	txHeader.DLC 	= 1;
	txHeader.TransmitGlobalTime = DISABLE;
	txMailbox 		= 0;

	txData[0] = byte;
//	txData[1] = byte;
//	txData[2] = byte;
//	txData[3] = byte;
//	txData[4] = byte;
//	txData[5] = byte;
//	txData[6] = byte;
//	txData[7] = byte;

	if ( HAL_CAN_AddTxMessage(&hcan, &txHeader, txData, &txMailbox) != HAL_OK )
	{
		Error_Handler();
	}

	/* Wait transmission complete */
	while( HAL_CAN_GetTxMailboxesFreeLevel(&hcan) != 3 ) {}

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

	minihdlc_init(SendPackDataViaUART, SendUnpackDataInTerm);

#else

	CanConfig();
	minihdlc_init(SendPackDataViaCAN, SendUnpackDataInTerm);

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
	HAL_StatusTypeDef status;
	uint8_t byte[1024];

	do
	{
		status = HAL_UART_Receive(&huart2, byte, sizeof(byte), UART_TIMEOUT);
	}
	while ( status == HAL_BUSY );

	if ( status != HAL_OK )
	{
//		Error_Handler();
	}

	for(int i = 0; i < sizeof(byte); i++)
	{
		minihdlc_char_receiver( byte[i] );
	}
#else
	uint8_t byte;

	/* Start the Reception process */
	if ( HAL_CAN_GetRxFifoFillLevel(&hcan, CAN_RX_FIFO0) > 0 )
	{
		if ( HAL_CAN_GetRxMessage(&hcan, CAN_RX_FIFO0, &rxHeader, rxData) != HAL_OK )
		{
			Error_Handler();
		}
		if ((rxHeader.StdId == 0x11) && (rxHeader.RTR == CAN_RTR_DATA) && (rxHeader.IDE == CAN_ID_STD))
		{
			byte = rxData [0];
//			minihdlc_char_receiver(byte);
			HAL_UART_Transmit(&huart1, &byte, sizeof(byte), UART_TIMEOUT);
		}
	}
#endif

	return;
}
