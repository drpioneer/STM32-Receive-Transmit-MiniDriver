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
void MiniDrv_Init(void)
{
//	minihdlc_init();
	return;
}

/**
 * @brief
 * @param
 * @retval
 */
void MiniDrv_Start(void)
{
	return;
}

/**
 * @brief
 * @param
 * @retval
 */
void MiniDrv_DataPack(void)
{

	return;
}

/**
 * @brief
 * @param
 * @retval
 */
void MiniDrv_DataUnpack(void)
{

	return;
}

/**
 * @brief
 * @param
 * @retval
 */
void MiniDrv_Receive(uint8_t *dataArray, uint8_t lengthArray)
{
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
//	HAL_UART_Transmit(&huart2, dataArray, lengthArray, 0xFFFF);
	HAL_GPIO_TogglePin(LED_GPIO_Port, LED_Pin);
	return;
}

/**
 * @brief
 * @param
 * @retval
 */
void MiniDrv_SendUART(void)
{
	return;
}

/**
 * @brief
 * @param
 * @retval
 */
void MiniDrv_ReceiveUART(void)
{
	return;
}

/**
 * @brief
 * @param
 * @retval
 */
void MiniDrv_SendCAN(void)
{
	return;
}

/**
 * @brief
 * @param
 * @retval
 */
void MiniDrv_ReceiveCAN(void)
{
	return;
}
