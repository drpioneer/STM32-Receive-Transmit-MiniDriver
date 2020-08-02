#ifndef minidrv_h
#define minidrv_h

#include "stm32f1xx_hal.h"
#include <stdbool.h>
//#include <stdint.h>

void MiniDrv_Init(void);
void MiniDrv_Start(void);
void MiniDrv_DataPack(void);
void MiniDrv_DataUnpack(void);
void MiniDrv_Receive(uint8_t *dataArray, uint16_t lengthArray);
void MiniDrv_Transmit(uint8_t *dataArray, uint16_t lengthArray);

#endif
