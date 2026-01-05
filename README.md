# 智能环境监测时钟系统

## 项目概述
本项目是一个基于STM32与ESP32双核架构的智能家居环境监测时钟系统，实现了环境感知（温湿度、烟雾浓度、光照）、实时时钟显示、语音交互及云端协同等完整功能链。系统创新性地采用人体存在检测雷达触发分级功耗管理，在无人状态下自动进入低功耗模式，整体功耗降低30%以上。



## 核心功能
- **环境监测**：实时采集温湿度、烟雾浓度、光照强度等环境参数
- **智能预警**：多级烟雾浓度预警系统，响应延迟<2秒
- **低功耗管理**：人体存在检测自动触发省电模式
- **双核协同**：STM32负责本地控制，ESP32处理网络通信
- **语音交互**：支持基础语音指令识别与反馈

## 系统架构

### 硬件架构
系统采用模块化设计，STM32作为主控制器，ESP32作为通信协处理器：



### 软件流程
系统启动后并行执行本地控制与网络通信任务：



## 硬件设计

### 硬件连接
项目采用STM32F103ZET6作为主控芯片，外接多种传感器模块：



### 引脚配置
| 序号 | 引脚号 | 连接元件引脚 | 连接元件 |
|------|--------|--------------|----------|
| 1 | PA1 | AO | MQ2烟雾传感器 |
| 2 | PA9 | RX | ESP32 AI系统 |
| 3 | PA10 | TX | ESP32 AI系统 |
| 4 | PB0 | OUT | HW416人体检测模块 |
| 5 | PB6 | SCL | OLED显示模块 |
| 6 | PB7 | SDA | OLED显示模块 |

### 实物连接
调试阶段采用杜邦线连接各模块：



## 软件设计

### MQ2烟雾传感器模块
采用ADC采集实现烟雾浓度实时监测：

```c
// ADC初始化配置
hadc1.Instance = ADC1;
hadc1.Init.ScanConvMode = ADC_SCAN_DISABLE;
hadc1.Init.ContinuousConvMode = ENABLE;
```



### 烟雾浓度预警系统
多级阈值触发与分级恢复机制：

```c
typedef enum {
    ALARM_LEVEL_INFO = 0,    // 正常状态
    ALARM_LEVEL_WARNING,     // 警告状态
    ALARM_LEVEL_ERROR,       // 错误状态
} Alarm_Level_t;
```



### DHT11温湿度传感器
单总线通信协议实现数据采集：

```c
void DHT11_IO_OUT() {
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    HAL_GPIO_Init(GPIOG, &GPIO_InitStruct);
}
```



### OLED显示模块
开源SSD1306驱动移植，实现信息可视化：



正常显示界面：


省电模式界面：


### RTC实时时钟
后备寄存器机制确保时间持续运行：

```c
void YY_RTC_Init(void) {
    uint32_t initFlag = HAL_RTCEx_BKUPRead(&hrtc, RTC_BKP_DR1);
    if (initFlag == RTC_INIT_FLAG_VALUE) return;
    // 初始化代码
}
```



### STM32-ESP32通信协议
自定义串口通信协议实现双核协同：

```
[AA 55][CMD][LEN][DATA...][CRC16][0D]
```

通信时序流程：


## 调试与开发

### 开发环境
- **主控芯片**：STM32F103ZET6（最终版本）
- **调试芯片**：STM32F103C8T6（开发阶段）
- **开发工具**：Keil MDK-ARM、VSCode
- **调试接口**：SWD、串口调试



### 外壳设计
基于产品需求设计3D打印外壳：



## 项目结构

```
SmartEnvClock/
├── Docs/                    # 项目文档
├── Firmware/               # 嵌入式固件
│   ├── STM32/              # STM32端代码
│   │   ├── Core/           # 核心驱动
│   │   ├── Drivers/        # 硬件驱动
│   │   └── Projects/       # 项目文件
│   └── ESP32/              # ESP32端代码
├── Hardware/               # 硬件设计文件
│   ├── Schematic/          # 原理图
│   ├── PCB/               # PCB设计
│   └── 3D_Models/         # 3D外壳模型
├── Tools/                  # 开发工具
└── README.md               # 项目说明
```

## 快速开始

### 硬件准备
1. STM32F103ZET6开发板
2. ESP32-WROOM-32模块
3. OLED显示屏（SSD1306）
4. MQ-2烟雾传感器
5. DHT11温湿度传感器
6. HW416人体检测模块

### 软件编译
1. 克隆仓库
```bash
git clone https://github.com/yourname/SmartEnvClock.git
```

2. 使用Keil打开项目
```bash
打开 Firmware/STM32/Projects/Project.uvprojx
```

3. 配置ESP32开发环境
```bash
cd Firmware/ESP32
idf.py build
```

## 通信协议详解

### 协议帧格式
```
帧头(2B) + 命令码(1B) + 数据长度(1B) + 数据(NB) + CRC16(2B) + 帧尾(1B)
```

### 命令码定义
| 命令码 | 功能说明 | 数据长度 |
|--------|----------|----------|
| 0x01 | 请求环境数据 | 0 |
| 0x02 | 请求特定传感器 | 1 |
| 0x10 | 响应就绪 | 0 |
| 0x11 | 发送环境数据 | 20 |

## 性能指标

- **温度测量精度**：±1℃
- **湿度测量精度**：±1%RH
- **烟雾报警响应时间**：<2秒
- **时钟日误差**：<3秒
- **功耗（正常模式）**：120mA
- **功耗（省电模式）**：65mA

## 贡献指南

欢迎提交Issue和Pull Request！主要开发方向包括：

1. **低功耗优化**：进一步降低系统功耗
2. **功能扩展**：增加更多传感器支持
3. **协议优化**：改进通信协议效率
4. **AI集成**：增强本地智能处理能力

## 许可证

本项目采用MIT开源许可证，详见LICENSE文件。

## 联系我们

- 项目主页：https://github.com/yourname/SmartEnvClock
- 问题反馈：https://github.com/yourname/SmartEnvClock/issues
- 邮箱：yourname@example.com

## 致谢

感谢深圳技术大学嵌入式系统设计课程的支持，特别感谢张智星老师的指导。

---

*最后更新：2025年1月*  
*版本号：v1.0.0*
