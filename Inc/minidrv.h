#ifndef minidrv_h
#define minidrv_h

#include "stm32f1xx_hal.h"

void MiniDrv_Init(void);
void MiniDrv_Receive(void);
void MiniDrv_Send(uint8_t *dataArray, uint8_t lengthArray);

#endif
