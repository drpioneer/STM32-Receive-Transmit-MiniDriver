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

#define UART;		// Оставить для работы с UART. Закомментировать для работы с CAN

CAN_HandleTypeDef hcan;
TIM_HandleTypeDef htim1;
UART_HandleTypeDef huart1;
UART_HandleTypeDef huart2;

extern uint8_t *pdataArray;
extern uint8_t *pSize;

#ifndef UART

CAN_FilterConfTypeDef*	sFilterConfig;
static CanTxMsgTypeDef	txHeader;
static CanRxMsgTypeDef  rxHeader;
uint8_t txData[8];
uint8_t txMailBox;

/**
 * @brief	Подготовка CAN к работе
 * @param
 * @retval
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
 * @brief
 * @param
 * @retval	lengthArray
 */
uint16_t MiniDrv_SendUnpackDataInTerminal(uint8_t *dataArray, uint16_t lengthArray)
{
//	pArray = dataArray;
//	pSize = &lengthArray;
	HAL_UART_Transmit(&huart1, dataArray, lengthArray, 0xFFFF);
	return lengthArray;
}

/**
 * @brief
 * @param
 * @retval
 */
void MiniDrv_SendPackDataViaUART(uint8_t byte)
{
	HAL_UART_Transmit(&huart2, &byte, sizeof(byte), 0xFFFF);
	return;
}

#ifndef UART
/**
 * @brief
 * @param
 * @retval
 */
void MiniDrv_SendPackDataViaCAN(uint8_t byte)
{
	txData[7] = byte;
	HAL_CAN_Transmit(&hcan, 10);
	return;
}
#endif

/**
 * @brief
 * @param
 * @retval
 */
void MiniDrv_Init(uint8_t *pData, uint8_t *pLength)
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
 * @brief
 * @param
 * @retval
 */
void MiniDrv_Send(uint8_t *dataArray, uint8_t lengthArray)
{
	/* Передача исходного пакета данных на упаковку и отсылку в канал */
	minihdlc_send_frame(dataArray, lengthArray);
	return;
}

/**
 * @brief
 * @param
 * @retval
 */
void MiniDrv_Receive()
{
	uint8_t byte[100];

	/* Приём упакованных данных из заданного канала */

#ifdef UART
	HAL_UART_Receive(&huart2, &byte, sizeof(byte), 0xFFFF);
#else
	HAL_CAN_Receive(&hcan, CAN_FIFO0, 10);
#endif

	/* Передача поступающих данных из канала на распаковку */
	for(int i = 0; i < sizeof(byte); i++)
	{
		minihdlc_char_receiver( byte[i] );
	}

	return;
}



