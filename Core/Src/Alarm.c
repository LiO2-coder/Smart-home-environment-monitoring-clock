/**
 ******************************************************************************
 * @file    Alarm.c
 * @brief   多级警报系统实现文件 - 采用独立错误恢复机制
 ******************************************************************************
 */

#include "Alarm.h"

// =============================================================================
// 警报系统配置参数
// =============================================================================

// 警报电压阈值定义（单位：伏特）
#define INFO_THRESHOLD 0.0f    // 信息级别电压阈值（正常范围）
#define WARNING_THRESHOLD 0.3f // 警告级别电压阈值（轻微异常）
#define ERROR_THRESHOLD 1.5f   // 错误级别电压阈值（严重异常）

// 警报计数步长定义
#define ALARM_INFO_COUNT 1    // 信息级别计数增量（安全状态）
#define ALARM_WARNING_COUNT 1 // 警告级别计数增量
#define ALARM_ERROR_COUNT 2   // 错误级别计数增量（快速触发）

// 警报触发标志定义
#define ALARM_WARNING_FLAG 3   // 触发警告警报的计数阈值
#define ALARM_ERROR_FLAG 5     // 触发错误警报的计数阈值
#define ALARM_RESET_FLAG 10    // 警告状态复位所需连续安全计数
#define ERROR_RECOVERY_FLAG 15 // 错误状态恢复所需连续安全计数（更严格）

// =============================================================================
// 静态变量定义
// =============================================================================

static uint8_t alarm_count = 0;                              // 警报累计计数器（触发用）
static uint8_t safe_count = 0;                               // 警告状态安全计数器
static uint8_t error_recovery_count = 0;                     // 错误状态恢复计数器（新增）
static bool warning_light_flag = false;                      // 警告灯状态标志
static bool error_light_flag = false;                        // 错误灯状态标志
static Alarm_Level_t current_alarm_level = ALARM_LEVEL_NONE; // 当前警报级别

// =============================================================================
// 硬件控制函数
// =============================================================================

/**
 * @brief  初始化警报系统
 * @note   执行硬件自检，依次点亮各色LED验证硬件连接状态
 * @retval None
 */
