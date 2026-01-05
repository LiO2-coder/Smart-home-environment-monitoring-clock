/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file           : main.c
 * @brief          : Main program body
 ******************************************************************************
 * @attention
 *
 * Copyright (c) 2025 STMicroelectronics.
 * All rights reserved.
 *
 * This software is licensed under terms that can be found in the LICENSE file
 * in the root directory of this software component.
 * If no LICENSE file comes with this software, it is provided AS-IS.
 *
 ******************************************************************************
 */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "adc.h"
#include "i2c.h"
#include "rtc.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "oled.h"
#include "DHT11.h"
#include "stdio.h"
#include "string.h"
#include "yy_rtc.h"
#include "Alarm.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
 * @brief  The application entry point.
 * @retval int
 */
int main(void)
{

  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_I2C1_Init();
  MX_I2C2_Init();
  MX_ADC1_Init();
  MX_RTC_Init();
  /* USER CODE BEGIN 2 */
  OLED_Init();
  OLED_CLS();

  // 初始化警报系�?
  Alarm_Init();

  // 原有变量定义
  u16 temp = 0;
  u16 humi = 0;
  uint32_t ADC_Value = 0;
  float voltage = 0;
  struct tm *now = NULL;

  char u_temp[20];
  char u_humi[20];
  char u_value[20];
  char ymd[50] = "";
  char hms[50] = "";
  char alarm_status_str[20] = "Status:Normal"; // 警报状�?�显示字符串

  int fputc(int ch, FILE *f);

  // 显示初始化界面
  OLED_ShowStr(0, 0, "Sensor Monitor", 2);
  OLED_ShowStr(0, 2, "Initializing...", 1);
  HAL_Delay(1000);
  OLED_CLS();
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
    // 如果检测到人体存在
    if (HAL_GPIO_ReadPin(HW416_GPIO_Port, HW416_Pin) == GPIO_PIN_RESET)
    {
      // 正常显示数据

      // 读取传感器数�?
      DHT11_Read_Data(&temp, &humi);

      // ADC读取电压�?
      HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, GPIO_PIN_RESET);
      HAL_ADC_Start(&hadc1);
      HAL_ADC_PollForConversion(&hadc1, 50);
      ADC_Value = HAL_ADC_GetValue(&hadc1);
      HAL_ADC_Stop(&hadc1);
      HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, GPIO_PIN_SET);
      voltage = ADC_Value * 3.3f / 4096.0f;

      // 使用新的多级警报系统
      Alarm_Check(voltage);

      // 获取当前警报级别用于显示
      Alarm_Level_t current_level = Alarm_GetCurrentLevel();

      // 根据警报级别更新状�?�显�?
      switch (current_level)
      {
      case ALARM_LEVEL_INFO:
        strcpy(alarm_status_str, "Status:Normal");
        break;
      case ALARM_LEVEL_WARNING:
        strcpy(alarm_status_str, "Status:Warning!");
        break;
      case ALARM_LEVEL_ERROR:
        strcpy(alarm_status_str, "Status:ERROR!!");
        break;
      default:
        strcpy(alarm_status_str, "Status:Unknown");
        break;
      }

      // 获取RTC时间
      now = YY_RTC_GetTime();

      // 格式化显示数�?
      sprintf(u_temp, "T:%.1fC", temp / 100.0f);
      sprintf(u_humi, "H:%.1f%%", humi / 100.0f);
      sprintf(u_value, "V:%.2fV", voltage);

      if (now != NULL)
      {
        sprintf(ymd, "%04d-%02d-%02d", now->tm_year + 1900, now->tm_mon + 1, now->tm_mday);
        sprintf(hms, "%02d:%02d:%02d", now->tm_hour, now->tm_min, now->tm_sec);
      }
      else
      {
        strcpy(ymd, "Date: --");
        strcpy(hms, "Time: --");
      }

      // 优化显示布局
      // OLED_CLS();
      OLED_ShowStr(0, 0, (unsigned char *)"Sensor Monitor", 1);
      OLED_ShowStr(100, 0, (unsigned char *)"ON", 1);
      OLED_ShowStr(0, 1, (unsigned char *)u_temp, 1);
      OLED_ShowStr(64, 1, (unsigned char *)u_humi, 1);
      OLED_ShowStr(0, 2, (unsigned char *)u_value, 1);
      OLED_ShowStr(25, 3, (unsigned char *)ymd, 2);
      OLED_ShowStr(32, 5, (unsigned char *)hms, 2);
      OLED_ShowStr(0, 7, (unsigned char *)alarm_status_str, 1);

      // 添加警报级别可视化指�?
      /*// char level_indicator[10];
      // sprintf(level_indicator, "Lvl:%d", current_level);
      // OLED_ShowStr(100, 5, (unsigned char *)level_indicator, 1);*/
    }
    else
    {
      // 省电模式显示屏保
      static uint32_t last_blink = 0;
      static uint8_t show_msg = 1;
      uint32_t current_time = HAL_GetTick();

      if (current_time - last_blink > 500)
      {
        show_msg = !show_msg;
        last_blink = current_time;

        OLED_CLS();
        if (show_msg)
        {
          OLED_ShowStr(25, 2.5, (unsigned char *)"Keep Happy", 2);
        }
      }

      // 传感器关闭时重置警报系统
      Alarm_Init();
    }

    HAL_Delay(1000); // 保持1秒采样间�?
  }
  /* USER CODE END 3 */
}

/**
 * @brief System Clock Configuration
 * @retval None
 */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
  RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

  /** Initializes the RCC Oscillators according to the specified parameters
   * in the RCC_OscInitTypeDef structure.
   */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE | RCC_OSCILLATORTYPE_LSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
  RCC_OscInitStruct.LSEState = RCC_LSE_ON;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
   */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_RTC | RCC_PERIPHCLK_ADC;
  PeriphClkInit.RTCClockSelection = RCC_RTCCLKSOURCE_LSE;
  PeriphClkInit.AdcClockSelection = RCC_ADCPCLK2_DIV6;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
 * @brief  This function is executed in case of error occurrence.
 * @retval None
 */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef USE_FULL_ASSERT
/**
 * @brief  Reports the name of the source file and the source line number
 *         where the assert_param error has occurred.
 * @param  file: pointer to the source file name
 * @param  line: assert_param error line source number
 * @retval None
 */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */


