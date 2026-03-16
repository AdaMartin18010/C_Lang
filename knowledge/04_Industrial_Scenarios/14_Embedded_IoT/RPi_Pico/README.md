# 树莓派 Pico 开发平台 (Raspberry Pi Pico)

> 基于 RP2040 双核 Cortex-M0+ 微控制器的高性能、低成本嵌入式开发平台

---

## 📋 平台概述

Raspberry Pi Pico 是树莓派基金会推出的首款微控制器开发板，搭载自研的 RP2040 芯片。该芯片采用独特的双核 Cortex-M0+ 架构，配合可编程 I/O（PIO）子系统，为嵌入式开发提供了强大的硬件基础和灵活的开发体验。

### RP2040 芯片规格

| 规格项 | 参数值 | 说明 |
|:-------|:-------|:-----|
| **CPU** | 双核 Cortex-M0+ | 133MHz 主频，可超频至 250MHz+ |
| **SRAM** | 264KB | 6 个独立的 4KB 存储体 |
| **Flash** | 2MB (QSPI) | 板载，可外扩至 16MB |
| **GPIO** | 30 个 | 其中 4 个支持 ADC |
| **ADC** | 3 通道 | 12 位分辨率 |
| **PIO** | 8 个状态机 | 2 个独立 PIO 块 |
| **USB** | USB 1.1 | 支持主机和设备模式 |
| **工作电压** | 1.8-3.3V | 灵活的 I/O 电压 |

### 相关开发板

| 开发板 | 特点 | 适用场景 |
|:-------|:-----|:---------|
| Raspberry Pi Pico | 基础款，2MB Flash | 学习、原型开发 |
| Pico W | 集成 CYW43439 WiFi/BT | IoT 应用 |
| Pico H | 带预焊接排针 | 快速原型 |
| Pico WH | WiFi + 排针 | 完整的 IoT 开发 |

---

## ⭐ 核心特性

### 1. 双核架构优势

```
┌─────────────────────────────────────────────────────────────┐
│                      RP2040 双核架构                         │
├─────────────────────────────────────────────────────────────┤
│  ┌──────────────┐    ┌──────────────┐                      │
│  │   Core 0     │◄──►│   Core 1     │   ← 双核通信        │
│  │  (主核心)    │ FIFO │  (从核心)   │    (FIFO/SIO)       │
│  │  @133MHz     │    │  @133MHz     │                      │
│  └──────┬───────┘    └──────┬───────┘                      │
│         │                   │                               │
│  ┌──────▼───────────────────▼──────┐                       │
│  │        AHB/APB 总线矩阵         │                       │
│  └──────┬───────────────────┬──────┘                       │
│         │                   │                               │
│  ┌──────▼──────┐   ┌────────▼────────┐                     │
│  │  264KB SRAM │   │  8x PIO 状态机   │                     │
│  │  (6 banks)  │   │  (2 blocks)     │                     │
│  └─────────────┘   └─────────────────┘                     │
└─────────────────────────────────────────────────────────────┘
```

### 2. 可编程 I/O (PIO)

PIO 是 RP2040 最具特色的功能，允许开发者自定义硬件协议：

- **8 个独立状态机**：2 个 PIO 块，每块 4 个状态机
- **32 条指令空间**：每个状态机独立的程序存储
- **专用 FIFO**：4 字 TX FIFO + 4 字 RX FIFO
- **灵活时钟分频**：支持任意速率协议

**典型应用**：

- WS2812B LED 灯带驱动
- DHT22 温湿度传感器时序
- 自定义 SPI/I2C 变体
- 步进电机控制

### 3. 丰富的外设接口

| 外设 | 数量 | 特性 |
|:-----|:-----|:-----|
| UART | 2 | 支持流控 |
| SPI | 2 | 主机/从机模式 |
| I2C | 2 | 标准/快速/快速+ |
| PWM | 8 通道 | 16 位分辨率 |
| USB | 1 | CDC/HID/MSC 设备 |
| ADC | 3 通道 | 12 位 + 温度传感器 |
| DMA | 12 通道 | 支持链式传输 |

---

## 🎯 应用场景

### 1. 实时控制系统

- 电机控制（PID、步进、伺服）
- 机器人运动控制
- CNC 控制器
- 无人机飞控

### 2. 协议转换网关

- UART 到 USB 转换器
- CAN 总线桥接
- 工业协议转换
- 传感器数据采集

### 3. LED 显示控制

- WS2812B 灯带控制
- LED 矩阵驱动
- DMX512 控制器
- 自定义显示协议

### 4. 音频应用

- I2S 音频播放
- USB 音频设备
- 合成器/鼓机
- 音频效果器

### 5. 教育与原型

- 嵌入式系统教学
- 快速原型验证
- 创客项目
- 算法验证平台

