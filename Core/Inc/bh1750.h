#ifndef BH_1750_H
#define BH_1750_H
 
#include "stm32f1xx_hal.h"
 
#define BH1750_WRITE_ADDR 0x46
#define BH1750_READ_ADDR 0x47
 
 
void BH1750_Send_CMD(uint8_t cmd);
void BH1750_Read_Data(uint8_t* pData);
uint16_t BH1750_Data_To_LX(uint8_t* pData);
 
#endif
 
 