#ifndef __YY_RTC_H
#define __YY_RTC_H

#include "stm32f1xx_hal.h"
#include "rtc.h"
#include "time.h"

extern struct tm user_time;


HAL_StatusTypeDef YY_RTC_SetTime(struct tm *time);
struct tm *YY_RTC_GetTime(void);
void YY_RTC_Init(void);

#endif