---

## 🛠️ 开发环境要求

### 硬件要求

| 物品 | 规格要求 | 说明 |
|:-----|:---------|:-----|
| Raspberry Pi Pico | 任意版本 | 推荐 Pico W 用于 IoT |
| Micro-USB 线 | 数据传输型 | 用于供电和编程 |
| 调试器（可选） | Picoprobe 或 J-Link | SWD 调试 |
| 面包板 | 标准尺寸 | 原型搭建 |

### 软件环境

#### Windows 环境搭建

```powershell
# 1. 安装 Chocolatey (如未安装)
Set-ExecutionPolicy Bypass -Scope Process -Force
iex ((New-Object System.Net.WebClient).DownloadString('https://chocolatey.org/install.ps1'))

# 2. 安装工具链
choco install git cmake mingw make python

# 3. 设置 Pico SDK
$env:PICO_SDK_PATH = "C:\pico\pico-sdk"
[Environment]::SetEnvironmentVariable("PICO_SDK_PATH", "C:\pico\pico-sdk", "User")

# 4. 克隆 SDK
git clone https://github.com/raspberrypi/pico-sdk.git C:\pico\pico-sdk
cd C:\pico\pico-sdk
git submodule update --init
```

#### Linux 环境搭建

```bash
# Ubuntu/Debian
sudo apt update
sudo apt install cmake gcc-arm-none-eabi libnewlib-arm-none-eabi \
    libstdc++-arm-none-eabi-newlib build-essential python3 git

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

#### macOS 环境搭建

```bash
# 安装依赖
brew install cmake arm-none-eabi-gcc

# 克隆 SDK
git clone https://github.com/raspberrypi/pico-sdk.git ~/pico/pico-sdk
cd ~/pico/pico-sdk
git submodule update --init

# 设置环境变量
echo 'export PICO_SDK_PATH=$HOME/pico/pico-sdk' >> ~/.zshrc
source ~/.zshrc
```

### 推荐的开发工具

| 工具 | 用途 | 推荐度 |
|:-----|:-----|:------:|
| VS Code + CMake Tools | 主要 IDE | ⭐⭐⭐⭐⭐ |
| CLion | JetBrains IDE | ⭐⭐⭐⭐ |
| Arduino IDE | 入门使用 | ⭐⭐⭐ |
| Thonny | MicroPython 开发 | ⭐⭐⭐⭐ |

---

## 📚 与主文档的关系

本目录是 [14_Embedded_IoT](../README.md) 双核实时控制专题的核心内容，与相邻目录形成技术互补。

### 文档架构

```
RPi_Pico/
├── README.md                      # 本文件 - 平台概览
├── 01_Pico_Complete_Guide.md      # 完整开发指南（900+ 行代码）
└── examples/                      # 示例代码
    ├── gpio_basics/               # GPIO 基础
    ├── pio_led/                   # PIO LED 控制
    ├── motor_control/             # 电机控制
    └── multicore_demo/            # 多核编程
```

### 技术关联

| 本目录内容 | 关联文档 | 技术对比/补充 |
|:-----------|:---------|:--------------|
| 双核编程 | [../ESP32/01_ESP32_Complete_Guide.md](../ESP32/01_ESP32_Complete_Guide.md) | ESP32 FreeRTOS vs Pico 裸机双核 |
| 内存优化 | [../Memory_Constrained/01_Memory_Constrained_Design.md](../Memory_Constrained/01_Memory_Constrained_Design.md) | 264KB SRAM 优化策略 |
| 协议实现 | [../IoT_Protocols/01_IoT_Protocols_Implementation.md](../IoT_Protocols/01_IoT_Protocols_Implementation.md) | 协议栈移植指南 |
| Linux 对比 | [../RaspberryPi/01_RaspberryPi_Embedded_Linux.md](../RaspberryPi/01_RaspberryPi_Embedded_Linux.md) | MCU vs Linux 方案选型 |

---

## 🚀 快速开始指引

### 步骤 1: 创建第一个项目

```bash
# 创建项目目录
mkdir ~/pico/my_project && cd ~/pico/my_project

# 创建 CMakeLists.txt
cat > CMakeLists.txt << 'EOF'
cmake_minimum_required(VERSION 3.13)
include($ENV{PICO_SDK_PATH}/external/pico_sdk_import.cmake)
project(my_project C CXX ASM)
set(CMAKE_C_STANDARD 11)
pico_sdk_init()

add_executable(my_project
    main.c
)

target_link_libraries(my_project
    pico_stdlib
    hardware_gpio
)

pico_enable_stdio_usb(my_project 1)
pico_enable_stdio_uart(my_project 0)
pico_add_extra_outputs(my_project)
EOF
```

### 步骤 2: 编写主程序

```c
#include "pico/stdlib.h"

