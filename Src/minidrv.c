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
#include "stdbool.h"
//#include "tim.h"
//#include "gpio.h"
//#include "usart.h"
//#include "can.h"
//#include "Common.h"

CAN_HandleTypeDef hcan;
TIM_HandleTypeDef htim1;
UART_HandleTypeDef huart1;
UART_HandleTypeDef huart2;

/**
 * @brief
 * @param
 * @retval
 */
void MiniDrv_ByteReceiveUART(uint8_t *dataArray, uint16_t lengthArray)
{
	HAL_UART_Transmit(&huart1, dataArray, lengthArray, 0xFFFF);
	return;
}

/**
 * @brief
 * @param
 * @retval
 */
void MiniDrv_ByteTransmitUART(uint8_t byte)
{
	HAL_UART_Transmit(&huart2, &byte, sizeof(byte), 0xFFFF);
	return;
}

/**
 * @brief
 * @param
 * @retval
 */
void MiniDrv_ByteReceiveCAN(uint8_t *dataArray, uint16_t lengthArray)
{
	HAL_CAN_Transmit(&hcan, 0xFFFF);
	return;
}

/**
 * @brief
 * @param
 * @retval
 */
void MiniDrv_ByteTransmitCAN(uint8_t byte)
{
	HAL_CAN_Transmit(&hcan, 0xFFFF);
	return;
}

/**
 * @brief
 * @param
 * @retval
 */
void MiniDrv_Init(void)
{
	minihdlc_init(MiniDrv_ByteTransmitUART, MiniDrv_ByteReceiveUART);
	return;
}

/**
 * @brief
 * @param
 * @retval
 */
void MiniDrv_ReceiveUART()
{
	uint8_t dataByte[100];
//	uint16_t dataLength = 1024;
//	uint16_t i;

	HAL_UART_Receive(&huart2, dataByte, sizeof(dataByte), 0xFFFF);

	for(int i = 0; i < sizeof(dataByte); i++)
	{
		minihdlc_char_receiver(dataByte[i]);
	}

	HAL_GPIO_TogglePin(LED_GPIO_Port, LED_Pin);
	return;
}

/**
 * @brief
 * @param
 * @retval
 */
void MiniDrv_ReceiveCAN()
{
	uint8_t dataByte[100];
	HAL_CAN_Receive(&hcan, CAN_FIFO0, 0xFFFF);

	for(int i = 0; i < sizeof(dataByte); i++)
	{
		minihdlc_char_receiver(dataByte[i]);
	}

	HAL_GPIO_TogglePin(LED_GPIO_Port, LED_Pin);
	return;
}

/**
 * @brief
 * @param
 * @retval
 */
void MiniDrv_Transmit(uint8_t *dataArray, uint8_t lengthArray)
{

	minihdlc_send_frame(dataArray, lengthArray);
//	fterm (dataArray, lengthArray);
//	HAL_UART_Transmit(&huart2, dataArray, lengthArray, 0xFFFF);
	HAL_GPIO_TogglePin(LED_GPIO_Port, LED_Pin);
	return;
}



