#include "dht11.h"

void delay_us(uint32_t us)
{
	uint32_t delay = (HAL_RCC_GetHCLKFreq() / 4000000 * us);
	while (delay--)
	{
		;
	}
}

/**
 * @brief  DATA引脚(PA7)设置为输出模式
 * @param  None
 * @retval None
 */
void DHT11_IO_OUT()
{
	GPIO_InitTypeDef GPIO_InitStruct = {0};
	GPIO_InitStruct.Pin = DHT11_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_PULLUP;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
	HAL_GPIO_Init(GPIOG, &GPIO_InitStruct);
}
/**
 * @brief  DATA引脚设置为输入模式
 * @param  None
 * @retval	None
 */
void DHT11_IO_IN()
{
	GPIO_InitTypeDef GPIO_InitStruct = {0};
	GPIO_InitStruct.Pin = DHT11_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
	HAL_GPIO_Init(GPIOG, &GPIO_InitStruct);
}

// 复位DHT11
void DHT11_Rst(void)
{
	DHT11_IO_OUT(); // SET OUTPUT
	DHT11_LOW;		// 拉低DQ
	HAL_Delay(20);	// 拉低至少18ms
	DHT11_HIGH;		// DQ=1
	delay_us(30);	// 主机拉高20~40us
}
// 等待DHT11的回应
// 返回1:未检测到DHT11的存在
// 返回0:存在
u8 DHT11_Check(void)
{
	u8 retry = 0;
	DHT11_IO_IN();					   // SET INPUT
	while (DHT11_DQ_IN && retry < 100) // DHT11会拉低40~80us
	{
		retry++;
		delay_us(1);
	};
	if (retry >= 100)
		return 1;
	else
		retry = 0;
	while (!DHT11_DQ_IN && retry < 100) // DHT11拉低后会再次拉高40~80us
	{
		retry++;
		delay_us(1);
	};
	if (retry >= 100)
		return 1;
	return 0;
}
// 从DHT11读取一个位
// 返回值：1/0
u8 DHT11_Read_Bit(void)
{
	u8 retry = 0;
	while (DHT11_DQ_IN && retry < 100) // 等待变为低电平
	{
		retry++;
		delay_us(1);
	}
	retry = 0;
	while (!DHT11_DQ_IN && retry < 100) // 等待变高电平
	{
		retry++;
		delay_us(1);
	}
	delay_us(40); // 等待40us
	if (DHT11_DQ_IN)
		return 1;
	else
		return 0;
}
// 从DHT11读取一个字节
// 返回值：读到的数据
u8 DHT11_Read_Byte(void)
{
	u8 i, dat;
	dat = 0;
	for (i = 0; i < 8; i++)
	{
		dat <<= 1;
		dat |= DHT11_Read_Bit();
	}
	return dat;
}
// 从DHT11读取一次数据
// temp:温度值(范围:0~50°)
// humi:湿度值(范围:20%~90%)
// 返回值：0,正常;1,读取失败
u8 DHT11_Read_Data(u16 *temp, u16 *humi)
{
	u8 buf[5];
	u8 i;
	DHT11_Rst();
	if (DHT11_Check() == 0)
	{
		for (i = 0; i < 5; i++) // 读取40位数据
		{
			buf[i] = DHT11_Read_Byte();
		}
		if ((buf[0] + buf[1] + buf[2] + buf[3]) == buf[4])
		{
			*humi = (u16)buf[0] * 100 + (u16)buf[1];
			*temp = (u16)buf[2] * 100 + (u16)buf[3];
		}
	}
	else
		return 1;
	return 0;
}
