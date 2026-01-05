#ifndef __DHT11_H
#define __DHT11_H 
#include "main.h"   
 
//IO方向设置
#define DHT11_HIGH     HAL_GPIO_WritePin(GPIOG, DHT11_Pin,	GPIO_PIN_SET)
#define DHT11_LOW      HAL_GPIO_WritePin(GPIOG, DHT11_Pin, GPIO_PIN_RESET)
#define DHT11_DQ_IN    HAL_GPIO_ReadPin(GPIOG, DHT11_Pin)
#define u8 uint8_t
#define u16 uint16_t
 
u8 DHT11_Init(void);//初始化DHT11
u8 DHT11_Read_Data(u16 *temp,u16 *humi);//读取温湿度
u8 DHT11_Read_Byte(void);	//读出一个字节
u8 DHT11_Read_Bit(void);	//读出一个位
u8 DHT11_Check(void);		//检测是否存在DHT11
void DHT11_Rst(void);		//复位DHT11    
#endif
 
 