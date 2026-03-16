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
