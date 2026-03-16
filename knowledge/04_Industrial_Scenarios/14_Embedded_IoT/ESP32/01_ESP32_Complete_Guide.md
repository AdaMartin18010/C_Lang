# ESP32 完全开发指南

> 适用版本: ESP-IDF v5.x | 硬件平台: ESP32/ESP32-S3/ESP32-C3/ESP32-S2

---

## 目录

- [ESP32 完全开发指南](#esp32-完全开发指南)
  - [目录](#目录)
  - [1. ESP32 硬件架构概览](#1-esp32-硬件架构概览)
    - [1.1 ESP32 系列芯片对比](#11-esp32-系列芯片对比)
    - [1.2 ESP32-S3 详细架构](#12-esp32-s3-详细架构)
    - [1.3 ESP32-C3 (RISC-V) 架构特点](#13-esp32-c3-risc-v-架构特点)
    - [1.4 内存布局详解](#14-内存布局详解)
  - [2. ESP-IDF 开发环境搭建](#2-esp-idf-开发环境搭建)
    - [2.1 Windows 环境搭建](#21-windows-环境搭建)
      - [2.1.1 使用 ESP-IDF 安装器 (推荐)](#211-使用-esp-idf-安装器-推荐)
      - [2.1.2 手动安装 (高级用户)](#212-手动安装-高级用户)
    - [2.2 Linux 环境搭建 (Ubuntu/Debian)](#22-linux-环境搭建-ubuntudebian)
    - [2.3 macOS 环境搭建](#23-macos-环境搭建)
    - [2.4 VS Code 开发环境配置](#24-vs-code-开发环境配置)
    - [2.5 第一个项目: Hello World](#25-第一个项目-hello-world)
  - [3. GPIO 控制基础](#3-gpio-控制基础)
    - [3.1 GPIO 基础操作](#31-gpio-基础操作)
    - [3.2 高级 GPIO 控制: PWM (LEDC)](#32-高级-gpio-控制-pwm-ledc)
    - [3.3 ADC (模数转换)](#33-adc-模数转换)
  - [4. WiFi 网络连接](#4-wifi-网络连接)
    - [4.1 WiFi Station (STA) 模式](#41-wifi-station-sta-模式)
    - [4.2 WiFi Access Point (AP) 模式](#42-wifi-access-point-ap-模式)
    - [4.3 HTTP Client 示例](#43-http-client-示例)
  - [5. 蓝牙低功耗 (BLE)](#5-蓝牙低功耗-ble)
    - [5.1 BLE GATT 服务器](#51-ble-gatt-服务器)
    - [5.2 BLE GATT 客户端](#52-ble-gatt-客户端)
  - [6. 低功耗模式](#6-低功耗模式)
    - [6.1 电源管理框架](#61-电源管理框架)
    - [6.2 低功耗 WiFi 连接](#62-低功耗-wifi-连接)
  - [7. FreeRTOS 任务管理](#7-freertos-任务管理)
    - [7.1 任务创建与管理](#71-任务创建与管理)
    - [7.2 任务间通信](#72-任务间通信)
  - [8. 外设驱动 (I2C/SPI/UART)](#8-外设驱动-i2cspiuart)
    - [8.1 I2C 驱动](#81-i2c-驱动)
    - [8.2 SPI 驱动](#82-spi-驱动)
    - [8.3 UART 驱动](#83-uart-驱动)
  - [9. 实战项目: 温湿度监测站](#9-实战项目-温湿度监测站)
    - [9.1 项目架构](#91-项目架构)
    - [9.2 项目配置文件](#92-项目配置文件)
    - [9.3 CMakeLists.txt](#93-cmakeliststxt)
  - [10. 调试技巧与常见问题](#10-调试技巧与常见问题)
    - [10.1 调试工具配置](#101-调试工具配置)
    - [10.2 常见问题排查](#102-常见问题排查)
      - [问题 1: 启动时 Guru Meditation Error](#问题-1-启动时-guru-meditation-error)
      - [问题 2: WiFi 连接不稳定](#问题-2-wifi-连接不稳定)
      - [问题 3: I2C 通信失败](#问题-3-i2c-通信失败)
    - [10.3 性能优化技巧](#103-性能优化技巧)
    - [10.4 安全最佳实践](#104-安全最佳实践)
  - [附录 A: 快速参考表](#附录-a-快速参考表)
    - [GPIO 引脚功能对照](#gpio-引脚功能对照)
    - [常用命令速查](#常用命令速查)
    - [资源链接](#资源链接)

---

## 1. ESP32 硬件架构概览

### 1.1 ESP32 系列芯片对比

| 特性 | ESP32 | ESP32-S3 | ESP32-C3 | ESP32-S2 |
|------|-------|----------|----------|----------|
| **CPU** | Xtensa LX6 双核 @ 240MHz | Xtensa LX7 双核 @ 240MHz | RISC-V 单核 @ 160MHz | Xtensa LX7 单核 @ 240MHz |
| **WiFi** | 802.11 b/g/n | 802.11 b/g/n | 802.11 b/g/n | 802.11 b/g/n |
| **蓝牙** | BLE 4.2 + 经典蓝牙 | BLE 5.0 + 经典蓝牙 | BLE 5.0 | 无 |
| **AI 加速** | 无 | 矢量指令 (AI) | 无 | 无 |
| **USB** | USB-OTG | USB-OTG | USB-Serial | USB-OTG |
| **安全** | 基础 | 数字签名/HMAC | 数字签名 | 基础 |
| **内存** | 520KB SRAM | 512KB SRAM | 400KB SRAM | 320KB SRAM |

### 1.2 ESP32-S3 详细架构

```
┌─────────────────────────────────────────────────────────────┐
│                    ESP32-S3 系统架构                          │
├─────────────────────────────────────────────────────────────┤
│  ┌──────────────┐    ┌──────────────┐    ┌──────────────┐  │
│  │   CPU0       │    │   CPU1       │    │  矢量指令单元  │  │
│  │  (Pro CPU)   │◄──►│  (App CPU)   │◄──►│   (AI Acc)   │  │
│  │  Xtensa LX7  │    │  Xtensa LX7  │    │              │  │
│  └──────┬───────┘    └──────┬───────┘    └──────────────┘  │
│         │                   │                                │
│  ┌──────▼───────────────────▼──────┐                       │
│  │      内部总线 (Internal Bus)     │                       │
│  └──────┬───────────────────┬──────┘                       │
│         │                   │                                │
│  ┌──────▼─────┐  ┌──────────▼───────┐  ┌──────────────┐    │
│  │  512KB SRAM │  │  ROM (384KB)     │  │  Cache       │    │
│  │  (IRAM/DRAM)│  │  (Bootloader)    │  │  (Flash cache)│   │
│  └─────────────┘  └──────────────────┘  └──────────────┘    │
│                                                              │
│  ┌─────────────┐  ┌─────────────┐  ┌──────────────────────┐│
│  │  WiFi/BT    │  │  USB OTG    │  │  外设 (Peripherals)   ││
│  │  (IEEE 802.11│  │  (FS/HS)    │  │  I2C/SPI/UART/ADC... ││
│  │  b/g/n + BLE)│  │             │  │                      ││
│  └─────────────┘  └─────────────┘  └──────────────────────┘│
└─────────────────────────────────────────────────────────────┘
```

### 1.3 ESP32-C3 (RISC-V) 架构特点

```
┌─────────────────────────────────────────────────────────────┐
│                    ESP32-C3 系统架构                          │
├─────────────────────────────────────────────────────────────┤
│  ┌─────────────────────────────────────────────────────┐   │
│  │           RISC-V RV32IMC FPU (160MHz)                │   │
│  │  • 32位整数指令集 (RV32I)                             │   │
│  │  • 乘除法扩展 (M)                                     │   │
│  │  • 压缩指令扩展 (C)                                   │   │
│  └─────────────────────────────────────────────────────┘   │
│                           │                                  │
│  ┌─────────────┐  ┌─────────────┐  ┌──────────────────────┐│
│  │  400KB SRAM  │  │  4MB Flash   │  │  外设接口             ││
│  │  (IRAM/DRAM) │  │  (可外接)    │  │  • 2x I2C            ││
│  │              │  │              │  │  • 3x UART           ││
│  └─────────────┘  └─────────────┘  │  • 2x SPI            ││
│                                     │  • 2x I2S            ││
│  ┌─────────────┐  ┌─────────────┐  │  • 5x PWM            ││
│  │  WiFi 802.11│  │  BLE 5.0    │  │  • 2x ADC (12-bit)   ││
│  │  b/g/n      │  │  (LE + BR/EDR)│ │  • Temperature Sensor││
│  └─────────────┘  └─────────────┘  └──────────────────────┘│
└─────────────────────────────────────────────────────────────┘
```

### 1.4 内存布局详解

```c
/* ESP32 内存映射 (以 ESP32-S3 为例)
 *
 * ┌────────────────────────┐ 0x3FF0_0000 (保留)
 * │      外设区域           │ 0x3FF0_0000 - 0x3FF8_0000
 * ├────────────────────────┤
 * │      DRAM (数据RAM)     │ 0x3FF8_0000 - 0x4000_0000
 * │      约 512KB           │
 * ├────────────────────────┤
 * │      IRAM (指令RAM)     │ 0x4000_0000 - 0x4040_0000
 * │      约 256KB           │
 * ├────────────────────────┤
 * │      ROM 代码           │ 0x4000_0000 - 0x4005_0000
 * │      (384KB)            │
 * └────────────────────────┘
 */

/* 内存区域宏定义 */
#define DRAM_START      0x3FF80000
#define DRAM_END        0x40000000
#define IRAM_START      0x40000000
#define IRAM_END        0x40400000
#define ROM_START       0x40000000
#define ROM_END         0x40060000
```

---

## 2. ESP-IDF 开发环境搭建

### 2.1 Windows 环境搭建

#### 2.1.1 使用 ESP-IDF 安装器 (推荐)

```powershell
# 1. 下载 ESP-IDF 工具安装器
# 访问: https://dl.espressif.com/dl/esp-idf/
# 选择: esp-idf-tools-setup-online-2.24.exe (在线安装器)

# 2. 安装选项:
#    - ESP-IDF 版本: v5.1.2 (LTS) 或 v5.2 (最新)
#    - 安装路径: C:\esp\esp-idf
#    - 工具链: 自动下载
#    - Python: 使用捆绑的 Python

# 3. 安装完成后,打开 "ESP-IDF 5.x PowerShell"
# 或 "ESP-IDF 5.x Command Prompt"

# 4. 验证安装
$idf.py --version
# 输出: ESP-IDF v5.1.2 or later

# 5. 编译示例项目
cd %IDF_PATH%\examples\get-started\hello_world
idf.py set-target esp32s3
idf.py build
```

#### 2.1.2 手动安装 (高级用户)

```powershell
# 1. 安装依赖
# 需要: Python 3.8+, Git, CMake, Ninja

# 2. 克隆 ESP-IDF
git clone -b v5.1.2 --recursive https://github.com/espressif/esp-idf.git C:\esp\esp-idf

# 3. 安装工具
cd C:\esp\esp-idf
.\install.bat esp32,esp32s3,esp32c3

# 4. 设置环境变量
.\export.bat
```

### 2.2 Linux 环境搭建 (Ubuntu/Debian)

```bash
#!/bin/bash
# esp32_setup.sh - ESP32 Linux 开发环境一键安装脚本

set -e

# 1. 安装系统依赖
sudo apt-get update
sudo apt-get install -y \
    git \
    wget \
    flex \
    bison \
    gperf \
    python3 \
    python3-pip \
    python3-venv \
    cmake \
    ninja-build \
    ccache \
    libffi-dev \
    libssl-dev \
    dfu-util \
    libusb-1.0-0

# 2. 创建安装目录
mkdir -p ~/esp
cd ~/esp

# 3. 克隆 ESP-IDF (指定稳定版本)
if [ ! -d "esp-idf" ]; then
    git clone -b v5.1.2 --recursive https://github.com/espressif/esp-idf.git
fi

cd esp-idf

# 4. 安装工具链 (支持所有 ESP32 系列)
./install.sh esp32,esp32s2,esp32s3,esp32c3,esp32c6

# 5. 设置环境变量 (添加到 .bashrc)
if ! grep -q "export.sh" ~/.bashrc; then
    echo "" >> ~/.bashrc
    echo "# ESP-IDF 环境变量" >> ~/.bashrc
    echo "alias get_idf='. $HOME/esp/esp-idf/export.sh'" >> ~/.bashrc
fi

echo "ESP-IDF 安装完成!"
echo "请运行: source ~/.bashrc"
echo "然后运行: get_idf"
```

### 2.3 macOS 环境搭建

```bash
#!/bin/bash
# esp32_macos_setup.sh

# 1. 安装 Homebrew (如未安装)
/bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)"

# 2. 安装依赖
brew install cmake ninja dfu-util ccache python3

# 3. 创建安装目录
mkdir -p ~/esp
cd ~/esp

# 4. 克隆 ESP-IDF
git clone -b v5.1.2 --recursive https://github.com/espressif/esp-idf.git

# 5. 安装工具链
cd ~/esp/esp-idf
./install.sh esp32,esp32s3,esp32c3

# 6. 添加环境变量到 .zshrc (macOS 默认使用 zsh)
echo "alias get_idf='. $HOME/esp/esp-idf/export.sh'" >> ~/.zshrc

echo "安装完成! 请重新打开终端或运行: source ~/.zshrc"
```

### 2.4 VS Code 开发环境配置

```json
// .vscode/settings.json
{
    "idf.espIdfPathWin": "C:\\esp\\esp-idf",
    "idf.espIdfPath": "${env:HOME}/esp/esp-idf",
    "idf.toolsPathWin": "C:\\esp\\.espressif",
    "idf.toolsPath": "${env:HOME}/.espressif",
    "idf.pythonBinPathWin": "C:\\esp\\.espressif\\python_env\\idf5.1_py3.11_env\\Scripts\\python.exe",
    "idf.pythonBinPath": "${env:HOME}/.espressif/python_env/idf5.1_py3.11_env/bin/python",
    "idf.gitPath": "git",

    // 编译目标
    "idf.saveBeforeBuild": true,
    "idf.flashType": "UART",
    "idf.portWin": "COM3",
    "idf.port": "/dev/ttyUSB0",
    "idf.baudRate": "921600",

    // 调试配置
    "idf.openOcdConfigs": [
        "interface/ftdi/esp32_devkitj_v1.cfg",
        "target/esp32s3.cfg"
    ]
}
```

```json
// .vscode/launch.json
{
    "version": "0.2.0",
    "configurations": [
        {
            "name": "GDB",
            "type": "cppdbg",
            "request": "launch",
            "program": "${workspaceFolder}/build/${config:idf.projectName}.elf",
            "cwd": "${workspaceFolder}",
            "environment": [],
            "MIMode": "gdb",
            "miDebuggerPath": "${config:idf.toolsPath}/tools/xtensa-esp-elf-gdb/12.1_20221002/xtensa-esp-elf-gdb/bin/xtensa-esp32s3-elf-gdb",
            "setupCommands": [
                {
                    "text": "target remote :3333"
                },
                {
                    "text": "set remote hardware-watchpoint-limit 2"
                },
                {
                    "text": "mon reset halt"
                },
                {
                    "text": "flushregs"
                }
            ]
        }
    ]
}
```

### 2.5 第一个项目: Hello World

```c
/*
 * main/hello_world_main.c
 * ESP32 Hello World 示例 - 完整的入门项目
 */

#include <stdio.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_chip_info.h"
#include "esp_flash.h"
#include "esp_system.h"

/* 任务函数声明 */
static void print_chip_info(void);
static void memory_monitor_task(void *pvParameters);

/**
 * @brief 应用程序入口点
 */
void app_main(void)
{
    printf("\n========================================\n");
    printf("    ESP32 Hello World - 系统信息\n");
    printf("========================================\n\n");

    /* 打印芯片信息 */
    print_chip_info();

    /* 创建内存监控任务 */
    xTaskCreate(
        memory_monitor_task,        /* 任务函数 */
        "mem_monitor",              /* 任务名称 */
        2048,                       /* 堆栈大小 (字) */
        NULL,                       /* 任务参数 */
        1,                          /* 优先级 */
        NULL                        /* 任务句柄 */
    );

    /* 主循环 */
    int counter = 0;
    while (1) {
        printf("Hello World! [%d]\n", counter++);
        vTaskDelay(pdMS_TO_TICKS(1000));  /* 延迟 1 秒 */
    }
}

/**
 * @brief 打印 ESP32 芯片详细信息
 */
static void print_chip_info(void)
{
    esp_chip_info_t chip_info;
    uint32_t flash_size;

    /* 获取芯片信息 */
    esp_chip_info(&chip_info);

    printf("芯片型号: %s\n",
        chip_info.model == CHIP_ESP32 ? "ESP32" :
        chip_info.model == CHIP_ESP32S2 ? "ESP32-S2" :
        chip_info.model == CHIP_ESP32S3 ? "ESP32-S3" :
        chip_info.model == CHIP_ESP32C3 ? "ESP32-C3" :
        chip_info.model == CHIP_ESP32C2 ? "ESP32-C2" :
        chip_info.model == CHIP_ESP32C6 ? "ESP32-C6" : "Unknown");

    printf("CPU 核心数: %d\n", chip_info.cores);
    printf("WiFi: %s\n", (chip_info.features & CHIP_FEATURE_WIFI_BGN) ? "支持" : "不支持");
    printf("蓝牙: %s\n", (chip_info.features & CHIP_FEATURE_BT) ? "经典蓝牙" :
                         (chip_info.features & CHIP_FEATURE_BLE) ? "BLE" : "不支持");
    printf("IEEE 802.15.4: %s\n", (chip_info.features & CHIP_FEATURE_IEEE802154) ? "支持" : "不支持");

    /* 获取 Flash 大小 */
    if (esp_flash_get_size(NULL, &flash_size) == ESP_OK) {
        printf("Flash 大小: %ld MB\n", flash_size / (1024 * 1024));
    }

    /* 获取 SDK 版本 */
    printf("ESP-IDF 版本: %s\n", esp_get_idf_version());
    printf("----------------------------------------\n\n");
}

/**
 * @brief 内存监控任务 - 定期报告内存使用情况
 */
static void memory_monitor_task(void *pvParameters)
{
    (void) pvParameters;  /* 未使用参数 */

    while (1) {
        /* 获取内存信息 */
        size_t free_heap = esp_get_free_heap_size();
        size_t min_free_heap = esp_get_minimum_free_heap_size();

        printf("[内存监控] 空闲堆: %d bytes, 最小空闲: %d bytes\n",
               (int)free_heap, (int)min_free_heap);

        /* 每 5 秒报告一次 */
        vTaskDelay(pdMS_TO_TICKS(5000));
    }
}
```

```cmake
# CMakeLists.txt (项目根目录)
cmake_minimum_required(VERSION 3.16)

# 设置目标芯片 (可选,可在命令行指定)
# set(IDF_TARGET esp32s3)

# 包含 ESP-IDF 项目构建系统
include($ENV{IDF_PATH}/tools/cmake/project.cmake)

# 定义项目名称
project(hello_world)
```

```cmake
# main/CMakeLists.txt
# 源文件列表
set(SOURCES
    "hello_world_main.c"
)

# 注册组件
idf_component_register(
    SRCS ${SOURCES}
    INCLUDE_DIRS "."
)
```

```yaml
# sdkconfig.defaults - 项目默认配置
# 此文件会在 menuconfig 之前应用

# WiFi 配置
CONFIG_ESP_WIFI_AUTH_WPA2_PSK=y

# FreeRTOS 配置
CONFIG_FREERTOS_HZ=1000
CONFIG_FREERTOS_UNICORE=n

# 日志级别
CONFIG_LOG_DEFAULT_LEVEL_INFO=y
CONFIG_LOG_DEFAULT_LEVEL=3

# 调试配置
CONFIG_ESP_DEBUG_OCDAWARE=y
```

---

## 3. GPIO 控制基础

### 3.1 GPIO 基础操作

```c
/*
 * gpio_basic_example.c
 * ESP32 GPIO 基础操作示例
 * 功能: LED 控制、按键检测、中断处理
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "driver/gpio.h"
#include "esp_log.h"

/* GPIO 引脚定义 - 根据实际硬件修改 */
#define LED_GPIO_PIN        GPIO_NUM_2   /* 板载 LED (大多数开发板) */
#define BUTTON_GPIO_PIN     GPIO_NUM_0   /* 启动按钮 */
#define EXT_LED_PIN         GPIO_NUM_4   /* 外部 LED */

/* 日志标签 */
static const char *TAG = "GPIO_EXAMPLE";

/* 全局变量 */
static volatile uint32_t g_button_press_count = 0;
static QueueHandle_t g_gpio_evt_queue = NULL;

/* 函数声明 */
static void gpio_init_led(gpio_num_t gpio_num);
static void gpio_init_button(gpio_num_t gpio_num);
static void gpio_isr_handler(void *arg);
static void gpio_task_example(void *arg);

/**
 * @brief 初始化 LED GPIO 为输出模式
 */
static void gpio_init_led(gpio_num_t gpio_num)
{
    /* 配置 GPIO */
    gpio_config_t io_conf = {
        .pin_bit_mask = (1ULL << gpio_num),  /* 选择 GPIO */
        .mode = GPIO_MODE_OUTPUT,             /* 设置为输出模式 */
        .pull_up_en = GPIO_PULLUP_DISABLE,    /* 禁用上拉 */
        .pull_down_en = GPIO_PULLDOWN_DISABLE,/* 禁用下拉 */
        .intr_type = GPIO_INTR_DISABLE,       /* 禁用中断 */
    };

    /* 应用配置 */
    ESP_ERROR_CHECK(gpio_config(&io_conf));

    /* 初始状态设为低电平 (LED 关闭) */
    ESP_ERROR_CHECK(gpio_set_level(gpio_num, 0));

    ESP_LOGI(TAG, "GPIO %d 配置为输出模式", gpio_num);
}

/**
 * @brief 初始化按键 GPIO 为输入模式 (带中断)
 */
static void gpio_init_button(gpio_num_t gpio_num)
{
    /* 配置 GPIO */
    gpio_config_t io_conf = {
        .pin_bit_mask = (1ULL << gpio_num),  /* 选择 GPIO */
        .mode = GPIO_MODE_INPUT,              /* 设置为输入模式 */
        .pull_up_en = GPIO_PULLUP_ENABLE,     /* 启用内部上拉 */
        .pull_down_en = GPIO_PULLDOWN_DISABLE,/* 禁用下拉 */
        .intr_type = GPIO_INTR_NEGEDGE,       /* 下降沿触发中断 */
    };

    /* 应用配置 */
    ESP_ERROR_CHECK(gpio_config(&io_conf));

    ESP_LOGI(TAG, "GPIO %d 配置为输入模式 (带中断)", gpio_num);
}

/**
 * @brief GPIO 中断服务程序
 * @note 中断中只做最小操作,通过队列通知任务
 */
static void IRAM_ATTR gpio_isr_handler(void *arg)
{
    uint32_t gpio_num = (uint32_t) arg;

    /* 将 GPIO 号发送到队列 (在中断中安全) */
    xQueueSendFromISR(g_gpio_evt_queue, &gpio_num, NULL);
}

/**
 * @brief GPIO 事件处理任务
 */
static void gpio_task_example(void *arg)
{
    uint32_t io_num;
    uint32_t last_press_time = 0;
    const uint32_t debounce_ms = 200;  /* 消抖时间 200ms */

    while (1) {
        /* 等待队列消息 */
        if (xQueueReceive(g_gpio_evt_queue, &io_num, portMAX_DELAY)) {
            uint32_t current_time = xTaskGetTickCount() * portTICK_PERIOD_MS;

            /* 简单软件消抖 */
            if ((current_time - last_press_time) > debounce_ms) {
                g_button_press_count++;
                last_press_time = current_time;

                ESP_LOGI(TAG, "GPIO[%d] 中断触发, 按下次数: %d",
                         (int)io_num, (int)g_button_press_count);

                /* 切换 LED 状态 */
                static int led_state = 0;
                led_state = !led_state;
                gpio_set_level(LED_GPIO_PIN, led_state);
                gpio_set_level(EXT_LED_PIN, led_state);
            }
        }
    }
}

/**
 * @brief LED 闪烁任务
 */
static void led_blink_task(void *pvParameters)
{
    gpio_num_t led_pin = (gpio_num_t)(uintptr_t)pvParameters;
    int blink_count = 0;

    ESP_LOGI(TAG, "LED 闪烁任务启动, GPIO: %d", (int)led_pin);

    while (1) {
        /* LED 亮 */
        gpio_set_level(led_pin, 1);
        vTaskDelay(pdMS_TO_TICKS(500));

        /* LED 灭 */
        gpio_set_level(led_pin, 0);
        vTaskDelay(pdMS_TO_TICKS(500));

        blink_count++;
        if (blink_count % 10 == 0) {
            ESP_LOGI(TAG, "LED 已闪烁 %d 次", blink_count);
        }
    }
}

/**
 * @brief 主函数
 */
void app_main(void)
{
    ESP_LOGI(TAG, "ESP32 GPIO 基础示例启动");

    /* 创建队列用于 GPIO 事件 */
    g_gpio_evt_queue = xQueueCreate(10, sizeof(uint32_t));
    if (g_gpio_evt_queue == NULL) {
        ESP_LOGE(TAG, "创建队列失败!");
        return;
    }

    /* 初始化 GPIO */
    gpio_init_led(LED_GPIO_PIN);
    gpio_init_led(EXT_LED_PIN);
    gpio_init_button(BUTTON_GPIO_PIN);

    /* 安装 GPIO 中断服务 */
    ESP_ERROR_CHECK(gpio_install_isr_service(0));

    /* 添加中断处理程序 */
    ESP_ERROR_CHECK(gpio_isr_handler_add(BUTTON_GPIO_PIN, gpio_isr_handler,
                                          (void *)BUTTON_GPIO_PIN));

    /* 创建 GPIO 事件处理任务 */
    xTaskCreate(gpio_task_example, "gpio_task", 2048, NULL, 10, NULL);

    /* 创建 LED 闪烁任务 */
    xTaskCreate(led_blink_task, "led_blink", 1024,
                (void *)(uintptr_t)EXT_LED_PIN, 5, NULL);

    ESP_LOGI(TAG, "系统初始化完成,等待事件...");
    ESP_LOGI(TAG, "按 GPIO%d 按键切换 LED 状态", (int)BUTTON_GPIO_PIN);
}
```

### 3.2 高级 GPIO 控制: PWM (LEDC)

```c
/*
 * ledc_pwm_example.c
 * ESP32 LEDC (PWM) 高级控制示例
 * 功能: LED 呼吸灯、舵机控制、PWM 音乐
 */

#include "driver/ledc.h"
#include "esp_err.h"

/* LEDC 配置 */
#define LEDC_TIMER              LEDC_TIMER_0
#define LEDC_MODE               LEDC_LOW_SPEED_MODE
#define LEDC_OUTPUT_IO          GPIO_NUM_5      /* PWM 输出引脚 */
#define LEDC_CHANNEL            LEDC_CHANNEL_0
#define LEDC_DUTY_RES           LEDC_TIMER_13_BIT  /* 13 位分辨率 */
#define LEDC_FREQUENCY          5000            /* 5 kHz 频率 */

/* 舵机配置 */
#define SERVO_FREQ              50              /* 舵机标准 50Hz */
#define SERVO_MIN_PULSE_US      500             /* 0 度: 0.5ms */
#define SERVO_MAX_PULSE_US      2500            /* 180 度: 2.5ms */

/**
 * @brief 初始化 LEDC PWM 定时器
 */
static esp_err_t ledc_pwm_init(void)
{
    /* 配置定时器 */
    ledc_timer_config_t ledc_timer = {
        .speed_mode       = LEDC_MODE,
        .duty_resolution  = LEDC_DUTY_RES,
        .timer_num        = LEDC_TIMER,
        .freq_hz          = LEDC_FREQUENCY,
        .clk_cfg          = LEDC_AUTO_CLK,
    };
    ESP_ERROR_CHECK(ledc_timer_config(&ledc_timer));

    /* 配置通道 */
    ledc_channel_config_t ledc_channel = {
        .speed_mode     = LEDC_MODE,
        .channel        = LEDC_CHANNEL,
        .timer_sel      = LEDC_TIMER,
        .intr_type      = LEDC_INTR_DISABLE,
        .gpio_num       = LEDC_OUTPUT_IO,
        .duty           = 0,                    /* 初始占空比 0 */
        .hpoint         = 0,
    };
    ESP_ERROR_CHECK(ledc_channel_config(&ledc_channel));

    /* 启用渐变功能 */
    ESP_ERROR_CHECK(ledc_fade_func_install(0));

    return ESP_OK;
}

/**
 * @brief 设置 PWM 占空比
 * @param duty 占空比值 (0 - 8191, 对应 13 位分辨率)
 */
static void ledc_set_duty_cycle(uint32_t duty)
{
    /* 设置占空比 */
    ESP_ERROR_CHECK(ledc_set_duty(LEDC_MODE, LEDC_CHANNEL, duty));
    /* 更新占空比 */
    ESP_ERROR_CHECK(ledc_update_duty(LEDC_MODE, LEDC_CHANNEL));
}

/**
 * @brief 设置舵机角度
 * @param angle 角度 (0 - 180)
 */
static void servo_set_angle(uint8_t angle)
{
    /* 限制角度范围 */
    if (angle > 180) angle = 180;

    /* 计算脉冲宽度 */
    uint32_t pulse_us = SERVO_MIN_PULSE_US +
                        (angle * (SERVO_MAX_PULSE_US - SERVO_MIN_PULSE_US) / 180);

    /* 计算占空比 (对于 50Hz, 周期为 20ms = 20000us)
     * 占空比 = pulse_us / 20000 * 8191 (13位)
     */
    uint32_t duty = (pulse_us * 8191) / 20000;

    ledc_set_duty_cycle(duty);

    ESP_LOGI(TAG, "舵机角度: %d°, 脉冲: %d us, 占空比: %d",
             angle, (int)pulse_us, (int)duty);
}

/**
 * @brief 呼吸灯效果 (使用硬件渐变)
 */
static void breathing_led_task(void *pvParameters)
{
    const uint32_t fade_time_ms = 2000;  /* 渐变时间 2 秒 */

    while (1) {
        /* 渐亮 */
        ESP_ERROR_CHECK(ledc_set_fade_with_time(
            LEDC_MODE, LEDC_CHANNEL, 8191, fade_time_ms));
        ESP_ERROR_CHECK(ledc_fade_start(LEDC_MODE, LEDC_CHANNEL, LEDC_FADE_NO_WAIT));
        vTaskDelay(pdMS_TO_TICKS(fade_time_ms));

        /* 渐暗 */
        ESP_ERROR_CHECK(ledc_set_fade_with_time(
            LEDC_MODE, LEDC_CHANNEL, 0, fade_time_ms));
        ESP_ERROR_CHECK(ledc_fade_start(LEDC_MODE, LEDC_CHANNEL, LEDC_FADE_NO_WAIT));
        vTaskDelay(pdMS_TO_TICKS(fade_time_ms));
    }
}

/**
 * @brief 舵机扫描任务
 */
static void servo_sweep_task(void *pvParameters)
{
    uint8_t angle = 0;
    int8_t direction = 1;

    while (1) {
        servo_set_angle(angle);

        angle += direction * 5;  /* 每次增加 5 度 */

        if (angle >= 180) {
            angle = 180;
            direction = -1;
        } else if (angle == 0) {
            direction = 1;
        }

        vTaskDelay(pdMS_TO_TICKS(50));  /* 每 50ms 移动一次 */
    }
}
```

### 3.3 ADC (模数转换)

```c
/*
 * adc_example.c
 * ESP32 ADC 模数转换示例
 */

#include "driver/adc.h"
#include "esp_adc_cal.h"

#define ADC_CHANNEL         ADC1_CHANNEL_6    /* GPIO34 */
#define ADC_ATTEN           ADC_ATTEN_DB_11   /* 0-3.3V 量程 */
#define ADC_WIDTH           ADC_WIDTH_BIT_12  /* 12 位分辨率 */

static esp_adc_cal_characteristics_t adc_chars;

/**
 * @brief 初始化 ADC
 */
static void adc_init(void)
{
    /* 配置 ADC 宽度 */
    ESP_ERROR_CHECK(adc1_config_width(ADC_WIDTH));

    /* 配置 ADC 衰减 (量程) */
    ESP_ERROR_CHECK(adc1_config_channel_atten(ADC_CHANNEL, ADC_ATTEN));

    /* 校准 ADC (使用默认 Vref, 或使用 efuse 校准值) */
    esp_adc_cal_characterize(ADC_UNIT_1, ADC_ATTEN, ADC_WIDTH,
                             DEFAULT_VREF, &adc_chars);
}

/**
 * @brief 读取 ADC 电压值
 * @return 电压值 (mV)
 */
static uint32_t adc_read_voltage(void)
{
    /* 多次采样取平均 */
    uint32_t adc_reading = 0;
    const int samples = 64;

    for (int i = 0; i < samples; i++) {
        adc_reading += adc1_get_raw(ADC_CHANNEL);
    }
    adc_reading /= samples;

    /* 转换为电压 (mV) */
    uint32_t voltage = esp_adc_cal_raw_to_voltage(adc_reading, &adc_chars);

    return voltage;
}

/**
 * @brief 温度传感器读取 (ESP32-S3/C3 内置)
 */
static float read_temperature(void)
{
    /* ESP32-S3/C3 内置温度传感器 */
    float temp_c = 0.0f;

    /* 启用温度传感器 */
    temperature_sensor_handle_t temp_handle = NULL;
    temperature_sensor_config_t temp_sensor = {
        .range_min = -10,
        .range_max = 80,
    };

    ESP_ERROR_CHECK(temperature_sensor_install(&temp_sensor, &temp_handle));
    ESP_ERROR_CHECK(temperature_sensor_enable(temp_handle));
    ESP_ERROR_CHECK(temperature_sensor_get_celsius(temp_handle, &temp_c));
    ESP_ERROR_CHECK(temperature_sensor_disable(temp_handle));
    ESP_ERROR_CHECK(temperature_sensor_uninstall(temp_handle));

    return temp_c;
}
```

---

## 4. WiFi 网络连接

### 4.1 WiFi Station (STA) 模式

```c
/*
 * wifi_sta_example.c
 * ESP32 WiFi Station 模式示例
 * 功能: 连接 WiFi, 自动重连, 获取 IP, 网络事件处理
 */

#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "esp_system.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "lwip/err.h"
#include "lwip/sys.h"

/* WiFi 配置 - 请修改为实际值 */
#define WIFI_SSID           "Your_SSID"
#define WIFI_PASS           "Your_Password"
#define WIFI_MAX_RETRY      5

/* FreeRTOS 事件组位 */
#define WIFI_CONNECTED_BIT  BIT0
#define WIFI_FAIL_BIT       BIT1

/* 全局变量 */
static EventGroupHandle_t s_wifi_event_group;
static const char *TAG = "WIFI_STA";
static int s_retry_num = 0;

/* 函数声明 */
static void wifi_event_handler(void *arg, esp_event_base_t event_base,
                               int32_t event_id, void *event_data);
static void wifi_init_sta(void);

/**
 * @brief WiFi 事件处理函数
 */
static void wifi_event_handler(void *arg, esp_event_base_t event_base,
                               int32_t event_id, void *event_data)
{
    if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_START) {
        /* WiFi 启动后开始连接 */
        esp_wifi_connect();
        ESP_LOGI(TAG, "WiFi 启动,开始连接...");

    } else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_DISCONNECTED) {
        /* 连接断开,尝试重连 */
        if (s_retry_num < WIFI_MAX_RETRY) {
            esp_wifi_connect();
            s_retry_num++;
            ESP_LOGI(TAG, "连接失败,重试 %d/%d", s_retry_num, WIFI_MAX_RETRY);
        } else {
            xEventGroupSetBits(s_wifi_event_group, WIFI_FAIL_BIT);
            ESP_LOGE(TAG, "连接失败,已达最大重试次数");
        }

    } else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP) {
        /* 获取 IP 地址 */
        ip_event_got_ip_t *event = (ip_event_got_ip_t *) event_data;
        ESP_LOGI(TAG, "获取 IP 地址: " IPSTR, IP2STR(&event->ip_info.ip));
        s_retry_num = 0;
        xEventGroupSetBits(s_wifi_event_group, WIFI_CONNECTED_BIT);
    }
}

/**
 * @brief 初始化 WiFi Station 模式
 */
static void wifi_init_sta(void)
{
    /* 创建事件组 */
    s_wifi_event_group = xEventGroupCreate();

    /* 初始化 TCP/IP 栈 */
    ESP_ERROR_CHECK(esp_netif_init());

    /* 创建默认事件循环 */
    ESP_ERROR_CHECK(esp_event_loop_create_default());

    /* 创建默认 WiFi Station */
    esp_netif_create_default_wifi_sta();

    /* 初始化 WiFi */
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));

    /* 注册事件处理程序 */
    esp_event_handler_instance_t instance_any_id;
    esp_event_handler_instance_t instance_got_ip;

    ESP_ERROR_CHECK(esp_event_handler_instance_register(
        WIFI_EVENT,
        ESP_EVENT_ANY_ID,
        &wifi_event_handler,
        NULL,
        &instance_any_id));

    ESP_ERROR_CHECK(esp_event_handler_instance_register(
        IP_EVENT,
        IP_EVENT_STA_GOT_IP,
        &wifi_event_handler,
        NULL,
        &instance_got_ip));

    /* 配置 WiFi */
    wifi_config_t wifi_config = {
        .sta = {
            .ssid = WIFI_SSID,
            .password = WIFI_PASS,
            .threshold.authmode = WIFI_AUTH_WPA2_PSK,
            .pmf_cfg = {
                .capable = true,
                .required = false
            },
        },
    };

    /* 设置 WiFi 模式并启动 */
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &wifi_config));
    ESP_ERROR_CHECK(esp_wifi_start());

    ESP_LOGI(TAG, "WiFi STA 初始化完成");

    /* 等待连接结果 */
    EventBits_t bits = xEventGroupWaitBits(s_wifi_event_group,
            WIFI_CONNECTED_BIT | WIFI_FAIL_BIT,
            pdFALSE,
            pdFALSE,
            portMAX_DELAY);

    if (bits & WIFI_CONNECTED_BIT) {
        ESP_LOGI(TAG, "成功连接到 AP, SSID:%s", WIFI_SSID);
    } else if (bits & WIFI_FAIL_BIT) {
        ESP_LOGE(TAG, "无法连接到 AP, SSID:%s", WIFI_SSID);
    } else {
        ESP_LOGE(TAG, "未知错误");
    }

    /* 清理 */
    ESP_ERROR_CHECK(esp_event_handler_instance_unregister(IP_EVENT,
                    IP_EVENT_STA_GOT_IP, instance_got_ip));
    ESP_ERROR_CHECK(esp_event_handler_instance_unregister(WIFI_EVENT,
                    ESP_EVENT_ANY_ID, instance_any_id));
    vEventGroupDelete(s_wifi_event_group);
}

/**
 * @brief 获取 WiFi 连接信息
 */
static void wifi_print_info(void)
{
    wifi_ap_record_t ap_info;

    if (esp_wifi_sta_get_ap_info(&ap_info) == ESP_OK) {
        ESP_LOGI(TAG, "已连接到:");
        ESP_LOGI(TAG, "  SSID: %s", ap_info.ssid);
        ESP_LOGI(TAG, "  BSSID: %02x:%02x:%02x:%02x:%02x:%02x",
                 ap_info.bssid[0], ap_info.bssid[1], ap_info.bssid[2],
                 ap_info.bssid[3], ap_info.bssid[4], ap_info.bssid[5]);
        ESP_LOGI(TAG, "  RSSI: %d dBm", ap_info.rssi);
        ESP_LOGI(TAG, "  Channel: %d", ap_info.primary);
        ESP_LOGI(TAG, "  Authmode: %s",
                 ap_info.authmode == WIFI_AUTH_OPEN ? "OPEN" :
                 ap_info.authmode == WIFI_AUTH_WPA2_PSK ? "WPA2_PSK" : "OTHER");
    }
}

void app_main(void)
{
    /* 初始化 NVS (必需,用于存储 WiFi 配置) */
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);

    ESP_LOGI(TAG, "ESP32 WiFi Station 示例");

    /* 初始化并连接 WiFi */
    wifi_init_sta();

    /* 打印连接信息 */
    wifi_print_info();

    /* 主循环 */
    while (1) {
        vTaskDelay(pdMS_TO_TICKS(10000));
        wifi_print_info();
    }
}
```

### 4.2 WiFi Access Point (AP) 模式

```c
/*
 * wifi_ap_example.c
 * ESP32 WiFi AP (热点) 模式示例
 */

#include "esp_wifi.h"

#define AP_SSID             "ESP32_AP"
#define AP_PASS             "12345678"
#define AP_MAX_CONNECTIONS  4
#define AP_CHANNEL          6

static const char *TAG_AP = "WIFI_AP";

/**
 * @brief 初始化 WiFi AP 模式
 */
static void wifi_init_softap(void)
{
    /* 初始化网络接口 */
    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());

    /* 创建 AP 接口 */
    esp_netif_create_default_wifi_ap();

    /* 初始化 WiFi */
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));

    /* 配置 AP */
    wifi_config_t wifi_config = {
        .ap = {
            .ssid = AP_SSID,
            .ssid_len = strlen(AP_SSID),
            .channel = AP_CHANNEL,
            .password = AP_PASS,
            .max_connection = AP_MAX_CONNECTIONS,
            .authmode = WIFI_AUTH_WPA_WPA2_PSK,
            .pmf_cfg = {
                .required = true,
            },
        },
    };

    if (strlen(AP_PASS) == 0) {
        wifi_config.ap.authmode = WIFI_AUTH_OPEN;
    }

    /* 设置模式并启动 AP */
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_AP));
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_AP, &wifi_config));
    ESP_ERROR_CHECK(esp_wifi_start());

    /* 打印 AP 信息 */
    ESP_LOGI(TAG_AP, "WiFi AP 初始化完成");
    ESP_LOGI(TAG_AP, "SSID: %s", AP_SSID);
    ESP_LOGI(TAG_AP, "密码: %s", AP_PASS);
    ESP_LOGI(TAG_AP, "Channel: %d", AP_CHANNEL);
    ESP_LOGI(TAG_AP, "最大连接数: %d", AP_MAX_CONNECTIONS);

    /* 获取并打印 IP 地址 */
    esp_netif_ip_info_t ip_info;
    esp_netif_get_ip_info(esp_netif_get_handle_from_ifkey("WIFI_AP_DEF"), &ip_info);
    ESP_LOGI(TAG_AP, "AP IP: " IPSTR, IP2STR(&ip_info.ip));
}

/**
 * @brief 获取已连接设备列表
 */
static void print_connected_stations(void)
{
    wifi_sta_list_t sta_list;

    if (esp_wifi_ap_get_sta_list(&sta_list) == ESP_OK) {
        ESP_LOGI(TAG_AP, "已连接设备数: %d", sta_list.num);

        for (int i = 0; i < sta_list.num; i++) {
            ESP_LOGI(TAG_AP, "  [%d] MAC: %02x:%02x:%02x:%02x:%02x:%02x, RSSI: %d",
                     i + 1,
                     sta_list.sta[i].mac[0], sta_list.sta[i].mac[1],
                     sta_list.sta[i].mac[2], sta_list.sta[i].mac[3],
                     sta_list.sta[i].mac[4], sta_list.sta[i].mac[5],
                     sta_list.sta[i].rssi);
        }
    }
}
```

### 4.3 HTTP Client 示例

```c
/*
 * http_client_example.c
 * ESP32 HTTP 客户端示例
 */

#include "esp_http_client.h"
#include "cJSON.h"

#define HTTP_TAG            "HTTP_CLIENT"
#define MAX_HTTP_OUTPUT_BUFFER 512

/**
 * @brief HTTP 事件处理回调
 */
static esp_err_t http_event_handler(esp_http_client_event_t *evt)
{
    static char *output_buffer;  /* 响应缓冲区 */
    static int output_len;       /* 已接收数据长度 */

    switch (evt->event_id) {
        case HTTP_EVENT_ERROR:
            ESP_LOGI(HTTP_TAG, "HTTP_EVENT_ERROR");
            break;

        case HTTP_EVENT_ON_CONNECTED:
            ESP_LOGI(HTTP_TAG, "HTTP_EVENT_ON_CONNECTED");
            break;

        case HTTP_EVENT_HEADER_SENT:
            ESP_LOGI(HTTP_TAG, "HTTP_EVENT_HEADER_SENT");
            break;

        case HTTP_EVENT_ON_HEADER:
            ESP_LOGI(HTTP_TAG, "HTTP_EVENT_ON_HEADER, key=%s, value=%s",
                     evt->header_key, evt->header_value);
            break;

        case HTTP_EVENT_ON_DATA:
            ESP_LOGI(HTTP_TAG, "HTTP_EVENT_ON_DATA, len=%d", evt->data_len);

            /* 累积响应数据 */
            if (!esp_http_client_is_chunked_response(evt->client)) {
                if (evt->user_data) {
                    memcpy(evt->user_data + output_len, evt->data, evt->data_len);
                }
                output_len += evt->data_len;
            }
            break;

        case HTTP_EVENT_ON_FINISH:
            ESP_LOGI(HTTP_TAG, "HTTP_EVENT_ON_FINISH");
            output_len = 0;
            break;

        case HTTP_EVENT_DISCONNECTED:
            ESP_LOGI(HTTP_TAG, "HTTP_EVENT_DISCONNECTED");
            output_len = 0;
            break;

        default:
            break;
    }
    return ESP_OK;
}

/**
 * @brief HTTP GET 请求
 */
static void http_get_request(const char *url)
{
    char local_response_buffer[MAX_HTTP_OUTPUT_BUFFER] = {0};

    esp_http_client_config_t config = {
        .url = url,
        .method = HTTP_METHOD_GET,
        .event_handler = http_event_handler,
        .user_data = local_response_buffer,
        .disable_auto_redirect = true,
        .timeout_ms = 10000,
    };

    esp_http_client_handle_t client = esp_http_client_init(&config);

    /* 执行请求 */
    esp_err_t err = esp_http_client_perform(client);

    if (err == ESP_OK) {
        int status_code = esp_http_client_get_status_code(client);
        int content_length = esp_http_client_get_content_length(client);

        ESP_LOGI(HTTP_TAG, "HTTP 状态码: %d, 内容长度: %d",
                 status_code, content_length);
        ESP_LOGI(HTTP_TAG, "响应: %s", local_response_buffer);
    } else {
        ESP_LOGE(HTTP_TAG, "HTTP GET 请求失败: %s", esp_err_to_name(err));
    }

    esp_http_client_cleanup(client);
}

/**
 * @brief HTTP POST 请求 (JSON)
 */
static void http_post_json(const char *url, const char *json_data)
{
    char local_response_buffer[MAX_HTTP_OUTPUT_BUFFER] = {0};

    esp_http_client_config_t config = {
        .url = url,
        .method = HTTP_METHOD_POST,
        .event_handler = http_event_handler,
        .user_data = local_response_buffer,
    };

    esp_http_client_handle_t client = esp_http_client_init(&config);

    /* 设置请求头 */
    esp_http_client_set_header(client, "Content-Type", "application/json");

    /* 设置 POST 数据 */
    esp_http_client_set_post_field(client, json_data, strlen(json_data));

    /* 执行请求 */
    esp_err_t err = esp_http_client_perform(client);

    if (err == ESP_OK) {
        ESP_LOGI(HTTP_TAG, "HTTP POST 成功, 状态码: %d",
                 esp_http_client_get_status_code(client));
    } else {
        ESP_LOGE(HTTP_TAG, "HTTP POST 请求失败: %s", esp_err_to_name(err));
    }

    esp_http_client_cleanup(client);
}
```


---

## 5. 蓝牙低功耗 (BLE)

### 5.1 BLE GATT 服务器

```c
/*
 * ble_gatt_server_example.c
 * ESP32 BLE GATT 服务器完整示例
 * 功能: 创建 BLE 服务、特征值读写、通知发送
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "esp_system.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "esp_bt.h"
#include "esp_gap_ble_api.h"
#include "esp_gatts_api.h"
#include "esp_bt_defs.h"
#include "esp_bt_main.h"
#include "esp_gatt_common_api.h"

#define GATTS_TAG               "GATTS_DEMO"
#define GATTS_SERVICE_UUID      0x00FF
#define GATTS_CHAR_UUID         0xFF01
#define GATTS_DESCR_UUID        0x3333
#define GATTS_NUM_HANDLE        4

#define DEVICE_NAME             "ESP32_GATTS"
#define GATTS_DEMO_CHAR_VAL_LEN_MAX 0x40

/* 应用程序 ID */
#define PROFILE_A_APP_ID        0
#define PROFILE_B_APP_ID        1

/* 属性表索引 */
enum {
    IDX_SVC,
    IDX_CHAR_A,
    IDX_CHAR_VAL_A,
    IDX_CHAR_CFG_A,
    IDX_NB,
};

/* 全局变量 */
static uint8_t char1_str[] = {0x11, 0x22, 0x33};
static esp_gatt_char_prop_t a_property = 0;
static uint8_t adv_config_done = 0;

/* GATT 属性表 */
static esp_gatts_attr_db_t gatt_db[IDX_NB];

/* 特性值 */
static esp_attr_value_t gatts_demo_char1_val = {
    .attr_max_len = GATTS_DEMO_CHAR_VAL_LEN_MAX,
    .attr_len     = sizeof(char1_str),
    .attr_value   = char1_str,
};

/* GATT 接口 */
static esp_gatt_if_t gatts_if_for_notify = ESP_GATT_IF_NONE;
static uint16_t notify_conn_id = 0;
static uint16_t notify_handle = 0;

/* 广播参数 */
static esp_ble_adv_params_t adv_params = {
    .adv_int_min        = 0x20,
    .adv_int_max        = 0x40,
    .adv_type           = ADV_TYPE_IND,
    .own_addr_type      = BLE_ADDR_TYPE_PUBLIC,
    .channel_map        = ADV_CHNL_ALL,
    .adv_filter_policy  = ADV_FILTER_ALLOW_SCAN_ANY_CON_ANY,
};

/* 广播数据 */
static uint8_t adv_service_uuid128[16] = {
    /* LSB <--------------------------------------------------------------------------------> MSB */
    0xfb, 0x34, 0x9b, 0x5f, 0x80, 0x00, 0x00, 0x80, 0x00, 0x10, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00,
};

static esp_ble_adv_data_t adv_data = {
    .set_scan_rsp = false,
    .include_name = true,
    .include_txpower = false,
    .min_interval = 0x0006,
    .max_interval = 0x0010,
    .appearance = 0x00,
    .manufacturer_len = 0,
    .p_manufacturer_data = NULL,
    .service_data_len = 0,
    .p_service_data = NULL,
    .service_uuid_len = sizeof(adv_service_uuid128),
    .p_service_uuid = adv_service_uuid128,
    .flag = (ESP_BLE_ADV_FLAG_GEN_DISC | ESP_BLE_ADV_FLAG_BREDR_NOT_SPT),
};

static esp_ble_adv_data_t scan_rsp_data = {
    .set_scan_rsp = true,
    .include_name = true,
    .include_txpower = true,
    .appearance = 0x00,
    .manufacturer_len = 0,
    .p_manufacturer_data = NULL,
    .service_data_len = 0,
    .p_service_data = NULL,
    .service_uuid_len = sizeof(adv_service_uuid128),
    .p_service_uuid = adv_service_uuid128,
    .flag = (ESP_BLE_ADV_FLAG_GEN_DISC | ESP_BLE_ADV_FLAG_BREDR_NOT_SPT),
};

/**
 * @brief 准备写入响应
 */
void example_prepare_write_event_env(esp_gatt_if_t gatts_if,
                                     prepare_type_env_t *prepare_write_env,
                                     esp_ble_gatts_cb_param_t *param)
{
    ESP_LOGI(GATTS_TAG, "准备写入, handle = %d, len = %d",
             param->write.handle, param->write.len);

    if (param->write.offset > prepare_write_env->prepare_len) {
        return;
    }

    if (prepare_write_env->prepare_buf == NULL) {
        prepare_write_env->prepare_buf = (uint8_t *)malloc(PREPARE_BUF_MAX_SIZE * sizeof(uint8_t));
        prepare_write_env->prepare_len = 0;
    }

    memcpy(prepare_write_env->prepare_buf + param->write.offset,
           param->write.value,
           param->write.len);

    prepare_write_env->prepare_len += param->write.len;
}

/**
 * @brief 执行写入响应
 */
void example_exec_write_event_env(prepare_type_env_t *prepare_write_env,
                                  esp_ble_gatts_cb_param_t *param)
{
    if (param->exec_write.exec_write_flag == ESP_GATT_PREP_WRITE_EXEC) {
        ESP_LOG_BUFFER_HEX(GATTS_TAG, prepare_write_env->prepare_buf, prepare_write_env->prepare_len);
    } else {
        ESP_LOGI(GATTS_TAG, "取消写入");
    }

    if (prepare_write_env->prepare_buf) {
        free(prepare_write_env->prepare_buf);
        prepare_write_env->prepare_buf = NULL;
    }
    prepare_write_env->prepare_len = 0;
}

/**
 * @brief GATT 事件处理
 */
static void gatts_profile_a_event_handler(esp_gatts_cb_event_t event,
                                          esp_gatt_if_t gatts_if,
                                          esp_ble_gatts_cb_param_t *param)
{
    switch (event) {
    case ESP_GATTS_REG_EVT: {
        ESP_LOGI(GATTS_TAG, "GATT 注册事件, status = %d, app_id = %d",
                 param->reg.status, param->reg.app_id);

        /* 设置设备名称 */
        esp_ble_gap_set_device_name(DEVICE_NAME);

        /* 配置广播数据 */
        esp_ble_gap_config_adv_data(&adv_data);
        adv_config_done |= adv_config_flag;

        /* 配置扫描响应数据 */
        esp_ble_gap_config_adv_data(&scan_rsp_data);
        adv_config_done |= scan_rsp_config_flag;

        /* 创建属性表 */
        esp_ble_gatts_create_attr_tab(gatt_db, gatts_if, IDX_NB, SVC_INST_ID);
        break;
    }

    case ESP_GATTS_READ_EVT: {
        ESP_LOGI(GATTS_TAG, "读取事件, conn_id %d, trans_id %d, handle %d",
                 param->read.conn_id, param->read.trans_id, param->read.handle);

        esp_gatt_rsp_t rsp;
        memset(&rsp, 0, sizeof(esp_gatt_rsp_t));
        rsp.attr_value.handle = param->read.handle;
        rsp.attr_value.len = 4;
        rsp.attr_value.value[0] = 0x12;
        rsp.attr_value.value[1] = 0x34;
        rsp.attr_value.value[2] = 0x56;
        rsp.attr_value.value[3] = 0x78;

        esp_ble_gatts_send_response(gatts_if, param->read.conn_id,
                                    param->read.trans_id, ESP_GATT_OK, &rsp);
        break;
    }

    case ESP_GATTS_WRITE_EVT: {
        ESP_LOGI(GATTS_TAG, "写入事件, handle = %d, len = %d",
                 param->write.handle, param->write.len);

        if (!param->write.is_prep) {
            ESP_LOGI(GATTS_TAG, "写入值:");
            ESP_LOG_BUFFER_HEX(GATTS_TAG, param->write.value, param->write.len);

            /* 检查是否需要响应 */
            if (param->write.need_rsp) {
                esp_ble_gatts_send_response(gatts_if, param->write.conn_id,
                                            param->write.trans_id, ESP_GATT_OK, NULL);
            }
        } else {
            /* 准备写入 (长数据) */
            example_prepare_write_event_env(gatts_if, &a_prepare_write_env, param);
        }
        break;
    }

    case ESP_GATTS_EXEC_WRITE_EVT: {
        ESP_LOGI(GATTS_TAG, "执行写入事件");
        example_exec_write_event_env(&a_prepare_write_env, param);
        break;
    }

    case ESP_GATTS_MTU_EVT: {
        ESP_LOGI(GATTS_TAG, "MTU 设置: %d", param->mtu.mtu);
        break;
    }

    case ESP_GATTS_CONF_EVT: {
        ESP_LOGI(GATTS_TAG, "通知/指示确认");
        break;
    }

    case ESP_GATTS_CONNECT_EVT: {
        ESP_LOGI(GATTS_TAG, "连接事件, conn_id = %d", param->connect.conn_id);

        /* 更新连接参数 */
        esp_ble_conn_update_params_t conn_params = {0};
        memcpy(conn_params.bda, param->connect.remote_bda, sizeof(esp_bd_addr_t));
        conn_params.latency = 0;
        conn_params.max_int = 0x20;
        conn_params.min_int = 0x10;
        conn_params.timeout = 400;

        gatts_if_for_notify = gatts_if;
        notify_conn_id = param->connect.conn_id;

        /* 发送连接参数更新 */
        esp_ble_gap_update_conn_params(&conn_params);
        break;
    }

    case ESP_GATTS_DISCONNECT_EVT: {
        ESP_LOGI(GATTS_TAG, "断开连接, reason = 0x%x", param->disconnect.reason);

        /* 重新开始广播 */
        esp_ble_gap_start_advertising(&adv_params);
        break;
    }

    case ESP_GATTS_CREAT_ATTR_TAB_EVT: {
        if (param->add_attr_tab.status != ESP_GATT_OK) {
            ESP_LOGE(GATTS_TAG, "创建属性表失败, error code = 0x%x",
                     param->add_attr_tab.status);
        } else if (param->add_attr_tab.num_handle != IDX_NB) {
            ESP_LOGE(GATTS_TAG, "创建的句柄数量错误");
        } else {
            ESP_LOGI(GATTS_TAG, "属性表创建成功, handles:");
            for (int i = 0; i < IDX_NB; i++) {
                ESP_LOGI(GATTS_TAG, "  handle[%d] = %d", i, param->add_attr_tab.handles[i]);
            }

            /* 保存通知句柄 */
            notify_handle = param->add_attr_tab.handles[IDX_CHAR_CFG_A];

            /* 启动服务 */
            esp_ble_gatts_start_service(param->add_attr_tab.handles[IDX_SVC]);
        }
        break;
    }

    default:
        break;
    }
}

/**
 * @brief GAP 事件处理
 */
static void gap_event_handler(esp_gap_ble_cb_event_t event, esp_ble_gap_cb_param_t *param)
{
    switch (event) {
    case ESP_GAP_BLE_ADV_DATA_SET_COMPLETE_EVT:
        adv_config_done &= (~adv_config_flag);
        if (adv_config_done == 0) {
            esp_ble_gap_start_advertising(&adv_params);
        }
        break;

    case ESP_GAP_BLE_SCAN_RSP_DATA_SET_COMPLETE_EVT:
        adv_config_done &= (~scan_rsp_config_flag);
        if (adv_config_done == 0) {
            esp_ble_gap_start_advertising(&adv_params);
        }
        break;

    case ESP_GAP_BLE_ADV_START_COMPLETE_EVT:
        if (param->adv_start_cmpl.status != ESP_BT_STATUS_SUCCESS) {
            ESP_LOGE(GATTS_TAG, "广播启动失败");
        } else {
            ESP_LOGI(GATTS_TAG, "广播启动成功");
        }
        break;

    case ESP_GAP_BLE_UPDATE_CONN_PARAMS_EVT:
        ESP_LOGI(GATTS_TAG, "连接参数更新:");
        ESP_LOGI(GATTS_TAG, "  status = %d", param->update_conn_params.status);
        ESP_LOGI(GATTS_TAG, "  min_int = %d", param->update_conn_params.conn_int_min);
        ESP_LOGI(GATTS_TAG, "  max_int = %d", param->update_conn_params.conn_int_max);
        ESP_LOGI(GATTS_TAG, "  latency = %d", param->update_conn_params.conn_latency);
        ESP_LOGI(GATTS_TAG, "  timeout = %d", param->update_conn_params.supervision_timeout);
        break;

    default:
        break;
    }
}

/**
 * @brief 发送通知任务
 */
static void notify_task(void *pvParameters)
{
    uint8_t notify_data[4] = {0x01, 0x02, 0x03, 0x04};
    uint8_t counter = 0;

    while (1) {
        vTaskDelay(pdMS_TO_TICKS(5000));

        if (gatts_if_for_notify != ESP_GATT_IF_NONE && notify_handle != 0) {
            /* 更新数据 */
            notify_data[3] = counter++;

            /* 发送通知 */
            esp_ble_gatts_send_indicate(gatts_if_for_notify, notify_conn_id,
                                        notify_handle, sizeof(notify_data),
                                        notify_data, false);

            ESP_LOGI(GATTS_TAG, "发送通知: %d", counter);
        }
    }
}

/**
 * @brief BLE 初始化
 */
static void ble_init(void)
{
    ESP_ERROR_CHECK(esp_bt_controller_mem_release(ESP_BT_MODE_CLASSIC_BT));

    /* 初始化 BLE 控制器 */
    esp_bt_controller_config_t bt_cfg = BT_CONTROLLER_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_bt_controller_init(&bt_cfg));
    ESP_ERROR_CHECK(esp_bt_controller_enable(ESP_BT_MODE_BLE));

    /* 初始化 Bluedroid */
    ESP_ERROR_CHECK(esp_bluedroid_init());
    ESP_ERROR_CHECK(esp_bluedroid_enable());

    /* 注册 GAP 回调 */
    ESP_ERROR_CHECK(esp_ble_gap_register_callback(gap_event_handler));

    /* 注册 GATT 回调 */
    ESP_ERROR_CHECK(esp_ble_gatts_register_callback(gatts_event_handler));

    /* 注册应用程序 */
    ESP_ERROR_CHECK(esp_ble_gatts_app_register(PROFILE_A_APP_ID));
}

void app_main(void)
{
    /* 初始化 NVS */
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);

    ESP_LOGI(GATTS_TAG, "ESP32 BLE GATT 服务器启动");

    /* 初始化 BLE */
    ble_init();

    /* 创建通知任务 */
    xTaskCreate(notify_task, "notify_task", 2048, NULL, 5, NULL);
}
```

### 5.2 BLE GATT 客户端

```c
/*
 * ble_gatt_client_example.c
 * ESP32 BLE GATT 客户端示例
 * 功能: 扫描设备、连接、发现服务、读写特征值
 */

#include "esp_gap_ble_api.h"
#include "esp_gattc_api.h"

#define GATTC_TAG           "GATTC_DEMO"
#define REMOTE_SERVICE_UUID 0x00FF
#define REMOTE_NOTIFY_UUID  0xFF01
#define PROFILE_NUM         1
#define PROFILE_A_APP_ID    0
#define INVALID_HANDLE      0

/* 扫描参数 */
static esp_ble_scan_params_t ble_scan_params = {
    .scan_type              = BLE_SCAN_TYPE_ACTIVE,
    .own_addr_type          = BLE_ADDR_TYPE_PUBLIC,
    .scan_filter_policy     = BLE_SCAN_FILTER_ALLOW_ALL,
    .scan_interval          = 0x50,
    .scan_window            = 0x30,
    .scan_duplicate         = BLE_SCAN_DUPLICATE_DISABLE
};

/* 连接配置 */
struct gattc_profile_inst {
    esp_gattc_cb_t gattc_cb;
    uint16_t gattc_if;
    uint16_t app_id;
    uint16_t conn_id;
    esp_bd_addr_t remote_bda;
    esp_gatt_if_t gattc_if;
    uint16_t service_start_handle;
    uint16_t service_end_handle;
    uint16_t char_handle;
    esp_bt_uuid_t remote_filter_service_uuid;
    esp_bt_uuid_t remote_filter_char_uuid;
};

static struct gattc_profile_inst gl_profile_tab[PROFILE_NUM] = {
    [PROFILE_A_APP_ID] = {
        .gattc_cb = gattc_profile_event_handler,
        .gattc_if = ESP_GATT_IF_NONE,
    },
};

/**
 * @brief GATT 客户端事件处理
 */
static void gattc_profile_event_handler(esp_gattc_cb_event_t event,
                                        esp_gatt_if_t gattc_if,
                                        esp_ble_gattc_cb_param_t *param)
{
    esp_ble_gattc_cb_param_t *p_data = (esp_ble_gattc_cb_param_t *)param;

    switch (event) {
    case ESP_GATTC_REG_EVT:
        ESP_LOGI(GATTC_TAG, "GATT 客户端注册成功");

        /* 设置扫描参数 */
        esp_err_t scan_ret = esp_ble_gap_set_scan_params(&ble_scan_params);
        if (scan_ret) {
            ESP_LOGE(GATTC_TAG, "设置扫描参数失败: %d", scan_ret);
        }
        break;

    case ESP_GATTC_CONNECT_EVT: {
        ESP_LOGI(GATTC_TAG, "已连接到设备, conn_id = %d", p_data->connect.conn_id);

        gl_profile_tab[PROFILE_A_APP_ID].conn_id = p_data->connect.conn_id;
        memcpy(gl_profile_tab[PROFILE_A_APP_ID].remote_bda,
               p_data->connect.remote_bda, sizeof(esp_bd_addr_t));

        /* 更新连接参数 */
        esp_ble_conn_update_params_t conn_params = {0};
        memcpy(conn_params.bda, p_data->connect.remote_bda, sizeof(esp_bd_addr_t));
        conn_params.latency = 0;
        conn_params.max_int = 0x30;
        conn_params.min_int = 0x10;
        conn_params.timeout = 500;

        /* 请求 MTU */
        esp_ble_gattc_send_mtu_req(gattc_if, p_data->connect.conn_id);
        break;
    }

    case ESP_GATTC_OPEN_EVT:
        if (param->open.status != ESP_GATT_OK) {
            ESP_LOGE(GATTC_TAG, "打开连接失败, status = %d", param->open.status);
        } else {
            ESP_LOGI(GATTC_TAG, "连接打开成功");
        }
        break;

    case ESP_GATTC_DIS_SRVC_CMPL_EVT:
        if (param->dis_srvc_cmpl.status != ESP_GATT_OK) {
            ESP_LOGE(GATTC_TAG, "发现服务失败");
            break;
        }
        ESP_LOGI(GATTC_TAG, "发现服务完成");

        /* 搜索指定服务 */
        esp_ble_gattc_search_service(gattc_if, param->dis_srvc_cmpl.conn_id,
                                     &remote_filter_service_uuid);
        break;

    case ESP_GATTC_SEARCH_RES_EVT: {
        ESP_LOGI(GATTC_TAG, "发现服务, UUID = 0x%04x",
                 param->search_res.srvc_id.uuid.uuid.uuid16);

        gl_profile_tab[PROFILE_A_APP_ID].service_start_handle =
            param->search_res.start_handle;
        gl_profile_tab[PROFILE_A_APP_ID].service_end_handle =
            param->search_res.end_handle;
        break;
    }

    case ESP_GATTC_SEARCH_CMPL_EVT:
        if (p_data->search_cmpl.status != ESP_GATT_OK) {
            ESP_LOGE(GATTC_TAG, "搜索服务失败");
            break;
        }

        if (p_data->search_cmpl.searched_service_source == ESP_GATT_SERVICE_FROM_REMOTE_DEVICE) {
            ESP_LOGI(GATTC_TAG, "从远程设备获取服务信息");
        }

        /* 获取特征值 */
        uint16_t count = 0;
        esp_gatt_status_t status = esp_ble_gattc_get_attr_count(
            gattc_if,
            p_data->search_cmpl.conn_id,
            ESP_GATT_DB_CHARACTERISTIC,
            gl_profile_tab[PROFILE_A_APP_ID].service_start_handle,
            gl_profile_tab[PROFILE_A_APP_ID].service_end_handle,
            INVALID_HANDLE,
            &count);

        if (status != ESP_GATT_OK) {
            ESP_LOGE(GATTC_TAG, "获取特征值数量失败");
        }

        ESP_LOGI(GATTC_TAG, "找到 %d 个特征值", count);

        /* 查找目标特征值 */
        if (count > 0) {
            esp_gattc_char_elem_t *char_elem_result =
                (esp_gattc_char_elem_t *)malloc(sizeof(esp_gattc_char_elem_t) * count);

            if (!char_elem_result) {
                ESP_LOGE(GATTC_TAG, "内存分配失败");
                break;
            }

            status = esp_ble_gattc_get_char_by_uuid(
                gattc_if,
                p_data->search_cmpl.conn_id,
                gl_profile_tab[PROFILE_A_APP_ID].service_start_handle,
                gl_profile_tab[PROFILE_A_APP_ID].service_end_handle,
                remote_filter_char_uuid,
                char_elem_result,
                &count);

            if (status != ESP_GATT_OK) {
                ESP_LOGE(GATTC_TAG, "获取特征值失败");
                free(char_elem_result);
                break;
            }

            /* 保存特征值句柄 */
            gl_profile_tab[PROFILE_A_APP_ID].char_handle = char_elem_result[0].char_handle;

            /* 注册通知 */
            esp_ble_gattc_register_for_notify(gattc_if,
                gl_profile_tab[PROFILE_A_APP_ID].remote_bda,
                char_elem_result[0].char_handle);

            free(char_elem_result);
        }
        break;

    case ESP_GATTC_REG_FOR_NOTIFY_EVT: {
        if (p_data->reg_for_notify.status != ESP_GATT_OK) {
            ESP_LOGE(GATTC_TAG, "注册通知失败");
        } else {
            ESP_LOGI(GATTC_TAG, "通知注册成功");

            /* 写入 CCCD 使能通知 */
            uint16_t notify_en = 1;
            esp_gatt_write_type_t write_type = ESP_GATT_WRITE_TYPE_NO_RSP;
            esp_ble_gattc_write_char(gattc_if,
                                     gl_profile_tab[PROFILE_A_APP_ID].conn_id,
                                     gl_profile_tab[PROFILE_A_APP_ID].char_handle,
                                     sizeof(notify_en),
                                     (uint8_t *)&notify_en,
                                     write_type,
                                     ESP_GATT_AUTH_REQ_NONE);
        }
        break;
    }

    case ESP_GATTC_NOTIFY_EVT:
        if (p_data->notify.is_notify) {
            ESP_LOGI(GATTC_TAG, "收到通知:");
        } else {
            ESP_LOGI(GATTC_TAG, "收到指示:");
        }
        ESP_LOG_BUFFER_HEX(GATTC_TAG, p_data->notify.value, p_data->notify.value_len);
        break;

    case ESP_GATTC_WRITE_DESCR_EVT:
        if (p_data->write.status != ESP_GATT_OK) {
            ESP_LOGE(GATTC_TAG, "写入描述符失败");
        } else {
            ESP_LOGI(GATTC_TAG, "写入描述符成功");
        }
        break;

    case ESP_GATTC_SRVC_CHG_EVT: {
        esp_bd_addr_t bda;
        memcpy(bda, p_data->srvc_chg.remote_bda, sizeof(esp_bd_addr_t));
        ESP_LOGI(GATTC_TAG, "服务变化, 地址 = %02x:%02x:%02x:%02x:%02x:%02x",
                 bda[0], bda[1], bda[2], bda[3], bda[4], bda[5]);
        break;
    }

    case ESP_GATTC_DISCONNECT_EVT:
        ESP_LOGI(GATTC_TAG, "断开连接, reason = %d", p_data->disconnect.reason);

        /* 开始扫描 */
        esp_ble_gap_start_scanning(30);
        break;

    default:
        break;
    }
}

/**
 * @brief GAP 事件处理
 */
static void esp_gap_cb(esp_gap_ble_cb_event_t event, esp_ble_gap_cb_param_t *param)
{
    uint8_t *adv_name = NULL;
    uint8_t adv_name_len = 0;

    switch (event) {
    case ESP_GAP_BLE_SCAN_PARAM_SET_COMPLETE_EVT:
        ESP_LOGI(GATTC_TAG, "扫描参数设置完成");
        /* 开始扫描 (30 秒) */
        esp_ble_gap_start_scanning(30);
        break;

    case ESP_GAP_BLE_SCAN_START_COMPLETE_EVT:
        if (param->scan_start_cmpl.status != ESP_BT_STATUS_SUCCESS) {
            ESP_LOGE(GATTC_TAG, "扫描启动失败");
        } else {
            ESP_LOGI(GATTC_TAG, "扫描启动成功");
        }
        break;

    case ESP_GAP_BLE_SCAN_RESULT_EVT: {
        esp_ble_gap_cb_param_t *scan_result = (esp_ble_gap_cb_param_t *)param;

        switch (scan_result->scan_rst.search_evt) {
        case ESP_GAP_SEARCH_INQ_RES_EVT:
            /* 解析广播数据,查找设备名称 */
            adv_name = esp_ble_resolve_adv_data(scan_result->scan_rst.ble_adv,
                                                ESP_BLE_AD_TYPE_NAME_CMPL,
                                                &adv_name_len);

            /* 打印扫描到的设备 */
            ESP_LOGI(GATTC_TAG, "扫描到设备:");
            ESP_LOGI(GATTC_TAG, "  地址: %02x:%02x:%02x:%02x:%02x:%02x",
                     scan_result->scan_rst.bda[0], scan_result->scan_rst.bda[1],
                     scan_result->scan_rst.bda[2], scan_result->scan_rst.bda[3],
                     scan_result->scan_rst.bda[4], scan_result->scan_rst.bda[5]);
            ESP_LOGI(GATTC_TAG, "  RSSI: %d dBm", scan_result->scan_rst.rssi);

            if (adv_name != NULL) {
                ESP_LOGI(GATTC_TAG, "  名称: %.*s", adv_name_len, adv_name);

                /* 检查是否是目标设备 */
                if (strlen(DEVICE_NAME) == adv_name_len &&
                    strncmp((char *)adv_name, DEVICE_NAME, adv_name_len) == 0) {

                    ESP_LOGI(GATTC_TAG, "找到目标设备!");

                    /* 停止扫描 */
                    esp_ble_gap_stop_scanning();

                    /* 连接设备 */
                    esp_ble_gattc_open(gl_profile_tab[PROFILE_A_APP_ID].gattc_if,
                                       scan_result->scan_rst.bda,
                                       scan_result->scan_rst.ble_addr_type, true);
                }
            }
            break;

        case ESP_GAP_SEARCH_INQ_CMPL_EVT:
            ESP_LOGI(GATTC_TAG, "扫描完成");
            break;

        default:
            break;
        }
        break;
    }

    case ESP_GAP_BLE_SCAN_STOP_COMPLETE_EVT:
        if (param->scan_stop_cmpl.status != ESP_BT_STATUS_SUCCESS) {
            ESP_LOGE(GATTC_TAG, "扫描停止失败");
        } else {
            ESP_LOGI(GATTC_TAG, "扫描停止成功");
        }
        break;

    default:
        break;
    }
}
```

---

## 6. 低功耗模式

### 6.1 电源管理框架

```c
/*
 * power_management_example.c
 * ESP32 电源管理和低功耗模式示例
 */

#include "esp_pm.h"
#include "esp_sleep.h"
#include "esp_timer.h"
#include "driver/rtc_io.h"

#define PM_TAG                  "POWER_MGMT"

/**
 * @brief 配置动态频率调节 (DFS)
 *
 * 支持的电源模式:
 * - ESP_PM_CPU_FREQ_MAX: CPU 最大频率, 最高性能
 * - ESP_PM_APB_FREQ_MAX: APB 总线最大频率
 * - ESP_PM_NO_LIGHT_SLEEP: 禁止轻度睡眠
 */
static void configure_power_management(void)
{
    /* 配置电源管理 */
    esp_pm_config_t pm_config = {
        .max_freq_mhz = 240,    /* 最大频率 240MHz */
        .min_freq_mhz = 80,     /* 最小频率 80MHz (轻负载时) */
        .light_sleep_enable = true,  /* 允许轻度睡眠 */
    };

    ESP_ERROR_CHECK(esp_pm_configure(&pm_config));
    ESP_LOGI(PM_TAG, "电源管理已配置: %d-%d MHz, Light Sleep: %s",
             pm_config.min_freq_mhz, pm_config.max_freq_mhz,
             pm_config.light_sleep_enable ? "启用" : "禁用");
}

/**
 * @brief 进入轻度睡眠模式
 *
 * 轻度睡眠特点:
 * - CPU 暂停, 但 RAM 保持
 * - 唤醒时间 < 1ms
 * - 可通过 GPIO/定时器/WiFi 等唤醒
 */
static void enter_light_sleep(uint32_t sleep_time_ms)
{
    ESP_LOGI(PM_TAG, "进入轻度睡眠 %d ms", sleep_time_ms);

    /* 获取当前时间 */
    int64_t start_time = esp_timer_get_time();

    /* 进入轻度睡眠 */
    esp_sleep_enable_timer_wakeup(sleep_time_ms * 1000);
    esp_light_sleep_start();

    /* 计算实际睡眠时间 */
    int64_t end_time = esp_timer_get_time();
    int64_t actual_sleep = (end_time - start_time) / 1000;

    /* 获取唤醒原因 */
    esp_sleep_wakeup_cause_t wakeup_reason = esp_sleep_get_wakeup_cause();

    ESP_LOGI(PM_TAG, "从轻度睡眠唤醒");
    ESP_LOGI(PM_TAG, "  计划睡眠: %d ms", sleep_time_ms);
    ESP_LOGI(PM_TAG, "  实际睡眠: %lld ms", actual_sleep);
    ESP_LOGI(PM_TAG, "  唤醒原因: %d", wakeup_reason);
}

/**
 * @brief 深度睡眠配置
 *
 * 深度睡眠特点:
 * - CPU 和大部分外设关闭
 * - 仅 RTC 内存保持 (默认 8KB)
 * - 唤醒后从 reset 向量重启
 * - 电流消耗 < 10uA
 */
static void configure_deep_sleep(void)
{
    /* 深度睡眠唤醒源配置 */

    /* 1. 定时器唤醒 */
    esp_sleep_enable_timer_wakeup(30 * 1000000);  /* 30 秒 */
    ESP_LOGI(PM_TAG, "已启用定时器唤醒: 30 秒");

    /* 2. GPIO 唤醒 (EXT0) - 单引脚 */
    ESP_ERROR_CHECK(esp_sleep_enable_ext0_wakeup(GPIO_NUM_33, 0));  /* 低电平触发 */
    ESP_LOGI(PM_TAG, "已启用 GPIO33 唤醒 (低电平)");

    /* 3. GPIO 唤醒 (EXT1) - 多引脚 */
    uint64_t ext1_wakeup_pins = (1ULL << GPIO_NUM_34) | (1ULL << GPIO_NUM_35);
    ESP_ERROR_CHECK(esp_sleep_enable_ext1_wakeup(ext1_wakeup_pins,
                                                  ESP_EXT1_WAKEUP_ANY_HIGH));
    ESP_LOGI(PM_TAG, "已启用 GPIO34/35 唤醒 (任意高电平)");

    /* 4. TouchPad 唤醒 (ESP32 系列支持) */
    /* ESP_ERROR_CHECK(esp_sleep_enable_touchpad_wakeup()); */

    /* 5. ULP 协处理器唤醒 */
    /* ESP_ERROR_CHECK(esp_sleep_enable_ulp_wakeup()); */
}

/**
 * @brief 进入深度睡眠
 */
static void enter_deep_sleep(void)
{
    ESP_LOGI(PM_TAG, "准备进入深度睡眠...");

    /* 保存必要数据到 RTC 慢速内存 (保留) */
    /* RTC_DATA_ATTR static int boot_count = 0; */

    /* 进入深度睡眠 */
    esp_deep_sleep_start();

    /* 不会执行到这里 */
}

/**
 * @brief 处理深度睡眠唤醒
 */
static void handle_deep_sleep_wakeup(void)
{
    /* 获取唤醒原因 */
    esp_sleep_wakeup_cause_t wakeup_reason = esp_sleep_get_wakeup_cause();

    switch (wakeup_reason) {
    case ESP_SLEEP_WAKEUP_EXT0:
        ESP_LOGI(PM_TAG, "由 EXT0 (GPIO) 唤醒");
        break;

    case ESP_SLEEP_WAKEUP_EXT1:
        ESP_LOGI(PM_TAG, "由 EXT1 (GPIOs) 唤醒");
        /* 获取唤醒的 GPIO */
        uint64_t wakeup_pin_mask = esp_sleep_get_ext1_wakeup_status();
        ESP_LOGI(PM_TAG, "  唤醒引脚掩码: 0x%llx", wakeup_pin_mask);
        break;

    case ESP_SLEEP_WAKEUP_TIMER:
        ESP_LOGI(PM_TAG, "由定时器唤醒");
        break;

    case ESP_SLEEP_WAKEUP_TOUCHPAD:
        ESP_LOGI(PM_TAG, "由 TouchPad 唤醒");
        break;

    case ESP_SLEEP_WAKEUP_ULP:
        ESP_LOGI(PM_TAG, "由 ULP 唤醒");
        break;

    default:
        ESP_LOGI(PM_TAG, "非深度睡眠唤醒 (上电/复位)");
        break;
    }
}

/**
 * @brief RTC GPIO 配置 (深度睡眠中使用)
 */
static void configure_rtc_gpio(void)
{
    /* RTC GPIO 可以在深度睡眠中保持状态 */

    /* 设置 RTC GPIO 为保持模式 */
    rtc_gpio_hold_en(GPIO_NUM_25);   /* 保持 GPIO25 状态 */
    rtc_gpio_hold_en(GPIO_NUM_26);   /* 保持 GPIO26 状态 */

    /* 禁用保持 (如果需要改变状态) */
    /* rtc_gpio_hold_dis(GPIO_NUM_25); */
}

/**
 * @brief 在深度睡眠中保持 WiFi 连接 (ESP32-S3/C3 支持)
 *
 * 注意: 此功能需要配置保持连接的 GPIO
 */
static void maintain_wifi_in_deep_sleep(void)
{
    /* 启用 WiFi 在睡眠中保持连接 (需要 BT/WiFi 共存配置) */
    ESP_ERROR_CHECK(esp_sleep_enable_wifi_wakeup());

    /* 配置 WiFi 唤醒间隔 */
    esp_wifi_set_listen_interval(10);  /* 100ms * 10 = 1s 间隔 */
}
```

### 6.2 低功耗 WiFi 连接

```c
/*
 * wifi_power_save.c
 * ESP32 WiFi 省电模式示例
 */

#include "esp_wifi.h"

/**
 * @brief 配置 WiFi 省电模式
 */
static void wifi_power_save_init(void)
{
    /* 配置 WiFi 为 STA 模式 */
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));

    /* 配置省电模式 */
    esp_wifi_set_ps(WIFI_PS_MIN_MODEM);  /* 最小功耗模式 */

    /* 省电模式选项:
     * - WIFI_PS_NONE:        不省电, 全速运行
     * - WIFI_PS_MIN_MODEM:   最小功耗, DTIM 1 (默认)
     * - WIFI_PS_MAX_MODEM:   最大功耗, DTIM 3 或更高
     */

    /* 配置监听间隔 (单位: 100ms) */
    ESP_ERROR_CHECK(esp_wifi_set_listen_interval(10));  /* 1000ms */

    ESP_LOGI(TAG, "WiFi 省电模式已配置");
}

/**
 * @brief 动态调整 WiFi 功耗
 *
 * @param high_throughput true: 高性能模式, false: 省电模式
 */
static void wifi_set_power_mode(bool high_throughput)
{
    if (high_throughput) {
        /* 高性能模式 - 关闭省电 */
        esp_wifi_set_ps(WIFI_PS_NONE);
        ESP_LOGI(TAG, "WiFi 切换到高性能模式");
    } else {
        /* 省电模式 */
        esp_wifi_set_ps(WIFI_PS_MAX_MODEM);
        ESP_LOGI(TAG, "WiFi 切换到最大省电模式");
    }
}
```

---

## 7. FreeRTOS 任务管理

### 7.1 任务创建与管理

```c
/*
 * freertos_tasks_example.c
 * ESP32 FreeRTOS 任务管理完整示例
 */

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "freertos/queue.h"
#include "freertos/event_groups.h"
#include "freertos/timers.h"

#define TASK_TAG            "FREERTOS"

/* 任务优先级定义 */
#define PRIORITY_HIGH       configMAX_PRIORITIES - 1
#define PRIORITY_NORMAL     configMAX_PRIORITIES / 2
#define PRIORITY_LOW        1

/* 任务句柄 */
static TaskHandle_t xHighPriorityTask = NULL;
static TaskHandle_t xNormalPriorityTask = NULL;
static TaskHandle_t xLowPriorityTask = NULL;

/* 同步原语 */
static SemaphoreHandle_t xMutex = NULL;
static SemaphoreHandle_t xBinarySemaphore = NULL;
static QueueHandle_t xDataQueue = NULL;
static EventGroupHandle_t xEventGroup = NULL;

/**
 * @brief 高优先级任务 - 紧急事件处理
 */
static void vHighPriorityTask(void *pvParameters)
{
    (void) pvParameters;
    uint32_t ulLoopCount = 0;

    ESP_LOGI(TASK_TAG, "高优先级任务启动");

    while (1) {
        /* 等待二进制信号量 */
        if (xSemaphoreTake(xBinarySemaphore, portMAX_DELAY) == pdTRUE) {
            ESP_LOGI(TASK_TAG, "高优先级任务: 处理紧急事件 #%d", ulLoopCount++);

            /* 模拟紧急处理 */
            vTaskDelay(pdMS_TO_TICKS(50));

            ESP_LOGI(TASK_TAG, "高优先级任务: 事件处理完成");
        }
    }
}

/**
 * @brief 普通优先级任务 - 常规数据处理
 */
static void vNormalPriorityTask(void *pvParameters)
{
    (void) pvParameters;
    uint32_t ulReceivedValue;
    uint32_t ulLoopCount = 0;

    ESP_LOGI(TASK_TAG, "普通优先级任务启动");

    while (1) {
        /* 从队列接收数据 */
        if (xQueueReceive(xDataQueue, &ulReceivedValue, portMAX_DELAY) == pdTRUE) {
            /* 获取互斥锁保护共享资源 */
            if (xSemaphoreTake(xMutex, portMAX_DELAY) == pdTRUE) {
                ESP_LOGI(TASK_TAG, "普通优先级任务: 处理数据 %d (计数: %d)",
                         (int)ulReceivedValue, (int)ulLoopCount++);

                /* 模拟数据处理 */
                vTaskDelay(pdMS_TO_TICKS(100));

                xSemaphoreGive(xMutex);
            }
        }
    }
}

/**
 * @brief 低优先级任务 - 后台维护
 */
static void vLowPriorityTask(void *pvParameters)
{
    (void) pvParameters;
    const EventBits_t xBitsToWaitFor = (BIT_0 | BIT_1);
    EventBits_t uxBits;
    uint32_t ulLoopCount = 0;

    ESP_LOGI(TASK_TAG, "低优先级任务启动");

    while (1) {
        /* 等待事件组 */
        uxBits = xEventGroupWaitBits(
            xEventGroup,        /* 事件组 */
            xBitsToWaitFor,     /* 等待的位 */
            pdTRUE,             /* 清除位 */
            pdFALSE,            /* 不需要所有位都设置 */
            pdMS_TO_TICKS(5000) /* 超时 5 秒 */
        );

        if ((uxBits & BIT_0) != 0) {
            ESP_LOGI(TASK_TAG, "低优先级任务: 收到事件 BIT_0 (计数: %d)",
                     (int)ulLoopCount++);
        }

        if ((uxBits & BIT_1) != 0) {
            ESP_LOGI(TASK_TAG, "低优先级任务: 收到事件 BIT_1 (计数: %d)",
                     (int)ulLoopCount++);
        }

        if (uxBits == 0) {
            /* 超时,执行周期性维护 */
            ESP_LOGD(TASK_TAG, "低优先级任务: 执行周期性维护");
        }

        /* 让出 CPU */
        vTaskDelay(pdMS_TO_TICKS(10));
    }
}

/**
 * @brief 软件定时器回调
 */
static void vTimerCallback(TimerHandle_t xTimer)
{
    static uint32_t ulTimerCount = 0;

    /* 获取定时器 ID */
    uint32_t ulTimerID = (uint32_t)pvTimerGetTimerID(xTimer);

    ESP_LOGI(TASK_TAG, "定时器 #%d 触发 (ID: %d)",
             (int)ulTimerCount++, (int)ulTimerID);

    /* 发送数据到队列 */
    uint32_t ulData = ulTimerCount;
    xQueueSend(xDataQueue, &ulData, 0);

    /* 设置事件位 */
    xEventGroupSetBits(xEventGroup, BIT_0);
}

/**
 * @brief 生产者任务 - 生成数据
 */
static void vProducerTask(void *pvParameters)
{
    (void) pvParameters;
    uint32_t ulData = 0;

    while (1) {
        /* 生成数据 */
        ulData++;

        /* 发送到队列 (不阻塞) */
        if (xQueueSend(xDataQueue, &ulData, pdMS_TO_TICKS(100)) != pdTRUE) {
            ESP_LOGW(TASK_TAG, "队列已满,数据丢弃");
        }

        /* 每隔 10 个数据触发一次紧急事件 */
        if (ulData % 10 == 0) {
            xSemaphoreGive(xBinarySemaphore);
        }

        /* 每隔 5 个数据设置事件 */
        if (ulData % 5 == 0) {
            xEventGroupSetBits(xEventGroup, BIT_1);
        }

        vTaskDelay(pdMS_TO_TICKS(500));
    }
}

/**
 * @brief 任务监控 - 打印任务状态
 */
static void vTaskMonitor(void *pvParameters)
{
    (void) pvParameters;
    TaskStatus_t *pxTaskStatusArray;
    volatile UBaseType_t uxArraySize, x;
    uint32_t ulTotalRunTime, ulStatsAsPercentage;

    while (1) {
        /* 获取任务数量 */
        uxArraySize = uxTaskGetNumberOfTasks();

        /* 分配内存 */
        pxTaskStatusArray = pvPortMalloc(uxArraySize * sizeof(TaskStatus_t));

        if (pxTaskStatusArray != NULL) {
            /* 获取任务状态 */
            uxArraySize = uxTaskGetSystemState(pxTaskStatusArray, uxArraySize, &ulTotalRunTime);

            ESP_LOGI(TASK_TAG, "=== 任务状态报告 ===");
            ESP_LOGI(TASK_TAG, "任务数: %d, 总运行时间: %d",
                     (int)uxArraySize, (int)ulTotalRunTime);

            /* 打印每个任务的信息 */
            for (x = 0; x < uxArraySize; x++) {
                /* 计算 CPU 使用率 */
                ulStatsAsPercentage = pxTaskStatusArray[x].ulRunTimeCounter /
                                      (ulTotalRunTime / 100UL);

                if (ulStatsAsPercentage > 0UL) {
                    ESP_LOGI(TASK_TAG, "  %-16s: 堆栈 %d/%d, CPU %d%%, 优先级 %d",
                             pxTaskStatusArray[x].pcTaskName,
                             (int)pxTaskStatusArray[x].usStackHighWaterMark,
                             (int)pxTaskStatusArray[x].uxCurrentPriority,
                             (int)ulStatsAsPercentage,
                             (int)pxTaskStatusArray[x].uxBasePriority);
                } else {
                    ESP_LOGI(TASK_TAG, "  %-16s: 堆栈 %d, 优先级 %d",
                             pxTaskStatusArray[x].pcTaskName,
                             (int)pxTaskStatusArray[x].usStackHighWaterMark,
                             (int)pxTaskStatusArray[x].uxBasePriority);
                }
            }

            vPortFree(pxTaskStatusArray);
        }

        vTaskDelay(pdMS_TO_TICKS(10000));  /* 每 10 秒报告一次 */
    }
}

/**
 * @brief FreeRTOS 演示初始化
 */
static void freertos_demo_init(void)
{
    /* 创建同步原语 */
    xMutex = xSemaphoreCreateMutex();
    xBinarySemaphore = xSemaphoreCreateBinary();
    xDataQueue = xQueueCreate(20, sizeof(uint32_t));
    xEventGroup = xEventGroupCreate();

    /* 创建任务 */
    xTaskCreatePinnedToCore(vHighPriorityTask, "HighTask", 2048, NULL,
                            PRIORITY_HIGH, &xHighPriorityTask, 1);

    xTaskCreatePinnedToCore(vNormalPriorityTask, "NormalTask", 2048, NULL,
                            PRIORITY_NORMAL, &xNormalPriorityTask, 0);

    xTaskCreatePinnedToCore(vLowPriorityTask, "LowTask", 2048, NULL,
                            PRIORITY_LOW, &xLowPriorityTask, 0);

    xTaskCreatePinnedToCore(vProducerTask, "Producer", 2048, NULL,
                            PRIORITY_NORMAL, NULL, 1);

    xTaskCreatePinnedToCore(vTaskMonitor, "Monitor", 4096, NULL,
                            PRIORITY_LOW, NULL, 0);

    /* 创建软件定时器 */
    TimerHandle_t xTimer = xTimerCreate("DemoTimer",
                                        pdMS_TO_TICKS(2000),  /* 2 秒周期 */
                                        pdTRUE,               /* 自动重载 */
                                        (void *)1,            /* 定时器 ID */
                                        vTimerCallback);

    if (xTimer != NULL) {
        xTimerStart(xTimer, 0);
    }

    ESP_LOGI(TASK_TAG, "FreeRTOS 演示系统初始化完成");
}
```

### 7.2 任务间通信

```c
/*
 * inter_task_communication.c
 * ESP32 任务间通信高级示例
 */

#include "freertos/stream_buffer.h"
#include "freertos/message_buffer.h"

/* 流缓冲区 (适合字节流数据) */
static StreamBufferHandle_t xStreamBuffer = NULL;

/* 消息缓冲区 (适合变长消息) */
static MessageBufferHandle_t xMessageBuffer = NULL;

/* 通知值 (轻量级同步) */
static TaskHandle_t xNotifiedTask = NULL;

/**
 * @brief 使用流缓冲区传输数据
 */
static void stream_buffer_example(void)
{
    /* 创建流缓冲区 (大小 256 字节, 触发级别 1) */
    xStreamBuffer = xStreamBufferCreate(256, 1);

    if (xStreamBuffer == NULL) {
        ESP_LOGE(TAG, "流缓冲区创建失败");
        return;
    }

    /* 发送任务 */
    uint8_t txData[] = {0x01, 0x02, 0x03, 0x04, 0x05};
    size_t xBytesSent = xStreamBufferSend(xStreamBuffer,
                                           txData,
                                           sizeof(txData),
                                           portMAX_DELAY);
    ESP_LOGI(TAG, "流缓冲区发送 %d 字节", (int)xBytesSent);

    /* 接收任务 */
    uint8_t rxData[10];
    size_t xReceivedBytes = xStreamBufferReceive(xStreamBuffer,
                                                  rxData,
                                                  sizeof(rxData),
                                                  portMAX_DELAY);
    ESP_LOGI(TAG, "流缓冲区接收 %d 字节", (int)xReceivedBytes);
}

/**
 * @brief 使用任务通知 (最高效)
 */
static void task_notification_example(void)
{
    /* 发送通知 (轻量级, 不经过内核) */
    xTaskNotifyGive(xNotifiedTask);

    /* 或发送带值的通知 */
    uint32_t ulValue = 0x12345678;
    xTaskNotify(xNotifiedTask, ulValue, eSetValueWithOverwrite);
}

/**
 * @brief 接收通知的任务
 */
static void vNotificationReceiver(void *pvParameters)
{
    (void) pvParameters;
    uint32_t ulNotificationValue;

    while (1) {
        /* 等待通知 */
        if (xTaskNotifyWait(0x00,              /* 进入时不清除 */
                           ULONG_MAX,         /* 退出时清除所有 */
                           &ulNotificationValue,
                           portMAX_DELAY) == pdTRUE) {

            ESP_LOGI(TAG, "收到通知值: 0x%08X", (unsigned int)ulNotificationValue);
        }
    }
}
```

---

## 8. 外设驱动 (I2C/SPI/UART)

### 8.1 I2C 驱动

```c
/*
 * i2c_driver_example.c
 * ESP32 I2C 主从模式完整示例
 * 支持: 标准模式(100KHz), 快速模式(400KHz), 快速模式+(1MHz)
 */

#include "driver/i2c.h"

#define I2C_MASTER_NUM          I2C_NUM_0        /* I2C 端口号 */
#define I2C_MASTER_SDA_IO       GPIO_NUM_21      /* SDA 引脚 */
#define I2C_MASTER_SCL_IO       GPIO_NUM_22      /* SCL 引脚 */
#define I2C_MASTER_FREQ_HZ      400000           /* 400KHz */
#define I2C_MASTER_TX_BUF_DISABLE   0
#define I2C_MASTER_RX_BUF_DISABLE   0
#define I2C_MASTER_TIMEOUT_MS   1000

/* SHT30 温湿度传感器 I2C 地址 */
#define SHT30_ADDR              0x44
#define SHT30_CMD_MEASURE       0x2C06

/**
 * @brief 初始化 I2C 主设备
 */
static esp_err_t i2c_master_init(void)
{
    i2c_config_t conf = {
        .mode = I2C_MODE_MASTER,
        .sda_io_num = I2C_MASTER_SDA_IO,
        .scl_io_num = I2C_MASTER_SCL_IO,
        .sda_pullup_en = GPIO_PULLUP_ENABLE,
        .scl_pullup_en = GPIO_PULLUP_ENABLE,
        .master.clk_speed = I2C_MASTER_FREQ_HZ,
    };

    /* 配置 I2C */
    esp_err_t err = i2c_param_config(I2C_MASTER_NUM, &conf);
    if (err != ESP_OK) {
        return err;
    }

    /* 安装驱动 */
    err = i2c_driver_install(I2C_MASTER_NUM, conf.mode,
                             I2C_MASTER_RX_BUF_DISABLE,
                             I2C_MASTER_TX_BUF_DISABLE, 0);

    return err;
}

/**
 * @brief I2C 读取寄存器
 */
static esp_err_t i2c_read_register(uint8_t dev_addr, uint8_t reg_addr,
                                   uint8_t *data, size_t len)
{
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();

    /* 开始信号 */
    i2c_master_start(cmd);

    /* 发送设备地址 + 写 */
    i2c_master_write_byte(cmd, (dev_addr << 1) | I2C_MASTER_WRITE, true);

    /* 发送寄存器地址 */
    i2c_master_write_byte(cmd, reg_addr, true);

    /* 重新开始 */
    i2c_master_start(cmd);

    /* 发送设备地址 + 读 */
    i2c_master_write_byte(cmd, (dev_addr << 1) | I2C_MASTER_READ, true);

    /* 读取数据 */
    if (len > 1) {
        i2c_master_read(cmd, data, len - 1, I2C_MASTER_ACK);
    }
    i2c_master_read_byte(cmd, data + len - 1, I2C_MASTER_NACK);

    /* 停止信号 */
    i2c_master_stop(cmd);

    /* 执行命令 */
    esp_err_t ret = i2c_master_cmd_begin(I2C_MASTER_NUM, cmd,
                                          pdMS_TO_TICKS(I2C_MASTER_TIMEOUT_MS));

    i2c_cmd_link_delete(cmd);
    return ret;
}

/**
 * @brief I2C 写入寄存器
 */
static esp_err_t i2c_write_register(uint8_t dev_addr, uint8_t reg_addr,
                                    uint8_t *data, size_t len)
{
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();

    /* 开始信号 */
    i2c_master_start(cmd);

    /* 发送设备地址 + 写 */
    i2c_master_write_byte(cmd, (dev_addr << 1) | I2C_MASTER_WRITE, true);

    /* 发送寄存器地址 */
    i2c_master_write_byte(cmd, reg_addr, true);

    /* 发送数据 */
    if (len > 0) {
        i2c_master_write(cmd, data, len, true);
    }

    /* 停止信号 */
    i2c_master_stop(cmd);

    /* 执行命令 */
    esp_err_t ret = i2c_master_cmd_begin(I2C_MASTER_NUM, cmd,
                                          pdMS_TO_TICKS(I2C_MASTER_TIMEOUT_MS));

    i2c_cmd_link_delete(cmd);
    return ret;
}

/**
 * @brief SHT30 温湿度传感器读取
 */
static esp_err_t sht30_read(float *temperature, float *humidity)
{
    uint8_t cmd[2] = {0x2C, 0x06};  /* 高精度测量命令 */
    uint8_t data[6];                 /* 温湿度 + CRC */

    i2c_cmd_handle_t cmd_handle = i2c_cmd_link_create();

    /* 发送测量命令 */
    i2c_master_start(cmd_handle);
    i2c_master_write_byte(cmd_handle, (SHT30_ADDR << 1) | I2C_MASTER_WRITE, true);
    i2c_master_write(cmd_handle, cmd, 2, true);
    i2c_master_stop(cmd_handle);

    esp_err_t ret = i2c_master_cmd_begin(I2C_MASTER_NUM, cmd_handle,
                                          pdMS_TO_TICKS(100));
    i2c_cmd_link_delete(cmd_handle);

    if (ret != ESP_OK) {
        return ret;
    }

    /* 等待测量完成 */
    vTaskDelay(pdMS_TO_TICKS(20));

    /* 读取数据 */
    cmd_handle = i2c_cmd_link_create();
    i2c_master_start(cmd_handle);
    i2c_master_write_byte(cmd_handle, (SHT30_ADDR << 1) | I2C_MASTER_READ, true);
    i2c_master_read(cmd_handle, data, 5, I2C_MASTER_ACK);
    i2c_master_read_byte(cmd_handle, data + 5, I2C_MASTER_NACK);
    i2c_master_stop(cmd_handle);

    ret = i2c_master_cmd_begin(I2C_MASTER_NUM, cmd_handle,
                               pdMS_TO_TICKS(100));
    i2c_cmd_link_delete(cmd_handle);

    if (ret != ESP_OK) {
        return ret;
    }

    /* 解析数据 */
    uint16_t temp_raw = (data[0] << 8) | data[1];
    uint16_t hum_raw = (data[3] << 8) | data[4];

    *temperature = -45 + 175 * ((float)temp_raw / 65535.0f);
    *humidity = 100 * ((float)hum_raw / 65535.0f);

    return ESP_OK;
}

/**
 * @brief I2C 总线扫描
 */
static void i2c_scan(void)
{
    ESP_LOGI(TAG, "开始 I2C 总线扫描...");
    ESP_LOGI(TAG, "     0  1  2  3  4  5  6  7  8  9  a  b  c  d  e  f");
    ESP_LOGI(TAG, "00:          ");

    for (uint8_t addr = 0x03; addr <= 0x77; addr++) {
        i2c_cmd_handle_t cmd = i2c_cmd_link_create();
        i2c_master_start(cmd);
        i2c_master_write_byte(cmd, (addr << 1) | I2C_MASTER_WRITE, true);
        i2c_master_stop(cmd);

        esp_err_t ret = i2c_master_cmd_begin(I2C_MASTER_NUM, cmd,
                                              pdMS_TO_TICKS(50));
        i2c_cmd_link_delete(cmd);

        if (addr % 16 == 0) {
            printf("\n%02x:", addr);
        }

        if (ret == ESP_OK) {
            printf(" %02x", addr);
        } else {
            printf(" --");
        }
    }
    printf("\n");
}
```

### 8.2 SPI 驱动

```c
/*
 * spi_driver_example.c
 * ESP32 SPI 主设备完整示例
 * 支持: SPI1(Flash), SPI2(HSPI), SPI3(VSPI)
 */

#include "driver/spi_master.h"
#include "driver/gpio.h"

#define SPI_HOST                SPI2_HOST       /* SPI2/HSPI */
#define PIN_NUM_MISO            GPIO_NUM_19
#define PIN_NUM_MOSI            GPIO_NUM_23
#define PIN_NUM_CLK             GPIO_NUM_18
#define PIN_NUM_CS              GPIO_NUM_5
#define SPI_DMA_CHAN            SPI_DMA_CH_AUTO

/* ILI9341 LCD 命令 */
#define ILI9341_SLPOUT          0x11
#define ILI9341_DISPON          0x29
#define ILI9341_CASET           0x2A
#define ILI9341_PASET           0x2B
#define ILI9341_RAMWR           0x2C

static spi_device_handle_t spi = NULL;

/**
 * @brief 初始化 SPI 主设备
 */
static esp_err_t spi_master_init(void)
{
    spi_bus_config_t buscfg = {
        .miso_io_num = PIN_NUM_MISO,
        .mosi_io_num = PIN_NUM_MOSI,
        .sclk_io_num = PIN_NUM_CLK,
        .quadwp_io_num = -1,
        .quadhd_io_num = -1,
        .max_transfer_sz = 4094,  /* 最大传输大小 */
    };

    spi_device_interface_config_t devcfg = {
        .clock_speed_hz = 26 * 1000 * 1000,  /* 26 MHz */
        .mode = 0,                           /* SPI 模式 0 (CPOL=0, CPHA=0) */
        .spics_io_num = PIN_NUM_CS,          /* CS 引脚 */
        .queue_size = 7,                     /* 事务队列大小 */
        .pre_cb = NULL,                      /* 传输前回调 */
    };

    /* 初始化 SPI 总线 */
    esp_err_t ret = spi_bus_initialize(SPI_HOST, &buscfg, SPI_DMA_CHAN);
    if (ret != ESP_OK) {
        return ret;
    }

    /* 添加设备 */
    ret = spi_bus_add_device(SPI_HOST, &devcfg, &spi);

    return ret;
}

/**
 * @brief SPI 传输数据
 */
static esp_err_t spi_transfer(const uint8_t *tx_data, uint8_t *rx_data, size_t len)
{
    spi_transaction_t t;

    memset(&t, 0, sizeof(t));
    t.length = len * 8;  /* 位长度 */
    t.tx_buffer = tx_data;
    t.rx_buffer = rx_data;
    t.user = (void *)1;  /* 用户数据 */

    return spi_device_polling_transmit(spi, &t);
}

/**
 * @brief 向 ILI9341 发送命令
 */
static void lcd_cmd(uint8_t cmd)
{
    gpio_set_level(PIN_NUM_DC, 0);  /* DC = 0 (命令) */

    spi_transaction_t t;
    memset(&t, 0, sizeof(t));
    t.length = 8;
    t.tx_buffer = &cmd;

    spi_device_polling_transmit(spi, &t);
}

/**
 * @brief 向 ILI9341 发送数据
 */
static void lcd_data(const uint8_t *data, int len)
{
    gpio_set_level(PIN_NUM_DC, 1);  /* DC = 1 (数据) */

    if (len == 0) return;

    spi_transaction_t t;
    memset(&t, 0, sizeof(t));
    t.length = len * 8;
    t.tx_buffer = data;

    spi_device_polling_transmit(spi, &t);
}

/**
 * @brief SPI 队列传输 (异步)
 */
static void spi_queue_example(void)
{
    /* 准备多个事务 */
    spi_transaction_t trans[3];
    uint8_t tx_buf1[10] = {0x01, 0x02, 0x03};
    uint8_t tx_buf2[10] = {0x04, 0x05, 0x06};
    uint8_t rx_buf[10];

    /* 事务 1: 仅发送 */
    memset(&trans[0], 0, sizeof(spi_transaction_t));
    trans[0].length = 3 * 8;
    trans[0].tx_buffer = tx_buf1;

    /* 事务 2: 仅接收 */
    memset(&trans[1], 0, sizeof(spi_transaction_t));
    trans[1].length = 3 * 8;
    trans[1].rx_buffer = rx_buf;

    /* 事务 3: 同时收发 */
    memset(&trans[2], 0, sizeof(spi_transaction_t));
    trans[2].length = 3 * 8;
    trans[2].tx_buffer = tx_buf2;
    trans[2].rx_buffer = rx_buf;

    /* 将事务加入队列 */
    for (int i = 0; i < 3; i++) {
        spi_device_queue_trans(spi, &trans[i], portMAX_DELAY);
    }

    /* 等待所有事务完成 */
    spi_transaction_t *rtrans;
    for (int i = 0; i < 3; i++) {
        spi_device_get_trans_result(spi, &rtrans, portMAX_DELAY);
    }
}
```

### 8.3 UART 驱动

```c
/*
 * uart_driver_example.c
 * ESP32 UART 完整示例
 * 支持: UART0(调试), UART1, UART2
 * 功能: 标准通信, RS485, 红外 (IrDA), 唤醒
 */

#include "driver/uart.h"
#include "string.h"

#define UART_PORT_NUM           UART_NUM_2
#define UART_TX_PIN             GPIO_NUM_17
#define UART_RX_PIN             GPIO_NUM_16
#define UART_RTS_PIN            GPIO_NUM_15
#define UART_CTS_PIN            GPIO_NUM_14
#define UART_BAUD_RATE          115200
#define UART_BUF_SIZE           1024

/**
 * @brief 初始化 UART
 */
static void uart_init(void)
{
    /* UART 配置 */
    const uart_config_t uart_config = {
        .baud_rate = UART_BAUD_RATE,
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_CTS_RTS,
        .rx_flow_ctrl_thresh = 122,
        .source_clk = UART_SCLK_DEFAULT,
    };

    /* 安装驱动 */
    ESP_ERROR_CHECK(uart_driver_install(UART_PORT_NUM,
                                        UART_BUF_SIZE * 2,
                                        UART_BUF_SIZE * 2,
                                        20, NULL, 0));

    /* 配置参数 */
    ESP_ERROR_CHECK(uart_param_config(UART_PORT_NUM, &uart_config));

    /* 设置引脚 */
    ESP_ERROR_CHECK(uart_set_pin(UART_PORT_NUM,
                                 UART_TX_PIN,
                                 UART_RX_PIN,
                                 UART_RTS_PIN,
                                 UART_CTS_PIN));
}

/**
 * @brief UART 发送数据
 */
static void uart_send(const uint8_t *data, size_t len)
{
    uart_write_bytes(UART_PORT_NUM, data, len);
}

/**
 * @brief UART 接收任务
 */
static void uart_rx_task(void *pvParameters)
{
    uint8_t data[UART_BUF_SIZE];

    while (1) {
        /* 读取数据 */
        int len = uart_read_bytes(UART_PORT_NUM, data,
                                  (UART_BUF_SIZE - 1),
                                  pdMS_TO_TICKS(100));

        if (len > 0) {
            data[len] = '\0';
            ESP_LOGI(TAG, "UART 接收 %d 字节: %s", len, data);

            /* 回显 */
            uart_send(data, len);
        }
    }
}

/**
 * @brief UART 模式检测 (自动波特率)
 */
static void uart_auto_baudrate_detect(void)
{
    /* 启用自动波特率检测 */
    ESP_ERROR_CHECK(uart_set_mode(UART_PORT_NUM, UART_MODE_RS485_HALF_DUPLEX));

    /* 或启用红外模式 */
    /* ESP_ERROR_CHECK(uart_set_mode(UART_PORT_NUM, UART_MODE_IRDA)); */
}

/**
 * @brief RS485 半双工模式配置
 */
static void uart_rs485_config(void)
{
    /* 配置 RS485 */
    ESP_ERROR_CHECK(uart_set_mode(UART_PORT_NUM, UART_MODE_RS485_HALF_DUPLEX));

    /* 配置 RS485 特定的冲突检测 */
    ESP_ERROR_CHECK(uart_set_rx_timeout(UART_PORT_NUM, 3));

    /* 设置 RTS 有效电平 (用于方向控制) */
    ESP_ERROR_CHECK(uart_set_line_inverse(UART_PORT_NUM, UART_SIGNAL_RTS_INV));
}

/**
 * @brief UART 事件处理
 */
static void uart_event_task(void *pvParameters)
{
    uart_event_t event;

    while (1) {
        /* 等待 UART 事件 */
        if (xQueueReceive(uart_queue, (void *)&event, portMAX_DELAY)) {
            switch (event.type) {
            case UART_DATA:
                ESP_LOGI(TAG, "[UART DATA]: %d", event.size);
                break;

            case UART_FIFO_OVF:
                ESP_LOGI(TAG, "[UART FIFO 溢出]");
                uart_flush_input(UART_PORT_NUM);
                break;

            case UART_BUFFER_FULL:
                ESP_LOGI(TAG, "[UART 缓冲区满]");
                uart_flush_input(UART_PORT_NUM);
                break;

            case UART_BREAK:
                ESP_LOGI(TAG, "[UART BREAK]");
                break;

            case UART_PARITY_ERR:
                ESP_LOGI(TAG, "[UART 奇偶校验错误]");
                break;

            case UART_FRAME_ERR:
                ESP_LOGI(TAG, "[UART 帧错误]");
                break;

            default:
                ESP_LOGI(TAG, "[UART 事件类型]: %d", event.type);
                break;
            }
        }
    }
}
```

---

## 9. 实战项目: 温湿度监测站

### 9.1 项目架构

```c
/*
 * weather_station.c
 * ESP32 温湿度监测站完整项目
 *
 * 功能:
 * - 读取 SHT30 温湿度传感器
 * - 通过 WiFi 连接到 MQTT 服务器
 * - 每 5 秒上报数据
 * - 本地 LCD 显示
 * - Web 服务器查看实时数据
 * - 支持 OTA 升级
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "freertos/queue.h"
#include "esp_system.h"
#include "esp_event.h"
#include "esp_log.h"
#include "esp_netif.h"
#include "esp_wifi.h"
#include "nvs_flash.h"
#include "esp_http_server.h"
#include "mqtt_client.h"
#include "driver/i2c.h"
#include "driver/spi_master.h"
#include "esp_ota_ops.h"
#include "esp_timer.h"

/* 项目配置 */
#define PROJECT_NAME        "ESP32 Weather Station"
#define FIRMWARE_VERSION    "1.0.0"

/* WiFi 配置 */
#define WIFI_SSID           "Your_SSID"
#define WIFI_PASS           "Your_Password"

/* MQTT 配置 */
#define MQTT_BROKER         "mqtt://broker.hivemq.com"
#define MQTT_PORT           1883
#define MQTT_TOPIC_TEMP     "esp32/weather/temperature"
#define MQTT_TOPIC_HUM      "esp32/weather/humidity"
#define MQTT_TOPIC_STATUS   "esp32/weather/status"

/* I2C 配置 */
#define I2C_MASTER_NUM      I2C_NUM_0
#define I2C_SDA_PIN         GPIO_NUM_21
#define I2C_SCL_PIN         GPIO_NUM_22
#define I2C_FREQ_HZ         100000

/* 传感器配置 */
#define SHT30_ADDR          0x44

/* 事件组位 */
#define WIFI_CONNECTED_BIT  BIT0
#define MQTT_CONNECTED_BIT  BIT1

/* 日志标签 */
static const char *TAG = "WEATHER_STATION";

/* 全局变量 */
static EventGroupHandle_t s_event_group = NULL;
static esp_mqtt_client_handle_t mqtt_client = NULL;
static float g_temperature = 0.0f;
static float g_humidity = 0.0f;
static uint32_t g_sensor_errors = 0;
static uint32_t g_mqtt_errors = 0;

/* 数据结构 */
typedef struct {
    float temperature;
    float humidity;
    uint32_t timestamp;
    uint32_t error_count;
} sensor_data_t;

static sensor_data_t g_sensor_data = {0};
static SemaphoreHandle_t g_data_mutex = NULL;

/* ==================== 传感器驱动 ==================== */

/**
 * @brief 初始化 I2C 主设备
 */
static esp_err_t sensor_i2c_init(void)
{
    i2c_config_t conf = {
        .mode = I2C_MODE_MASTER,
        .sda_io_num = I2C_SDA_PIN,
        .scl_io_num = I2C_SCL_PIN,
        .sda_pullup_en = GPIO_PULLUP_ENABLE,
        .scl_pullup_en = GPIO_PULLUP_ENABLE,
        .master.clk_speed = I2C_FREQ_HZ,
    };

    ESP_ERROR_CHECK(i2c_param_config(I2C_MASTER_NUM, &conf));
    ESP_ERROR_CHECK(i2c_driver_install(I2C_MASTER_NUM, conf.mode, 0, 0, 0));

    return ESP_OK;
}

/**
 * @brief CRC8 校验计算 (SHT30)
 */
static uint8_t crc8_calculate(const uint8_t *data, size_t len)
{
    const uint8_t POLYNOMIAL = 0x31;
    uint8_t crc = 0xFF;

    for (size_t j = len; j > 0; --j) {
        crc ^= *data++;
        for (int i = 8; i > 0; --i) {
            crc = (crc & 0x80) ? ((crc << 1) ^ POLYNOMIAL) : (crc << 1);
        }
    }
    return crc;
}

/**
 * @brief 读取 SHT30 传感器数据
 */
static esp_err_t sht30_read(float *temperature, float *humidity)
{
    uint8_t cmd[2] = {0x2C, 0x06};  /* 高精度测量 */
    uint8_t data[6];                 /* T_MSB, T_LSB, T_CRC, H_MSB, H_LSB, H_CRC */

    /* 发送测量命令 */
    i2c_cmd_handle_t cmd_handle = i2c_cmd_link_create();
    i2c_master_start(cmd_handle);
    i2c_master_write_byte(cmd_handle, (SHT30_ADDR << 1) | I2C_MASTER_WRITE, true);
    i2c_master_write(cmd_handle, cmd, 2, true);
    i2c_master_stop(cmd_handle);

    esp_err_t ret = i2c_master_cmd_begin(I2C_MASTER_NUM, cmd_handle,
                                          pdMS_TO_TICKS(100));
    i2c_cmd_link_delete(cmd_handle);

    if (ret != ESP_OK) {
        return ret;
    }

    /* 等待测量完成 (15ms 对于高精度模式) */
    vTaskDelay(pdMS_TO_TICKS(20));

    /* 读取数据 */
    cmd_handle = i2c_cmd_link_create();
    i2c_master_start(cmd_handle);
    i2c_master_write_byte(cmd_handle, (SHT30_ADDR << 1) | I2C_MASTER_READ, true);
    i2c_master_read(cmd_handle, data, 5, I2C_MASTER_ACK);
    i2c_master_read_byte(cmd_handle, data + 5, I2C_MASTER_NACK);
    i2c_master_stop(cmd_handle);

    ret = i2c_master_cmd_begin(I2C_MASTER_NUM, cmd_handle,
                               pdMS_TO_TICKS(100));
    i2c_cmd_link_delete(cmd_handle);

    if (ret != ESP_OK) {
        return ret;
    }

    /* CRC 校验 */
    if (crc8_calculate(data, 2) != data[2] ||
        crc8_calculate(data + 3, 2) != data[5]) {
        ESP_LOGE(TAG, "SHT30 CRC 校验失败");
        return ESP_ERR_INVALID_CRC;
    }

    /* 解析温度 */
    uint16_t temp_raw = (data[0] << 8) | data[1];
    *temperature = -45.0f + 175.0f * ((float)temp_raw / 65535.0f);

    /* 解析湿度 */
    uint16_t hum_raw = (data[3] << 8) | data[4];
    *humidity = 100.0f * ((float)hum_raw / 65535.0f);

    /* 限制范围 */
    if (*humidity > 100.0f) *humidity = 100.0f;
    if (*humidity < 0.0f) *humidity = 0.0f;

    return ESP_OK;
}

/* ==================== WiFi 连接 ==================== */

static void wifi_event_handler(void *arg, esp_event_base_t event_base,
                               int32_t event_id, void *event_data)
{
    if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_START) {
        esp_wifi_connect();
    } else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_DISCONNECTED) {
        ESP_LOGI(TAG, "WiFi 断开, 5秒后重连...");
        vTaskDelay(pdMS_TO_TICKS(5000));
        esp_wifi_connect();
    } else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP) {
        ip_event_got_ip_t *event = (ip_event_got_ip_t *)event_data;
        ESP_LOGI(TAG, "获取 IP: " IPSTR, IP2STR(&event->ip_info.ip));
        xEventGroupSetBits(s_event_group, WIFI_CONNECTED_BIT);
    }
}

static void wifi_init(void)
{
    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());
    esp_netif_create_default_wifi_sta();

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));

    esp_event_handler_instance_t instance_any_id;
    esp_event_handler_instance_t instance_got_ip;

    ESP_ERROR_CHECK(esp_event_handler_instance_register(WIFI_EVENT,
                                                        ESP_EVENT_ANY_ID,
                                                        &wifi_event_handler,
                                                        NULL,
                                                        &instance_any_id));
    ESP_ERROR_CHECK(esp_event_handler_instance_register(IP_EVENT,
                                                        IP_EVENT_STA_GOT_IP,
                                                        &wifi_event_handler,
                                                        NULL,
                                                        &instance_got_ip));

    wifi_config_t wifi_config = {
        .sta = {
            .ssid = WIFI_SSID,
            .password = WIFI_PASS,
            .threshold.authmode = WIFI_AUTH_WPA2_PSK,
        },
    };

    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &wifi_config));
    ESP_ERROR_CHECK(esp_wifi_start());

    ESP_LOGI(TAG, "WiFi 初始化完成");
}

/* ==================== MQTT 客户端 ==================== */

static void mqtt_event_handler(void *handler_args, esp_event_base_t base,
                               int32_t event_id, void *event_data)
{
    esp_mqtt_event_handle_t event = event_data;

    switch ((esp_mqtt_event_id_t)event_id) {
    case MQTT_EVENT_CONNECTED:
        ESP_LOGI(TAG, "MQTT 连接成功");
        xEventGroupSetBits(s_event_group, MQTT_CONNECTED_BIT);
        esp_mqtt_client_publish(mqtt_client, MQTT_TOPIC_STATUS, "online", 0, 1, 1);
        break;

    case MQTT_EVENT_DISCONNECTED:
        ESP_LOGI(TAG, "MQTT 断开连接");
        xEventGroupClearBits(s_event_group, MQTT_CONNECTED_BIT);
        break;

    case MQTT_EVENT_PUBLISHED:
        ESP_LOGD(TAG, "MQTT 消息已发布");
        break;

    case MQTT_EVENT_ERROR:
        ESP_LOGE(TAG, "MQTT 错误");
        g_mqtt_errors++;
        break;

    default:
        break;
    }
}

static void mqtt_init(void)
{
    esp_mqtt_client_config_t mqtt_cfg = {
        .broker.address.uri = MQTT_BROKER,
        .broker.address.port = MQTT_PORT,
        .credentials.client_id = "esp32_weather_station",
        .session.keepalive = 60,
        .session.last_will.topic = MQTT_TOPIC_STATUS,
        .session.last_will.msg = "offline",
        .session.last_will.qos = 1,
        .session.last_will.retain = true,
    };

    mqtt_client = esp_mqtt_client_init(&mqtt_cfg);
    esp_mqtt_client_register_event(mqtt_client, ESP_EVENT_ANY_ID,
                                    mqtt_event_handler, NULL);
    esp_mqtt_client_start(mqtt_client);
}

/* ==================== HTTP Web 服务器 ==================== */

static esp_err_t root_get_handler(httpd_req_t *req)
{
    char resp_str[512];

    xSemaphoreTake(g_data_mutex, portMAX_DELAY);
    snprintf(resp_str, sizeof(resp_str),
        "<!DOCTYPE html>"
        "<html><head><meta charset='UTF-8'><title>ESP32 气象站</title>"
        "<meta http-equiv='refresh' content='5'></head>"
        "<body><h1>ESP32 温湿度监测站</h1>"
        "<p>温度: %.2f °C</p>"
        "<p>湿度: %.2f %%</p>"
        "<p>固件版本: %s</p>"
        "<p>运行时间: %d 秒</p>"
        "<p>传感器错误: %d, MQTT错误: %d</p>"
        "</body></html>",
        g_sensor_data.temperature,
        g_sensor_data.humidity,
        FIRMWARE_VERSION,
        (int)(esp_timer_get_time() / 1000000),
        (int)g_sensor_errors,
        (int)g_mqtt_errors);
    xSemaphoreGive(g_data_mutex);

    httpd_resp_set_type(req, "text/html; charset=utf-8");
    httpd_resp_send(req, resp_str, HTTPD_RESP_USE_STRLEN);
    return ESP_OK;
}

static esp_err_t api_get_handler(httpd_req_t *req)
{
    char resp_str[256];

    xSemaphoreTake(g_data_mutex, portMAX_DELAY);
    snprintf(resp_str, sizeof(resp_str),
        "{\"temperature\":%.2f,\"humidity\":%.2f,\"version\":\"%s\","
        "\"uptime\":%d,\"errors\":%d}",
        g_sensor_data.temperature,
        g_sensor_data.humidity,
        FIRMWARE_VERSION,
        (int)(esp_timer_get_time() / 1000000),
        (int)(g_sensor_errors + g_mqtt_errors));
    xSemaphoreGive(g_data_mutex);

    httpd_resp_set_type(req, "application/json");
    httpd_resp_send(req, resp_str, HTTPD_RESP_USE_STRLEN);
    return ESP_OK;
}

static void start_webserver(void)
{
    httpd_config_t config = HTTPD_DEFAULT_CONFIG();
    config.server_port = 80;

    httpd_handle_t server = NULL;

    if (httpd_start(&server, &config) == ESP_OK) {
        httpd_uri_t root_uri = {
            .uri = "/",
            .method = HTTP_GET,
            .handler = root_get_handler,
            .user_ctx = NULL
        };
        httpd_register_uri_handler(server, &root_uri);

        httpd_uri_t api_uri = {
            .uri = "/api/data",
            .method = HTTP_GET,
            .handler = api_get_handler,
            .user_ctx = NULL
        };
        httpd_register_uri_handler(server, &api_uri);

        ESP_LOGI(TAG, "Web 服务器启动在端口 %d", config.server_port);
    }
}

/* ==================== 主任务 ==================== */

static void sensor_task(void *pvParameters)
{
    (void)pvParameters;

    while (1) {
        float temp, hum;
        esp_err_t ret = sht30_read(&temp, &hum);

        if (ret == ESP_OK) {
            xSemaphoreTake(g_data_mutex, portMAX_DELAY);
            g_sensor_data.temperature = temp;
            g_sensor_data.humidity = hum;
            g_sensor_data.timestamp = (uint32_t)(esp_timer_get_time() / 1000000);
            xSemaphoreGive(g_data_mutex);

            ESP_LOGI(TAG, "传感器读取: T=%.2f°C, H=%.2f%%", temp, hum);
        } else {
            g_sensor_errors++;
            ESP_LOGE(TAG, "传感器读取失败: %s", esp_err_to_name(ret));
        }

        vTaskDelay(pdMS_TO_TICKS(5000));  /* 5 秒间隔 */
    }
}

static void mqtt_publish_task(void *pvParameters)
{
    (void)pvParameters;
    char payload[64];

    while (1) {
        EventBits_t bits = xEventGroupWaitBits(s_event_group,
                                                MQTT_CONNECTED_BIT,
                                                pdFALSE,
                                                pdTRUE,
                                                pdMS_TO_TICKS(5000));

        if (bits & MQTT_CONNECTED_BIT) {
            xSemaphoreTake(g_data_mutex, portMAX_DELAY);
            float temp = g_sensor_data.temperature;
            float hum = g_sensor_data.humidity;
            xSemaphoreGive(g_data_mutex);

            /* 发布温度 */
            snprintf(payload, sizeof(payload), "%.2f", temp);
            esp_mqtt_client_publish(mqtt_client, MQTT_TOPIC_TEMP, payload, 0, 1, 0);

            /* 发布湿度 */
            snprintf(payload, sizeof(payload), "%.2f", hum);
            esp_mqtt_client_publish(mqtt_client, MQTT_TOPIC_HUM, payload, 0, 1, 0);

            ESP_LOGI(TAG, "MQTT 数据已发布");
        }

        vTaskDelay(pdMS_TO_TICKS(30000));  /* 30 秒上报间隔 */
    }
}

/* ==================== 主函数 ==================== */

void app_main(void)
{
    /* 初始化 NVS */
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);

    /* 初始化 */
    ESP_LOGI(TAG, "==============================================");
    ESP_LOGI(TAG, "  %s v%s", PROJECT_NAME, FIRMWARE_VERSION);
    ESP_LOGI(TAG, "==============================================");

    /* 创建资源 */
    s_event_group = xEventGroupCreate();
    g_data_mutex = xSemaphoreCreateMutex();

    /* 初始化硬件 */
    ESP_ERROR_CHECK(sensor_i2c_init());

    /* 初始化网络 */
    wifi_init();

    /* 等待 WiFi 连接 */
    xEventGroupWaitBits(s_event_group, WIFI_CONNECTED_BIT, pdFALSE, pdTRUE, portMAX_DELAY);

    /* 启动服务 */
    mqtt_init();
    start_webserver();

    /* 创建任务 */
    xTaskCreate(sensor_task, "sensor", 2048, NULL, 5, NULL);
    xTaskCreate(mqtt_publish_task, "mqtt_pub", 2048, NULL, 4, NULL);

    ESP_LOGI(TAG, "系统启动完成");

    /* 主循环 */
    while (1) {
        vTaskDelay(pdMS_TO_TICKS(10000));

        /* 系统状态报告 */
        ESP_LOGI(TAG, "系统状态 - 堆: %d, 最小堆: %d, 传感器错误: %d, MQTT错误: %d",
                 (int)esp_get_free_heap_size(),
                 (int)esp_get_minimum_free_heap_size(),
                 (int)g_sensor_errors,
                 (int)g_mqtt_errors);
    }
}
```

### 9.2 项目配置文件

```yaml
# sdkconfig.defaults - 项目默认配置

# WiFi 配置
CONFIG_ESP_WIFI_AUTH_WPA2_PSK=y
CONFIG_ESP_WIFI_ENABLE_WPA3_SAE=y

# FreeRTOS 配置
CONFIG_FREERTOS_HZ=1000
CONFIG_FREERTOS_USE_TRACE_FACILITY=y
CONFIG_FREERTOS_USE_STATS_FORMATTING_FUNCTIONS=y

# LWIP 配置
CONFIG_LWIP_MAX_SOCKETS=16
CONFIG_LWIP_SO_RCVRCVBUF=y

# MQTT 配置
CONFIG_MQTT_PROTOCOL_311=y
CONFIG_MQTT_TRANSPORT_SSL=y
CONFIG_MQTT_TRANSPORT_WEBSOCKET=y

# HTTP 服务器配置
CONFIG_HTTPD_MAX_REQ_HDR_LEN=1024
CONFIG_HTTPD_MAX_URI_LEN=512

# 日志级别
CONFIG_LOG_DEFAULT_LEVEL_INFO=y
CONFIG_LOG_COLORS=y

# 启用 C++ 异常支持 (如果需要)
CONFIG_COMPILER_CXX_EXCEPTIONS=n

# 分区表
CONFIG_PARTITION_TABLE_CUSTOM=y
CONFIG_PARTITION_TABLE_CUSTOM_FILENAME="partitions.csv"
```

```csv
# partitions.csv - 自定义分区表
# Name,   Type, SubType, Offset,  Size,    Flags
nvs,      data, nvs,     0x9000,  0x6000,
phy_init, data, phy,     0xf000,  0x1000,
factory,  app,  factory, 0x10000, 0x1F0000,
storage,  data, spiffs,  0x200000,0x100000,
```

### 9.3 CMakeLists.txt

```cmake
# CMakeLists.txt (项目根目录)
cmake_minimum_required(VERSION 3.16)

include($ENV{IDF_PATH}/tools/cmake/project.cmake)
project(weather_station)

# 设置 C 标准
set(CMAKE_C_STANDARD 11)
```

```cmake
# main/CMakeLists.txt
idf_component_register(
    SRCS "weather_station.c"
    INCLUDE_DIRS "."
    REQUIRES
        driver
        esp_wifi
        esp_http_server
        mqtt
        nvs_flash
        esp_timer
        esp_ota_ops
)
```

---

## 10. 调试技巧与常见问题

### 10.1 调试工具配置

```c
/*
 * debug_config.c
 * ESP32 调试配置和技巧
 */

#include "esp_debug_helpers.h"
#include "esp_log.h"

/**
 * @brief 配置日志级别
 */
static void configure_logging(void)
{
    /* 设置全局日志级别 */
    esp_log_level_set("*", ESP_LOG_INFO);

    /* 为特定组件设置日志级别 */
    esp_log_level_set("wifi", ESP_LOG_WARN);       /* 只显示警告及以上 */
    esp_log_level_set("mqtt", ESP_LOG_DEBUG);      /* 显示所有日志 */
    esp_log_level_set("WEATHER_STATION", ESP_LOG_VERBOSE);
}

/**
 * @brief 打印堆栈回溯
 */
static void print_backtrace(void)
{
    ESP_LOGI(TAG, "===== 堆栈回溯 =====");
    esp_backtrace_print(100);  /* 最多 100 层 */
}

/**
 * @brief 断言处理
 */
void __assert_func(const char *file, int line, const char *func, const char *expr)
{
    ESP_LOGE(TAG, "断言失败: %s:%d (%s): %s", file, line, func, expr);
    print_backtrace();

    /* 可选: 重启或进入调试循环 */
    abort();
}

/**
 * @brief 内存检查
 */
static void check_memory(void)
{
    /* 堆信息 */
    ESP_LOGI(TAG, "堆信息:");
    ESP_LOGI(TAG, "  总空闲: %d", (int)esp_get_free_heap_size());
    ESP_LOGI(TAG, "  最小空闲: %d", (int)esp_get_minimum_free_heap_size());
    ESP_LOGI(TAG, "  最大块: %d", (int)heap_caps_get_largest_free_block(MALLOC_CAP_8BIT));

    /* 内部 RAM */
    ESP_LOGI(TAG, "内部 DRAM:");
    ESP_LOGI(TAG, "  空闲: %d",
             (int)heap_caps_get_free_size(MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT));

    /* SPIRAM (如可用) */
    if (esp_psram_is_initialized()) {
        ESP_LOGI(TAG, "外部 PSRAM:");
        ESP_LOGI(TAG, "  总大小: %d", (int)esp_psram_get_size());
        ESP_LOGI(TAG, "  空闲: %d",
                 (int)heap_caps_get_free_size(MALLOC_CAP_SPIRAM));
    }
}
```

### 10.2 常见问题排查

#### 问题 1: 启动时 Guru Meditation Error

```c
/* 常见原因和解决 */

/* 1. 堆栈溢出 */
/* 解决: 增加任务堆栈大小 */
xTaskCreate(task_func, "task", 4096, NULL, 5, NULL);  /* 增加堆栈 */

/* 2. 看门狗超时 */
/* 解决: 在长时间操作中加入喂狗或让出 CPU */
while (processing) {
    /* 长时间处理 */
    vTaskDelay(pdMS_TO_TICKS(1));  /* 让出 CPU */
    esp_task_wdt_reset();           /* 喂狗 */
}

/* 3. 内存访问越界 */
/* 解决: 启用堆栈保护检查 */
// sdkconfig: CONFIG_COMPILER_STACK_CHECK_MODE_STRONG=y
```

#### 问题 2: WiFi 连接不稳定

```c
/* 优化 WiFi 连接稳定性 */

static void optimize_wifi_stability(void)
{
    /* 1. 启用 AMSDU (提高吞吐量) */
    esp_wifi_set_protocol(WIFI_IF_STA, WIFI_PROTOCOL_11B |
                          WIFI_PROTOCOL_11G | WIFI_PROTOCOL_11N);

    /* 2. 设置静态 IP (可选,加快连接速度) */
    esp_netif_ip_info_t ip_info = {
        .ip.addr = ipaddr_addr("192.168.1.100"),
        .netmask.addr = ipaddr_addr("255.255.255.0"),
        .gw.addr = ipaddr_addr("192.168.1.1"),
    };
    esp_netif_set_ip_info(sta_netif, &ip_info);

    /* 3. 启用自动重连 */
    esp_wifi_set_auto_connect(true);

    /* 4. 配置省电模式 (根据需要) */
    esp_wifi_set_ps(WIFI_PS_MIN_MODEM);
}
```

#### 问题 3: I2C 通信失败

```c
/* I2C 调试和修复 */

static void debug_i2c_communication(void)
{
    /* 1. 检查上拉电阻 */
    /* SDA 和 SCL 必须有 4.7K - 10K 上拉电阻 */

    /* 2. 降低 I2C 速度测试 */
    i2c_config_t conf = {
        .master.clk_speed = 50000,  /* 降低到 50KHz */
    };

    /* 3. 增加超时时间 */
    i2c_set_timeout(I2C_MASTER_NUM, 20000);  /* 200ms */

    /* 4. 总线扫描 */
    for (uint8_t addr = 0x08; addr < 0x78; addr++) {
        i2c_cmd_handle_t cmd = i2c_cmd_link_create();
        i2c_master_start(cmd);
        i2c_master_write_byte(cmd, (addr << 1) | I2C_MASTER_WRITE, true);
        i2c_master_stop(cmd);

        if (i2c_master_cmd_begin(I2C_NUM_0, cmd, pdMS_TO_TICKS(50)) == ESP_OK) {
            ESP_LOGI(TAG, "找到设备: 0x%02X", addr);
        }
        i2c_cmd_link_delete(cmd);
    }
}
```

### 10.3 性能优化技巧

```c
/*
 * performance_tips.c
 * ESP32 性能优化技巧
 */

/**
 * @brief IRAM_ATTR 修饰关键函数
 *
 * 将频繁调用的函数放在 IRAM (指令 RAM) 中,
 * 避免从 Flash 执行时的缓存未命中延迟
 */
static void IRAM_ATTR critical_function(void)
{
    /* 高频调用的代码 */
}

/**
 * @brief 使用 DMA 缓冲区对齐
 */
static void dma_buffer_alignment(void)
{
    /* DMA 缓冲区需要 4 字节对齐 */
    static DRAM_ATTR uint8_t dma_buffer[1024] __attribute__((aligned(4)));

    /* 或使用 heap_caps_malloc */
    uint8_t *aligned_buf = heap_caps_malloc(1024, MALLOC_CAP_DMA);

    /* 使用后释放 */
    free(aligned_buf);
}

/**
 * @brief 零拷贝技术
 */
static void zero_copy_technique(void)
{
    /* 使用 ringbuffer 避免数据复制 */
    RingbufHandle_t ringbuf = xRingbufferCreate(2048, RINGBUF_TYPE_BYTEBUF);

    /* 发送: 直接发送数据指针 */
    uint8_t data[100];
    xRingbufferSend(ringbuf, data, sizeof(data), portMAX_DELAY);

    /* 接收: 获取数据指针,无需复制 */
    size_t item_size;
    uint8_t *item = (uint8_t *)xRingbufferReceive(ringbuf, &item_size, portMAX_DELAY);

    /* 使用数据 */
    process_data(item, item_size);

    /* 释放 (不释放内存,只是归还 ringbuffer) */
    vRingbufferReturnItem(ringbuf, item);
}

/**
 * @brief 使用 PSRAM 扩展内存
 */
static void use_psram(void)
{
    /* 检查 PSRAM 是否可用 */
    if (esp_psram_is_initialized()) {
        /* 分配 PSRAM 内存 */
        uint8_t *psram_buf = heap_caps_malloc(1024 * 1024, MALLOC_CAP_SPIRAM);

        /* 大型缓冲区、图像数据等可以放在 PSRAM */
        ESP_LOGI(TAG, "使用 PSRAM: %p", psram_buf);

        free(psram_buf);
    }
}

/**
 * @brief 编译优化设置
 */
/* CMakeLists.txt 中添加:
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O2")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O2")

# 或针对特定文件优化
set_source_files_properties(fast_code.c PROPERTIES COMPILE_FLAGS "-O3")
*/
```

### 10.4 安全最佳实践

```c
/*
 * security_best_practices.c
 * ESP32 安全编码最佳实践
 */

/**
 * @brief 禁用 JTAG 调试 (生产环境)
 */
static void disable_jtag(void)
{
    /* 在 menuconfig 中启用安全启动后会自动禁用 JTAG */
    /* 或手动禁用: */
    esp_efuse_write_field_bit(ESP_EFUSE_DIS_PAD_JTAG);
}

/**
 * @brief 安全字符串处理
 */
static void secure_string_handling(void)
{
    char buffer[32];
    const char *input = "user_input";

    /* 错误: 可能导致缓冲区溢出 */
    /* strcpy(buffer, input); */

    /* 正确: 使用 strncpy 并确保 null 终止 */
    strncpy(buffer, input, sizeof(buffer) - 1);
    buffer[sizeof(buffer) - 1] = '\0';

    /* 或更安全的版本 (C11) */
    strncpy_s(buffer, sizeof(buffer), input, sizeof(buffer) - 1);
}

/**
 * @brief 启用 Flash 加密
 */
/* sdkconfig:
CONFIG_SECURE_FLASH_ENC_ENABLED=y
CONFIG_SECURE_FLASH_ENCRYPTION_MODE_RELEASE=y
*/

/**
 * @brief 启用安全启动
 */
/* sdkconfig:
CONFIG_SECURE_BOOT=y
CONFIG_SECURE_BOOT_V2_ENABLED=y
CONFIG_SECURE_BOOT_SIGNING_KEY="secure_boot_signing_key.pem"
*/
```

---

## 附录 A: 快速参考表

### GPIO 引脚功能对照

| GPIO | 输入 | 输出 | 特殊功能 | 注意 |
|------|------|------|----------|------|
| 0 | ✓ | ✓ | Boot | 上拉电阻, 启动模式 |
| 1 | ✗ | ✓ | TXD0 | 调试串口发送 |
| 2 | ✓ | ✓ | - | 板载 LED (大多数板子) |
| 3 | ✓ | ✗ | RXD0 | 调试串口接收 |
| 4 | ✓ | ✓ | - | - |
| 5 | ✓ | ✓ | SS | SPI Flash 保留 |
| 6-11 | - | - | SPI Flash | 不可用 |
| 12-14 | ✓ | ✓ | JTAG | 需注意启动模式 |
| 15 | ✓ | ✓ | - | 启动模式 |
| 16-17 | ✓ | ✓ | UART2 | - |
| 18-19 | ✓ | ✓ | SPI | - |
| 20 | - | - | - | 不可用 (USB) |
| 21-23 | ✓ | ✓ | I2C/SPI | - |
| 25-27 | ✓ | ✓ | DAC | 模拟输出 |
| 32-39 | ✓ | ✗ | ADC | 仅输入 |

### 常用命令速查

```bash
# 设置目标芯片
idf.py set-target esp32s3

# 编译
idf.py build

# 烧录并监控
idf.py -p COM3 flash monitor

# 仅擦除 Flash
idf.py -p COM3 erase-flash

# 打开配置菜单
idf.py menuconfig

# 生成项目报告
idf.py size
idf.py size-files

# 导出编译数据库 (用于代码分析)
idf.py reconfigure

# 单元测试
idf.py -p COM3 test

# 创建新组件
idf.py create-component my_component
```

### 资源链接

- [ESP-IDF 官方文档](https://docs.espressif.com/projects/esp-idf/)
- [ESP32 技术参考手册](https://www.espressif.com/sites/default/files/documentation/esp32_technical_reference_manual_en.pdf)
- [ESP32-S3 技术规格书](https://www.espressif.com/sites/default/files/documentation/esp32-s3_datasheet_en.pdf)
- [ESP32 论坛](https://esp32.com/)
- [GitHub ESP-IDF](https://github.com/espressif/esp-idf)

---

> **文档版本**: 1.0
> **最后更新**: 2026-03-17
> **作者**: C_Lang Knowledge Base
> **许可证**: MIT