#define LED_PIN 25

int main(void) {
    // 初始化标准 I/O
    stdio_init_all();

    // 配置 GPIO
    gpio_init(LED_PIN);
    gpio_set_dir(LED_PIN, GPIO_OUT);

    printf("Pico 启动成功!\n");

    while (1) {
        gpio_put(LED_PIN, 1);
        printf("LED ON\n");
        sleep_ms(500);

        gpio_put(LED_PIN, 0);
        printf("LED OFF\n");
        sleep_ms(500);
    }

    return 0;
}
```

### 步骤 3: 编译与烧录

```bash
# 创建构建目录
mkdir build && cd build

# 配置和编译
cmake ..
make -j4

# 烧录方式 1: USB 大容量存储模式
# 1. 按住 BOOTSEL 按钮
# 2. 连接 USB
# 3. 松开 BOOTSEL 按钮
# 4. 复制 .uf2 文件到出现的磁盘
cp my_project.uf2 /media/$USER/RPI-RP2/

# 烧录方式 2: 使用 picotool
picotool load my_project.uf2
picotool reboot
```

### 步骤 4: 多核编程示例

```c
#include "pico/multicore.h"
#include "pico/stdlib.h"

// Core 1 入口函数
void core1_entry() {
    while (1) {
        // Core 1 处理任务
        sleep_ms(1000);
    }
}

int main() {
    stdio_init_all();

    // 启动 Core 1
    multicore_launch_core1(core1_entry);

    // Core 0 主循环
    while (1) {
        // Core 0 处理任务
        sleep_ms(500);
    }
}
```

---

## ❓ 常见问题

### 编译问题

| 问题 | 原因 | 解决方案 |
|:-----|:-----|:---------|
| `PICO_SDK_PATH not set` | 环境变量未配置 | 设置 PICO_SDK_PATH 环境变量 |
| `arm-none-eabi-gcc not found` | 工具链未安装 | 安装 arm-none-eabi-gcc |
| CMake 配置失败 | 子模块未初始化 | 运行 `git submodule update --init` |

### 烧录问题

| 问题 | 原因 | 解决方案 |
|:-----|:-----|:---------|
| 无法进入 BOOTSEL 模式 | 按键时机不对 | 先按住 BOOTSEL，再插 USB |
| 没有显示磁盘 | USB 线问题 | 更换数据线（需要数据+供电线） |
| 程序无法运行 | 编译目标错误 | 检查 CMakeLists.txt 配置 |

### 调试问题

| 问题 | 原因 | 解决方案 |
|:-----|:-----|:---------|
| 串口无输出 | USB CDC 未启用 | 添加 `pico_enable_stdio_usb(target 1)` |
| 程序卡死 | 堆栈溢出 | 检查递归调用，增大堆栈 |
| 双核崩溃 | 资源冲突 | 使用互斥锁保护共享资源 |

### 性能优化

1. **超频使用**

   ```c
   #include "hardware/clocks.h"

   // 设置系统时钟到 250MHz
   set_sys_clock_khz(250000, true);
   ```

2. **PIO 调试技巧**

   ```bash
   # 查看 PIO 程序状态
   picotool info --all
   ```

3. **内存使用监控**

   ```c
   extern char __stack_bottom;
   printf("Stack usage: %d bytes\n", (int)(&__stack_bottom - (char*)__builtin_frame_address(0)));
   ```

---

## 📖 推荐学习路径

### 初级路径

1. 阅读 [01_Pico_Complete_Guide.md](./01_Pico_Complete_Guide.md) 第 1-4 章
2. 完成 GPIO 控制示例
3. 学习 UART/SPI/I2C 通信
4. 尝试 ADC 采样

### 中级路径

1. 掌握 PWM 电机控制
2. 学习 PIO 可编程 I/O
3. 实现 DMA 数据传输
4. 双核并行编程

### 高级路径

1. 自定义 PIO 协议
2. USB 设备开发（HID/CDC/MSC）
3. 实时性优化
4. 复杂项目实战（PID 控制器等）

---

## 🔗 返回上级

[← 返回 14_Embedded_IoT 目录](../README.md)

---

## 📞 支持与资源

- **官方文档**: <https://datasheets.raspberrypi.com/pico/raspberry-pi-pico-c-sdk.pdf>
- **GitHub**: <https://github.com/raspberrypi/pico-sdk>
- **论坛**: <https://forums.raspberrypi.com/viewforum.php?f=143>
- **MicroPython**: <https://micropython.org/download/rp2-pico/>

---

> **最后更新**: 2026-03-17
> **维护者**: C_Lang Knowledge Base Team
> **版本**: Pico SDK 1.5+
