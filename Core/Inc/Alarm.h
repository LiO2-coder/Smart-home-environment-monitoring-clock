#ifndef __ALARM_H__
#define __ALARM_H__

#include "main.h"
#include "gpio.h"
#include <stdbool.h>  // 添加标准布尔类型支持

// 警报级别枚举定义
typedef enum
{
    ALARM_LEVEL_INFO = 0, // 信息级别警报
    ALARM_LEVEL_WARNING,  // 警告级别警报
    ALARM_LEVEL_ERROR,    // 错误级别警报
    ALARM_LEVEL_NONE      // 无警报状态
} Alarm_Level_t;

// 函数声明
void Alarm_Init(void);
void Alarm_Info(void);
void Alarm_Warning(void);
void Alarm_Error(void);
void Alarm_Check(float voltage);
void Safe_Check(float voltage);
Alarm_Level_t Alarm_GetCurrentLevel(void);
bool Alarm_IsInAlertState(void);
void Alarm_GetStatus(uint8_t *alarm_cnt, uint8_t *safe_cnt, uint8_t *error_rec_cnt);

#endif /* __ALARM_H__ */
