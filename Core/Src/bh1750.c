#include"bh1750.h"
 
extern I2C_HandleTypeDef hi2c1;
 
//写命令
void BH1750_Send_CMD(uint8_t cmd){
	
		HAL_I2C_Master_Transmit(&hi2c1, BH1750_WRITE_ADDR, &cmd, 1, HAL_MAX_DELAY);
	
}
 
//读结果
void BH1750_Read_Data(uint8_t* pData){
 
		HAL_I2C_Master_Receive(&hi2c1, BH1750_READ_ADDR, pData, 2, HAL_MAX_DELAY);
 
}
 
//转换结果
uint16_t BH1750_Data_To_LX(uint8_t* pData){
 
	uint16_t light = pData[0];
	
	light <<= 8;
	
	light += pData[1];	
	
	light = (uint16_t)(light/1.2); 
	
	return light;
	
}