void Alarm_Init(void)
{
    // 硬件自检序列：依次点亮各色LED
    HAL_GPIO_WritePin(Info_GPIO_Port, Info_Pin, GPIO_PIN_SET);       // 点亮绿色LED
    HAL_Delay(300);                                                  // 保持100ms
    HAL_GPIO_WritePin(Warning_GPIO_Port, Warning_Pin, GPIO_PIN_SET); // 点亮黄色LED
    HAL_Delay(300);                                                  // 保持100ms
    HAL_GPIO_WritePin(Error_GPIO_Port, Error_Pin, GPIO_PIN_SET);     // 点亮红色LED
    HAL_Delay(300);                                                  // 保持100ms

    // 熄灭所有LED，完成自检
    HAL_GPIO_WritePin(Info_GPIO_Port, Info_Pin, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(Warning_GPIO_Port, Warning_Pin, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(Error_GPIO_Port, Error_Pin, GPIO_PIN_RESET);

    // 初始化状态变量
    current_alarm_level = ALARM_LEVEL_NONE;
    alarm_count = 0;
    safe_count = 0;
    error_recovery_count = 0;
    warning_light_flag = false;
    error_light_flag = false;
}

/**
 * @brief  信息级别警报（绿色LED）
 * @note   表示系统运行正常，仅点亮绿色LED
 * @retval None
 */
void Alarm_Info(void)
{
    HAL_GPIO_WritePin(Info_GPIO_Port, Info_Pin, GPIO_PIN_SET);         // 绿色LED亮
    HAL_GPIO_WritePin(Warning_GPIO_Port, Warning_Pin, GPIO_PIN_RESET); // 黄色LED灭
    HAL_GPIO_WritePin(Error_GPIO_Port, Error_Pin, GPIO_PIN_RESET);     // 红色LED灭
    current_alarm_level = ALARM_LEVEL_INFO;
}

/**
 * @brief  警告级别警报（黄色LED）
 * @note   表示系统存在需要注意的异常，点亮黄色LED
 * @retval None
 */
void Alarm_Warning(void)
{
    HAL_GPIO_WritePin(Info_GPIO_Port, Info_Pin, GPIO_PIN_RESET);     // 绿色LED灭
    HAL_GPIO_WritePin(Warning_GPIO_Port, Warning_Pin, GPIO_PIN_SET); // 黄色LED亮
    HAL_GPIO_WritePin(Error_GPIO_Port, Error_Pin, GPIO_PIN_RESET);   // 红色LED灭
    current_alarm_level = ALARM_LEVEL_WARNING;
}

/**
 * @brief  错误级别警报（红色LED）
 * @note   表示系统存在严重异常需要立即处理，点亮红色LED
 * @retval None
 */
void Alarm_Error(void)
{
    HAL_GPIO_WritePin(Info_GPIO_Port, Info_Pin, GPIO_PIN_RESET);       // 绿色LED灭
    HAL_GPIO_WritePin(Warning_GPIO_Port, Warning_Pin, GPIO_PIN_RESET); // 黄色LED灭
    HAL_GPIO_WritePin(Error_GPIO_Port, Error_Pin, GPIO_PIN_SET);       // 红色LED亮
    current_alarm_level = ALARM_LEVEL_ERROR;
}

// =============================================================================
// 核心逻辑函数
// =============================================================================

/**
 * @brief  安全检查与状态恢复函数（方案二实现）
 * @param  voltage: 当前检测电压值（单位：伏特）
 * @note   采用独立错误恢复计数器机制，实现分级恢复逻辑
 * @retval None
 */
void Safe_Check(float voltage)
{
    // 输入电压有效性检查
    if (voltage < 0)
    {
        voltage = 0.0f; // 负电压强制归零，避免异常处理
    }

    // 仅当电压恢复正常范围（低于警告阈值）时进行恢复判断
    if (voltage < WARNING_THRESHOLD)
    {
        // 分级恢复逻辑：错误级别和警告级别使用不同的恢复机制
        if (current_alarm_level == ALARM_LEVEL_ERROR)
        {
            // 错误级别恢复逻辑：需要更严格的恢复条件
            error_recovery_count += ALARM_INFO_COUNT;

            if (error_recovery_count >= ERROR_RECOVERY_FLAG)
            {
                // 错误状态恢复：重置所有计数器和标志位
                error_recovery_count = 0;
                safe_count = 0;
                alarm_count = 0;
                warning_light_flag = false;
                error_light_flag = false;
                Alarm_Info(); // 恢复到正常状态
            }
        }
        else if (warning_light_flag == true)
        {
            // 警告级别恢复逻辑：使用原有安全计数机制
            safe_count += ALARM_INFO_COUNT;

            if (safe_count >= ALARM_RESET_FLAG)
            {
                // 警告状态恢复：重置相关计数器和标志位
                safe_count = 0;
                alarm_count = 0;
                warning_light_flag = false;
                Alarm_Info(); // 恢复到正常状态
            }
        }
        // 正常状态（ALARM_LEVEL_INFO）无需特殊处理
    }
    else
    {
        // 电压异常：重置所有恢复计数器
        safe_count = 0;
        error_recovery_count = 0;
    }
}

/**
 * @brief  主警报检查函数
 * @param  voltage: 当前检测电压值（单位：伏特）
 * @note   负责警报触发逻辑和状态转移控制
 * @retval None
 */
void Alarm_Check(float voltage)
{
    // 输入电压有效性检查
    if (voltage < 0)
    {
        voltage = 0.0f; // 负电压强制归零
    }

    // 仅当系统未处于激活警报状态时进行触发判断
    if (error_light_flag == false && warning_light_flag == false)
    {
        // 警报级别判断与计数累加
        if (voltage >= ERROR_THRESHOLD)
        {
            // 达到错误级别阈值：快速累计（步长为2）
            alarm_count += ALARM_ERROR_COUNT;
        }
        else if (voltage >= WARNING_THRESHOLD)
        {
            // 达到警告级别阈值：正常累计（步长为1）
            alarm_count += ALARM_WARNING_COUNT;
        }
        // 正常电压范围不进行计数累加

        // 警报状态触发判断
        if (alarm_count >= ALARM_ERROR_FLAG)
        {
            // 触发错误级别警报
            Alarm_Error();
            error_light_flag = true;
            warning_light_flag = true;      // 错误状态包含警告状态
            alarm_count = ALARM_ERROR_FLAG; // 防止计数溢出
        }
        else if (alarm_count >= ALARM_WARNING_FLAG)
        {
            // 触发警告级别警报
            Alarm_Warning();
            warning_light_flag = true;
        }
        // 未达到触发阈值时保持当前状态
    }

    // 执行安全检查与状态恢复（方案二实现）
    Safe_Check(voltage);
}

// =============================================================================
// 状态查询函数（可选扩展）
// =============================================================================

/**
 * @brief  获取当前警报级别
 * @note   可用于外部监控系统状态
 * @retval Alarm_Level_t 当前警报级别枚举值
 */
Alarm_Level_t Alarm_GetCurrentLevel(void)
{
    return current_alarm_level;
}

/**
 * @brief  检查系统是否处于警报状态
 * @note   快速状态查询接口
 * @retval bool true-警报中, false-正常状态
 */
bool Alarm_IsInAlertState(void)
{
    return (warning_light_flag || error_light_flag);
}

/**
 * @brief  获取详细状态信息（调试用）
 * @note   返回所有计数器的当前值，用于系统调试
 * @retval None
 */
void Alarm_GetStatus(uint8_t *alarm_cnt, uint8_t *safe_cnt, uint8_t *error_rec_cnt)
{
    if (alarm_cnt != NULL)
        *alarm_cnt = alarm_count;
    if (safe_cnt != NULL)
        *safe_cnt = safe_count;
    if (error_rec_cnt != NULL)
        *error_rec_cnt = error_recovery_count;
}