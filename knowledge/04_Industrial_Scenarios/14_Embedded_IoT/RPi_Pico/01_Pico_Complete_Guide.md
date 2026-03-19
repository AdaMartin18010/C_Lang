# Raspberry Pi Pico 完整开发指南

## 目录

- [Raspberry Pi Pico 完整开发指南](#raspberry-pi-pico-完整开发指南)
  - [目录](#目录)
  - [1. 概述](#1-概述)
  - [2. RP2040双核架构介绍](#2-rp2040双核架构介绍)
    - [2.1 架构概述](#21-架构概述)
    - [2.2 双核特性](#22-双核特性)
    - [2.3 启动流程](#23-启动流程)
  - [3. Pico SDK环境搭建](#3-pico-sdk环境搭建)
    - [3.1 Windows 环境搭建](#31-windows-环境搭建)
      - [3.1.1 安装依赖](#311-安装依赖)
      - [3.1.2 下载并配置 SDK](#312-下载并配置-sdk)
    - [3.2 Linux/macOS 环境搭建](#32-linuxmacos-环境搭建)
    - [3.3 创建第一个项目](#33-创建第一个项目)
      - [3.3.1 项目结构](#331-项目结构)
      - [3.3.2 CMakeLists.txt](#332-cmakeliststxt)
      - [3.3.3 main.c - LED 闪烁](#333-mainc---led-闪烁)
      - [3.3.4 编译和烧录](#334-编译和烧录)
  - [4. GPIO控制](#4-gpio控制)
    - [4.1 GPIO 基础](#41-gpio-基础)
    - [4.2 GPIO 输入输出](#42-gpio-输入输出)
    - [4.3 GPIO 中断](#43-gpio-中断)
    - [4.4 高级 GPIO 操作](#44-高级-gpio-操作)
  - [5. PIO可编程IO](#5-pio可编程io)
    - [5.1 PIO 概述](#51-pio-概述)
    - [5.2 PIO 汇编指令集](#52-pio-汇编指令集)
    - [5.3 PIO LED 闪烁示例](#53-pio-led-闪烁示例)
    - [5.4 PIO WS2812B LED 驱动](#54-pio-ws2812b-led-驱动)
    - [5.5 PIO 自定义协议 - DHT22 温湿度传感器](#55-pio-自定义协议---dht22-温湿度传感器)
  - [6. PWM控制电机](#6-pwm控制电机)
    - [6.1 PWM 基础](#61-pwm-基础)
    - [6.2 基础 PWM LED 调光](#62-基础-pwm-led-调光)
    - [6.3 直流电机 H 桥控制](#63-直流电机-h-桥控制)
    - [6.4 舵机 (Servo) 控制](#64-舵机-servo-控制)
  - [7. ADC读取模拟传感器](#7-adc读取模拟传感器)
    - [7.1 ADC 基础](#71-adc-基础)
    - [7.2 基础 ADC 读取](#72-基础-adc-读取)
    - [7.3 ADC DMA 连续采样](#73-adc-dma-连续采样)
  - [8. I2C/SPI通信](#8-i2cspi通信)
    - [8.1 I2C 通信](#81-i2c-通信)
    - [8.2 SPI 通信](#82-spi-通信)
  - [9. DMA传输](#9-dma传输)
    - [9.1 DMA 概述](#91-dma-概述)
    - [9.2 基础 DMA 内存复制](#92-基础-dma-内存复制)
    - [9.3 DMA 与 GPIO 快速输出](#93-dma-与-gpio-快速输出)
  - [10. 多核编程](#10-多核编程)
    - [10.1 双核通信机制](#101-双核通信机制)
    - [10.2 多核负载均衡](#102-多核负载均衡)
  - [11. 实际项目：PID电机控制器](#11-实际项目pid电机控制器)
    - [11.1 完整 PID 电机控制系统](#111-完整-pid-电机控制系统)
    - [11.2 CMakeLists.txt (PID 项目)](#112-cmakeliststxt-pid-项目)
  - [12. USB设备模式](#12-usb设备模式)
    - [12.1 USB CDC (虚拟串口)](#121-usb-cdc-虚拟串口)
    - [12.2 USB HID 设备 (键盘/鼠标)](#122-usb-hid-设备-键盘鼠标)
    - [12.3 USB 大容量存储设备 (MSC)](#123-usb-大容量存储设备-msc)
    - [12.4 USB 复合设备 (CDC + MSC)](#124-usb-复合设备-cdc--msc)
  - [13. 调试技巧](#13-调试技巧)
    - [13.1 串口调试输出](#131-串口调试输出)
    - [13.2 使用 SWD 调试](#132-使用-swd-调试)
    - [13.3 断言和错误处理](#133-断言和错误处理)
    - [13.4 运行时统计](#134-运行时统计)
  - [14. 参考资料](#14-参考资料)
    - [官方资源](#官方资源)
    - [社区资源](#社区资源)
    - [推荐书籍](#推荐书籍)
    - [在线工具](#在线工具)
  - [附录 A: 常用命令速查表](#附录-a-常用命令速查表)
    - [编译命令](#编译命令)
    - [烧录命令](#烧录命令)
    - [调试命令](#调试命令)
  - [文档信息](#文档信息)

---

## 1. 概述

Raspberry Pi Pico 是基于 RP2040 微控制器的低成本、高性能开发板。RP2040 是树莓派基金会设计的首款微控制器芯片，具有以下特性：

- **双核 Cortex-M0+** @ 133MHz
- **264KB SRAM**
- **2MB 板载 Flash**（可外扩至 16MB）
- **30 个 GPIO**（其中 4 个可作为 ADC 输入）
- **8 个可编程 IO (PIO)** 状态机
- **USB 1.1** 主机/设备支持
- **DMA 控制器**

---

## 2. RP2040双核架构介绍

### 2.1 架构概述

RP2040 采用对称双核 Cortex-M0+ 处理器设计：

```
┌─────────────────────────────────────────────────────────────┐
│                      RP2040 架构图                           │
├─────────────────────────────────────────────────────────────┤
│  ┌──────────────┐    ┌──────────────┐                      │
│  │   Core 0     │    │   Core 1     │                      │
│  │ Cortex-M0+   │    │ Cortex-M0+   │                      │
│  │  @133MHz     │    │  @133MHz     │                      │
│  └──────┬───────┘    └──────┬───────┘                      │
│         │                   │                               │
│         └─────────┬─────────┘                               │
│                   │                                         │
│         ┌─────────┴─────────┐                              │
│         │   AHB/APB 总线    │                              │
│         └─────────┬─────────┘                              │
│                   │                                         │
│    ┌──────────────┼──────────────┐                        │
│    │              │              │                         │
│ ┌──┴──┐      ┌───┴───┐     ┌────┴────┐                   │
│ │SRAM │      │  PIO  │     │  DMA    │                   │
│ │264KB│      │8 SM   │     │ 12 Ch   │                   │
│ └─────┘      └───────┘     └─────────┘                   │
│ ┌──────┐     ┌───────┐     ┌─────────┐                   │
│ │ GPIO │     │ Timer │     │  USB    │                   │
│ │ 30pin│     │       │     │1.1 HS/FS│                   │
│ └──────┘     └───────┘     └─────────┘                   │
│ ┌──────┐     ┌───────┐     ┌─────────┐                   │
│ │ ADC  │     │ I2C   │     │  SPI    │                   │
│ │ 4ch  │     │ 2x    │     │ 2x      │                   │
│ └──────┘     └───────┘     └─────────┘                   │
│ ┌──────┐     ┌───────┐                                   │
│ │ UART │     │ PWM   │                                   │
│ │ 2x   │     │ 8x    │                                   │
│ └──────┘     └───────┘                                   │
└─────────────────────────────────────────────────────────────┘
```

### 2.2 双核特性

| 特性 | 说明 |
|------|------|
| 对称架构 | 两个核心完全相同，可运行不同代码 |
| 共享内存 | 两个核心共享 264KB SRAM |
| 独立堆栈 | 每个核心有独立的堆栈 |
| FIFO 通信 | 核心间通过 32 位 FIFO 通信 |
| 中断控制 | 独立的中断控制器 (NVIC) |

### 2.3 启动流程

```c
/**
 * @file boot_sequence.c
 * @brief RP2040 启动流程示例
 */

#include "pico/stdlib.h"
#include "pico/multicore.h"
#include <stdio.h>

// Core 1 的入口函数
void core1_entry(void) {
    printf("Core 1 启动!\n");

    while (1) {
        // Core 1 任务
        sleep_ms(1000);
        printf("Core 1 运行中...\n");
    }
}

int main(void) {
    // 初始化标准库
    stdio_init_all();

    printf("Core 0 启动!\n");

    // 启动 Core 1
    multicore_launch_core1(core1_entry);

    // Core 0 主循环
    while (1) {
        printf("Core 0 运行中...\n");
        sleep_ms(500);
    }

    return 0;
}
```

---

## 3. Pico SDK环境搭建

### 3.1 Windows 环境搭建

#### 3.1.1 安装依赖

```powershell
# 1. 安装 Chocolatey (如果未安装)
Set-ExecutionPolicy Bypass -Scope Process -Force;
[System.Net.ServicePointManager]::SecurityProtocol = [System.Net.ServicePointManager]::SecurityProtocol -bor 3072;
iex ((New-Object System.Net.WebClient).DownloadString('https://community.chocolatey.org/install.ps1'))

# 2. 安装必要工具
choco install git cmake mingw make
```

#### 3.1.2 下载并配置 SDK

```powershell
# 创建项目目录
mkdir E:\RaspberryPi\pico
cd E:\RaspberryPi\pico

# 克隆 SDK
git clone https://github.com/raspberrypi/pico-sdk.git
cd pico-sdk
git submodule update --init

# 设置环境变量
setx PICO_SDK_PATH "E:\RaspberryPi\pico\pico-sdk"
```

### 3.2 Linux/macOS 环境搭建

```bash
# 安装依赖 (Ubuntu/Debian)
sudo apt update
sudo apt install cmake gcc-arm-none-eabi libnewlib-arm-none-eabi libstdc++-arm-none-eabi-newlib

# 安装依赖 (macOS)
brew install cmake
brew tap ArmMbed/homebrew-formulae
brew install arm-none-eabi-gcc

# 克隆 SDK
cd ~
mkdir pico && cd pico
git clone https://github.com/raspberrypi/pico-sdk.git
cd pico-sdk
git submodule update --init

# 设置环境变量
echo 'export PICO_SDK_PATH=$HOME/pico/pico-sdk' >> ~/.bashrc
source ~/.bashrc
```

### 3.3 创建第一个项目

#### 3.3.1 项目结构

```
my_pico_project/
├── CMakeLists.txt
├── pico_sdk_import.cmake
└── main.c
```

#### 3.3.2 CMakeLists.txt

```cmake
# 指定最小 CMake 版本
cmake_minimum_required(VERSION 3.13)

# 包含 Pico SDK
include(pico_sdk_import.cmake)

# 项目名称
project(my_pico_project C CXX ASM)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

# 初始化 Pico SDK
pico_sdk_init()

# 创建可执行文件
add_executable(my_pico_project
    main.c
)

# 链接库
target_link_libraries(my_pico_project
    pico_stdlib              # 标准库
    hardware_gpio            # GPIO 硬件库
    hardware_timer           # 定时器
)

# 启用 USB 输出，禁用 UART 输出
pico_enable_stdio_usb(my_pico_project 1)
pico_enable_stdio_uart(my_pico_project 0)

# 生成 uf2、bin、hex 文件
pico_add_extra_outputs(my_pico_project)
```

#### 3.3.3 main.c - LED 闪烁

```c
/**
 * @file main.c
 * @brief Pico LED 闪烁示例 - 第一个程序
 */

#include "pico/stdlib.h"
#include "hardware/gpio.h"

// LED 引脚定义 (Pico 板载 LED 连接到 GPIO25)
#define LED_PIN 25

int main(void) {
    // 初始化 GPIO
    gpio_init(LED_PIN);
    gpio_set_dir(LED_PIN, GPIO_OUT);

    // 初始化标准 I/O (用于 USB 串口输出)
    stdio_init_all();

    printf("Pico 启动!\n");

    while (1) {
        // LED 点亮
        gpio_put(LED_PIN, 1);
        printf("LED ON\n");
        sleep_ms(500);  // 延时 500ms

        // LED 熄灭
        gpio_put(LED_PIN, 0);
        printf("LED OFF\n");
        sleep_ms(500);  // 延时 500ms
    }

    return 0;
}
```

#### 3.3.4 编译和烧录

```bash
# 创建构建目录
mkdir build
cd build

# 配置和编译
cmake ..
make -j4

# 编译完成后会生成 my_pico_project.uf2 文件
```

**烧录步骤：**

1. 按住 Pico 的 BOOTSEL 按钮
2. 连接 USB 到电脑
3. 松开 BOOTSEL 按钮
4. Pico 会以 U 盘模式出现
5. 将 `.uf2` 文件复制到该驱动器
6. Pico 会自动重启并运行程序

---

## 4. GPIO控制

### 4.1 GPIO 基础

RP2040 有 30 个 GPIO 引脚，编号 0-29。每个 GPIO 可配置为：

- 数字输入/输出
- 模拟输入 (ADC)
- 专用功能 (I2C、SPI、UART、PWM 等)

### 4.2 GPIO 输入输出

```c
/**
 * @file gpio_basic.c
 * @brief GPIO 基础输入输出示例
 */

#include "pico/stdlib.h"
#include "hardware/gpio.h"

// 引脚定义
#define LED_PIN     25
#define BUTTON_PIN  14

int main(void) {
    stdio_init_all();

    // LED 配置为输出
    gpio_init(LED_PIN);
    gpio_set_dir(LED_PIN, GPIO_OUT);

    // 按钮配置为输入，启用内部上拉电阻
    gpio_init(BUTTON_PIN);
    gpio_set_dir(BUTTON_PIN, GPIO_IN);
    gpio_pull_up(BUTTON_PIN);  // 启用上拉，按钮按下时读低电平

    printf("GPIO 基础示例启动\n");

    while (1) {
        // 读取按钮状态
        bool button_pressed = !gpio_get(BUTTON_PIN);  // 取反因为上拉

        if (button_pressed) {
            gpio_put(LED_PIN, 1);  // 点亮 LED
            printf("按钮按下\n");
        } else {
            gpio_put(LED_PIN, 0);  // 熄灭 LED
        }

        sleep_ms(50);  // 简单消抖
    }

    return 0;
}
```

### 4.3 GPIO 中断

```c
/**
 * @file gpio_interrupt.c
 * @brief GPIO 中断示例 - 按钮中断触发
 */

#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "hardware/irq.h"

#define BUTTON_PIN  14
#define LED_PIN     25

// 全局计数器
volatile uint32_t button_press_count = 0;
volatile bool button_event = false;

// GPIO 中断处理函数
void gpio_callback(uint gpio, uint32_t events) {
    if (gpio == BUTTON_PIN) {
        button_press_count++;
        button_event = true;
    }
}

int main(void) {
    stdio_init_all();

    // LED 配置
    gpio_init(LED_PIN);
    gpio_set_dir(LED_PIN, GPIO_OUT);

    // 按钮配置
    gpio_init(BUTTON_PIN);
    gpio_set_dir(BUTTON_PIN, GPIO_IN);
    gpio_pull_up(BUTTON_PIN);

    // 配置 GPIO 中断
    // 触发条件：下降沿（按钮按下，从上拉到接地）
    gpio_set_irq_enabled_with_callback(
        BUTTON_PIN,
        GPIO_IRQ_EDGE_FALL,
        true,
        &gpio_callback
    );

    printf("GPIO 中断示例启动\n");
    printf("按按钮触发中断...\n");

    while (1) {
        if (button_event) {
            button_event = false;

            // 切换 LED 状态
            static bool led_state = false;
            led_state = !led_state;
            gpio_put(LED_PIN, led_state);

            printf("中断触发 #%lu, LED=%d\n", button_press_count, led_state);
        }

        // 低功耗等待
        __wfi();  // Wait For Interrupt
    }

    return 0;
}
```

### 4.4 高级 GPIO 操作

```c
/**
 * @file gpio_advanced.c
 * @brief 高级 GPIO 操作示例
 */

#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "hardware/sync.h"

// 多位 GPIO 同时操作
#define LED_MASK    ((1UL << 25) | (1UL << 24) | (1UL << 23))

void gpio_advanced_demo(void) {
    // 同时设置多个 GPIO 为输出
    for (int pin = 23; pin <= 25; pin++) {
        gpio_init(pin);
        gpio_set_dir(pin, GPIO_OUT);
    }

    // 使用掩码同时操作多个 GPIO
    gpio_set_mask(LED_MASK);    // 同时设置 bit 23,24,25 为高
    sleep_ms(500);
    gpio_clr_mask(LED_MASK);    // 同时清除 bit 23,24,25
    sleep_ms(500);
    gpio_xor_mask(LED_MASK);    // 同时翻转 bit 23,24,25

    // 原子操作示例
    uint32_t save = save_and_disable_interrupts();
    // 临界区代码 - 不会被中断打断
    gpio_put(25, 1);
    gpio_put(24, 1);
    restore_interrupts(save);

    // 驱动强度设置
    gpio_set_drive_strength(25, GPIO_DRIVE_STRENGTH_12MA);

    // 斜率控制
    gpio_set_slew_rate(25, GPIO_SLEW_RATE_FAST);
}
```

---

## 5. PIO可编程IO

### 5.1 PIO 概述

PIO (Programmable I/O) 是 RP2040 最强大的特性之一：

- **8 个独立状态机**，分为 2 个 PIO 块 (PIO0, PIO1)
- 每个状态机有 **32 条指令**空间
- **专用 FIFO** (4 字 TX, 4 字 RX)
- 可编程时钟分频器
- 支持多种输出格式

### 5.2 PIO 汇编指令集

| 指令 | 描述 | 示例 |
|------|------|------|
| `jmp` | 跳转 | `jmp x--, label` |
| `wait` | 等待条件 | `wait 1 gpio, 0` |
| `in` | 移位输入 | `in pins, 8` |
| `out` | 移位输出 | `out pins, 8` |
| `push` | 推送到 RX FIFO | `push noblock` |
| `pull` | 从 TX FIFO 拉取 | `pull block` |
| `mov` | 数据移动 | `mov x, y` |
| `irq` | 设置/清除 IRQ | `irq set 0` |
| `set` | 设置引脚/寄存器 | `set pins, 1` |

### 5.3 PIO LED 闪烁示例

```c
/**
 * @file pio_blink.c
 * @brief 使用 PIO 控制 LED 闪烁
 */

#include "pico/stdlib.h"
#include "hardware/pio.h"
#include "hardware/clocks.h"

// PIO 程序 - LED 闪烁
// 这个程序从 FIFO 获取延时计数，然后翻转 LED
static const uint16_t pio_blink_program[] = {
    // .wrap_target
    0x8080,  // 0: pull   block          ; 从 TX FIFO 获取计数值
    0xa027,  // 1: mov    x, osr          ; 复制到 X 寄存器
    0xe022,  // 2: set    pins, 0          ; LED 熄灭
    // delay_off:
    0x0044,  // 3: jmp    x--, delay_off  ; X--, 如果不为零则跳转
    0x8080,  // 4: pull   block          ; 获取下一个计数值
    0xa027,  // 5: mov    x, osr          ; 复制到 X 寄存器
    0xe023,  // 6: set    pins, 1          ; LED 点亮
    // delay_on:
    0x0047,  // 7: jmp    x--, delay_on   ; X--, 如果不为零则跳转
             // .wrap
};

static const struct pio_program pio_blink_program_struct = {
    .instructions = pio_blink_program,
    .length = 8,
    .origin = -1,
};

int main(void) {
    stdio_init_all();

    PIO pio = pio0;
    uint sm = 0;
    uint offset;

    // 加载程序到 PIO
    bool success = pio_claim_free_sm_and_add_program_for_gpio_range(
        &pio_blink_program_struct,
        &pio,
        &sm,
        &offset,
        25,       // GPIO 25
        1,        // 1 个引脚
        true      // 必需
    );

    if (!success) {
        printf("PIO 程序加载失败\n");
        return -1;
    }

    // 配置状态机
    pio_sm_config c = pio_get_default_sm_config();

    // 设置输出引脚
    sm_config_set_set_pins(&c, 25, 1);

    // 设置时钟分频 (系统时钟 / 2)
    sm_config_set_clkdiv(&c, 2.0f);

    // 初始化引脚
    pio_gpio_init(pio, 25);
    pio_sm_set_consecutive_pindirs(pio, sm, 25, 1, true);

    // 加载配置并启动状态机
    pio_sm_init(pio, sm, offset, &c);
    pio_sm_set_enabled(pio, sm, true);

    printf("PIO LED 闪烁启动\n");

    // 主循环 - 向 PIO 发送延时值
    while (1) {
        // 向 PIO 发送熄灭时间 (约 500ms)
        pio_sm_put_blocking(pio, sm, 10000000);

        // 向 PIO 发送点亮时间 (约 500ms)
        pio_sm_put_blocking(pio, sm, 10000000);
    }

    return 0;
}
```

### 5.4 PIO WS2812B LED 驱动

```c
/**
 * @file pio_ws2812.c
 * @brief 使用 PIO 驱动 WS2812B RGB LED 灯带
 */

#include "pico/stdlib.h"
#include "hardware/pio.h"
#include "hardware/clocks.h"

// WS2812B 时序要求 (800KHz):
// T0H: 0.4us ±150ns, T0L: 0.85us ±150ns (bit 0)
// T1H: 0.8us ±150ns, T1L: 0.45us ±150ns (bit 1)
// RESET: >50us

// PIO 程序 - WS2812B 驱动
// 使用 side-set 控制输出引脚
static const uint16_t ws2812_program[] = {
    // .wrap_target
    0x6221,  // 0: out    x, 1        side 0 [2]  ; 移出 1 位到 X
    0x1123,  // 1: jmp    !x, 3       side 1 [1]  ; 如果为 0, 跳转到 bit 0
    0x1400,  // 2: jmp    0            side 1 [4]  ; bit 1, 跳回
    0xa242,  // 3: nop                side 0 [4]  ; bit 0
             // .wrap
};

static const struct pio_program ws2812_program_struct = {
    .instructions = ws2812_program,
    .length = 4,
    .origin = -1,
};

#define WS2812_PIN      16
#define NUM_PIXELS      30

// 颜色数据结构
typedef struct {
    uint8_t g;  // WS2812B 使用 GRB 顺序
    uint8_t r;
    uint8_t b;
} ws2812_color_t;

// 像素缓冲区
ws2812_color_t pixel_buffer[NUM_PIXELS];

void ws2812_init(PIO pio, uint sm) {
    uint offset;

    bool success = pio_claim_free_sm_and_add_program_for_gpio_range(
        &ws2812_program_struct, &pio, &sm, &offset,
        WS2812_PIN, 1, true
    );

    if (!success) {
        printf("WS2812 PIO 程序加载失败\n");
        return;
    }

    pio_sm_config c = pio_get_default_sm_config();

    // 设置 side-set 引脚
    sm_config_set_sideset_pins(&c, WS2812_PIN);
    sm_config_set_sideset(&c, 1, false, false);

    // 计算时钟分频以实现 800KHz
    // 每个 bit 需要 1.25us, 8 个指令周期
    // sysclk / (800000 * 8) = 分频值
    float div = (float)clock_get_hz(clk_sys) / (800000.0f * 8.0f);
    sm_config_set_clkdiv(&c, div);

    // 配置移位寄存器 (左移，先发送高位)
    sm_config_set_out_shift(&c, false, true, 24);

    // 配置 FIFO (8 位数据)
    sm_config_set_fifo_join(&c, PIO_FIFO_JOIN_TX);

    // 初始化引脚
    pio_gpio_init(pio, WS2812_PIN);
    pio_sm_set_consecutive_pindirs(pio, sm, WS2812_PIN, 1, true);

    pio_sm_init(pio, sm, offset, &c);
    pio_sm_set_enabled(pio, sm, true);
}

void ws2812_set_pixel(uint index, uint8_t r, uint8_t g, uint8_t b) {
    if (index < NUM_PIXELS) {
        pixel_buffer[index].r = r;
        pixel_buffer[index].g = g;
        pixel_buffer[index].b = b;
    }
}

void ws2812_show(PIO pio, uint sm) {
    // 发送所有像素数据
    for (int i = 0; i < NUM_PIXELS; i++) {
        uint32_t grb = ((uint32_t)pixel_buffer[i].g << 16) |
                       ((uint32_t)pixel_buffer[i].r << 8) |
                       (uint32_t)pixel_buffer[i].b;
        pio_sm_put_blocking(pio, sm, grb);
    }

    // RESET 延时 (>50us)
    sleep_us(60);
}

// HSV 转 RGB 辅助函数
void hsv_to_rgb(uint8_t h, uint8_t s, uint8_t v, uint8_t* r, uint8_t* g, uint8_t* b) {
    uint8_t region, remainder, p, q, t;

    if (s == 0) {
        *r = v; *g = v; *b = v;
        return;
    }

    region = h / 43;
    remainder = (h - (region * 43)) * 6;

    p = (v * (255 - s)) >> 8;
    q = (v * (255 - ((s * remainder) >> 8))) >> 8;
    t = (v * (255 - ((s * (255 - remainder)) >> 8))) >> 8;

    switch (region) {
        case 0: *r = v; *g = t; *b = p; break;
        case 1: *r = q; *g = v; *b = p; break;
        case 2: *r = p; *g = v; *b = t; break;
        case 3: *r = p; *g = q; *b = v; break;
        case 4: *r = t; *g = p; *b = v; break;
        default: *r = v; *g = p; *b = q; break;
    }
}

int main(void) {
    stdio_init_all();

    PIO pio = pio0;
    uint sm = 0;

    ws2812_init(pio, sm);

    printf("WS2812B 灯带驱动启动\n");

    uint8_t hue = 0;

    while (1) {
        // 彩虹效果
        for (int i = 0; i < NUM_PIXELS; i++) {
            uint8_t r, g, b;
            uint8_t pixel_hue = (hue + (i * 255 / NUM_PIXELS)) % 256;
            hsv_to_rgb(pixel_hue, 255, 128, &r, &g, &b);
            ws2812_set_pixel(i, r, g, b);
        }

        ws2812_show(pio, sm);

        hue++;
        sleep_ms(20);
    }

    return 0;
}
```

### 5.5 PIO 自定义协议 - DHT22 温湿度传感器

```c
/**
 * @file pio_dht22.c
 * @brief 使用 PIO 读取 DHT22 温湿度传感器
 */

#include "pico/stdlib.h"
#include "hardware/pio.h"
#include "hardware/dma.h"

#define DHT22_PIN   15

// PIO 程序 - DHT22 读取
// 测量脉冲宽度来区分 0 和 1
static const uint16_t dht22_program[] = {
    // .wrap_target
    0x8080,  // 0: pull   block          ; 等待触发信号
    // wait_start:
    0x00c3,  // 1: jmp    pin, wait_start ; 等待引脚变低
    // wait_low:
    0x0042,  // 2: jmp    !pin, wait_low  ; 等待引脚变高 (响应信号)
    // wait_high:
    0x00c4,  // 3: jmp    pin, wait_high  ; 等待引脚变低 (数据开始)
    // bit_loop:
    0xe020,  // 4: set    x, 0             ; 清零计数器
    // count_low:
    0x4045,  // 5: jmp    x--, count_low   ; 计算低电平时间
    // wait_data_high:
    0x0047,  // 6: jmp    !pin, wait_data_high ; 等待引脚变高
    // count_high:
    0x4048,  // 7: jmp    x--, count_high   ; 计算高电平时间
    0x6008,  // 8: in     x, 8              ; 输入计数值
    // wait_data_low:
    0x00ca,  // 9: jmp    pin, wait_data_low ; 等待下一个位
    0x0004,  // a: jmp    bit_loop          ; 继续下一位
             // .wrap
};

static const struct pio_program dht22_program_struct = {
    .instructions = dht22_program,
    .length = 11,
    .origin = -1,
};

// DHT22 数据结构
typedef struct {
    uint8_t humidity_high;
    uint8_t humidity_low;
    uint8_t temp_high;
    uint8_t temp_low;
    uint8_t checksum;
} dht22_data_t;

bool dht22_read(PIO pio, uint sm, float* temperature, float* humidity) {
    uint8_t data[5] = {0};

    // 触发 DHT22
    gpio_init(DHT22_PIN);
    gpio_set_dir(DHT22_PIN, GPIO_OUT);
    gpio_put(DHT22_PIN, 0);
    sleep_ms(1);
    gpio_put(DHT22_PIN, 1);
    sleep_us(30);
    gpio_set_dir(DHT22_PIN, GPIO_IN);
    gpio_pull_up(DHT22_PIN);

    // 启动 PIO 状态机
    pio_sm_put_blocking(pio, sm, 1);

    // 读取 40 位数据 (5 字节)
    for (int i = 0; i < 5; i++) {
        for (int j = 0; j < 8; j++) {
            uint32_t val = pio_sm_get_blocking(pio, sm);
            // 根据计数值判断是 0 还是 1
            data[i] <<= 1;
            if (val < 40) {  // 阈值，根据实际调整
                data[i] |= 1;
            }
        }
    }

    // 校验和检查
    uint8_t checksum = data[0] + data[1] + data[2] + data[3];
    if (checksum != data[4]) {
        return false;
    }

    // 解析数据
    *humidity = ((data[0] << 8) | data[1]) / 10.0f;
    int16_t temp_raw = (data[2] << 8) | data[3];
    if (temp_raw & 0x8000) {
        temp_raw = -(temp_raw & 0x7FFF);
    }
    *temperature = temp_raw / 10.0f;

    return true;
}

int main(void) {
    stdio_init_all();

    PIO pio = pio0;
    uint sm = 0;
    uint offset;

    // 加载 PIO 程序
    bool success = pio_claim_free_sm_and_add_program_for_gpio_range(
        &dht22_program_struct, &pio, &sm, &offset,
        DHT22_PIN, 1, true
    );

    if (!success) {
        printf("DHT22 PIO 程序加载失败\n");
        return -1;
    }

    pio_sm_config c = pio_get_default_sm_config();
    sm_config_set_in_pins(&c, DHT22_PIN);
    sm_config_set_jmp_pin(&c, DHT22_PIN);
    sm_config_set_in_shift(&c, true, false, 32);

    float clkdiv = (float)clock_get_hz(clk_sys) / 1000000.0f;  // 1MHz
    sm_config_set_clkdiv(&c, clkdiv);

    pio_sm_init(pio, sm, offset, &c);
    pio_sm_set_enabled(pio, sm, true);

    printf("DHT22 传感器读取启动\n");

    while (1) {
        float temp, humidity;

        if (dht22_read(pio, sm, &temp, &humidity)) {
            printf("温度: %.1f°C, 湿度: %.1f%%\n", temp, humidity);
        } else {
            printf("DHT22 读取失败\n");
        }

        sleep_ms(2000);  // DHT22 最小采样间隔 2 秒
    }

    return 0;
}
```



---

## 6. PWM控制电机

### 6.1 PWM 基础

RP2040 有 8 个 PWM 通道，每个通道可以独立控制两个 GPIO 引脚（A/B 相），支持：

- 可编程频率和占空比
- 相位校正模式
- 死区插入（用于电机驱动）

### 6.2 基础 PWM LED 调光

```c
/**
 * @file pwm_basics.c
 * @brief PWM 基础示例 - LED 调光
 */

#include "pico/stdlib.h"
#include "hardware/pwm.h"
#include "hardware/clocks.h"

#define LED_PIN     25

void pwm_init_pin(uint pin) {
    // 设置 GPIO 为 PWM 功能
    gpio_set_function(pin, GPIO_FUNC_PWM);

    // 获取该引脚对应的 PWM 切片
    uint slice_num = pwm_gpio_to_slice_num(pin);
    uint channel = pwm_gpio_to_channel(pin);

    // 配置 PWM
    pwm_config config = pwm_get_default_config();

    // 设置分频，使 PWM 频率约为 1kHz
    // PWM 频率 = 系统时钟 / (分频值 * (TOP + 1))
    // 125MHz / (125 * 1000) = 1kHz
    pwm_config_set_clkdiv(&config, 125.0f);
    pwm_config_set_wrap(&config, 999);  // TOP = 999

    // 初始占空比 0
    pwm_set_chan_level(slice_num, channel, 0);

    // 启用 PWM
    pwm_init(slice_num, &config, true);
}

void pwm_set_duty(uint pin, float duty_percent) {
    uint slice_num = pwm_gpio_to_slice_num(pin);
    uint channel = pwm_gpio_to_channel(pin);

    // 计算占空比值 (0-100% 映射到 0-999)
    uint16_t level = (uint16_t)((duty_percent / 100.0f) * 999.0f);
    pwm_set_chan_level(slice_num, channel, level);
}

int main(void) {
    stdio_init_all();

    // 初始化 PWM
    pwm_init_pin(LED_PIN);

    printf("PWM LED 调光演示\n");

    float duty = 0;
    int direction = 1;

    while (1) {
        // 设置占空比
        pwm_set_duty(LED_PIN, duty);

        // 渐变
        duty += direction * 0.5f;
        if (duty >= 100.0f) {
            duty = 100.0f;
            direction = -1;
        } else if (duty <= 0.0f) {
            duty = 0.0f;
            direction = 1;
        }

        sleep_ms(10);
    }

    return 0;
}
```

### 6.3 直流电机 H 桥控制

```c
/**
 * @file pwm_motor.c
 * @brief 使用 PWM 控制直流电机 (H 桥驱动)
 */

#include "pico/stdlib.h"
#include "hardware/pwm.h"
#include "hardware/gpio.h"

// H 桥引脚定义 (L298N 或类似驱动)
#define MOTOR_PWM_PIN       16  // ENA - 速度控制
#define MOTOR_DIR_PIN1      17  // IN1 - 方向 1
#define MOTOR_DIR_PIN2      18  // IN2 - 方向 2

// 编码器引脚 (用于测速)
#define ENCODER_A_PIN       19
#define ENCODER_B_PIN       20

// 电机控制结构体
typedef struct {
    uint pwm_pin;
    uint dir_pin1;
    uint dir_pin2;
    uint pwm_slice;
    uint pwm_channel;
    uint16_t pwm_wrap;
    volatile int32_t encoder_count;
    volatile int32_t last_encoder_count;
} motor_t;

motor_t motor;

// 编码器中断处理
void encoder_callback(uint gpio, uint32_t events) {
    bool a = gpio_get(ENCODER_A_PIN);
    bool b = gpio_get(ENCODER_B_PIN);

    // 判断方向
    if (a == b) {
        motor.encoder_count++;
    } else {
        motor.encoder_count--;
    }
}

void motor_init(motor_t* m, uint pwm_pin, uint dir1, uint dir2) {
    m->pwm_pin = pwm_pin;
    m->dir_pin1 = dir1;
    m->dir_pin2 = dir2;
    m->pwm_wrap = 1000;  // TOP 值
    m->encoder_count = 0;
    m->last_encoder_count = 0;

    // 配置方向引脚
    gpio_init(dir1);
    gpio_init(dir2);
    gpio_set_dir(dir1, GPIO_OUT);
    gpio_set_dir(dir2, GPIO_OUT);
    gpio_put(dir1, 0);
    gpio_put(dir2, 0);

    // 配置 PWM
    gpio_set_function(pwm_pin, GPIO_FUNC_PWM);
    m->pwm_slice = pwm_gpio_to_slice_num(pwm_pin);
    m->pwm_channel = pwm_gpio_to_channel(pwm_pin);

    pwm_config config = pwm_get_default_config();
    pwm_config_set_clkdiv(&config, 125.0f);  // 1kHz
    pwm_config_set_wrap(&config, m->pwm_wrap - 1);
    pwm_init(m->pwm_slice, &config, true);

    pwm_set_chan_level(m->pwm_slice, m->pwm_channel, 0);

    // 配置编码器
    gpio_init(ENCODER_A_PIN);
    gpio_init(ENCODER_B_PIN);
    gpio_set_dir(ENCODER_A_PIN, GPIO_IN);
    gpio_set_dir(ENCODER_B_PIN, GPIO_IN);
    gpio_pull_up(ENCODER_A_PIN);
    gpio_pull_up(ENCODER_B_PIN);

    // 配置编码器中断
    gpio_set_irq_enabled_with_callback(
        ENCODER_A_PIN,
        GPIO_IRQ_EDGE_RISE | GPIO_IRQ_EDGE_FALL,
        true,
        encoder_callback
    );
}

void motor_set_speed(motor_t* m, int16_t speed) {
    // speed: -1000 到 +1000
    if (speed > 0) {
        // 正转
        gpio_put(m->dir_pin1, 1);
        gpio_put(m->dir_pin2, 0);
        uint16_t level = (speed > 1000) ? 1000 : speed;
        pwm_set_chan_level(m->pwm_slice, m->pwm_channel, level);
    } else if (speed < 0) {
        // 反转
        gpio_put(m->dir_pin1, 0);
        gpio_put(m->dir_pin2, 1);
        uint16_t level = (-speed > 1000) ? 1000 : -speed;
        pwm_set_chan_level(m->pwm_slice, m->pwm_channel, level);
    } else {
        // 停止
        gpio_put(m->dir_pin1, 0);
        gpio_put(m->dir_pin2, 0);
        pwm_set_chan_level(m->pwm_slice, m->pwm_channel, 0);
    }
}

void motor_brake(motor_t* m) {
    gpio_put(m->dir_pin1, 1);
    gpio_put(m->dir_pin2, 1);
    pwm_set_chan_level(m->pwm_slice, m->pwm_channel, m->pwm_wrap);
}

int32_t motor_get_speed(motor_t* m) {
    int32_t count = m->encoder_count;
    int32_t delta = count - m->last_encoder_count;
    m->last_encoder_count = count;
    return delta;
}

int main(void) {
    stdio_init_all();

    // 初始化电机
    motor_init(&motor, MOTOR_PWM_PIN, MOTOR_DIR_PIN1, MOTOR_DIR_PIN2);

    printf("电机控制示例启动\n");

    // 速度测试
    int16_t speeds[] = {300, 600, 1000, -300, -600, -1000, 0};
    int speed_idx = 0;
    absolute_time_t last_time = get_absolute_time();

    while (1) {
        // 每 2 秒切换一次速度
        if (absolute_time_diff_us(last_time, get_absolute_time()) > 2000000) {
            last_time = get_absolute_time();

            int16_t speed = speeds[speed_idx];
            motor_set_speed(&motor, speed);
            printf("设置速度: %d\n", speed);

            speed_idx = (speed_idx + 1) % (sizeof(speeds) / sizeof(speeds[0]));
        }

        // 每 100ms 读取编码器速度
        static absolute_time_t last_speed_time = {0};
        if (absolute_time_diff_us(last_speed_time, get_absolute_time()) > 100000) {
            last_speed_time = get_absolute_time();
            int32_t rpm = motor_get_speed(&motor) * 10;  // 转换为 RPM 估算
            printf("编码器计数: %ld, 速度: %ld\n",
                   (long)motor.encoder_count, (long)rpm);
        }

        sleep_ms(10);
    }

    return 0;
}
```

### 6.4 舵机 (Servo) 控制

```c
/**
 * @file pwm_servo.c
 * @brief 舵机控制示例 (SG90, MG995 等)
 */

#include "pico/stdlib.h"
#include "hardware/pwm.h"

#define SERVO_PIN   15

// 舵机参数 (SG90)
// 周期: 20ms (50Hz)
// 0度:   0.5ms 脉冲 (2.5% 占空比)
// 90度:  1.5ms 脉冲 (7.5% 占空比)
// 180度: 2.5ms 脉冲 (12.5% 占空比)

void servo_init(uint pin) {
    gpio_set_function(pin, GPIO_FUNC_PWM);

    uint slice_num = pwm_gpio_to_slice_num(pin);
    uint channel = pwm_gpio_to_channel(pin);

    // 配置 50Hz (20ms 周期)
    pwm_config config = pwm_get_default_config();
    pwm_config_set_clkdiv(&config, 125.0f);  // 1MHz
    pwm_config_set_wrap(&config, 19999);     // 20ms 周期

    pwm_init(slice_num, &config, true);
    pwm_set_chan_level(slice_num, channel, 1500);  // 初始位置 90度
}

void servo_set_angle(uint pin, float angle) {
    // 限制角度范围
    if (angle < 0) angle = 0;
    if (angle > 180) angle = 180;

    uint slice_num = pwm_gpio_to_slice_num(pin);
    uint channel = pwm_gpio_to_channel(pin);

    // 计算脉冲宽度 (微秒)
    // 0度 = 500us, 180度 = 2500us
    uint16_t pulse_us = (uint16_t)(500 + (angle / 180.0f) * 2000);

    pwm_set_chan_level(slice_num, channel, pulse_us);
}

void servo_set_microseconds(uint pin, uint16_t us) {
    if (us < 500) us = 500;
    if (us > 2500) us = 2500;

    uint slice_num = pwm_gpio_to_slice_num(pin);
    uint channel = pwm_gpio_to_channel(pin);

    pwm_set_chan_level(slice_num, channel, us);
}

int main(void) {
    stdio_init_all();

    servo_init(SERVO_PIN);

    printf("舵机控制示例启动\n");

    while (1) {
        // 从 0 度到 180 度扫描
        for (float angle = 0; angle <= 180; angle += 2) {
            servo_set_angle(SERVO_PIN, angle);
            sleep_ms(20);
        }

        sleep_ms(500);

        // 从 180 度到 0 度扫描
        for (float angle = 180; angle >= 0; angle -= 2) {
            servo_set_angle(SERVO_PIN, angle);
            sleep_ms(20);
        }

        sleep_ms(500);
    }

    return 0;
}
```

---

## 7. ADC读取模拟传感器

### 7.1 ADC 基础

RP2040 有 4 个 ADC 通道：

- **ADC0**: GPIO26
- **ADC1**: GPIO27
- **ADC2**: GPIO28
- **ADC3**: 内部温度传感器

ADC 特性：

- 12 位分辨率 (0-4095)
- 500ksps 采样率
- 内部温度传感器
- 可选的 FIFO 和 DMA 支持

### 7.2 基础 ADC 读取

```c
/**
 * @file adc_basic.c
 * @brief ADC 基础读取示例
 */

#include "pico/stdlib.h"
#include "hardware/adc.h"
#include "hardware/gpio.h"

// ADC 输入引脚
#define ADC_POT_PIN     26  // 电位器
#define ADC_LDR_PIN     27  // 光敏电阻
#define ADC_TEMP_PIN    28  // 外部温度传感器 (如 LM35)

// 参考电压 (Vref)
#define ADC_VREF        3.3f
#define ADC_RESOLUTION  4096.0f  // 12-bit

void adc_init_all(void) {
    // 初始化 ADC 硬件
    adc_init();

    // 启用温度传感器
    adc_set_temp_sensor_enabled(true);
}

float adc_read_voltage(uint input) {
    adc_select_input(input);
    uint16_t raw = adc_read();
    return (raw / ADC_RESOLUTION) * ADC_VREF;
}

float adc_read_temperature_internal(void) {
    // 内部温度传感器读取
    adc_select_input(4);  // 温度传感器是输入 4
    uint16_t raw = adc_read();

    // 温度计算公式 (来自数据手册)
    // Vbe = raw * Vref / 4096
    // Temperature = 27 - (Vbe - 0.706) / 0.001721
    float voltage = raw * ADC_VREF / ADC_RESOLUTION;
    float temperature = 27.0f - (voltage - 0.706f) / 0.001721f;

    return temperature;
}

float lm35_read_temperature(uint adc_input) {
    // LM35: 10mV/°C
    float voltage = adc_read_voltage(adc_input);
    return voltage * 100.0f;  // °C
}

float read_battery_voltage(uint adc_input, float divider_ratio) {
    // 通过分压电阻读取电池电压
    float voltage = adc_read_voltage(adc_input);
    return voltage * divider_ratio;
}

int main(void) {
    stdio_init_all();
    adc_init_all();

    // 配置 GPIO 为 ADC 功能 (不需要，adc_select_input 自动处理)

    printf("ADC 读取示例启动\n");

    while (1) {
        // 读取电位器 (ADC0 - GPIO26)
        float pot_voltage = adc_read_voltage(0);

        // 读取光敏电阻 (ADC1 - GPIO27)
        float ldr_voltage = adc_read_voltage(1);

        // 读取内部温度
        float int_temp = adc_read_temperature_internal();

        // 读取外部温度 (假设连接到 ADC2)
        // float ext_temp = lm35_read_temperature(2);

        printf("电位器: %.2fV, 光敏: %.2fV, 内部温度: %.1f°C\n",
               pot_voltage, ldr_voltage, int_temp);

        sleep_ms(500);
    }

    return 0;
}
```

### 7.3 ADC DMA 连续采样

```c
/**
 * @file adc_dma.c
 * @brief 使用 DMA 进行连续 ADC 采样
 */

#include "pico/stdlib.h"
#include "hardware/adc.h"
#include "hardware/dma.h"
#include "hardware/irq.h"

#define ADC_CHANNEL     0       // GPIO26
#define SAMPLE_RATE     10000   // 10kHz
#define BUFFER_SIZE     1024    // 采样缓冲区大小

// 双缓冲区
uint16_t buffer_a[BUFFER_SIZE];
uint16_t buffer_b[BUFFER_SIZE];

volatile bool buffer_ready = false;
volatile uint16_t* ready_buffer = NULL;

// DMA 通道
int dma_channel;

// DMA 中断处理
void dma_handler(void) {
    dma_hw->ints0 = 1u << dma_channel;  // 清除中断标志

    // 交换缓冲区
    if (ready_buffer == buffer_a) {
        ready_buffer = buffer_b;
        dma_channel_set_read_addr(dma_channel, buffer_b, false);
    } else {
        ready_buffer = buffer_a;
        dma_channel_set_read_addr(dma_channel, buffer_a, false);
    }

    buffer_ready = true;

    // 重新触发 DMA
    dma_channel_set_trans_count(dma_channel, BUFFER_SIZE, true);
}

void adc_dma_init(void) {
    // 初始化 ADC
    adc_init();
    adc_select_input(ADC_CHANNEL);
    adc_fifo_setup(
        true,   // 启用 FIFO
        true,   // 启用 DMA 请求
        1,      // 在至少 1 个样本时断言 DREQ
        false,  // 不使用错误位
        false   // 不右移 (保持 12-bit)
    );

    // 设置采样率
    // ADC 时钟 = 48MHz
    // 采样时间 = (cycles + 1) / 48MHz
    // 目标 10kHz -> 4800 个时钟周期每采样
    adc_set_clkdiv(4800.0f);

    // 申请 DMA 通道
    dma_channel = dma_claim_unused_channel(true);

    // 配置 DMA
    dma_channel_config config = dma_channel_get_default_config(dma_channel);

    // 从 ADC FIFO 读取 (16-bit)
    channel_config_set_transfer_data_size(&config, DMA_SIZE_16);
    channel_config_set_read_increment(&config, false);  // 固定读取地址
    channel_config_set_write_increment(&config, true);  // 写入地址递增

    //  paced by ADC DREQ
    channel_config_set_dreq(&config, DREQ_ADC);

    // 配置链式传输（双缓冲）
    channel_config_set_chain_to(&config, dma_channel);

    dma_channel_configure(
        dma_channel,
        &config,
        buffer_a,           // 写入地址
        &adc_hw->fifo,      // 读取地址
        BUFFER_SIZE,        // 传输计数
        false               // 不立即启动
    );

    // 配置 DMA 中断
    dma_channel_set_irq0_enabled(dma_channel, true);
    irq_set_exclusive_handler(DMA_IRQ_0, dma_handler);
    irq_set_enabled(DMA_IRQ_0, true);
}

void process_samples(uint16_t* buffer, size_t size) {
    // 计算平均值
    uint32_t sum = 0;
    for (size_t i = 0; i < size; i++) {
        sum += buffer[i];
    }
    float average = (sum / (float)size) * 3.3f / 4096.0f;

    // 计算峰值
    uint16_t max_val = 0, min_val = 4096;
    for (size_t i = 0; i < size; i++) {
        if (buffer[i] > max_val) max_val = buffer[i];
        if (buffer[i] < min_val) min_val = buffer[i];
    }

    float max_voltage = max_val * 3.3f / 4096.0f;
    float min_voltage = min_val * 3.3f / 4096.0f;

    printf("平均: %.3fV, 峰值: %.3fV, 谷值: %.3fV\n",
           average, max_voltage, min_voltage);
}

int main(void) {
    stdio_init_all();

    printf("ADC DMA 采样示例启动\n");

    adc_dma_init();

    // 启动 ADC 自由运行模式
    adc_run(true);

    // 启动 DMA
    dma_channel_start(dma_channel);

    while (1) {
        if (buffer_ready) {
            buffer_ready = false;

            // 处理已准备好的缓冲区
            process_samples((uint16_t*)ready_buffer, BUFFER_SIZE);
        }

        // 主循环可以做其他事情
        sleep_ms(1);
    }

    return 0;
}
```

---

## 8. I2C/SPI通信

### 8.1 I2C 通信

```c
/**
 * @file i2c_example.c
 * @brief I2C 通信示例 - BME280 传感器
 */

#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include <stdio.h>

// I2C 配置
#define I2C_PORT        i2c0
#define I2C_SDA_PIN     4
#define I2C_SCL_PIN     5
#define I2C_BAUDRATE    400000  // 400kHz

// BME280 地址
#define BME280_ADDR     0x76

// BME280 寄存器
#define BME280_REG_ID           0xD0
#define BME280_REG_RESET        0xE0
#define BME280_REG_CTRL_HUM     0xF2
#define BME280_REG_STATUS       0xF3
#define BME280_REG_CTRL_MEAS    0xF4
#define BME280_REG_CONFIG       0xF5
#define BME280_REG_PRESS_MSB    0xF7
#define BME280_REG_CALIB        0x88

// BME280 校准数据
typedef struct {
    uint16_t dig_T1;
    int16_t  dig_T2;
    int16_t  dig_T3;
    uint16_t dig_P1;
    int16_t  dig_P2;
    int16_t  dig_P3;
    int16_t  dig_P4;
    int16_t  dig_P5;
    int16_t  dig_P6;
    int16_t  dig_P7;
    int16_t  dig_P8;
    int16_t  dig_P9;
    uint8_t  dig_H1;
    int16_t  dig_H2;
    uint8_t  dig_H3;
    int16_t  dig_H4;
    int16_t  dig_H5;
    int8_t   dig_H6;
} bme280_calib_data_t;

bme280_calib_data_t calib_data;
int32_t t_fine;

void i2c_init_custom(void) {
    // 初始化 I2C
    i2c_init(I2C_PORT, I2C_BAUDRATE);

    // 配置 GPIO
    gpio_set_function(I2C_SDA_PIN, GPIO_FUNC_I2C);
    gpio_set_function(I2C_SCL_PIN, GPIO_FUNC_I2C);
    gpio_pull_up(I2C_SDA_PIN);
    gpio_pull_up(I2C_SCL_PIN);
}

int i2c_write_reg(uint8_t addr, uint8_t reg, uint8_t data) {
    uint8_t buf[2] = {reg, data};
    return i2c_write_blocking(I2C_PORT, addr, buf, 2, false);
}

int i2c_read_reg(uint8_t addr, uint8_t reg, uint8_t* data, size_t len) {
    i2c_write_blocking(I2C_PORT, addr, &reg, 1, true);
    return i2c_read_blocking(I2C_PORT, addr, data, len, false);
}

bool bme280_init(void) {
    // 检查设备 ID
    uint8_t id;
    i2c_read_reg(BME280_ADDR, BME280_REG_ID, &id, 1);

    if (id != 0x60) {
        printf("BME280 未找到! ID=0x%02X\n", id);
        return false;
    }

    printf("BME280 检测到!\n");

    // 软复位
    i2c_write_reg(BME280_ADDR, BME280_REG_RESET, 0xB6);
    sleep_ms(10);

    // 读取校准数据
    uint8_t calib[26];
    i2c_read_reg(BME280_ADDR, BME280_REG_CALIB, calib, 26);

    calib_data.dig_T1 = (calib[1] << 8) | calib[0];
    calib_data.dig_T2 = (calib[3] << 8) | calib[2];
    calib_data.dig_T3 = (calib[5] << 8) | calib[4];
    calib_data.dig_P1 = (calib[7] << 8) | calib[6];
    calib_data.dig_P2 = (calib[9] << 8) | calib[8];
    calib_data.dig_P3 = (calib[11] << 8) | calib[10];
    calib_data.dig_P4 = (calib[13] << 8) | calib[12];
    calib_data.dig_P5 = (calib[15] << 8) | calib[14];
    calib_data.dig_P6 = (calib[17] << 8) | calib[16];
    calib_data.dig_P7 = (calib[19] << 8) | calib[18];
    calib_data.dig_P8 = (calib[21] << 8) | calib[20];
    calib_data.dig_P9 = (calib[23] << 8) | calib[22];

    // 配置传感器
    i2c_write_reg(BME280_ADDR, BME280_REG_CTRL_HUM, 0x01);   // 湿度过采样 x1
    i2c_write_reg(BME280_ADDR, BME280_REG_CTRL_MEAS, 0x27);  // 温度/压力过采样 x1, 正常模式
    i2c_write_reg(BME280_ADDR, BME280_REG_CONFIG, 0x00);     // 配置

    return true;
}

// 温度补偿 (来自 BME280 数据手册)
int32_t compensate_temperature(int32_t adc_T) {
    int32_t var1, var2, T;
    var1 = ((((adc_T >> 3) - ((int32_t)calib_data.dig_T1 << 1))) *
            ((int32_t)calib_data.dig_T2)) >> 11;
    var2 = (((((adc_T >> 4) - ((int32_t)calib_data.dig_T1)) *
              ((adc_T >> 4) - ((int32_t)calib_data.dig_T1))) >> 12) *
            ((int32_t)calib_data.dig_T3)) >> 14;
    t_fine = var1 + var2;
    T = (t_fine * 5 + 128) >> 8;
    return T;
}

// 压力补偿
uint32_t compensate_pressure(int32_t adc_P) {
    int64_t var1, var2, p;
    var1 = ((int64_t)t_fine) - 128000;
    var2 = var1 * var1 * (int64_t)calib_data.dig_P6;
    var2 = var2 + ((var1 * (int64_t)calib_data.dig_P5) << 17);
    var2 = var2 + (((int64_t)calib_data.dig_P4) << 35);
    var1 = ((var1 * var1 * (int64_t)calib_data.dig_P3) >> 8) +
           ((var1 * (int64_t)calib_data.dig_P2) << 12);
    var1 = (((((int64_t)1) << 47) + var1)) * ((int64_t)calib_data.dig_P1) >> 33;

    if (var1 == 0) return 0;

    p = 1048576 - adc_P;
    p = (((p << 31) - var2) * 3125) / var1;
    var1 = (((int64_t)calib_data.dig_P9) * (p >> 13) * (p >> 13)) >> 25;
    var2 = (((int64_t)calib_data.dig_P8) * p) >> 19;
    p = ((p + var1 + var2) >> 8) + (((int64_t)calib_data.dig_P7) << 4);

    return (uint32_t)p;
}

void bme280_read(float* temperature, float* pressure, float* humidity) {
    uint8_t data[8];
    i2c_read_reg(BME280_ADDR, BME280_REG_PRESS_MSB, data, 8);

    int32_t adc_P = ((int32_t)data[0] << 12) | ((int32_t)data[1] << 4) | (data[2] >> 4);
    int32_t adc_T = ((int32_t)data[3] << 12) | ((int32_t)data[4] << 4) | (data[5] >> 4);
    int32_t adc_H = ((int32_t)data[6] << 8) | data[7];

    // 补偿
    int32_t temp = compensate_temperature(adc_T);
    *temperature = temp / 100.0f;

    uint32_t press = compensate_pressure(adc_P);
    *pressure = press / 25600.0f;  // hPa

    // 简化湿度计算
    *humidity = adc_H / 1024.0f;  // 简化版
}

// 扫描 I2C 总线上的设备
void i2c_scan(void) {
    printf("扫描 I2C 总线...\n");
    for (uint8_t addr = 0x08; addr < 0x78; addr++) {
        uint8_t dummy;
        int ret = i2c_read_blocking(I2C_PORT, addr, &dummy, 1, false);
        if (ret >= 0) {
            printf("发现设备: 0x%02X\n", addr);
        }
    }
    printf("扫描完成\n");
}

int main(void) {
    stdio_init_all();
    sleep_ms(2000);  // 等待 USB 串口连接

    printf("I2C BME280 示例启动\n");

    i2c_init_custom();
    i2c_scan();

    if (!bme280_init()) {
        printf("BME280 初始化失败\n");
        while (1) tight_loop_contents();
    }

    while (1) {
        float temp, press, hum;
        bme280_read(&temp, &press, &hum);

        printf("温度: %.2f°C, 压力: %.2f hPa, 湿度: %.1f%%\n",
               temp, press, hum);

        sleep_ms(1000);
    }

    return 0;
}
```

### 8.2 SPI 通信

```c
/**
 * @file spi_example.c
 * @brief SPI 通信示例 - W25Q128 闪存 + ILI9341 TFT 显示屏
 */

#include "pico/stdlib.h"
#include "hardware/spi.h"
#include <stdio.h>
#include <string.h>

// SPI0 配置 (用于 Flash)
#define SPI_PORT_FLASH  spi0
#define FLASH_MISO_PIN  16
#define FLASH_MOSI_PIN  19
#define FLASH_SCK_PIN   18
#define FLASH_CS_PIN    17

// SPI1 配置 (用于 TFT 显示屏)
#define SPI_PORT_TFT    spi1
#define TFT_MISO_PIN    12
#define TFT_MOSI_PIN    15
#define TFT_SCK_PIN     14
#define TFT_CS_PIN      13
#define TFT_DC_PIN      11
#define TFT_RST_PIN     10

// W25Q128 命令
#define W25X_WRITE_ENABLE       0x06
#define W25X_WRITE_DISABLE      0x04
#define W25X_READ_STATUS_REG    0x05
#define W25X_READ_DATA          0x03
#define W25X_PAGE_PROGRAM       0x02
#define W25X_SECTOR_ERASE       0x20
#define W25X_CHIP_ERASE         0xC7
#define W25X_JEDEC_ID           0x9F

// ILI9341 命令
#define ILI9341_SWRESET         0x01
#define ILI9341_SLPOUT          0x11
#define ILI9341_DISPON          0x29
#define ILI9341_CASET           0x2A
#define ILI9341_PASET           0x2B
#define ILI9341_RAMWR           0x2C

// ==================== Flash 操作 ====================

void flash_cs_select(void) {
    gpio_put(FLASH_CS_PIN, 0);
    sleep_us(1);
}

void flash_cs_deselect(void) {
    sleep_us(1);
    gpio_put(FLASH_CS_PIN, 1);
}

void flash_init(void) {
    // 初始化 SPI
    spi_init(SPI_PORT_FLASH, 1000000);  // 1MHz

    // 配置 GPIO
    gpio_set_function(FLASH_MISO_PIN, GPIO_FUNC_SPI);
    gpio_set_function(FLASH_MOSI_PIN, GPIO_FUNC_SPI);
    gpio_set_function(FLASH_SCK_PIN, GPIO_FUNC_SPI);

    // 配置 CS 引脚
    gpio_init(FLASH_CS_PIN);
    gpio_set_dir(FLASH_CS_PIN, GPIO_OUT);
    flash_cs_deselect();
}

uint32_t flash_read_jedec_id(void) {
    flash_cs_select();

    uint8_t tx = W25X_JEDEC_ID;
    uint8_t rx[3] = {0};

    spi_write_blocking(SPI_PORT_FLASH, &tx, 1);
    spi_read_blocking(SPI_PORT_FLASH, 0, rx, 3);

    flash_cs_deselect();

    return ((uint32_t)rx[0] << 16) | ((uint32_t)rx[1] << 8) | rx[2];
}

void flash_read(uint32_t addr, uint8_t* buffer, size_t len) {
    flash_cs_select();

    uint8_t cmd[4] = {
        W25X_READ_DATA,
        (addr >> 16) & 0xFF,
        (addr >> 8) & 0xFF,
        addr & 0xFF
    };

    spi_write_blocking(SPI_PORT_FLASH, cmd, 4);
    spi_read_blocking(SPI_PORT_FLASH, 0, buffer, len);

    flash_cs_deselect();
}

void flash_wait_busy(void) {
    flash_cs_select();

    uint8_t cmd = W25X_READ_STATUS_REG;
    uint8_t status;

    spi_write_blocking(SPI_PORT_FLASH, &cmd, 1);

    do {
        spi_read_blocking(SPI_PORT_FLASH, 0, &status, 1);
    } while (status & 0x01);  // BUSY bit

    flash_cs_deselect();
}

void flash_write_enable(void) {
    flash_cs_select();
    uint8_t cmd = W25X_WRITE_ENABLE;
    spi_write_blocking(SPI_PORT_FLASH, &cmd, 1);
    flash_cs_deselect();
}

void flash_sector_erase(uint32_t addr) {
    flash_write_enable();

    flash_cs_select();

    uint8_t cmd[4] = {
        W25X_SECTOR_ERASE,
        (addr >> 16) & 0xFF,
        (addr >> 8) & 0xFF,
        addr & 0xFF
    };

    spi_write_blocking(SPI_PORT_FLASH, cmd, 4);
    flash_cs_deselect();

    flash_wait_busy();
}

void flash_page_program(uint32_t addr, const uint8_t* data, size_t len) {
    if (len > 256) len = 256;  // 页大小限制

    flash_write_enable();

    flash_cs_select();

    uint8_t cmd[4] = {
        W25X_PAGE_PROGRAM,
        (addr >> 16) & 0xFF,
        (addr >> 8) & 0xFF,
        addr & 0xFF
    };

    spi_write_blocking(SPI_PORT_FLASH, cmd, 4);
    spi_write_blocking(SPI_PORT_FLASH, data, len);

    flash_cs_deselect();

    flash_wait_busy();
}

// ==================== TFT 显示屏操作 ====================

void tft_write_command(uint8_t cmd) {
    gpio_put(TFT_DC_PIN, 0);  // DC = 0 for command
    gpio_put(TFT_CS_PIN, 0);
    spi_write_blocking(SPI_PORT_TFT, &cmd, 1);
    gpio_put(TFT_CS_PIN, 1);
}

void tft_write_data(uint8_t data) {
    gpio_put(TFT_DC_PIN, 1);  // DC = 1 for data
    gpio_put(TFT_CS_PIN, 0);
    spi_write_blocking(SPI_PORT_TFT, &data, 1);
    gpio_put(TFT_CS_PIN, 1);
}

void tft_write_data16(uint16_t data) {
    gpio_put(TFT_DC_PIN, 1);
    gpio_put(TFT_CS_PIN, 0);
    uint8_t buf[2] = {data >> 8, data & 0xFF};
    spi_write_blocking(SPI_PORT_TFT, buf, 2);
    gpio_put(TFT_CS_PIN, 1);
}

void tft_init(void) {
    // 初始化 SPI
    spi_init(SPI_PORT_TFT, 40000000);  // 40MHz

    // 配置 GPIO
    gpio_set_function(TFT_MISO_PIN, GPIO_FUNC_SPI);
    gpio_set_function(TFT_MOSI_PIN, GPIO_FUNC_SPI);
    gpio_set_function(TFT_SCK_PIN, GPIO_FUNC_SPI);

    // 控制引脚
    gpio_init(TFT_CS_PIN);
    gpio_init(TFT_DC_PIN);
    gpio_init(TFT_RST_PIN);
    gpio_set_dir(TFT_CS_PIN, GPIO_OUT);
    gpio_set_dir(TFT_DC_PIN, GPIO_OUT);
    gpio_set_dir(TFT_RST_PIN, GPIO_OUT);

    gpio_put(TFT_CS_PIN, 1);
    gpio_put(TFT_DC_PIN, 1);

    // 复位显示屏
    gpio_put(TFT_RST_PIN, 1);
    sleep_ms(5);
    gpio_put(TFT_RST_PIN, 0);
    sleep_ms(20);
    gpio_put(TFT_RST_PIN, 1);
    sleep_ms(150);

    // 初始化序列
    tft_write_command(ILI9341_SWRESET);
    sleep_ms(150);

    tft_write_command(ILI9341_SLPOUT);
    sleep_ms(500);

    tft_write_command(ILI9341_DISPON);
    sleep_ms(100);

    printf("TFT 初始化完成\n");
}

void tft_fill_screen(uint16_t color) {
    // 设置列地址
    tft_write_command(ILI9341_CASET);
    tft_write_data16(0);        // X start
    tft_write_data16(239);      // X end

    // 设置页地址
    tft_write_command(ILI9341_PASET);
    tft_write_data16(0);        // Y start
    tft_write_data16(319);      // Y end

    // 开始写入
    tft_write_command(ILI9341_RAMWR);

    // 批量写入
    gpio_put(TFT_DC_PIN, 1);
    gpio_put(TFT_CS_PIN, 0);

    for (int i = 0; i < 240 * 320; i++) {
        uint8_t buf[2] = {color >> 8, color & 0xFF};
        spi_write_blocking(SPI_PORT_TFT, buf, 2);
    }

    gpio_put(TFT_CS_PIN, 1);
}

int main(void) {
    stdio_init_all();
    sleep_ms(2000);

    printf("SPI 示例启动\n");

    // 初始化 Flash
    flash_init();
    uint32_t jedec_id = flash_read_jedec_id();
    printf("Flash JEDEC ID: 0x%06X\n", jedec_id);

    // Flash 读写测试
    uint8_t write_buf[256];
    uint8_t read_buf[256];

    for (int i = 0; i < 256; i++) {
        write_buf[i] = i;
    }

    printf("擦除扇区...\n");
    flash_sector_erase(0);

    printf("写入数据...\n");
    flash_page_program(0, write_buf, 256);

    printf("读取数据...\n");
    flash_read(0, read_buf, 256);

    // 验证
    bool match = true;
    for (int i = 0; i < 256; i++) {
        if (write_buf[i] != read_buf[i]) {
            match = false;
            break;
        }
    }
    printf("数据验证: %s\n", match ? "通过" : "失败");

    // 初始化 TFT
    tft_init();

    // 填充红色
    tft_fill_screen(0xF800);
    sleep_ms(1000);

    // 填充绿色
    tft_fill_screen(0x07E0);
    sleep_ms(1000);

    // 填充蓝色
    tft_fill_screen(0x001F);

    printf("完成!\n");

    while (1) {
        tight_loop_contents();
    }

    return 0;
}
```



---

## 9. DMA传输

### 9.1 DMA 概述

RP2040 的 DMA 控制器特性：

- **12 个独立通道**
- **4 个总线主接口** (每个 32 位)
- **FIFO** 用于快速重新配置
- **中断** 支持 (每个通道独立)
- **链式传输** 支持

### 9.2 基础 DMA 内存复制

```c
/**
 * @file dma_basic.c
 * @brief DMA 基础示例 - 内存复制
 */

#include "pico/stdlib.h"
#include "hardware/dma.h"
#include <string.h>
#include <stdio.h>

#define BUFFER_SIZE     1024

uint32_t source_buffer[BUFFER_SIZE];
uint32_t dest_buffer[BUFFER_SIZE];

void dma_memcopy_example(void) {
    // 初始化源数据
    for (int i = 0; i < BUFFER_SIZE; i++) {
        source_buffer[i] = i * 0x11111111;
    }

    // 申请 DMA 通道
    int channel = dma_claim_unused_channel(true);

    // 配置 DMA
    dma_channel_config config = dma_channel_get_default_config(channel);

    // 32 位传输
    channel_config_set_transfer_data_size(&config, DMA_SIZE_32);

    // 读取地址递增
    channel_config_set_read_increment(&config, true);

    // 写入地址递增
    channel_config_set_write_increment(&config, true);

    // 配置传输
    dma_channel_configure(
        channel,
        &config,
        dest_buffer,        // 目标地址
        source_buffer,      // 源地址
        BUFFER_SIZE,        // 传输数量
        true                // 立即启动
    );

    // 等待完成
    dma_channel_wait_for_finish_blocking(channel);

    // 验证
    bool match = true;
    for (int i = 0; i < BUFFER_SIZE; i++) {
        if (source_buffer[i] != dest_buffer[i]) {
            match = false;
            printf("Mismatch at %d: src=%08X, dst=%08X\n",
                   i, source_buffer[i], dest_buffer[i]);
            break;
        }
    }

    printf("DMA 内存复制: %s\n", match ? "成功" : "失败");

    // 释放通道
    dma_channel_unclaim(channel);
}

// DMA 链式传输示例
void dma_chained_example(void) {
    uint32_t buf1[256], buf2[256], buf3[256];

    // 初始化
    for (int i = 0; i < 256; i++) {
        buf1[i] = i;
    }

    // 申请两个通道
    int chan_a = dma_claim_unused_channel(true);
    int chan_b = dma_claim_unused_channel(true);

    // 配置通道 B (buf2 -> buf3)
    dma_channel_config cfg_b = dma_channel_get_default_config(chan_b);
    channel_config_set_transfer_data_size(&cfg_b, DMA_SIZE_32);
    channel_config_set_read_increment(&cfg_b, true);
    channel_config_set_write_increment(&cfg_b, true);

    dma_channel_configure(
        chan_b,
        &cfg_b,
        buf3,
        buf2,
        256,
        false  // 不立即启动，等链式触发
    );

    // 配置通道 A (buf1 -> buf2)，链式到通道 B
    dma_channel_config cfg_a = dma_channel_get_default_config(chan_a);
    channel_config_set_transfer_data_size(&cfg_a, DMA_SIZE_32);
    channel_config_set_read_increment(&cfg_a, true);
    channel_config_set_write_increment(&cfg_a, true);
    channel_config_set_chain_to(&cfg_a, chan_b);  // 完成后触发通道 B

    dma_channel_configure(
        chan_a,
        &cfg_a,
        buf2,
        buf1,
        256,
        true  // 启动通道 A
    );

    // 等待两个通道完成
    dma_channel_wait_for_finish_blocking(chan_b);

    // 验证
    bool match = true;
    for (int i = 0; i < 256; i++) {
        if (buf1[i] != buf3[i]) {
            match = false;
            break;
        }
    }

    printf("DMA 链式传输: %s\n", match ? "成功" : "失败");

    dma_channel_unclaim(chan_a);
    dma_channel_unclaim(chan_b);
}

// DMA 中断示例
volatile bool dma_complete = false;

void dma_irq_handler(void) {
    // 清除中断标志
    if (dma_hw->ints0 & (1u << 0)) {
        dma_hw->ints0 = 1u << 0;
        dma_complete = true;
    }
}

void dma_interrupt_example(void) {
    uint32_t src[100], dst[100];

    for (int i = 0; i < 100; i++) {
        src[i] = 0xDEADBEEF + i;
    }

    int channel = dma_claim_unused_channel(true);

    // 配置中断
    dma_channel_set_irq0_enabled(channel, true);
    irq_set_exclusive_handler(DMA_IRQ_0, dma_irq_handler);
    irq_set_enabled(DMA_IRQ_0, true);

    // 配置 DMA
    dma_channel_config config = dma_channel_get_default_config(channel);
    channel_config_set_transfer_data_size(&config, DMA_SIZE_32);
    channel_config_set_read_increment(&config, true);
    channel_config_set_write_increment(&config, true);

    dma_complete = false;
    dma_channel_configure(
        channel,
        &config,
        dst,
        src,
        100,
        true
    );

    // 等待中断
    while (!dma_complete) {
        tight_loop_contents();
    }

    printf("DMA 中断完成\n");

    irq_set_enabled(DMA_IRQ_0, false);
    dma_channel_unclaim(channel);
}

int main(void) {
    stdio_init_all();
    sleep_ms(2000);

    printf("\n=== DMA 示例 ===\n\n");

    dma_memcopy_example();
    dma_chained_example();
    dma_interrupt_example();

    printf("\n所有测试完成!\n");

    while (1) tight_loop_contents();
}
```

### 9.3 DMA 与 GPIO 快速输出

```c
/**
 * @file dma_gpio_fast.c
 * @brief 使用 DMA 实现 GPIO 高速输出
 */

#include "pico/stdlib.h"
#include "hardware/dma.h"
#include "hardware/gpio.h"

#define DATA_PIN_BASE   0   // GPIO 0-7 作为数据总线
#define NUM_DATA_PINS   8
#define BUFFER_SIZE     256

// 输出数据缓冲区
uint8_t output_buffer[BUFFER_SIZE];

// 使用 DMA 将缓冲区数据快速输出到 GPIO
void dma_gpio_burst_output(void) {
    // 配置 GPIO 0-7 为输出
    for (int i = 0; i < NUM_DATA_PINS; i++) {
        gpio_init(DATA_PIN_BASE + i);
        gpio_set_dir(DATA_PIN_BASE + i, GPIO_OUT);
    }

    // 填充测试数据
    for (int i = 0; i < BUFFER_SIZE; i++) {
        output_buffer[i] = i;
    }

    // 使用 DMA 将数据写入 GPIO 输出寄存器
    // GPIO 输出寄存器地址: SIO_BASE + 0x014 (GPIO_OUT)
    int channel = dma_claim_unused_channel(true);

    dma_channel_config config = dma_channel_get_default_config(channel);
    channel_config_set_transfer_data_size(&config, DMA_SIZE_8);
    channel_config_set_read_increment(&config, true);
    channel_config_set_write_increment(&config, false);  // 固定写入地址

    dma_channel_configure(
        channel,
        &config,
        (volatile void*)(SIO_BASE + SIO_GPIO_OUT_OFFSET),  // GPIO 输出寄存器
        output_buffer,
        BUFFER_SIZE,
        true
    );

    // 等待完成
    dma_channel_wait_for_finish_blocking(channel);

    dma_channel_unclaim(channel);

    printf("DMA GPIO 突发输出完成: %d 字节 @ %d MHz 等效\n",
           BUFFER_SIZE, (int)(clock_get_hz(clk_sys) / 1000000));
}
```

---

## 10. 多核编程

### 10.1 双核通信机制

```c
/**
 * @file multicore_advanced.c
 * @brief 多核高级编程示例
 */

#include "pico/stdlib.h"
#include "pico/multicore.h"
#include "hardware/irq.h"
#include <stdio.h>

// FIFO 消息类型
typedef enum {
    MSG_TEMPERATURE = 1,
    MSG_PRESSURE,
    MSG_COMMAND,
    MSG_ACK
} message_type_t;

// 共享数据（使用互斥锁保护）
typedef struct {
    mutex_t lock;
    float temperature;
    float pressure;
    uint32_t sample_count;
    bool data_ready;
} shared_data_t;

shared_data_t shared_data;

// Core 1 入口函数
void core1_entry(void) {
    printf("Core 1 启动\n");

    // 配置 Core 1 中断
    multicore_fifo_clear_irq();
    irq_set_exclusive_handler(SIO_IRQ_PROC1, core1_fifo_handler);
    irq_set_enabled(SIO_IRQ_PROC1, true);

    while (1) {
        // 模拟传感器读取
        float temp = 25.0f + (float)(rand() % 100) / 10.0f;
        float pressure = 1013.25f + (float)(rand() % 50) / 10.0f;

        // 使用互斥锁更新共享数据
        mutex_enter_blocking(&shared_data.lock);
        shared_data.temperature = temp;
        shared_data.pressure = pressure;
        shared_data.sample_count++;
        shared_data.data_ready = true;
        mutex_exit(&shared_data.lock);

        // 发送通知到 Core 0
        multicore_fifo_push_blocking(MSG_TEMPERATURE);

        sleep_ms(1000);
    }
}

// Core 1 FIFO 中断处理
void core1_fifo_handler(void) {
    uint32_t msg = multicore_fifo_pop_blocking();

    switch (msg) {
        case MSG_COMMAND:
            printf("Core 1: 收到命令\n");
            multicore_fifo_push_blocking(MSG_ACK);
            break;
    }

    multicore_fifo_clear_irq();
}

// 安全的双核数据访问
void safe_data_access_example(void) {
    mutex_init(&shared_data.lock);
    shared_data.data_ready = false;

    multicore_launch_core1(core1_entry);

    while (1) {
        // 等待数据就绪
        bool ready = false;
        while (!ready) {
            mutex_enter_blocking(&shared_data.lock);
            ready = shared_data.data_ready;
            mutex_exit(&shared_data.lock);
            sleep_ms(10);
        }

        // 读取数据
        mutex_enter_blocking(&shared_data.lock);
        float temp = shared_data.temperature;
        float pressure = shared_data.pressure;
        uint32_t count = shared_data.sample_count;
        shared_data.data_ready = false;
        mutex_exit(&shared_data.lock);

        printf("样本 #%lu: 温度=%.1f°C, 压力=%.2f hPa\n",
               count, temp, pressure);
    }
}

// 使用事件组进行多核同步
typedef volatile uint32_t event_group_t;
event_group_t core_events = 0;

#define EVENT_SENSOR_READY  (1 << 0)
#define EVENT_WIFI_CONNECTED (1 << 1)
#define EVENT_DATA_SENT     (1 << 2)

void set_event(event_group_t* events, uint32_t event) {
    uint32_t save = save_and_disable_interrupts();
    *events |= event;
    restore_interrupts(save);
}

void clear_event(event_group_t* events, uint32_t event) {
    uint32_t save = save_and_disable_interrupts();
    *events &= ~event;
    restore_interrupts(save);
}

bool wait_for_events(event_group_t* events, uint32_t wait_mask,
                     uint32_t* received, uint32_t timeout_ms) {
    absolute_time_t timeout = make_timeout_time_ms(timeout_ms);

    while (1) {
        uint32_t current;
        uint32_t save = save_and_disable_interrupts();
        current = *events;
        restore_interrupts(save);

        if (current & wait_mask) {
            *received = current & wait_mask;
            return true;
        }

        if (absolute_time_diff_us(get_absolute_time(), timeout) <= 0) {
            return false;
        }

        sleep_ms(1);
    }
}

// Core 1 任务处理
void core1_task_processor(void) {
    printf("Core 1: 任务处理器启动\n");

    while (1) {
        uint32_t received;
        if (wait_for_events(&core_events,
                           EVENT_SENSOR_READY | EVENT_WIFI_CONNECTED,
                           &received, 1000)) {

            if (received & EVENT_SENSOR_READY) {
                printf("Core 1: 处理传感器数据\n");
                clear_event(&core_events, EVENT_SENSOR_READY);
            }

            if (received & EVENT_WIFI_CONNECTED) {
                printf("Core 1: WiFi 已连接，启动数据传输\n");
                clear_event(&core_events, EVENT_WIFI_CONNECTED);
            }
        }
    }
}

int main(void) {
    stdio_init_all();
    sleep_ms(2000);

    printf("\n=== 多核高级编程 ===\n\n");

    multicore_launch_core1(core1_task_processor);

    // 模拟传感器就绪
    sleep_ms(500);
    set_event(&core_events, EVENT_SENSOR_READY);

    sleep_ms(1000);
    set_event(&core_events, EVENT_WIFI_CONNECTED);

    while (1) {
        sleep_ms(1000);
    }
}
```

### 10.2 多核负载均衡

```c
/**
 * @file multicore_load_balance.c
 * @brief 多核负载均衡示例 - FFT 并行计算
 */

#include "pico/stdlib.h"
#include "pico/multicore.h"
#include <math.h>
#include <complex.h>

#define FFT_SIZE        1024
#define NUM_CORES       2

// FFT 缓冲区
double complex fft_input[FFT_SIZE];
double complex fft_output[FFT_SIZE];

// 核心完成标志
volatile bool core_complete[NUM_CORES] = {false};

// 复数 FFT 旋转因子
double complex twiddle_factors[FFT_SIZE / 2];

void init_twiddle_factors(void) {
    for (int k = 0; k < FFT_SIZE / 2; k++) {
        double angle = -2.0 * M_PI * k / FFT_SIZE;
        twiddle_factors[k] = cos(angle) + I * sin(angle);
    }
}

// 位反转置换
unsigned int bit_reverse(unsigned int x, int bits) {
    unsigned int y = 0;
    for (int i = 0; i < bits; i++) {
        y = (y << 1) | (x & 1);
        x >>= 1;
    }
    return y;
}

// Core 1 FFT 计算任务
void core1_fft_task(void) {
    int start = FFT_SIZE / NUM_CORES;
    int end = FFT_SIZE;

    // 蝴蝶运算 (后半部分)
    for (int len = 2; len <= FFT_SIZE; len <<= 1) {
        int half_len = len >> 1;
        int step = FFT_SIZE / len;

        for (int i = start; i < end; i += len) {
            for (int j = 0; j < half_len; j++) {
                int idx = i + j;
                double complex t = twiddle_factors[j * step] * fft_output[idx + half_len];
                double complex u = fft_output[idx];
                fft_output[idx] = u + t;
                fft_output[idx + half_len] = u - t;
            }
        }

        // 等待 Core 0 完成当前阶段
        multicore_fifo_push_blocking(1);
        multicore_fifo_pop_blocking();
    }

    core_complete[1] = true;
}

// 并行 FFT
void parallel_fft(void) {
    // 初始化输入数据
    for (int i = 0; i < FFT_SIZE; i++) {
        fft_input[i] = sin(2.0 * M_PI * i / FFT_SIZE);
    }

    // 位反转重排序
    int bits = (int)log2(FFT_SIZE);
    for (int i = 0; i < FFT_SIZE; i++) {
        int rev = bit_reverse(i, bits);
        fft_output[rev] = fft_input[i];
    }

    // 启动 Core 1
    multicore_launch_core1(core1_fft_task);

    // Core 0 处理前半部分
    int start = 0;
    int end = FFT_SIZE / NUM_CORES;

    for (int len = 2; len <= FFT_SIZE; len <<= 1) {
        int half_len = len >> 1;
        int step = FFT_SIZE / len;

        for (int i = start; i < end; i += len) {
            for (int j = 0; j < half_len; j++) {
                int idx = i + j;
                double complex t = twiddle_factors[j * step] * fft_output[idx + half_len];
                double complex u = fft_output[idx];
                fft_output[idx] = u + t;
                fft_output[idx + half_len] = u - t;
            }
        }

        // 同步 Core 1
        multicore_fifo_pop_blocking();
        multicore_fifo_push_blocking(1);
    }

    // 等待 Core 1 完成
    while (!core_complete[1]) {
        tight_loop_contents();
    }

    printf("并行 FFT 完成\n");
}

int main(void) {
    stdio_init_all();
    sleep_ms(2000);

    printf("\n=== 多核负载均衡 (FFT) ===\n\n");

    init_twiddle_factors();
    parallel_fft();

    // 打印部分结果
    printf("FFT 结果 (前 8 点):\n");
    for (int i = 0; i < 8; i++) {
        printf("X[%d] = %.4f + %.4fi\n", i,
               creal(fft_output[i]), cimag(fft_output[i]));
    }

    while (1) tight_loop_contents();
}
```

---

## 11. 实际项目：PID电机控制器

### 11.1 完整 PID 电机控制系统

```c
/**
 * @file pid_motor_controller.c
 * @brief 完整的 PID 直流电机控制器
 *
 * 功能特性：
 * - PID 速度闭环控制
 * - 编码器反馈
 * - PWM 驱动
 * - USB 串口调参
 * - 实时数据输出
 */

#include "pico/stdlib.h"
#include "pico/multicore.h"
#include "hardware/pwm.h"
#include "hardware/gpio.h"
#include "hardware/timer.h"
#include "hardware/irq.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>

// ============= 硬件引脚定义 =============
// 电机驱动 (L298N 或类似)
#define MOTOR_PWM_PIN       16  // ENA
#define MOTOR_DIR1_PIN      17  // IN1
#define MOTOR_DIR2_PIN      18  // IN2

// 编码器 (增量式)
#define ENCODER_A_PIN       19
#define ENCODER_B_PIN       20

// 状态 LED
#define LED_PIN             25

// ============= 配置常量 =============
#define PWM_FREQUENCY       20000       // 20kHz PWM
#define PWM_RESOLUTION      1000        // 1000 级分辨率
#define ENCODER_PPR         360         // 编码器每转脉冲数
#define SAMPLE_RATE_HZ      100         // 控制频率 100Hz
#define SAMPLE_PERIOD_US    (1000000 / SAMPLE_RATE_HZ)

// ============= 数据结构 =============

/**
 * @brief PID 控制器结构体
 */
typedef struct {
    float kp;           // 比例系数
    float ki;           // 积分系数
    float kd;           // 微分系数

    float setpoint;     // 目标值
    float input;        // 当前输入
    float output;       // 输出值

    float integral;     // 积分累积
    float last_error;   // 上次误差
    float last_input;   // 上次输入 (用于微分)

    float output_min;   // 输出限制最小值
    float output_max;   // 输出限制最大值

    float integral_min; // 积分限制最小值 (抗饱和)
    float integral_max; // 积分限制最大值

    bool enabled;       // 控制器使能
    uint32_t sample_time_us;  // 采样周期
} pid_controller_t;

/**
 * @brief 电机状态结构体
 */
typedef struct {
    // 编码器
    volatile int32_t encoder_count;
    volatile int32_t last_encoder_count;
    int32_t delta_counts;

    // 速度计算
    float speed_rpm;            // 当前转速 (RPM)
    float speed_filtered;       // 滤波后转速
    float speed_target_rpm;     // 目标转速

    // PWM 输出
    int16_t pwm_duty;           // -1000 到 +1000
    uint pwm_slice;
    uint pwm_channel;

    // 方向
    bool direction_forward;

    // 统计
    uint32_t total_pulses;
} motor_state_t;

/**
 * @brief 系统状态
 */
typedef struct {
    bool running;
    bool debug_mode;
    uint32_t loop_count;
    uint32_t last_print_time;
    float cpu_load_percent;
} system_state_t;

// ============= 全局变量 =============
pid_controller_t g_pid;
motor_state_t g_motor;
system_state_t g_system;

// ============= 函数声明 =============
void pid_init(pid_controller_t* pid, float kp, float ki, float kd);
float pid_compute(pid_controller_t* pid, float input);
void pid_set_setpoint(pid_controller_t* pid, float setpoint);
void pid_set_tunings(pid_controller_t* pid, float kp, float ki, float kd);
void pid_set_output_limits(pid_controller_t* pid, float min, float max);

void motor_init(void);
void motor_set_pwm(int16_t pwm);
void motor_update_speed(void);
void encoder_isr(uint gpio, uint32_t events);

void control_loop(void);
void timer_irq_handler(void);
void handle_serial_command(const char* cmd);
void print_status(void);

// ============= PID 控制器实现 =============

void pid_init(pid_controller_t* pid, float kp, float ki, float kd) {
    pid->kp = kp;
    pid->ki = ki;
    pid->kd = kd;

    pid->setpoint = 0;
    pid->input = 0;
    pid->output = 0;

    pid->integral = 0;
    pid->last_error = 0;
    pid->last_input = 0;

    pid->output_min = -PWM_RESOLUTION;
    pid->output_max = PWM_RESOLUTION;
    pid->integral_min = -PWM_RESOLUTION / 2;
    pid->integral_max = PWM_RESOLUTION / 2;

    pid->enabled = false;
    pid->sample_time_us = SAMPLE_PERIOD_US;
}

void pid_set_tunings(pid_controller_t* pid, float kp, float ki, float kd) {
    // 根据采样时间调整积分和微分系数
    float sample_time_sec = pid->sample_time_us / 1000000.0f;
    pid->kp = kp;
    pid->ki = ki * sample_time_sec;
    pid->kd = kd / sample_time_sec;
}

void pid_set_output_limits(pid_controller_t* pid, float min, float max) {
    pid->output_min = min;
    pid->output_max = max;

    // 限制积分
    if (pid->integral_max > max) pid->integral_max = max;
    if (pid->integral_min < min) pid->integral_min = min;
}

void pid_set_setpoint(pid_controller_t* pid, float setpoint) {
    pid->setpoint = setpoint;
}

float pid_compute(pid_controller_t* pid, float input) {
    if (!pid->enabled) return 0;

    pid->input = input;

    // 计算误差
    float error = pid->setpoint - input;

    // 比例项
    float p_term = pid->kp * error;

    // 积分项
    pid->integral += error;
    // 积分限幅 (抗饱和)
    if (pid->integral > pid->integral_max) pid->integral = pid->integral_max;
    if (pid->integral < pid->integral_min) pid->integral = pid->integral_min;
    float i_term = pid->ki * pid->integral;

    // 微分项 (使用输入变化而非误差变化，避免设定值突变引起的冲击)
    float d_term = pid->kd * (pid->last_input - input);
    pid->last_input = input;
    pid->last_error = error;

    // 计算输出
    float output = p_term + i_term + d_term;

    // 输出限幅
    if (output > pid->output_max) output = pid->output_max;
    if (output < pid->output_min) output = pid->output_min;

    pid->output = output;
    return output;
}

// ============= 电机控制实现 =============

void motor_init(void) {
    memset(&g_motor, 0, sizeof(g_motor));

    // 初始化方向引脚
    gpio_init(MOTOR_DIR1_PIN);
    gpio_init(MOTOR_DIR2_PIN);
    gpio_set_dir(MOTOR_DIR1_PIN, GPIO_OUT);
    gpio_set_dir(MOTOR_DIR2_PIN, GPIO_OUT);
    gpio_put(MOTOR_DIR1_PIN, 0);
    gpio_put(MOTOR_DIR2_PIN, 0);

    // 初始化 PWM
    gpio_set_function(MOTOR_PWM_PIN, GPIO_FUNC_PWM);
    g_motor.pwm_slice = pwm_gpio_to_slice_num(MOTOR_PWM_PIN);
    g_motor.pwm_channel = pwm_gpio_to_channel(MOTOR_PWM_PIN);

    pwm_config pwm_cfg = pwm_get_default_config();
    float clkdiv = (float)clock_get_hz(clk_sys) / (PWM_FREQUENCY * PWM_RESOLUTION);
    pwm_config_set_clkdiv(&pwm_cfg, clkdiv);
    pwm_config_set_wrap(&pwm_cfg, PWM_RESOLUTION - 1);
    pwm_init(g_motor.pwm_slice, &pwm_cfg, true);
    pwm_set_chan_level(g_motor.pwm_slice, g_motor.pwm_channel, 0);

    // 初始化编码器
    gpio_init(ENCODER_A_PIN);
    gpio_init(ENCODER_B_PIN);
    gpio_set_dir(ENCODER_A_PIN, GPIO_IN);
    gpio_set_dir(ENCODER_B_PIN, GPIO_IN);
    gpio_pull_up(ENCODER_A_PIN);
    gpio_pull_up(ENCODER_B_PIN);

    // 配置编码器中断
    gpio_set_irq_enabled_with_callback(
        ENCODER_A_PIN,
        GPIO_IRQ_EDGE_RISE | GPIO_IRQ_EDGE_FALL,
        true,
        encoder_isr
    );
}

void motor_set_pwm(int16_t pwm) {
    // 限制范围
    if (pwm > PWM_RESOLUTION) pwm = PWM_RESOLUTION;
    if (pwm < -PWM_RESOLUTION) pwm = -PWM_RESOLUTION;

    g_motor.pwm_duty = pwm;

    if (pwm > 0) {
        // 正转
        gpio_put(MOTOR_DIR1_PIN, 1);
        gpio_put(MOTOR_DIR2_PIN, 0);
        pwm_set_chan_level(g_motor.pwm_slice, g_motor.pwm_channel, pwm);
        g_motor.direction_forward = true;
    } else if (pwm < 0) {
        // 反转
        gpio_put(MOTOR_DIR1_PIN, 0);
        gpio_put(MOTOR_DIR2_PIN, 1);
        pwm_set_chan_level(g_motor.pwm_slice, g_motor.pwm_channel, -pwm);
        g_motor.direction_forward = false;
    } else {
        // 停止
        gpio_put(MOTOR_DIR1_PIN, 0);
        gpio_put(MOTOR_DIR2_PIN, 0);
        pwm_set_chan_level(g_motor.pwm_slice, g_motor.pwm_channel, 0);
    }
}

void encoder_isr(uint gpio, uint32_t events) {
    (void)gpio;
    (void)events;

    bool a = gpio_get(ENCODER_A_PIN);
    bool b = gpio_get(ENCODER_B_PIN);

    // 判断方向
    if (a == b) {
        g_motor.encoder_count++;
    } else {
        g_motor.encoder_count--;
    }

    g_motor.total_pulses++;
}

void motor_update_speed(void) {
    // 计算脉冲变化
    int32_t current_count = g_motor.encoder_count;
    g_motor.delta_counts = current_count - g_motor.last_encoder_count;
    g_motor.last_encoder_count = current_count;

    // 计算 RPM
    // RPM = (delta_counts / PPR) * (60 * SAMPLE_RATE)
    float counts_per_sec = g_motor.delta_counts * SAMPLE_RATE_HZ;
    g_motor.speed_rpm = (counts_per_sec / ENCODER_PPR) * 60.0f;

    // 低通滤波
    float alpha = 0.3f;  // 滤波系数
    g_motor.speed_filtered = alpha * g_motor.speed_rpm +
                             (1 - alpha) * g_motor.speed_filtered;
}

// ============= 控制循环 =============

void control_loop(void) {
    // 更新速度
    motor_update_speed();

    // PID 计算
    float pid_output = pid_compute(&g_pid, g_motor.speed_filtered);

    // 设置电机 PWM
    motor_set_pwm((int16_t)pid_output);

    // 更新系统状态
    g_system.loop_count++;
}

void timer_irq_handler(void) {
    // 清除中断标志
    hw_clear_bits(&timer_hw->intr, 1u << 0);

    // 设置下一次中断
    uint64_t next_alarm = timer_hw->timerawl + SAMPLE_PERIOD_US;
    timer_hw->alarm[0] = (uint32_t)next_alarm;

    // 执行控制循环
    control_loop();
}

void init_control_timer(void) {
    // 配置定时器中断
    hw_set_bits(&timer_hw->inte, 1u << 0);
    irq_set_exclusive_handler(TIMER_IRQ_0, timer_irq_handler);
    irq_set_enabled(TIMER_IRQ_0, true);

    // 设置首次中断
    uint64_t next_alarm = timer_hw->timerawl + SAMPLE_PERIOD_US;
    timer_hw->alarm[0] = (uint32_t)next_alarm;
}

// ============= 串口通信 =============

void handle_serial_command(const char* cmd) {
    char buffer[64];
    strncpy(buffer, cmd, sizeof(buffer) - 1);
    buffer[sizeof(buffer) - 1] = '\0';

    // 去除换行符
    char* newline = strchr(buffer, '\n');
    if (newline) *newline = '\0';
    newline = strchr(buffer, '\r');
    if (newline) *newline = '\0';

    // 解析命令
    char* token = strtok(buffer, " ");
    if (!token) return;

    if (strcmp(token, "set") == 0) {
        // set speed <rpm>
        token = strtok(NULL, " ");
        if (token && strcmp(token, "speed") == 0) {
            token = strtok(NULL, " ");
            if (token) {
                float speed = atof(token);
                g_motor.speed_target_rpm = speed;
                pid_set_setpoint(&g_pid, speed);
                printf("OK: 目标速度设置为 %.1f RPM\n", speed);
            }
        }
    }
    else if (strcmp(token, "tune") == 0) {
        // tune kp ki kd
        float kp = atof(strtok(NULL, " "));
        float ki = atof(strtok(NULL, " "));
        float kd = atof(strtok(NULL, " "));
        pid_set_tunings(&g_pid, kp, ki, kd);
        printf("OK: PID 参数设置为 Kp=%.4f Ki=%.4f Kd=%.4f\n", kp, ki, kd);
    }
    else if (strcmp(token, "start") == 0) {
        g_pid.enabled = true;
        printf("OK: PID 控制器已启动\n");
    }
    else if (strcmp(token, "stop") == 0) {
        g_pid.enabled = false;
        motor_set_pwm(0);
        printf("OK: PID 控制器已停止\n");
    }
    else if (strcmp(token, "reset") == 0) {
        g_pid.integral = 0;
        g_pid.last_error = 0;
        printf("OK: PID 积分已重置\n");
    }
    else if (strcmp(token, "debug") == 0) {
        g_system.debug_mode = !g_system.debug_mode;
        printf("OK: 调试模式 %s\n", g_system.debug_mode ? "开启" : "关闭");
    }
    else if (strcmp(token, "status") == 0) {
        print_status();
    }
    else if (strcmp(token, "help") == 0) {
        printf("\n=== PID 电机控制器命令 ===\n");
        printf("set speed <rpm>    - 设置目标速度\n");
        printf("tune <kp> <ki> <kd> - 设置 PID 参数\n");
        printf("start              - 启动控制器\n");
        printf("stop               - 停止控制器\n");
        printf("reset              - 重置积分\n");
        printf("debug              - 切换调试模式\n");
        printf("status             - 显示状态\n");
        printf("help               - 显示帮助\n\n");
    }
}

void print_status(void) {
    printf("\n=== 系统状态 ===\n");
    printf("目标速度: %.1f RPM\n", g_motor.speed_target_rpm);
    printf("当前速度: %.1f RPM (滤波: %.1f)\n",
           g_motor.speed_rpm, g_motor.speed_filtered);
    printf("PWM 输出: %d\n", g_motor.pwm_duty);
    printf("编码器: %ld\n", (long)g_motor.encoder_count);
    printf("PID: Kp=%.4f Ki=%.4f Kd=%.4f\n",
           g_pid.kp, g_pid.ki, g_pid.kd);
    printf("PID 输出: %.1f\n", g_pid.output);
    printf("积分: %.1f\n", g_pid.integral);
    printf("控制器状态: %s\n", g_pid.enabled ? "运行中" : "停止");
    printf("===============\n\n");
}

void debug_output(void) {
    // CSV 格式输出，便于绘图
    uint32_t now = time_us_32();
    printf("%u,%.1f,%.1f,%d,%.1f,%.1f\n",
           now,
           g_motor.speed_target_rpm,
           g_motor.speed_filtered,
           g_motor.pwm_duty,
           g_pid.output,
           g_pid.integral);
}

// ============= 主程序 =============

int main(void) {
    stdio_init_all();

    // 初始化状态 LED
    gpio_init(LED_PIN);
    gpio_set_dir(LED_PIN, GPIO_OUT);

    // 等待 USB 连接
    sleep_ms(2000);

    printf("\n");
    printf("╔═══════════════════════════════════════╗\n");
    printf("║     PID 直流电机控制器 v1.0           ║\n");
    printf("║     RP2040 @ 133MHz                   ║\n");
    printf("╚═══════════════════════════════════════╝\n");
    printf("\n输入 'help' 查看命令列表\n\n");

    // 初始化系统
    memset(&g_system, 0, sizeof(g_system));
    g_system.running = true;

    // 初始化 PID 控制器
    pid_init(&g_pid, 2.0f, 0.5f, 0.1f);
    pid_set_output_limits(&g_pid, -PWM_RESOLUTION, PWM_RESOLUTION);

    // 初始化电机
    motor_init();

    // 启动定时器控制
    init_control_timer();

    // 主循环
    char cmd_buffer[64];
    int cmd_pos = 0;
    absolute_time_t last_debug_time = get_absolute_time();

    while (g_system.running) {
        // 处理串口输入
        int c = getchar_timeout_us(0);
        if (c != PICO_ERROR_TIMEOUT) {
            if (c == '\n' || c == '\r') {
                if (cmd_pos > 0) {
                    cmd_buffer[cmd_pos] = '\0';
                    handle_serial_command(cmd_buffer);
                    cmd_pos = 0;
                }
            } else if (cmd_pos < sizeof(cmd_buffer) - 1) {
                cmd_buffer[cmd_pos++] = (char)c;
            }
        }

        // 调试输出
        if (g_system.debug_mode) {
            if (absolute_time_diff_us(last_debug_time, get_absolute_time()) > 10000) {
                last_debug_time = get_absolute_time();
                debug_output();
            }
        }

        // 状态 LED 闪烁
        static absolute_time_t last_blink = {0};
        if (absolute_time_diff_us(last_blink, get_absolute_time()) > 500000) {
            last_blink = get_absolute_time();
            gpio_xor_mask(1u << LED_PIN);
        }
    }

    // 清理
    motor_set_pwm(0);
    printf("系统已停止\n");

    return 0;
}
```

### 11.2 CMakeLists.txt (PID 项目)

```cmake
cmake_minimum_required(VERSION 3.13)
include(pico_sdk_import.cmake)

project(pid_motor_controller C CXX ASM)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

pico_sdk_init()

add_executable(pid_motor_controller
    pid_motor_controller.c
)

target_link_libraries(pid_motor_controller
    pico_stdlib
    pico_multicore
    hardware_pwm
    hardware_gpio
    hardware_timer
    hardware_irq
    m
)

pico_enable_stdio_usb(pid_motor_controller 1)
pico_enable_stdio_uart(pid_motor_controller 0)

pico_add_extra_outputs(pid_motor_controller)
```



---

## 12. USB设备模式

### 12.1 USB CDC (虚拟串口)

```c
/**
 * @file usb_cdc_example.c
 * @brief USB CDC 虚拟串口示例
 */

#include "pico/stdlib.h"
#include "tusb.h"
#include <stdio.h>

// 在 tusb_config.h 中启用 CDC
// #define CFG_TUD_CDC 1

void cdc_task(void);

int main(void) {
    // 初始化板载 LED
    gpio_init(25);
    gpio_set_dir(25, GPIO_OUT);

    // 初始化 TinyUSB
    tusb_init();

    while (1) {
        tud_task();  // 处理 USB 事件
        cdc_task();

        // LED 心跳
        static uint32_t last_blink = 0;
        if (board_millis() - last_blink > 500) {
            last_blink = board_millis();
            gpio_xor_mask(1u << 25);
        }
    }
}

// CDC 接收处理
void cdc_task(void) {
    // 连接状态检查
    if (!tud_cdc_connected()) return;

    // 读取数据
    if (tud_cdc_available()) {
        uint8_t buf[64];
        uint32_t count = tud_cdc_read(buf, sizeof(buf));

        // 回显
        tud_cdc_write(buf, count);
        tud_cdc_write_flush();

        // 处理命令
        for (uint32_t i = 0; i < count; i++) {
            if (buf[i] == '1') {
                gpio_put(25, 1);
            } else if (buf[i] == '0') {
                gpio_put(25, 0);
            }
        }
    }
}

// TinyUSB 回调函数
void tud_cdc_rx_cb(uint8_t itf) {
    (void)itf;
    // 数据接收回调
}

void tud_cdc_tx_complete_cb(uint8_t itf) {
    (void)itf;
    // 发送完成回调
}
```

### 12.2 USB HID 设备 (键盘/鼠标)

```c
/**
 * @file usb_hid_example.c
 * @brief USB HID 键盘和鼠标示例
 */

#include "pico/stdlib.h"
#include "tusb.h"
#include "class/hid/hid_device.h"

// HID 报告描述符
uint8_t const desc_hid_report[] = {
    // 键盘报告
    TUD_HID_REPORT_DESC_KEYBOARD(),

    // 鼠标报告
    TUD_HID_REPORT_DESC_MOUSE()
};

// HID 接口配置
enum {
    ITF_NUM_HID,
    ITF_NUM_TOTAL
};

// 设备描述符
static tusb_desc_device_t const desc_device = {
    .bLength = sizeof(tusb_desc_device_t),
    .bDescriptorType = TUSB_DESC_DEVICE,
    .bcdUSB = 0x0200,
    .bDeviceClass = 0x00,
    .bDeviceSubClass = 0x00,
    .bDeviceProtocol = 0x00,
    .bMaxPacketSize0 = CFG_TUD_ENDPOINT0_SIZE,
    .idVendor = 0xCafe,
    .idProduct = 0x0001,
    .bcdDevice = 0x0100,
    .iManufacturer = 0x01,
    .iProduct = 0x02,
    .iSerialNumber = 0x03,
    .bNumConfigurations = 0x01
};

// 键盘按键发送
void hid_keyboard_send_key(uint8_t keycode) {
    uint8_t keycode_array[6] = {0};
    keycode_array[0] = keycode;

    // 按下
    tud_hid_keyboard_report(0, 0, keycode_array);
    sleep_ms(10);

    // 释放
    tud_hid_keyboard_report(0, 0, NULL);
}

// 鼠标移动
void hid_mouse_move(int8_t x, int8_t y, uint8_t buttons) {
    tud_hid_mouse_report(1, buttons, x, y, 0, 0);
}

// 字符串发送
void hid_send_string(const char* str) {
    while (*str) {
        uint8_t keycode = 0;
        uint8_t modifier = 0;

        // 简单的 ASCII 到键码转换
        char c = *str;
        if (c >= 'a' && c <= 'z') {
            keycode = HID_KEY_A + (c - 'a');
        } else if (c >= 'A' && c <= 'Z') {
            keycode = HID_KEY_A + (c - 'A');
            modifier = KEYBOARD_MODIFIER_LEFTSHIFT;
        } else if (c >= '0' && c <= '9') {
            keycode = HID_KEY_0 + (c - '0');
            if (c == '0') keycode = HID_KEY_0;
        } else if (c == ' ') {
            keycode = HID_KEY_SPACE;
        }

        if (keycode != 0) {
            uint8_t keycode_array[6] = {keycode, 0, 0, 0, 0, 0};
            tud_hid_keyboard_report(0, modifier, keycode_array);
            sleep_ms(10);
            tud_hid_keyboard_report(0, 0, NULL);
            sleep_ms(10);
        }

        str++;
    }
}

int main(void) {
    gpio_init(25);
    gpio_set_dir(25, GPIO_OUT);

    tusb_init();

    uint32_t count = 0;

    while (1) {
        tud_task();

        // 等待 USB 就绪
        if (!tud_hid_ready()) continue;

        // 每 3 秒发送一个测试字符串
        static uint32_t last_send = 0;
        if (board_millis() - last_send > 3000) {
            last_send = board_millis();

            gpio_put(25, 1);

            // 发送 "Hello from Pico!"
            hid_send_string("Hello from Pico! ");

            // 发送数字计数
            char num_str[16];
            sprintf(num_str, "%lu", count++);
            hid_send_string(num_str);

            // 发送回车
            hid_keyboard_send_key(HID_KEY_RETURN);

            gpio_put(25, 0);
        }
    }
}

// TinyUSB HID 回调
uint16_t tud_hid_get_report_cb(uint8_t itf, uint8_t report_id,
                                hid_report_type_t report_type,
                                uint8_t* buffer, uint16_t reqlen) {
    (void)itf;
    (void)report_id;
    (void)report_type;
    (void)buffer;
    (void)reqlen;
    return 0;
}

void tud_hid_set_report_cb(uint8_t itf, uint8_t report_id,
                           hid_report_type_t report_type,
                           uint8_t const* buffer, uint16_t bufsize) {
    (void)itf;
    (void)report_id;
    (void)report_type;
    (void)buffer;
    (void)bufsize;
}
```

### 12.3 USB 大容量存储设备 (MSC)

```c
/**
 * @file usb_msc_example.c
 * @brief USB MSC (大容量存储) 示例 - 虚拟 U 盘
 */

#include "pico/stdlib.h"
#include "tusb.h"
#include "class/msc/msc_device.h"

// 虚拟磁盘大小 (64KB)
#define DISK_BLOCK_NUM  128
#define DISK_BLOCK_SIZE 512

static uint8_t msc_disk[DISK_BLOCK_NUM][DISK_BLOCK_SIZE];

// SCSI 命令响应
static bool ejected = false;

// 初始化虚拟磁盘
void msc_disk_init(void) {
    // 清零磁盘
    memset(msc_disk, 0, sizeof(msc_disk));

    // 创建简单的 MBR 和 FAT12 文件系统
    // 这里简化为空磁盘，实际应用需要格式化
}

// MSC 回调函数
int32_t tud_msc_read10_cb(uint8_t lun, uint32_t lba,
                          uint32_t offset, void* buffer,
                          uint32_t bufsize) {
    (void)lun;

    if (lba >= DISK_BLOCK_NUM) return -1;

    uint8_t const* addr = msc_disk[lba] + offset;
    memcpy(buffer, addr, bufsize);

    return (int32_t)bufsize;
}

int32_t tud_msc_write10_cb(uint8_t lun, uint32_t lba,
                           uint32_t offset, uint8_t const* buffer,
                           uint32_t bufsize) {
    (void)lun;

    if (lba >= DISK_BLOCK_NUM) return -1;

    uint8_t* addr = msc_disk[lba] + offset;
    memcpy(addr, buffer, bufsize);

    return (int32_t)bufsize;
}

void tud_msc_inquiry_cb(uint8_t lun, uint8_t vendor_id[8],
                        uint8_t product_id[16],
                        uint8_t product_rev[4]) {
    (void)lun;

    const char vid[] = "Pico";
    const char pid[] = "Mass Storage";
    const char rev[] = "1.0";

    memcpy(vendor_id, vid, strlen(vid));
    memcpy(product_id, pid, strlen(pid));
    memcpy(product_rev, rev, strlen(rev));
}

bool tud_msc_test_unit_ready_cb(uint8_t lun) {
    (void)lun;

    if (ejected) {
        tud_msc_set_sense(lun, SCSI_SENSE_NOT_READY,
                          0x3a, 0x00);
        return false;
    }

    return true;
}

void tud_msc_capacity_cb(uint8_t lun, uint32_t* block_count,
                         uint16_t* block_size) {
    (void)lun;

    *block_count = DISK_BLOCK_NUM;
    *block_size = DISK_BLOCK_SIZE;
}

bool tud_msc_start_stop_cb(uint8_t lun, uint8_t power_condition,
                           bool start, bool load_eject) {
    (void)lun;
    (void)power_condition;

    if (load_eject) {
        if (start) {
            // 加载磁盘
            msc_disk_init();
            ejected = false;
        } else {
            // 弹出磁盘
            ejected = true;
        }
    }

    return true;
}

int32_t tud_msc_scsi_cb(uint8_t lun, uint8_t const scsi_cmd[16],
                        void* buffer, uint16_t bufsize) {
    (void)lun;
    (void)buffer;
    (void)bufsize;

    // 处理 SCSI 命令
    switch (scsi_cmd[0]) {
        case SCSI_CMD_PREVENT_ALLOW_MEDIUM_REMOVAL:
            return 0;
        default:
            tud_msc_set_sense(lun, SCSI_SENSE_ILLEGAL_REQUEST,
                              0x20, 0x00);
            return -1;
    }
}

int main(void) {
    gpio_init(25);
    gpio_set_dir(25, GPIO_OUT);

    msc_disk_init();
    tusb_init();

    while (1) {
        tud_task();

        // 连接状态指示
        static bool last_mounted = false;
        bool mounted = tud_mounted();

        if (mounted != last_mounted) {
            last_mounted = mounted;
            gpio_put(25, mounted);
        }
    }
}
```

### 12.4 USB 复合设备 (CDC + MSC)

```cmake
# tusb_config.h 配置示例
# 启用多个设备类

#ifndef _TUSB_CONFIG_H_
#define _TUSB_CONFIG_H_

#define CFG_TUSB_RHPORT0_MODE   (OPT_MODE_DEVICE)

// 启用 CDC
#define CFG_TUD_CDC             1
#define CFG_TUD_CDC_RX_BUFSIZE  256
#define CFG_TUD_CDC_TX_BUFSIZE  256

// 启用 MSC
#define CFG_TUD_MSC             1
#define CFG_TUD_MSC_BUFSIZE     512

// 启用 HID
#define CFG_TUD_HID             1
#define CFG_TUD_HID_EP_BUFSIZE  64

// 总端点数
#define CFG_TUD_ENDPOINT0_SIZE  64

#endif
```

```c
/**
 * @file usb_composite.c
 * @brief USB 复合设备示例 (CDC + MSC)
 */

#include "pico/stdlib.h"
#include "tusb.h"
#include <stdio.h>

// USB 描述符字符串
static char const* string_desc_arr[] = {
    (const char[]) {0x09, 0x04},  // 语言
    "Raspberry Pi",               // 厂商
    "Pico Composite Device",      // 产品
    "123456",                     // 序列号
    "Pico Serial",                // CDC 接口
    "Pico Storage"                // MSC 接口
};

void cdc_task(void) {
    if (!tud_cdc_connected()) return;

    if (tud_cdc_available()) {
        uint8_t buf[64];
        uint32_t count = tud_cdc_read(buf, sizeof(buf));

        // 处理命令
        if (count > 0) {
            switch (buf[0]) {
                case 'i':
                    tud_cdc_write_str("Pico USB Composite Device\r\n");
                    tud_cdc_write_str("Commands: i=info, r=reset disk\r\n");
                    break;

                case 'r':
                    tud_cdc_write_str("Resetting virtual disk...\r\n");
                    // 重置磁盘逻辑
                    break;

                default:
                    tud_cdc_write(buf, count);
                    break;
            }
            tud_cdc_write_flush();
        }
    }
}

int main(void) {
    stdio_init_all();

    printf("USB Composite Device Starting...\n");

    tusb_init();

    while (1) {
        tud_task();
        cdc_task();
    }
}

// 设备描述符回调
uint8_t const* tud_descriptor_device_cb(void) {
    static tusb_desc_device_t desc_device = {
        .bLength = sizeof(tusb_desc_device_t),
        .bDescriptorType = TUSB_DESC_DEVICE,
        .bcdUSB = 0x0200,
        .bDeviceClass = TUSB_CLASS_MISC,
        .bDeviceSubClass = MISC_SUBCLASS_COMMON,
        .bDeviceProtocol = MISC_PROTOCOL_IAD,
        .bMaxPacketSize0 = CFG_TUD_ENDPOINT0_SIZE,
        .idVendor = 0x2E8A,  // Raspberry Pi
        .idProduct = 0x000A,
        .bcdDevice = 0x0100,
        .iManufacturer = 0x01,
        .iProduct = 0x02,
        .iSerialNumber = 0x03,
        .bNumConfigurations = 0x01
    };
    return (uint8_t const*) &desc_device;
}

// 字符串描述符回调
uint16_t const* tud_descriptor_string_cb(uint8_t index, uint16_t langid) {
    (void)langid;

    static uint16_t desc_str[32];

    if (index == 0) {
        // 语言描述符
        desc_str[0] = (TUSB_DESC_STRING << 8) | (2 * 2 + 2);
        desc_str[1] = 0x0409;  // English
        return desc_str;
    }

    if (index >= sizeof(string_desc_arr)/sizeof(string_desc_arr[0])) {
        return NULL;
    }

    const char* str = string_desc_arr[index];
    uint8_t len = strlen(str);

    desc_str[0] = (TUSB_DESC_STRING << 8) | (2 * len + 2);
    for (uint8_t i = 0; i < len; i++) {
        desc_str[1 + i] = str[i];
    }

    return desc_str;
}
```

---

## 13. 调试技巧

### 13.1 串口调试输出

```c
/**
 * @file debug_utils.c
 * @brief 调试工具集
 */

#include "pico/stdlib.h"
#include <stdio.h>
#include <stdarg.h>
#include <string.h>

// 调试级别
typedef enum {
    DEBUG_LEVEL_NONE = 0,
    DEBUG_LEVEL_ERROR,
    DEBUG_LEVEL_WARN,
    DEBUG_LEVEL_INFO,
    DEBUG_LEVEL_DEBUG,
    DEBUG_LEVEL_VERBOSE
} debug_level_t;

static debug_level_t current_level = DEBUG_LEVEL_DEBUG;

void debug_printf(debug_level_t level, const char* file, int line,
                  const char* func, const char* fmt, ...) {
    if (level > current_level) return;

    const char* level_str[] = {"", "ERR", "WRN", "INF", "DBG", "VRB"};

    // 打印前缀
    printf("[%s] %s:%d %s: ", level_str[level], file, line, func);

    // 打印消息
    va_list args;
    va_start(args, fmt);
    vprintf(fmt, args);
    va_end(args);

    printf("\n");
}

#define DEBUG_ERROR(...) \
    debug_printf(DEBUG_LEVEL_ERROR, __FILE__, __LINE__, __func__, __VA_ARGS__)
#define DEBUG_WARN(...) \
    debug_printf(DEBUG_LEVEL_WARN, __FILE__, __LINE__, __func__, __VA_ARGS__)
#define DEBUG_INFO(...) \
    debug_printf(DEBUG_LEVEL_INFO, __FILE__, __LINE__, __func__, __VA_ARGS__)
#define DEBUG_DEBUG(...) \
    debug_printf(DEBUG_LEVEL_DEBUG, __FILE__, __LINE__, __func__, __VA_ARGS__)

// 内存转储
void hexdump(const void* data, size_t size) {
    const uint8_t* p = (const uint8_t*)data;

    for (size_t i = 0; i < size; i += 16) {
        printf("%04X: ", (unsigned int)i);

        // 十六进制
        for (size_t j = 0; j < 16 && i + j < size; j++) {
            printf("%02X ", p[i + j]);
        }

        // 对齐
        for (size_t j = (size - i < 16) ? size - i : 16; j < 16; j++) {
            printf("   ");
        }

        printf(" ");

        // ASCII
        for (size_t j = 0; j < 16 && i + j < size; j++) {
            uint8_t c = p[i + j];
            printf("%c", (c >= 32 && c < 127) ? c : '.');
        }

        printf("\n");
    }
}

// 性能计时器
typedef struct {
    absolute_time_t start;
    const char* name;
} perf_timer_t;

perf_timer_t perf_start(const char* name) {
    perf_timer_t timer = {
        .start = get_absolute_time(),
        .name = name
    };
    return timer;
}

void perf_end(perf_timer_t* timer) {
    int64_t elapsed = absolute_time_diff_us(timer->start, get_absolute_time());
    DEBUG_DEBUG("%s: %lld us", timer->name, elapsed);
}

#define PERF_SCOPE(name) \
    perf_timer_t _perf_timer __attribute__((cleanup(perf_end_cleanup))) = perf_start(name)

static void perf_end_cleanup(perf_timer_t* timer) {
    perf_end(timer);
}
```

### 13.2 使用 SWD 调试

```bash
# 使用 OpenOCD 进行调试

# 1. 安装 OpenOCD (如果未安装)
sudo apt install openocd

# 2. 启动 OpenOCD 服务器
openocd -f interface/raspberrypi-swd.cfg -f target/rp2040.cfg

# 3. 使用 GDB 连接
gdb-multiarch your_program.elf

# 在 GDB 中:
(gdb) target remote localhost:3333
(gdb) load
(gdb) monitor reset init
(gdb) break main
(gdb) continue
```

### 13.3 断言和错误处理

```c
/**
 * @file assert_utils.h
 * @brief 断言和错误处理
 */

#ifndef ASSERT_UTILS_H
#define ASSERT_UTILS_H

#include "pico/stdlib.h"

// 自定义断言
#define ASSERT(cond) \
    do { \
        if (!(cond)) { \
            assertion_failed(#cond, __FILE__, __LINE__, __func__); \
        } \
    } while(0)

#define ASSERT_MSG(cond, msg) \
    do { \
        if (!(cond)) { \
            assertion_failed_msg(#cond, msg, __FILE__, __LINE__, __func__); \
        } \
    } while(0)

// 断言失败处理
static inline void assertion_failed(const char* condition,
                                     const char* file, int line,
                                     const char* func) {
    printf("\n\n!!! ASSERTION FAILED !!!\n");
    printf("Condition: %s\n", condition);
    printf("File: %s\n", file);
    printf("Line: %d\n", line);
    printf("Function: %s\n", func);
    printf("\n");

    // 闪烁 LED 指示错误
    gpio_init(25);
    gpio_set_dir(25, GPIO_OUT);

    while (1) {
        gpio_put(25, 1);
        sleep_ms(100);
        gpio_put(25, 0);
        sleep_ms(100);
    }
}

static inline void assertion_failed_msg(const char* condition,
                                         const char* msg,
                                         const char* file, int line,
                                         const char* func) {
    printf("\n\n!!! ASSERTION FAILED !!!\n");
    printf("Condition: %s\n", condition);
    printf("Message: %s\n", msg);
    printf("File: %s\n", file);
    printf("Line: %d\n", line);
    printf("Function: %s\n", func);
    printf("\n");

    while (1) {
        tight_loop_contents();
    }
}

// 错误代码
typedef enum {
    ERR_OK = 0,
    ERR_INVALID_PARAM,
    ERR_OUT_OF_MEMORY,
    ERR_TIMEOUT,
    ERR_NOT_FOUND,
    ERR_IO,
    ERR_BUSY,
    ERR_UNKNOWN
} error_code_t;

// 检查返回码
#define CHECK_ERROR(code) \
    do { \
        error_code_t _err = (code); \
        if (_err != ERR_OK) { \
            printf("Error at %s:%d: %d\n", __FILE__, __LINE__, _err); \
            return _err; \
        } \
    } while(0)

#endif // ASSERT_UTILS_H
```

### 13.4 运行时统计

```c
/**
 * @file runtime_stats.c
 * @brief 运行时统计信息
 */

#include "pico/stdlib.h"
#include <stdio.h>

void print_runtime_stats(void) {
    printf("\n=== 运行时统计 ===\n");

    // 系统时钟
    uint32_t sys_clk = clock_get_hz(clk_sys);
    printf("系统时钟: %u MHz\n", sys_clk / 1000000);

    // USB 时钟
    uint32_t usb_clk = clock_get_hz(clk_usb);
    printf("USB 时钟: %u MHz\n", usb_clk / 1000000);

    // ADC 时钟
    uint32_t adc_clk = clock_get_hz(clk_adc);
    printf("ADC 时钟: %u MHz\n", adc_clk / 1000000);

    // RTC 时钟
    uint32_t rtc_clk = clock_get_hz(clk_rtc);
    printf("RTC 时钟: %u Hz\n", rtc_clk);

    // 温度
    adc_select_input(4);
    uint16_t raw = adc_read();
    float voltage = raw * 3.3f / 4096.0f;
    float temp = 27.0f - (voltage - 0.706f) / 0.001721f;
    printf("芯片温度: %.1f C\n", temp);

    // 电压
    printf("\n电压调节器:\n");
    printf("  DVDD: 由内部调节\n");
    printf("  USB_VDD: 3.3V\n");
    printf("  ADC_AVDD: 3.3V\n");

    printf("=================\n\n");
}
```

---

## 14. 参考资料

### 官方资源

| 资源 | 链接 |
|------|------|
| Pico C/C++ SDK 文档 | <https://www.raspberrypi.com/documentation/pico-sdk/> |
| Pico 数据手册 | <https://datasheets.raspberrypi.com/pico/pico-datasheet.pdf> |
| RP2040 数据手册 | <https://datasheets.raspberrypi.com/rp2040/rp2040-datasheet.pdf> |
| Pico SDK GitHub | <https://github.com/raspberrypi/pico-sdk> |
| Pico 示例项目 | <https://github.com/raspberrypi/pico-examples> |

### 社区资源

| 资源 | 描述 |
|------|------|
| Pico-PIO-USB | 使用 PIO 实现的 USB 主机 |
| Arduino-Pico | Arduino 框架支持 |
| MicroPython | Python 固件 |
| CircuitPython | Adafruit 的 Python 固件 |
| FreeRTOS-Pico | FreeRTOS 移植 |

### 推荐书籍

1. **《Get Started with MicroPython on Raspberry Pi Pico》**
   - Raspberry Pi 官方出版

2. **《RP2040 Assembly Language Programming》**
   - ARM 汇编语言指南

3. **《Programming the Raspberry Pi Pico》**
   - 综合编程指南

### 在线工具

- **PIO 汇编器**: <https://wokwi.com/tools/pioasm>
- **GPIO 配置工具**: 使用 Pico SDK 头文件
- **时钟配置**: SDK 自动处理

---

## 附录 A: 常用命令速查表

### 编译命令

```bash
# 创建构建目录
mkdir build && cd build

# 配置 (Release 模式)
cmake -DCMAKE_BUILD_TYPE=Release ..

# 配置 (Debug 模式)
cmake -DCMAKE_BUILD_TYPE=Debug ..

# 编译
make -j4

# 清理
make clean
```

### 烧录命令

```bash
# 使用 picotool
picotool load firmware.uf2
picotool reboot

# 或手动复制到 RPI-RP2 驱动器
cp firmware.uf2 /media/$USER/RPI-RP2/
```

### 调试命令

```bash
# 串口监视
minicom -D /dev/ttyACM0 -b 115200

# 或
screen /dev/ttyACM0 115200

# 使用 picoprobe 调试
openocd -f interface/picoprobe.cfg -f target/rp2040.cfg
```

---

## 文档信息

- **版本**: 1.0
- **最后更新**: 2026-03-17
- **适用 SDK 版本**: Pico SDK 2.0+
- **适用硬件**: Raspberry Pi Pico / Pico W / Pico 2

---

*本指南由 AI 助手生成，涵盖了 Raspberry Pi Pico 开发的各个方面。如有错误或建议，欢迎反馈。*


---

## 深入理解

### 核心原理

深入探讨技术原理和实现细节。

### 实践应用

- 应用场景1
- 应用场景2
- 应用场景3

### 最佳实践

1. 理解基础概念
2. 掌握核心机制
3. 应用到实际项目

---

> **最后更新**: 2026-03-21  
> **维护者**: AI Code Review
