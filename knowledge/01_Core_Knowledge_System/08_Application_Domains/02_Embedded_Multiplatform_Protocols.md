---

## 🔗 文档关联

### 核心关联
| 文档 | 关系类型 | 说明 |
|:-----|:---------|:-----|
| [内存管理](../../../01_Core_Knowledge_System/02_Core_Layer/02_Memory_Management.md) | 核心关联 | 内存管理基础 |
| [指针深度](../../../01_Core_Knowledge_System/02_Core_Layer/01_Pointer_Depth.md) | 核心关联 | 指针深度基础 |
| [并发编程](../../../03_System_Technology_Domains/14_Concurrency_Parallelism/README.md) | 核心关联 | 并发编程基础 |
| [数据类型](../../../01_Core_Knowledge_System/01_Basic_Layer/02_Data_Type_System.md) | 核心关联 | 数据类型基础 |
| [数组与指针](../../../01_Core_Knowledge_System/02_Core_Layer/05_Arrays_Pointers.md) | 核心关联 | 数组与指针基础 |

### 扩展阅读
| 文档 | 关系类型 | 说明 |
|:-----|:---------|:-----|
| [软件工程](../../../01_Core_Knowledge_System/05_Engineering_Layer/README.md) | 核心关联 | 软件工程基础 |
| [形式语义](../../../02_Formal_Semantics_and_Physics/README.md) | 核心关联 | 形式语义基础 |
| [系统技术](../../../03_System_Technology_Domains/README.md) | 核心关联 | 系统技术基础 |
| [工业场景](../../../04_Industrial_Scenarios/README.md) | 核心关联 | 工业场景基础 |
| [思维表征](../../../06_Thinking_Representation/README.md) | 核心关联 | 思维表征基础 |
﻿# C语言嵌入式多平台开发与协议栈深度实现

> **层级定位**: 01 Core Knowledge System / 08 Application Domains
> **主题**: 多平台开发(STM32/ESP32/RP2040)、协议栈深度实现、高级技巧
> **难度级别**: L4 分析 → L5 专家
> **预估学习时间**: 25-35 小时

---

## 📋 目录

- [📋 目录](#-目录)
- [📑 目录](#-目录-1)
- [一、多平台架构对比](#一多平台架构对比)
  - [1.1 三大平台特性矩阵](#11-三大平台特性矩阵)
  - [1.2 启动流程对比](#12-启动流程对比)
  - [1.3 内存布局差异](#13-内存布局差异)
- [二、ESP32系列深度开发](#二esp32系列深度开发)
  - [2.1 ESP32系列芯片选型](#21-esp32系列芯片选型)
  - [2.2 ESP32 GPIO与中断矩阵](#22-esp32-gpio与中断矩阵)
  - [2.3 ESP32 FreeRTOS集成](#23-esp32-freertos集成)
  - [2.4 ESP32 WiFi与网络](#24-esp32-wifi与网络)
- [三、树莓派Pico(RP2040)开发](#三树莓派picorp2040开发)
  - [3.1 RP2040独特架构](#31-rp2040独特架构)
  - [3.2 PIO可编程I/O](#32-pio可编程io)
  - [3.3 RP2040 DMA链式传输](#33-rp2040-dma链式传输)
- [四、GPIO高级技巧与语义](#四gpio高级技巧与语义)
  - [4.1 GPIO模式深度解析](#41-gpio模式深度解析)
  - [4.2 中断与事件系统](#42-中断与事件系统)
- [五、I2C协议栈完整实现](#五i2c协议栈完整实现)
  - [5.1 I2C协议深度解析](#51-i2c协议深度解析)
  - [5.2 软件I2C实现（位 bang）](#52-软件i2c实现位-bang)
  - [5.3 I2C设备驱动示例（BMP280气压传感器）](#53-i2c设备驱动示例bmp280气压传感器)
- [六、SPI协议栈完整实现](#六spi协议栈完整实现)
  - [6.1 SPI协议深度解析](#61-spi协议深度解析)
  - [6.2 软件SPI实现](#62-软件spi实现)
  - [6.3 SPI驱动W25Q Flash示例](#63-spi驱动w25q-flash示例)
- [七、UART与串口通信](#七uart与串口通信)
  - [7.1 UART协议深度解析](#71-uart协议深度解析)
  - [7.2 软件UART实现](#72-软件uart实现)
  - [7.3 DMA UART高效接收](#73-dma-uart高效接收)
- [八、DMA与高效数据传输](#八dma与高效数据传输)
  - [8.1 DMA架构对比](#81-dma架构对比)
  - [8.2 链式DMA (RP2040)](#82-链式dma-rp2040)
- [九、WiFi/BLE网络栈](#九wifible网络栈)
  - [9.1 ESP32 WiFi事件驱动架构](#91-esp32-wifi事件驱动架构)
- [十、多核与并发编程](#十多核与并发编程)
  - [10.1 RP2040双核通信](#101-rp2040双核通信)
- [十一、电源管理与低功耗](#十一电源管理与低功耗)
  - [11.1 ESP32深度睡眠](#111-esp32深度睡眠)
  - [11.2 RP2040低功耗](#112-rp2040低功耗)
- [十二、实战项目：传感器网关](#十二实战项目传感器网关)
- [深入理解](#深入理解)
  - [技术原理](#技术原理)
  - [实践指南](#实践指南)
  - [相关资源](#相关资源)

---


---

## 📑 目录

- [📋 目录](#-目录)
- [📑 目录](#-目录-1)
- [一、多平台架构对比](#一多平台架构对比)
  - [1.1 三大平台特性矩阵](#11-三大平台特性矩阵)
  - [1.2 启动流程对比](#12-启动流程对比)
  - [1.3 内存布局差异](#13-内存布局差异)
- [二、ESP32系列深度开发](#二esp32系列深度开发)
  - [2.1 ESP32系列芯片选型](#21-esp32系列芯片选型)
  - [2.2 ESP32 GPIO与中断矩阵](#22-esp32-gpio与中断矩阵)
  - [2.3 ESP32 FreeRTOS集成](#23-esp32-freertos集成)
  - [2.4 ESP32 WiFi与网络](#24-esp32-wifi与网络)
- [三、树莓派Pico(RP2040)开发](#三树莓派picorp2040开发)
  - [3.1 RP2040独特架构](#31-rp2040独特架构)
  - [3.2 PIO可编程I/O](#32-pio可编程io)
  - [3.3 RP2040 DMA链式传输](#33-rp2040-dma链式传输)
- [四、GPIO高级技巧与语义](#四gpio高级技巧与语义)
  - [4.1 GPIO模式深度解析](#41-gpio模式深度解析)
  - [4.2 中断与事件系统](#42-中断与事件系统)
- [五、I2C协议栈完整实现](#五i2c协议栈完整实现)
  - [5.1 I2C协议深度解析](#51-i2c协议深度解析)
  - [5.2 软件I2C实现（位 bang）](#52-软件i2c实现位-bang)
  - [5.3 I2C设备驱动示例（BMP280气压传感器）](#53-i2c设备驱动示例bmp280气压传感器)
- [六、SPI协议栈完整实现](#六spi协议栈完整实现)
  - [6.1 SPI协议深度解析](#61-spi协议深度解析)
  - [6.2 软件SPI实现](#62-软件spi实现)
  - [6.3 SPI驱动W25Q Flash示例](#63-spi驱动w25q-flash示例)
- [七、UART与串口通信](#七uart与串口通信)
  - [7.1 UART协议深度解析](#71-uart协议深度解析)
  - [7.2 软件UART实现](#72-软件uart实现)
  - [7.3 DMA UART高效接收](#73-dma-uart高效接收)
- [八、DMA与高效数据传输](#八dma与高效数据传输)
  - [8.1 DMA架构对比](#81-dma架构对比)
  - [8.2 链式DMA (RP2040)](#82-链式dma-rp2040)
- [九、WiFi/BLE网络栈](#九wifible网络栈)
  - [9.1 ESP32 WiFi事件驱动架构](#91-esp32-wifi事件驱动架构)
- [十、多核与并发编程](#十多核与并发编程)
  - [10.1 RP2040双核通信](#101-rp2040双核通信)
- [十一、电源管理与低功耗](#十一电源管理与低功耗)
  - [11.1 ESP32深度睡眠](#111-esp32深度睡眠)
  - [11.2 RP2040低功耗](#112-rp2040低功耗)
- [十二、实战项目：传感器网关](#十二实战项目传感器网关)
- [深入理解](#深入理解)
  - [技术原理](#技术原理)
  - [实践指南](#实践指南)
  - [相关资源](#相关资源)


---

## 一、多平台架构对比

### 1.1 三大平台特性矩阵

```
┌─────────────────────────────────────────────────────────────────────────────┐
│                        嵌入式平台特性对比矩阵                                │
├──────────────────┬─────────────────┬─────────────────┬──────────────────────┤
│     特性         │    STM32F4      │   ESP32系列     │    RP2040 (Pico)     │
├──────────────────┼─────────────────┼─────────────────┼──────────────────────┤
│ 核心架构         │ ARM Cortex-M4   │ Xtensa LX6/Dual │ ARM Cortex-M0+ Dual  │
│                  │ (F4) / M7 (H7)  │ / RISC-V (C6)   │                      │
├──────────────────┼─────────────────┼─────────────────┼──────────────────────┤
│ 主频             │ 168-480 MHz     │ 160-240 MHz     │ 133 MHz (可超频)     │
├──────────────────┼─────────────────┼─────────────────┼──────────────────────┤
│ 内存             │ 128KB-1MB SRAM  │ 520KB SRAM      │ 264KB SRAM           │
│                  │ 无内置Flash     │ 无(外接)        │ 无(外接QSPI)         │
├──────────────────┼─────────────────┼─────────────────┼──────────────────────┤
│ 程序存储         │ 外部Flash       │ 外部Flash       │ 外部QSPI Flash       │
│                  │ (1-16MB)        │ (4-16MB)        │ (2-16MB)             │
├──────────────────┼─────────────────┼─────────────────┼──────────────────────┤
│ 特色功能         │ 硬件FPU、       │ WiFi+BLE双模、  │ 可编程I/O (PIO)、    │
│                  │ DSP指令、       │ 超低功耗模式、  │ 双核对称、           │
│                  │ 丰富外设        │ 安全启动        │ 无内部NVM            │
├──────────────────┼─────────────────┼─────────────────┼──────────────────────┤
│ 开发难度         │ ⭐⭐⭐           │ ⭐⭐⭐⭐         │ ⭐⭐⭐                │
│ 生态成熟度       │ ⭐⭐⭐⭐⭐        │ ⭐⭐⭐⭐⭐        │ ⭐⭐⭐⭐               │
│ 成本             │ 中等            │ 低              │ 极低                 │
├──────────────────┼─────────────────┼─────────────────┼──────────────────────┤
│ 适用场景         │ 工业控制、      │ IoT、可穿戴、   │ 教育、快速原型、     │
│                  │ 汽车电子、      │ 智能家居        │ 实时控制             │
│                  │ 高性能DSP       │                 │                      │
└──────────────────┴─────────────────┴─────────────────┴──────────────────────┘
```

### 1.2 启动流程对比

```c
// ============================================================
// 启动流程差异分析
// ============================================================

// ========== STM32启动流程 ==========
// 1. 上电后从0x08000000(Flash)或0x1FFF0000(系统存储器)启动
// 2. 读取栈顶指针(0x08000000)和复位向量(0x08000004)
// 3. 执行启动文件(assembly)初始化栈、复制.data、清零.bss
// 4. 调用SystemInit()配置时钟
// 5. 调用main()

// startup_stm32f4xx.s (简化)
__isr_vector:
    .word _estack          // Top of stack
    .word Reset_Handler    // Reset Handler
    .word NMI_Handler      // NMI Handler
    // ...

Reset_Handler:
    // 复制.data从Flash到RAM
    ldr r0, =_sidata
    ldr r1, =_sdata
    ldr r2, =_edata
    b LoopCopyDataInit
CopyDataInit:
    ldr r3, [r0, #0]
    str r3, [r1, #0]
    adds r0, r0, #4
    adds r1, r1, #4
LoopCopyDataInit:
    cmp r1, r2
    bcc CopyDataInit

    // 清零.bss
    // ...

    // 调用系统初始化
    bl SystemInit

    // 调用main
    bl main
    bx lr

// ========== ESP32启动流程 ==========
// ESP32使用二级引导程序(Bootloader)
// 1. ROM中的第一级引导(不可修改)加载Flash 0x1000的Bootloader
// 2. Bootloader读取分区表，加载应用程序
// 3. 初始化DRAM、配置Cache、设置CPU频率
// 4. 跳转到应用程序入口(app_main)

// ESP32的特殊性: FreeRTOS是默认OS
void app_main(void) {
    // ESP-IDF框架下，main变成了app_main
    // 系统已经初始化完成，包括WiFi/BLE栈

    // 创建任务
    xTaskCreate(sensor_task, "sensor", 4096, NULL, 5, NULL);
    xTaskCreate(wifi_task, "wifi", 8192, NULL, 5, NULL);
}

// ========== RP2040启动流程 ==========
// RP2040从片外QSPI Flash启动
// 1. Bootrom(ROM)执行，检测启动模式(Flash/USB/UART)
// 2. 从Flash 0x10000000加载第二级引导
// 3. 初始化PLL、配置时钟、初始化外设
// 4. 启动第二个核心
// 5. 跳转到main()

// RP2040特有的多核启动
void main(void) {
    // 核心0执行这里

    // 启动核心1
    multicore_launch_core1(core1_entry);

    // 核心0继续执行
    core0_main();
}

void core1_entry(void) {
    // 核心1从这里开始执行
    core1_main();
}
```

### 1.3 内存布局差异

```c
// ============================================================
// 内存映射对比
// ============================================================

// ========== STM32F4内存映射 ==========
// 0x0000 0000 - 0x000F FFFF: 代码区 (别名区，可映射到Flash/SRAM)
// 0x0800 0000 - 0x080F FFFF: Flash (1MB)
// 0x1FFF 0000 - 0x1FFF 77FF: 系统存储器 (Bootloader)
// 0x2000 0000 - 0x2001 FFFF: SRAM1 (128KB)
// 0x2002 0000 - 0x2003 FFFF: SRAM2 (128KB)
// 0x4000 0000 - 0x5FFF FFFF: 外设区
// 0xE000 0000 - 0xE00F FFFF: Cortex-M4内部外设

// 链接脚本关键部分
MEMORY {
    RAM (rwx) : ORIGIN = 0x20000000, LENGTH = 128K
    FLASH (rx) : ORIGIN = 0x08000000, LENGTH = 1024K
}

// ========== ESP32内存映射 ==========
// ESP32使用虚拟内存映射，通过MMU访问外部Flash
// 0x3F40 0000 - 0x3F7F FFFF: 外部Flash缓存 (通过MMU映射)
// 0x3FF8 0000 - 0x3FFF FFFF: DRAM (520KB)
// 0x4000 0000 - 0x400C 1FFF: 内部ROM
// 0x400C 2000 - 0x40BF FFFF: 外部Flash指令缓存

// ESP32内存分配特殊性
// 使用heap_caps分配不同区域的内存
void *ptr = heap_caps_malloc(size, MALLOC_CAP_DMA);  // DMA-capable内存
void *iram = heap_caps_malloc(size, MALLOC_CAP_32BIT); // 32位对齐，可执行

// ========== RP2040内存映射 ==========
// 0x0000 0000 - 0x3FFF FFFF: XIP (Flash映射，通过QSPI)
// 0x2000 0000 - 0x2003 FFFF: SRAM (264KB)
// 0x4000 0000 - 0x4001 3FFF: APB外设
// 0x4001 4000 - 0x4001 7FFF: AHB外设
// 0x4002 0000 - 0x4002 0FFF: SIO (单周期I/O)
// 0xD000 0000 - 0xD000 01FF: Cortex-M0+内部外设

// RP2040的SRAM分为6个bank，支持并行访问
// Bank0-3: 通用
// Bank4-5: 可单独断电（低功耗）
```

---

## 二、ESP32系列深度开发

### 2.1 ESP32系列芯片选型

```
┌────────────────────────────────────────────────────────────────────┐
│                      ESP32系列芯片特性                              │
├──────────────┬──────────┬──────────┬──────────┬────────────────────┤
│    型号      │   内核   │   制程   │  特色    │     适用场景       │
├──────────────┼──────────┼──────────┼──────────┼────────────────────┤
│ ESP32        │ LX6双核  │  40nm    │ 经典款   │ 通用IoT、原型开发  │
├──────────────┼──────────┼──────────┼──────────┼────────────────────┤
│ ESP32-S3     │ LX7双核  │  28nm    │ AI加速   │ 语音唤醒、图像识别 │
│              │          │          │ 向量指令 │                    │
├──────────────┼──────────┼──────────┼──────────┼────────────────────┤
│ ESP32-C3     │ RISC-V   │  22nm    │ 单核、   │ 低成本、低功耗     │
│              │ 单核     │          │  BLE 5.0 │ 简单控制类应用     │
├──────────────┼──────────┼──────────┼──────────┼────────────────────┤
│ ESP32-C6     │ RISC-V   │  22nm    │ WiFi 6   │ 最新协议、低功耗   │
│              │ 双核     │          │ 802.15.4 │ matter协议         │
├──────────────┼──────────┼──────────┼──────────┼────────────────────┤
│ ESP32-H2     │ RISC-V   │  22nm    │ 纯802.15 │ Zigbee、Thread     │
│              │ 单核     │          │  无WiFi  │ 边界路由器         │
└──────────────┴──────────┴──────────┴──────────┴────────────────────┘
```

### 2.2 ESP32 GPIO与中断矩阵

```c
// ============================================================
// ESP32 GPIO架构特殊性
// ESP32有34个GPIO，通过GPIO矩阵可以灵活映射外设到任意引脚
// ============================================================

#include "driver/gpio.h"
#include "esp_intr_alloc.h"

// ESP32 GPIO配置 (比STM32更灵活)
void esp32_gpio_init(void) {
    gpio_config_t io_conf = {
        .pin_bit_mask = (1ULL << GPIO_NUM_18),  // GPIO18
        .mode = GPIO_MODE_OUTPUT,
        .pull_up_en = GPIO_PULLUP_DISABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_DISABLE,
    };
    gpio_config(&io_conf);

    // 设置输出
    gpio_set_level(GPIO_NUM_18, 1);
}

// ESP32特有的GPIO矩阵 - 任意外设可映射到任意GPIO
// 这是ESP32最大的优势之一！
void esp32_gpio_matrix(void) {
    // UART0默认使用GPIO1(TX)和GPIO3(RX)
    // 但可以通过矩阵重新映射到任意GPIO

    // 将UART0 TX映射到GPIO18
    gpio_matrix_out(GPIO_NUM_18, U0TXD_OUT_IDX, false, false);

    // 将UART0 RX映射到GPIO19
    gpio_matrix_in(GPIO_NUM_19, U0RXD_IN_IDX, false);
}

// ESP32中断矩阵 - 灵活的中断路由
void esp32_interrupt_setup(void) {
    // GPIO中断
    gpio_set_intr_type(GPIO_NUM_4, GPIO_INTR_POSEDGE);  // 上升沿触发
    gpio_intr_enable(GPIO_NUM_4);

    // 注册中断处理 (ESP32使用FreeRTOS中断API)
    gpio_install_isr_service(ESP_INTR_FLAG_LEVEL1);
    gpio_isr_handler_add(GPIO_NUM_4, gpio_isr_handler, (void*)4);
}

// ISR处理 (ESP32要求IRAM_ATTR，中断处理放在IRAM)
static void IRAM_ATTR gpio_isr_handler(void* arg) {
    uint32_t gpio_num = (uint32_t)arg;
    // 中断处理必须在IRAM中，不能访问Flash
    // 通常设置标志，在主任务中处理
    gpio_intr_disable(gpio_num);
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;
    // 通知任务...
    portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
}
```

### 2.3 ESP32 FreeRTOS集成

```c
// ============================================================
// ESP32默认运行FreeRTOS，双核对称运行
// 需要理解任务亲和性(Core Affinity)
// ============================================================

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

// 创建绑定到核心0的任务
void task_create_core0(void) {
    xTaskCreatePinnedToCore(
        sensor_task,        // 任务函数
        "sensor",           // 任务名
        4096,               // 栈大小
        NULL,               // 参数
        5,                  // 优先级
        NULL,               // 任务句柄
        0                   // 核心0 (PRO_CPU)
    );
}

// 创建绑定到核心1的任务
void task_create_core1(void) {
    xTaskCreatePinnedToCore(
        wifi_task,          // WiFi任务通常放在核心0
        "wifi",             // 因为WiFi中断在核心0
        8192,
        NULL,
        5,
        NULL,
        1                   // 核心1 (APP_CPU)
    );
}

// 双核任务间通信
#include "freertos/queue.h"

static QueueHandle_t data_queue;

void dual_core_communication(void) {
    // 创建队列
    data_queue = xQueueCreate(10, sizeof(sensor_data_t));

    // 核心0: 传感器采集
    xTaskCreatePinnedToCore(core0_sensor_task, "sensor", 4096, NULL, 5, NULL, 0);

    // 核心1: 数据处理+上传
    xTaskCreatePinnedToCore(core1_process_task, "process", 4096, NULL, 5, NULL, 1);
}

void core0_sensor_task(void *pvParameters) {
    sensor_data_t data;
    while (1) {
        // 读取传感器
        read_sensor(&data);

        // 发送到队列 (不阻塞)
        xQueueSend(data_queue, &data, 0);

        vTaskDelay(pdMS_TO_TICKS(100));
    }
}

void core1_process_task(void *pvParameters) {
    sensor_data_t data;
    while (1) {
        // 接收数据 (阻塞等待)
        if (xQueueReceive(data_queue, &data, portMAX_DELAY)) {
            // 处理并上传
            process_and_upload(&data);
        }
    }
}
```

### 2.4 ESP32 WiFi与网络

```c
// ============================================================
// ESP32 WiFi STA/AP模式
// 事件驱动架构
// ============================================================

#include "esp_wifi.h"
#include "esp_event.h"
#include "nvs_flash.h"

// WiFi事件处理
static void wifi_event_handler(void* arg, esp_event_base_t event_base,
                               int32_t event_id, void* event_data) {
    if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_START) {
        esp_wifi_connect();
    } else if (event_base == WIFI_EVENT &&
               event_id == WIFI_EVENT_STA_DISCONNECTED) {
        // 断线重连
        esp_wifi_connect();
    } else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP) {
        ip_event_got_ip_t* event = (ip_event_got_ip_t*)event_data;
        ESP_LOGI(TAG, "Got IP: " IPSTR, IP2STR(&event->ip_info.ip));
    }
}

void wifi_init_sta(void) {
    // 初始化NVS (存储WiFi配置)
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES ||
        ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);

    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());

    // 创建默认WiFi STA
    esp_netif_create_default_wifi_sta();

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));

    // 注册事件处理
    ESP_ERROR_CHECK(esp_event_handler_register(WIFI_EVENT,
                                               ESP_EVENT_ANY_ID,
                                               &wifi_event_handler, NULL));
    ESP_ERROR_CHECK(esp_event_handler_register(IP_EVENT,
                                               IP_EVENT_STA_GOT_IP,
                                               &wifi_event_handler, NULL));

    // 配置WiFi
    wifi_config_t wifi_config = {
        .sta = {
            .ssid = EXAMPLE_ESP_WIFI_SSID,
            .password = EXAMPLE_ESP_WIFI_PASS,
            .threshold.authmode = WIFI_AUTH_WPA2_PSK,
        },
    };

    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &wifi_config));
    ESP_ERROR_CHECK(esp_wifi_start());
}

// HTTP客户端
#include "esp_http_client.h"

void http_post_task(void *pvParameters) {
    char local_response_buffer[MAX_HTTP_OUTPUT_BUFFER] = {0};

    esp_http_client_config_t config = {
        .url = "http://api.example.com/sensor",
        .method = HTTP_METHOD_POST,
        .event_handler = _http_event_handler,
        .user_data = local_response_buffer,
    };

    esp_http_client_handle_t client = esp_http_client_init(&config);

    // POST JSON数据
    const char *post_data = "{\"temperature\":25.5,\"humidity\":60}";
    esp_http_client_set_post_field(client, post_data, strlen(post_data));
    esp_http_client_set_header(client, "Content-Type", "application/json");

    esp_err_t err = esp_http_client_perform(client);
    if (err == ESP_OK) {
        ESP_LOGI(TAG, "HTTP POST Status = %d",
                 esp_http_client_get_status_code(client));
    }

    esp_http_client_cleanup(client);
}
```

---

## 三、树莓派Pico(RP2040)开发

### 3.1 RP2040独特架构

```c
// ============================================================
// RP2040核心特性
// - 双核Cortex-M0+ (无FPU)
// - 可编程I/O (PIO): 最大特色
// - 所有SRAM在统一地址空间
// - 启动必须从外部QSPI Flash
// ============================================================

#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "hardware/pio.h"
#include "hardware/dma.h"
#include "hardware/sync.h"

// RP2040启动代码特殊性
// main() 在核心0上运行，核心1默认处于等待状态
int main(void) {
    // 初始化Pico SDK
    stdio_init_all();

    // 启动核心1
    multicore_launch_core1(core1_main);

    // 核心0主循环
    while (1) {
        // 核心0工作
        tight_loop_contents();
    }
}

void core1_main(void) {
    // 核心1从这里开始
    while (1) {
        // 核心1工作
        tight_loop_contents();
    }
}

// 双核FIFO通信 (RP2040特有的硬件FIFO)
void multicore_fifo_example(void) {
    // 核心0发送
    multicore_fifo_push_blocking(0x12345678);

    // 核心1接收
    uint32_t data = multicore_fifo_pop_blocking();
}

// 原子操作 (Cortex-M0+没有LDREX/STREX，需要特殊处理)
// RP2040提供硬件互斥锁
void atomic_operation_example(void) {
    static uint32_t shared_counter = 0;

    // 获取硬件互斥锁
    uint32_t save = spin_lock_blocking(spinlock);

    // 临界区
    shared_counter++;

    // 释放锁
    spin_unlock(spinlock, save);
}
```

### 3.2 PIO可编程I/O

```c
// ============================================================
// PIO (Programmable I/O) 是RP2040最独特的功能
// 可以自定义协议时序，不占用CPU时间
// 4个状态机并行执行，每个最多32条指令
// ============================================================

// ========== 示例1: PIO实现WS2812B LED控制 ==========

// PIO汇编程序
// .pio文件会被pioasm编译成C头文件
/*
.program ws2812
.side_set 1

.define public T1 2
.define public T2 5
.define public T3 3

.lang_opt python sideset_init = pico.PIO.OUT_HIGH
.lang_opt python out_init = pico.PIO.OUT_HIGH
.lang_opt python out_shiftdir = 1

.entry_point
bitloop:
    out x, 1       side 0 [T3 - 1] ; 输出1位到X，同时拉低side pin，延时T3-1
    jmp !x do_zero side 1 [T1 - 1] ; 如果X=0跳转，同时拉高，延时T1-1
    jmp bitloop    side 1 [T2 - 1] ; X=1时继续，保持高电平，延时T2-1
do_zero:
    nop            side 0 [T2 - 1] ; X=0时拉低，延时T2-1

% c-sdk {
static inline void ws2812_program_init(PIO pio, uint sm, uint offset, uint pin) {
    pio_gpio_init(pio, pin);
    pio_sm_set_consecutive_pindirs(pio, sm, pin, 1, true);

    pio_sm_config c = ws2812_program_get_default_config(offset);
    sm_config_set_sideset_pins(&c, pin);
    sm_config_set_out_shift(&c, false, true, 24);
    sm_config_set_fifo_join(&c, PIO_FIFO_JOIN_TX);

    int cycles_per_bit = T1 + T2 + T3;
    float div = clock_get_hz(clk_sys) / (800000.0f * cycles_per_bit);
    sm_config_set_clkdiv(&c, div);

    pio_sm_init(pio, sm, offset, &c);
    pio_sm_set_enabled(pio, sm, true);
}
%}
*/

// 使用PIO控制WS2812
#include "ws2812.pio.h"  // 由pioasm生成的头文件

#define WS2812_PIN 16
#define NUM_PIXELS 30

void ws2812_init(void) {
    PIO pio = pio0;
    uint offset = pio_add_program(pio, &ws2812_program);
    ws2812_program_init(pio, 0, offset, WS2812_PIN);
}

void ws2812_put_pixel(uint32_t pixel_grb) {
    pio_sm_put_blocking(pio0, 0, pixel_grb << 8u);
}

// ========== 示例2: PIO实现自定义SPI从机 ==========

/*
.program spi_slave
.side_set 1

; CPHA=0, CPOL=0 SPI从机
; SCK输入，MOSI输入，MISO输出，CS输入

    wait 0 pin 2    ; 等待CS低电平
    wait 0 pin 1    ; 等待SCK下降沿 (CPOL=0)

.wrap_target
    wait 1 pin 1    ; 等待SCK上升沿
    in pins, 1      ; 读取MOSI
    out pin, 1      ; 输出MISO
.wrap

% c-sdk {
static inline void spi_slave_program_init(PIO pio, uint sm, uint offset,
                                          uint mosi_pin, uint miso_pin,
                                          uint sck_pin, uint cs_pin) {
    pio_sm_config c = spi_slave_program_get_default_config(offset);

    sm_config_set_in_pins(&c, mosi_pin);
    sm_config_set_out_pins(&c, miso_pin, 1);
    sm_config_set_sideset_pins(&c, 0);
    sm_config_set_in_shift(&c, false, false, 8);
    sm_config_set_out_shift(&c, false, false, 8);

    pio_sm_init(pio, sm, offset, &c);
    pio_sm_set_enabled(pio, sm, true);
}
%}
*/
```

### 3.3 RP2040 DMA链式传输

```c
// ============================================================
// RP2040 DMA支持链式传输和复杂的触发模式
// ============================================================

#include "hardware/dma.h"

// 简单的内存到内存DMA
void dma_memory_copy(void *dst, const void *src, size_t len) {
    int channel = dma_claim_unused_channel(true);

    dma_channel_config c = dma_channel_get_default_config(channel);
    channel_config_set_transfer_data_size(&c, DMA_SIZE_32);  // 32位传输
    channel_config_set_read_increment(&c, true);
    channel_config_set_write_increment(&c, true);

    dma_channel_configure(
        channel,
        &c,
        dst,        // 目标
        src,        // 源
        len / 4,    // 传输计数 (32位字)
        true        // 立即启动
    );

    dma_channel_wait_for_finish_blocking(channel);
    dma_channel_unclaim(channel);
}

// ADC采样+DMA (连续采样到内存)
void adc_dma_continuous(void) {
    const uint16_t num_samples = 1000;
    uint16_t capture_buf[num_samples];

    // 配置ADC
    adc_init();
    adc_gpio_init(26);  // ADC0
    adc_select_input(0);
    adc_fifo_setup(
        true,   // 使能FIFO
        true,   // 使能DMA请求
        1,      // DREQ阈值
        false,  // 错误位
        false   // 字节移位
    );

    // 配置DMA
    int dma_chan = dma_claim_unused_channel(true);
    dma_channel_config cfg = dma_channel_get_default_config(dma_chan);

    // 从ADC FIFO读取，不增加读地址
    channel_config_set_transfer_data_size(&cfg, DMA_SIZE_16);
    channel_config_set_read_increment(&cfg, false);  // ADC FIFO地址固定
    channel_config_set_write_increment(&cfg, true);  // 内存地址增加

    // 设置数据传输请求 (DREQ) - 由ADC触发
    channel_config_set_dreq(&cfg, DREQ_ADC);

    dma_channel_configure(
        dma_chan,
        &cfg,
        capture_buf,        // 目标
        &adc_hw->fifo,      // 源 (ADC FIFO)
        num_samples,
        false               // 不立即启动
    );

    // 启动DMA和ADC
    dma_channel_start(dma_chan);
    adc_run(true);  // 开始自由运行模式

    // 等待完成
    dma_channel_wait_for_finish_blocking(dma_chan);

    // 停止ADC
    adc_run(false);
    adc_fifo_drain();
}
```

---

## 四、GPIO高级技巧与语义

### 4.1 GPIO模式深度解析

```c
// ============================================================
// GPIO模式语义详解
// ============================================================

// ========== 输入模式 ==========
// 浮空输入: 外部必须有上拉/下拉，否则悬空易干扰
#define GPIO_MODE_INPUT_FLOATING    0x00

// 上拉输入: 内部上拉电阻(通常30-50kΩ)，默认高电平
#define GPIO_MODE_INPUT_PULLUP      0x01

// 下拉输入: 内部下拉电阻，默认低电平
#define GPIO_MODE_INPUT_PULLDOWN    0x02

// 模拟输入: 连接到ADC，数字功能禁用
#define GPIO_MODE_ANALOG            0x03

// ========== 输出模式 ==========
// 推挽输出: 可主动输出高/低，驱动能力强
//   P-MOS导通 → 输出高(VDD)
//   N-MOS导通 → 输出低(VSS)
#define GPIO_MODE_OUTPUT_PP         0x10

// 开漏输出: 只能拉低或释放(外部上拉决定高电平)
//   应用: I2C、电平转换(如5V→3.3V)、线与逻辑
#define GPIO_MODE_OUTPUT_OD         0x11

// ========== 复用功能 ==========
// 复用推挽: 由外设(USART/SPI等)控制输出
#define GPIO_MODE_AF_PP             0x20

// 复用开漏: I2C必须使用
#define GPIO_MODE_AF_OD             0x21

// ========== 实际应用示例 ==========

// I2C SDA/SCL必须配置为开漏+上拉
void i2c_gpio_config(void) {
    // STM32
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    GPIO_InitStruct.Pin = GPIO_PIN_6 | GPIO_PIN_7;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_OD;        // 复用开漏！
    GPIO_InitStruct.Pull = GPIO_PULLUP;            // 必须上拉
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF4_I2C1;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
}

// 5V到3.3V电平转换
void level_shifter_config(void) {
    // 开漏输出 + 外部上拉到3.3V
    // MCU(5V容忍)输出开漏 → 外部上拉电阻 → 3.3V器件
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_OD;
    GPIO_InitStruct.Pull = GPIO_NOPULL;  // 外部上拉
}

// 按键检测 (上拉输入)
void button_config(void) {
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_PULLUP;  // 按键按下为低电平
}
```

### 4.2 中断与事件系统

```c
// ============================================================
// GPIO中断的高级用法
// ============================================================

// ========== STM32外部中断线(EXTI) ==========
// EXTI线0-15对应GPIO pin 0-15，同一时刻每个线只能映射一个GPIO

void stm32_exti_config(void) {
    // 使能SYSCFG时钟
    __HAL_RCC_SYSCFG_CLK_ENABLE();

    // 配置PA0为外部中断
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    GPIO_InitStruct.Pin = GPIO_PIN_0;
    GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;  // 上升沿触发
    GPIO_InitStruct.Pull = GPIO_PULLDOWN;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    // 配置中断优先级
    HAL_NVIC_SetPriority(EXTI0_IRQn, 2, 0);
    HAL_NVIC_EnableIRQ(EXTI0_IRQn);
}

// EXTI0中断处理
void EXTI0_IRQHandler(void) {
    HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_0);
}

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin) {
    if (GPIO_Pin == GPIO_PIN_0) {
        // 处理中断
    }
}

// ========== ESP32 GPIO中断 ==========
// ESP32每个GPIO都有独立的中断能力

void esp32_gpio_interrupt(void) {
    gpio_config_t io_conf = {
        .pin_bit_mask = (1ULL << GPIO_NUM_0),
        .mode = GPIO_MODE_INPUT,
        .pull_up_en = GPIO_PULLUP_ENABLE,
        .intr_type = GPIO_INTR_NEGEDGE,  // 下降沿触发
    };
    gpio_config(&io_conf);

    // 安装ISR服务
    gpio_install_isr_service(ESP_INTR_FLAG_EDGE);

    // 添加处理函数
    gpio_isr_handler_add(GPIO_NUM_0, gpio_isr_handler, (void*)0);
}

// ========== RP2040 GPIO中断 ==========
// RP2040每个核心有独立的NVIC

void rp2040_gpio_irq(void) {
    // 配置GPIO中断
    gpio_set_irq_enabled_with_callback(0, GPIO_IRQ_EDGE_FALL, true, gpio_callback);
}

void gpio_callback(uint gpio, uint32_t events) {
    // 处理中断
    // RP2040的GPIO中断处理非常简单
}

// ========== 按键消抖的三种方法 ==========

// 方法1: 软件延时消抖 (最简单但阻塞)
bool button_read_debounced_simple(uint32_t pin) {
    if (gpio_get(pin) == 0) {  // 按键按下
        sleep_ms(20);  // 延时20ms
        return gpio_get(pin) == 0;  // 再次检测
    }
    return false;
}

// 方法2: 定时器消抖 (推荐，非阻塞)
typedef struct {
    uint32_t pin;
    uint32_t last_time;
    bool last_state;
    bool stable_state;
} DebounceButton;

void button_update(DebounceButton *btn) {
    bool current = gpio_get(btn->pin);
    uint32_t now = time_us_32();

    if (current != btn->last_state) {
        btn->last_time = now;
        btn->last_state = current;
    }

    // 状态稳定超过20ms
    if ((now - btn->last_time) > 20000) {
        btn->stable_state = current;
    }
}

// 方法3: 状态机消抖 (最完善，支持长按、双击)
typedef enum {
    BTN_IDLE,
    BTN_PRESSING,
    BTN_PRESSED,
    BTN_RELEASING
} ButtonState;

typedef struct {
    ButtonState state;
    uint32_t press_time;
    uint32_t release_time;
    bool pressed_event;
    bool released_event;
    bool long_press_event;
} ButtonFSM;

void button_fsm_update(ButtonFSM *fsm, bool raw_state) {
    uint32_t now = time_us_32();

    switch (fsm->state) {
        case BTN_IDLE:
            if (!raw_state) {  // 按下
                fsm->state = BTN_PRESSING;
                fsm->press_time = now;
            }
            break;

        case BTN_PRESSING:
            if (raw_state) {  // 抖动释放
                fsm->state = BTN_IDLE;
            } else if ((now - fsm->press_time) > 20000) {  // 确认按下
                fsm->state = BTN_PRESSED;
                fsm->pressed_event = true;
            }
            break;

        case BTN_PRESSED:
            if (!raw_state) {  // 长按检测
                if ((now - fsm->press_time) > 1000000) {  // 1秒长按
                    fsm->long_press_event = true;
                }
            } else {  // 释放
                fsm->state = BTN_RELEASING;
                fsm->release_time = now;
            }
            break;

        case BTN_RELEASING:
            if (!raw_state) {  // 抖动按下
                fsm->state = BTN_PRESSED;
            } else if ((now - fsm->release_time) > 20000) {  // 确认释放
                fsm->state = BTN_IDLE;
                fsm->released_event = true;
            }
            break;
    }
}
```

---

## 五、I2C协议栈完整实现

### 5.1 I2C协议深度解析

```
┌─────────────────────────────────────────────────────────────────────┐
│                        I2C协议帧格式                                 │
├─────────────────────────────────────────────────────────────────────┤
│                                                                      │
│  起始(S) │  地址(7bit)  │ R/W │ ACK │ 数据1 │ ACK │ ... │ 停止(P) │
│     ▼    │      ▼       │  ▼  │  ▼  │   ▼   │  ▼  │     │    ▼    │
│   SDA↓   │  A6...A0    │ 0/1 │ 0/1 │ D7..D0│ 0/1 │     │  SDA↑   │
│   SCL高  │   SCL脉冲   │     │     │       │     │     │  SCL高  │
│                                                                      │
│  时序:                                                               │
│  SCL: ─┐  ┌─┐  ┌─┐  ┌─┐  ┌─┐  ┌─┐  ┌─┐  ┌─┐  ┌─┐                   │
│        └──┘  └──┘  └──┘  └──┘  └──┘  └──┘  └──┘  └──┘              │
│  SDA: ───┐      ┌──────┐     ┌────────┐     ┌───────────            │
│          └──────┘      └─────┘        └─────┘         (S+P示例)     │
│                                                                      │
└─────────────────────────────────────────────────────────────────────┘
```

### 5.2 软件I2C实现（位 bang）

```c
// ============================================================
// 软件I2C (Bit-Bang) 实现
// 用于没有硬件I2C或需要特殊时序的场景
// ============================================================

#include <stdint.h>
#include <stdbool.h>

// I2C引脚定义
#define SDA_PIN     0
#define SCL_PIN     1

// 延时 (根据CPU频率调整)
static inline void i2c_delay(void) {
    for (volatile int i = 0; i < 10; i++);
}

// 设置SDA方向
static inline void sda_output(void) {
    gpio_set_dir(SDA_PIN, GPIO_OUT);
}

static inline void sda_input(void) {
    gpio_set_dir(SDA_PIN, GPIO_IN);
}

// 设置/读取引脚
static inline void sda_high(void) {
    gpio_put(SDA_PIN, 1);
}

static inline void sda_low(void) {
    gpio_put(SDA_PIN, 0);
}

static inline bool sda_read(void) {
    return gpio_get(SDA_PIN);
}

static inline void scl_high(void) {
    gpio_put(SCL_PIN, 1);
}

static inline void scl_low(void) {
    gpio_put(SCL_PIN, 0);
}

// I2C起始条件: SCL高时SDA下降沿
void i2c_start(void) {
    sda_output();
    sda_high();
    scl_high();
    i2c_delay();
    sda_low();
    i2c_delay();
    scl_low();
    i2c_delay();
}

// I2C停止条件: SCL高时SDA上升沿
void i2c_stop(void) {
    sda_output();
    sda_low();
    i2c_delay();
    scl_high();
    i2c_delay();
    sda_high();
    i2c_delay();
}

// 发送一个字节，返回ACK状态
bool i2c_write_byte(uint8_t data) {
    sda_output();

    // 发送8位数据 (MSB first)
    for (int i = 7; i >= 0; i--) {
        if (data & (1 << i)) {
            sda_high();
        } else {
            sda_low();
        }
        i2c_delay();
        scl_high();
        i2c_delay();
        scl_low();
        i2c_delay();
    }

    // 读取ACK
    sda_input();
    sda_high();  // 释放总线
    i2c_delay();
    scl_high();
    i2c_delay();
    bool ack = !sda_read();  // ACK=SDA低
    scl_low();
    i2c_delay();

    return ack;
}

// 读取一个字节
uint8_t i2c_read_byte(bool ack) {
    sda_input();
    sda_high();  // 释放总线

    uint8_t data = 0;

    // 读取8位数据
    for (int i = 7; i >= 0; i--) {
        scl_high();
        i2c_delay();
        if (sda_read()) {
            data |= (1 << i);
        }
        scl_low();
        i2c_delay();
    }

    // 发送ACK/NACK
    sda_output();
    if (ack) {
        sda_low();  // ACK
    } else {
        sda_high(); // NACK
    }
    i2c_delay();
    scl_high();
    i2c_delay();
    scl_low();
    i2c_delay();

    return data;
}

// 完整写入操作
bool i2c_write(uint8_t addr, const uint8_t *data, size_t len) {
    i2c_start();

    // 发送地址+写(0)
    if (!i2c_write_byte((addr << 1) | 0)) {
        i2c_stop();
        return false;  // 设备无响应
    }

    // 发送数据
    for (size_t i = 0; i < len; i++) {
        if (!i2c_write_byte(data[i])) {
            i2c_stop();
            return false;
        }
    }

    i2c_stop();
    return true;
}

// 完整读取操作
bool i2c_read(uint8_t addr, uint8_t *data, size_t len) {
    i2c_start();

    // 发送地址+读(1)
    if (!i2c_write_byte((addr << 1) | 1)) {
        i2c_stop();
        return false;
    }

    // 读取数据
    for (size_t i = 0; i < len; i++) {
        bool ack = (i < len - 1);  // 最后一个字节发NACK
        data[i] = i2c_read_byte(ack);
    }

    i2c_stop();
    return true;
}

// 带寄存器地址的写入 (常用格式)
bool i2c_write_reg(uint8_t addr, uint8_t reg, const uint8_t *data, size_t len) {
    i2c_start();

    if (!i2c_write_byte((addr << 1) | 0)) {
        i2c_stop();
        return false;
    }

    // 发送寄存器地址
    if (!i2c_write_byte(reg)) {
        i2c_stop();
        return false;
    }

    // 发送数据
    for (size_t i = 0; i < len; i++) {
        if (!i2c_write_byte(data[i])) {
            i2c_stop();
            return false;
        }
    }

    i2c_stop();
    return true;
}

// 带寄存器地址的读取
bool i2c_read_reg(uint8_t addr, uint8_t reg, uint8_t *data, size_t len) {
    // 先写寄存器地址
    i2c_start();
    if (!i2c_write_byte((addr << 1) | 0)) {
        i2c_stop();
        return false;
    }
    if (!i2c_write_byte(reg)) {
        i2c_stop();
        return false;
    }

    // 重复起始，然后读
    i2c_start();
    if (!i2c_write_byte((addr << 1) | 1)) {
        i2c_stop();
        return false;
    }

    for (size_t i = 0; i < len; i++) {
        bool ack = (i < len - 1);
        data[i] = i2c_read_byte(ack);
    }

    i2c_stop();
    return true;
}
```

### 5.3 I2C设备驱动示例（BMP280气压传感器）

```c
// ============================================================
// BMP280驱动完整实现
// I2C地址: 0x76 (SDO接地) 或 0x77 (SDO接VCC)
// ============================================================

#define BMP280_ADDR         0x76
#define BMP280_REG_ID       0xD0
#define BMP280_REG_RESET    0xE0
#define BMP280_REG_STATUS   0xF3
#define BMP280_REG_CTRL     0xF4
#define BMP280_REG_CONFIG   0xF5
#define BMP280_REG_DATA     0xF7
#define BMP280_REG_CALIB    0x88

#define BMP280_CHIP_ID      0x58

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
    int32_t t_fine;  // 中间计算值
} BMP280_CalibData;

static BMP280_CalibData calib;

// 读取校准数据
bool bmp280_read_calibration(void) {
    uint8_t calib_raw[24];

    if (!i2c_read_reg(BMP280_ADDR, BMP280_REG_CALIB, calib_raw, 24)) {
        return false;
    }

    // 解析校准数据 (小端)
    calib.dig_T1 = (calib_raw[1] << 8) | calib_raw[0];
    calib.dig_T2 = (calib_raw[3] << 8) | calib_raw[2];
    calib.dig_T3 = (calib_raw[5] << 8) | calib_raw[4];
    calib.dig_P1 = (calib_raw[7] << 8) | calib_raw[6];
    calib.dig_P2 = (calib_raw[9] << 8) | calib_raw[8];
    calib.dig_P3 = (calib_raw[11] << 8) | calib_raw[10];
    calib.dig_P4 = (calib_raw[13] << 8) | calib_raw[12];
    calib.dig_P5 = (calib_raw[15] << 8) | calib_raw[14];
    calib.dig_P6 = (calib_raw[17] << 8) | calib_raw[16];
    calib.dig_P7 = (calib_raw[19] << 8) | calib_raw[18];
    calib.dig_P8 = (calib_raw[21] << 8) | calib_raw[20];
    calib.dig_P9 = (calib_raw[23] << 8) | calib_raw[22];

    return true;
}

// 初始化BMP280
bool bmp280_init(void) {
    // 读取芯片ID
    uint8_t id;
    if (!i2c_read_reg(BMP280_ADDR, BMP280_REG_ID, &id, 1)) {
        return false;
    }
    if (id != BMP280_CHIP_ID) {
        return false;  // 设备不存在
    }

    // 读取校准数据
    if (!bmp280_read_calibration()) {
        return false;
    }

    // 配置: 温度x2过采样, 气压x16过采样, 正常模式
    uint8_t ctrl = (0x02 << 5) | (0x05 << 2) | 0x03;
    if (!i2c_write_reg(BMP280_ADDR, BMP280_REG_CTRL, &ctrl, 1)) {
        return false;
    }

    // 配置IIR滤波器
    uint8_t config = 0x04 << 2;  // IIR系数=16
    if (!i2c_write_reg(BMP280_ADDR, BMP280_REG_CONFIG, &config, 1)) {
        return false;
    }

    return true;
}

// 补偿温度 (BMP280算法)
int32_t bmp280_compensate_T(int32_t adc_T) {
    int32_t var1 = ((((adc_T >> 3) - ((int32_t)calib.dig_T1 << 1))) *
                    ((int32_t)calib.dig_T2)) >> 11;
    int32_t var2 = (((((adc_T >> 4) - ((int32_t)calib.dig_T1)) *
                      ((adc_T >> 4) - ((int32_t)calib.dig_T1))) >> 12) *
                    ((int32_t)calib.dig_T3)) >> 14;
    calib.t_fine = var1 + var2;
    return (calib.t_fine * 5 + 128) >> 8;  // 返回0.01°C
}

// 补偿气压
uint32_t bmp280_compensate_P(int32_t adc_P) {
    int64_t var1, var2, p;

    var1 = ((int64_t)calib.t_fine) - 128000;
    var2 = var1 * var1 * (int64_t)calib.dig_P6;
    var2 = var2 + ((var1 * (int64_t)calib.dig_P5) << 17);
    var2 = var2 + (((int64_t)calib.dig_P4) << 35);
    var1 = ((var1 * var1 * (int64_t)calib.dig_P3) >> 8) +
           ((var1 * (int64_t)calib.dig_P2) << 12);
    var1 = (((((int64_t)1) << 47) + var1)) * ((int64_t)calib.dig_P1) >> 33;

    if (var1 == 0) return 0;

    p = 1048576 - adc_P;
    p = (((p << 31) - var2) * 3125) / var1;
    var1 = (((int64_t)calib.dig_P9) * (p >> 13) * (p >> 13)) >> 25;
    var2 = (((int64_t)calib.dig_P8) * p) >> 19;

    p = ((p + var1 + var2) >> 8) + (((int64_t)calib.dig_P7) << 4);
    return (uint32_t)p;  // 返回Pa (帕斯卡)
}

// 读取测量值
bool bmp280_read(int32_t *temperature, uint32_t *pressure) {
    uint8_t data[6];

    if (!i2c_read_reg(BMP280_ADDR, BMP280_REG_DATA, data, 6)) {
        return false;
    }

    int32_t adc_P = ((int32_t)data[0] << 12) | ((int32_t)data[1] << 4) | (data[2] >> 4);
    int32_t adc_T = ((int32_t)data[3] << 12) | ((int32_t)data[4] << 4) | (data[5] >> 4);

    *temperature = bmp280_compensate_T(adc_T);    // 0.01°C
    *pressure = bmp280_compensate_P(adc_P);       // Pa

    return true;
}
```

---

## 六、SPI协议栈完整实现

### 6.1 SPI协议深度解析

```
┌─────────────────────────────────────────────────────────────────────┐
│                        SPI协议时序                                   │
├─────────────────────────────────────────────────────────────────────┤
│                                                                      │
│  CPOL=0, CPHA=0 (模式0) - 最常用                                     │
│  ─────────────────────────────────────────────                       │
│  SCK:  ─┐  ┌─┐  ┌─┐  ┌─┐  ┌─┐  ┌─┐  ┌─┐  ┌─┐  ┌─┐                   │
│         └──┘  └──┘  └──┘  └──┘  └──┘  └──┘  └──┘  └───              │
│  MOSI: ────┐      ┌──────┐     ┌────────┐     ┌────────              │
│            └──────┘      └─────┘        └─────┘                     │
│  MISO: ───────┐      ┌──────┐     ┌────────┐     ┌──────             │
│               └──────┘      └─────┘        └─────┘                  │
│                                                                      │
│  采样点: SCK上升沿 (数据在下降沿变化，上升沿稳定)                      │
│                                                                      │
├─────────────────────────────────────────────────────────────────────┤
│  CPOL=1, CPHA=1 (模式3)                                              │
│  ─────────────────────────────────────────────                       │
│  SCK:  ┐  ┌─┐  ┌─┐  ┌─┐  ┌─┐  ┌─┐  ┌─┐  ┌─┐  ┌─┐                   │
│        └──┘  └──┘  └──┘  └──┘  └──┘  └──┘  └──┘  └──               │
│                                                                      │
│  采样点: SCK上升沿 (空闲时SCK为高)                                    │
└─────────────────────────────────────────────────────────────────────┘

SPI模式总结:
┌──────┬──────┬──────┬────────────────────────────┐
│ 模式 │ CPOL │ CPHA │ 描述                       │
├──────┼──────┼──────┼────────────────────────────┤
│  0   │  0   │  0   │ 上升沿采样，空闲低         │
│  1   │  0   │  1   │ 下降沿采样，空闲低         │
│  2   │  1   │  0   │ 下降沿采样，空闲高         │
│  3   │  1   │  1   │ 上升沿采样，空闲高         │
└──────┴──────┴──────┴────────────────────────────┘
```

### 6.2 软件SPI实现

```c
// ============================================================
// 软件SPI (Bit-Bang) 实现
// 支持4种模式
// ============================================================

typedef struct {
    uint32_t mosi_pin;
    uint32_t miso_pin;
    uint32_t sck_pin;
    uint32_t cs_pin;
    uint8_t mode;  // 0-3
} SoftSPI;

// 初始化软件SPI
void softspi_init(SoftSPI *spi, uint8_t mode) {
    spi->mode = mode & 0x03;

    // 配置引脚
    gpio_init(spi->mosi_pin);
    gpio_init(spi->miso_pin);
    gpio_init(spi->sck_pin);
    gpio_init(spi->cs_pin);

    gpio_set_dir(spi->mosi_pin, GPIO_OUT);
    gpio_set_dir(spi->miso_pin, GPIO_IN);
    gpio_set_dir(spi->sck_pin, GPIO_OUT);
    gpio_set_dir(spi->cs_pin, GPIO_OUT);

    // 设置初始状态
    gpio_put(spi->cs_pin, 1);  // CS高 (未选中)

    // 根据CPOL设置初始时钟状态
    uint8_t cpol = (spi->mode >> 1) & 1;
    gpio_put(spi->sck_pin, cpol);
}

// 延时
static inline void spi_delay(void) {
    for (volatile int i = 0; i < 5; i++);
}

// 发送接收一个字节
uint8_t softspi_transfer(SoftSPI *spi, uint8_t data) {
    uint8_t received = 0;
    uint8_t cpol = (spi->mode >> 1) & 1;
    uint8_t cpha = spi->mode & 1;

    for (int i = 7; i >= 0; i--) {
        // 准备数据 (MSB first)
        if (data & (1 << i)) {
            gpio_put(spi->mosi_pin, 1);
        } else {
            gpio_put(spi->mosi_pin, 0);
        }

        if (cpha == 0) {
            // 模式0,2: 第一个边沿采样
            spi_delay();
            gpio_put(spi->sck_pin, !cpol);  // 时钟翻转
            spi_delay();
            received = (received << 1) | gpio_get(spi->miso_pin);
            gpio_put(spi->sck_pin, cpol);   // 时钟恢复
        } else {
            // 模式1,3: 第二个边沿采样
            spi_delay();
            gpio_put(spi->sck_pin, !cpol);  // 时钟翻转 (数据变化)
            spi_delay();
            gpio_put(spi->sck_pin, cpol);   // 时钟恢复 (采样)
            spi_delay();
            received = (received << 1) | gpio_get(spi->miso_pin);
        }
    }

    return received;
}

// CS控制
void softspi_begin(SoftSPI *spi) {
    gpio_put(spi->cs_pin, 0);
    spi_delay();
}

void softspi_end(SoftSPI *spi) {
    spi_delay();
    gpio_put(spi->cs_pin, 1);
}

// 发送数据
void softspi_write(SoftSPI *spi, const uint8_t *data, size_t len) {
    softspi_begin(spi);
    for (size_t i = 0; i < len; i++) {
        softspi_transfer(spi, data[i]);
    }
    softspi_end(spi);
}

// 接收数据
void softspi_read(SoftSPI *spi, uint8_t *data, size_t len) {
    softspi_begin(spi);
    for (size_t i = 0; i < len; i++) {
        data[i] = softspi_transfer(spi, 0xFF);  // 发送0xFF，读取MISO
    }
    softspi_end(spi);
}
```

### 6.3 SPI驱动W25Q Flash示例

```c
// ============================================================
// W25Q128 Flash驱动 (16MB SPI Flash)
// 常用于存储固件、配置、日志
// ============================================================

#define W25X_WRITE_ENABLE       0x06
#define W25X_WRITE_DISABLE      0x04
#define W25X_READ_STATUS_REG    0x05
#define W25X_READ_DATA          0x03
#define W25X_PAGE_PROGRAM       0x02
#define W25X_SECTOR_ERASE       0x20
#define W25X_CHIP_ERASE         0xC7
#define W25X_POWER_DOWN         0xB9
#define W25X_RELEASE_POWER_DOWN 0xAB
#define W25X_JEDEC_ID           0x9F

#define W25Q_PAGE_SIZE          256
#define W25Q_SECTOR_SIZE        4096
#define W25Q_BLOCK_SIZE         65536

// 读取JEDEC ID
bool w25q_read_id(SoftSPI *spi, uint32_t *id) {
    softspi_begin(spi);
    softspi_transfer(spi, W25X_JEDEC_ID);
    uint8_t manufacturer = softspi_transfer(spi, 0xFF);
    uint8_t memory_type = softspi_transfer(spi, 0xFF);
    uint8_t capacity = softspi_transfer(spi, 0xFF);
    softspi_end(spi);

    *id = (manufacturer << 16) | (memory_type << 8) | capacity;
    return (manufacturer == 0xEF);  // Winbond厂商ID
}

// 等待忙碌完成
void w25q_wait_busy(SoftSPI *spi) {
    softspi_begin(spi);
    softspi_transfer(spi, W25X_READ_STATUS_REG);
    while (softspi_transfer(spi, 0xFF) & 0x01) {
        // 忙等待
    }
    softspi_end(spi);
}

// 写使能
void w25q_write_enable(SoftSPI *spi) {
    softspi_begin(spi);
    softspi_transfer(spi, W25X_WRITE_ENABLE);
    softspi_end(spi);
}

// 擦除扇区 (4KB)
bool w25q_sector_erase(SoftSPI *spi, uint32_t addr) {
    w25q_write_enable(spi);

    softspi_begin(spi);
    softspi_transfer(spi, W25X_SECTOR_ERASE);
    softspi_transfer(spi, (addr >> 16) & 0xFF);
    softspi_transfer(spi, (addr >> 8) & 0xFF);
    softspi_transfer(spi, addr & 0xFF);
    softspi_end(spi);

    w25q_wait_busy(spi);
    return true;
}

// 页编程 (最多256字节)
bool w25q_page_program(SoftSPI *spi, uint32_t addr, const uint8_t *data, size_t len) {
    if (len > W25Q_PAGE_SIZE) return false;
    if ((addr & 0xFF) + len > W25Q_PAGE_SIZE) return false;  // 跨页检查

    w25q_write_enable(spi);

    softspi_begin(spi);
    softspi_transfer(spi, W25X_PAGE_PROGRAM);
    softspi_transfer(spi, (addr >> 16) & 0xFF);
    softspi_transfer(spi, (addr >> 8) & 0xFF);
    softspi_transfer(spi, addr & 0xFF);

    for (size_t i = 0; i < len; i++) {
        softspi_transfer(spi, data[i]);
    }
    softspi_end(spi);

    w25q_wait_busy(spi);
    return true;
}

// 读取数据
bool w25q_read_data(SoftSPI *spi, uint32_t addr, uint8_t *data, size_t len) {
    softspi_begin(spi);
    softspi_transfer(spi, W25X_READ_DATA);
    softspi_transfer(spi, (addr >> 16) & 0xFF);
    softspi_transfer(spi, (addr >> 8) & 0xFF);
    softspi_transfer(spi, addr & 0xFF);

    for (size_t i = 0; i < len; i++) {
        data[i] = softspi_transfer(spi, 0xFF);
    }
    softspi_end(spi);

    return true;
}

// 大容量写入 (自动处理扇区擦除和页编程)
bool w25q_write(SoftSPI *spi, uint32_t addr, const uint8_t *data, size_t len) {
    // 计算涉及的扇区范围
    uint32_t start_sector = addr / W25Q_SECTOR_SIZE;
    uint32_t end_sector = (addr + len - 1) / W25Q_SECTOR_SIZE;

    // 需要逐个扇区处理
    for (uint32_t sector = start_sector; sector <= end_sector; sector++) {
        uint32_t sector_addr = sector * W25Q_SECTOR_SIZE;
        uint32_t sector_offset = (sector == start_sector) ?
                                 (addr % W25Q_SECTOR_SIZE) : 0;
        uint32_t sector_bytes = (sector == end_sector) ?
                                ((addr + len) - sector_addr - sector_offset) :
                                (W25Q_SECTOR_SIZE - sector_offset);

        // 读取整个扇区到缓冲区
        uint8_t sector_buffer[W25Q_SECTOR_SIZE];
        w25q_read_data(spi, sector_addr, sector_buffer, W25Q_SECTOR_SIZE);

        // 修改缓冲区
        memcpy(sector_buffer + sector_offset,
               data + (sector_addr + sector_offset - addr),
               sector_bytes);

        // 擦除扇区
        w25q_sector_erase(spi, sector_addr);

        // 重新编程
        for (int page = 0; page < W25Q_SECTOR_SIZE / W25Q_PAGE_SIZE; page++) {
            uint32_t page_addr = sector_addr + page * W25Q_PAGE_SIZE;
            w25q_page_program(spi, page_addr,
                             sector_buffer + page * W25Q_PAGE_SIZE,
                             W25Q_PAGE_SIZE);
        }
    }

    return true;
}
```

---

## 七、UART与串口通信

### 7.1 UART协议深度解析

```
┌─────────────────────────────────────────────────────────────────────┐
│                     UART帧格式 (8N1)                                 │
├─────────────────────────────────────────────────────────────────────┤
│                                                                      │
│    起始位 │ 数据位(LSB first)  │ 校验位 │ 停止位                    │
│       ▼   │   ▼ ▼ ▼ ▼ ▼ ▼ ▼ ▼  │   ▼   │   ▼                       │
│       0   │   D0-D7           │  P    │   1                         │
│       │   │                   │       │   │                         │
│  ─────┘   └───────────────────┘       └─────┘                       │
│       1bit      8bits            0/1bit    1bit                     │
│                                                                      │
│  波特率: 每秒传输的bit数                                             │
│  常见: 9600, 19200, 38400, 57600, 115200                             │
│                                                                      │
│  时间计算 (以115200bps为例):                                         │
│  每bit时间 = 1/115200 ≈ 8.68μs                                       │
│  每字节(8N1) = 10bits = 86.8μs                                       │
└─────────────────────────────────────────────────────────────────────┘
```

### 7.2 软件UART实现

```c
// ============================================================
// 软件UART (Bit-Bang) 实现
// 适用于只有一个UART或需要特殊引脚的场景
// ============================================================

typedef struct {
    uint32_t tx_pin;
    uint32_t rx_pin;
    uint32_t baudrate;
    uint32_t bit_delay_us;
} SoftUART;

void softuart_init(SoftUART *uart, uint32_t baudrate) {
    uart->baudrate = baudrate;
    uart->bit_delay_us = 1000000 / baudrate;

    gpio_init(uart->tx_pin);
    gpio_init(uart->rx_pin);
    gpio_set_dir(uart->tx_pin, GPIO_OUT);
    gpio_set_dir(uart->rx_pin, GPIO_IN);
    gpio_pull_up(uart->rx_pin);

    gpio_put(uart->tx_pin, 1);  // 空闲高电平
}

// 精确延时
static inline void delay_us(uint32_t us) {
    uint32_t start = time_us_32();
    while (time_us_32() - start < us);
}

// 发送一个字节
void softuart_putc(SoftUART *uart, char c) {
    // 起始位
    gpio_put(uart->tx_pin, 0);
    delay_us(uart->bit_delay_us);

    // 数据位 (LSB first)
    for (int i = 0; i < 8; i++) {
        gpio_put(uart->tx_pin, (c >> i) & 1);
        delay_us(uart->bit_delay_us);
    }

    // 停止位
    gpio_put(uart->tx_pin, 1);
    delay_us(uart->bit_delay_us);
}

// 接收一个字节 (轮询方式)
bool softuart_getc(SoftUART *uart, char *c, uint32_t timeout_us) {
    uint32_t start = time_us_32();

    // 等待起始位
    while (gpio_get(uart->rx_pin)) {
        if (time_us_32() - start > timeout_us) return false;
    }

    // 等待半个bit时间，采样到bit中间
    delay_us(uart->bit_delay_us / 2);

    // 读取8位数据
    char data = 0;
    for (int i = 0; i < 8; i++) {
        delay_us(uart->bit_delay_us);
        data |= (gpio_get(uart->rx_pin) << i);
    }

    // 等待停止位
    delay_us(uart->bit_delay_us);

    *c = data;
    return true;
}

// 发送字符串
void softuart_puts(SoftUART *uart, const char *str) {
    while (*str) {
        softuart_putc(uart, *str++);
    }
}
```

### 7.3 DMA UART高效接收

```c
// ============================================================
// STM32 UART + DMA 环形缓冲区接收
// 高效处理高吞吐量数据流
// ============================================================

#define RX_BUFFER_SIZE 256

static uint8_t rx_buffer[RX_BUFFER_SIZE];
static volatile uint16_t rx_write_index = 0;
static volatile uint16_t rx_read_index = 0;

// 初始化UART DMA接收
void uart_dma_init(UART_HandleTypeDef *huart) {
    // 配置DMA循环模式
    HAL_UART_Receive_DMA(huart, rx_buffer, RX_BUFFER_SIZE);
}

// 获取已接收的数据长度
uint16_t uart_rx_available(void) {
    return (rx_write_index - rx_read_index) & (RX_BUFFER_SIZE - 1);
}

// DMA半传输完成回调
void HAL_UART_RxHalfCpltCallback(UART_HandleTypeDef *huart) {
    rx_write_index = RX_BUFFER_SIZE / 2;
}

// DMA传输完成回调
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart) {
    rx_write_index = RX_BUFFER_SIZE;
}

// 读取一个字节
bool uart_read(uint8_t *data) {
    if (rx_read_index == rx_write_index) {
        // 检查DMA当前位置 (处理DMA循环)
        uint16_t dma_pos = RX_BUFFER_SIZE - __HAL_DMA_GET_COUNTER(huart->hdmarx);
        rx_write_index = dma_pos;

        if (rx_read_index == rx_write_index) {
            return false;  // 无数据
        }
    }

    *data = rx_buffer[rx_read_index & (RX_BUFFER_SIZE - 1)];
    rx_read_index++;
    return true;
}

// Modbus RTU over UART
bool modbus_rtu_receive(UART_HandleTypeDef *huart, uint8_t *frame,
                        uint16_t max_len, uint16_t *received_len,
                        uint32_t timeout_ms) {
    uint32_t start = HAL_GetTick();
    uint16_t idx = 0;
    uint32_t last_byte_time = start;

    while (idx < max_len) {
        uint8_t byte;
        if (uart_read(&byte)) {
            frame[idx++] = byte;
            last_byte_time = HAL_GetTick();
        }

        // Modbus 3.5字符时间间隔表示帧结束
        // 9600bps: 3.5 * 11bits = 4ms
        // 115200bps: 3.5 * 11bits = 0.3ms
        if (idx > 0 && (HAL_GetTick() - last_byte_time) > 5) {
            break;  // 帧间隔，认为帧结束
        }

        if (HAL_GetTick() - start > timeout_ms) {
            return false;  // 超时
        }
    }

    *received_len = idx;

    // CRC校验
    if (idx >= 2) {
        uint16_t crc = modbus_crc16(frame, idx - 2);
        uint16_t rx_crc = frame[idx-2] | (frame[idx-1] << 8);
        return (crc == rx_crc);
    }

    return false;
}
```

---

## 八、DMA与高效数据传输

### 8.1 DMA架构对比

```
┌─────────────────────────────────────────────────────────────────────┐
│                    各平台DMA架构对比                                 │
├─────────────────────────────────────────────────────────────────────┤
│                                                                      │
│  STM32 DMA:                                                          │
│  - DMA1 (7通道), DMA2 (5通道)                                        │
│  - 支持存储器到存储器、外设到存储器、存储器到外设                    │
│  - 支持循环模式、双缓冲                                              │
│  - FIFO模式减少总线竞争                                              │
│                                                                      │
│  ESP32 DMA:                                                          │
│  - GDMA (通用DMA), 支持13个通道                                      │
│  - 支持链表描述符 (复杂传输序列)                                     │
│  - 支持外设自动寻址 (SPI/UART/I2S等)                                 │
│  - 支持DMA到DMA传输                                                  │
│                                                                      │
│  RP2040 DMA:                                                         │
│  - 12个独立通道                                                      │
│  - 支持链式传输 (自动加载下一个配置)                                 │
│  - 支持IRQ触发                                                       │
│  - 与PIO紧密配合                                                     │
│                                                                      │
└─────────────────────────────────────────────────────────────────────┘
```

### 8.2 链式DMA (RP2040)

```c
// ============================================================
// RP2040 链式DMA示例
// 自动连续传输多个缓冲区，无需CPU干预
// ============================================================

typedef struct {
    uint8_t buffer[1024];
    uint16_t length;
    bool ready;
} DMA_Buffer;

#define NUM_BUFFERS 3

static DMA_Buffer dma_buffers[NUM_BUFFERS];
static volatile uint8_t current_buffer = 0;
static volatile uint8_t filled_buffer = 0;

// DMA链式配置
void setup_chained_dma(uint32_t src_pin) {
    int ctrl_chan = dma_claim_unused_channel(true);
    int data_chan = dma_claim_unused_channel(true);

    // 控制通道: 重新配置数据通道
    dma_channel_config ctrl_cfg = dma_channel_get_default_config(ctrl_chan);
    channel_config_set_transfer_data_size(&ctrl_cfg, DMA_SIZE_32);
    channel_config_set_read_increment(&ctrl_cfg, false);
    channel_config_set_write_increment(&ctrl_cfg, false);

    // 数据通道配置
    dma_channel_config data_cfg = dma_channel_get_default_config(data_chan);
    channel_config_set_transfer_data_size(&data_cfg, DMA_SIZE_8);
    channel_config_set_read_increment(&data_cfg, false);  // 从PIO读取
    channel_config_set_write_increment(&data_cfg, true);
    channel_config_set_dreq(&data_cfg, DREQ_PIO0_RX0);   // 由PIO触发

    // 启动链式传输
    // ...
}

// 双缓冲+乒乓模式
void ping_pong_dma_example(void) {
    // 使用两个缓冲区交替填充和处理
    uint8_t buffer_a[1024];
    uint8_t buffer_b[1024];

    volatile uint8_t *filling = buffer_a;
    volatile uint8_t *processing = NULL;

    while (1) {
        // 等待当前填充完成
        dma_channel_wait_for_finish_blocking(dma_chan);

        // 交换缓冲区
        if (filling == buffer_a) {
            processing = buffer_a;
            filling = buffer_b;
        } else {
            processing = buffer_b;
            filling = buffer_a;
        }

        // 立即启动下一次DMA到另一个缓冲区
        dma_channel_transfer_to_buffer_now(dma_chan, (void*)filling, 1024);

        // 处理刚完成的缓冲区
        process_buffer((uint8_t*)processing, 1024);
    }
}
```

---

## 九、WiFi/BLE网络栈

### 9.1 ESP32 WiFi事件驱动架构

```c
// ============================================================
// ESP32 WiFi完整示例
// STA模式 + MQTT客户端
// ============================================================

#include "esp_wifi.h"
#include "esp_event.h"
#include "mqtt_client.h"

typedef enum {
    WIFI_DISCONNECTED,
    WIFI_CONNECTING,
    WIFI_CONNECTED,
    WIFI_GOT_IP
} wifi_state_t;

static volatile wifi_state_t wifi_state = WIFI_DISCONNECTED;
static esp_mqtt_client_handle_t mqtt_client = NULL;

// WiFi事件处理
static void wifi_event_handler(void* arg, esp_event_base_t event_base,
                               int32_t event_id, void* event_data) {
    if (event_base == WIFI_EVENT) {
        switch (event_id) {
            case WIFI_EVENT_STA_START:
                esp_wifi_connect();
                wifi_state = WIFI_CONNECTING;
                break;

            case WIFI_EVENT_STA_CONNECTED:
                wifi_state = WIFI_CONNECTED;
                break;

            case WIFI_EVENT_STA_DISCONNECTED:
                wifi_state = WIFI_DISCONNECTED;
                // 5秒后重试
                vTaskDelay(pdMS_TO_TICKS(5000));
                esp_wifi_connect();
                break;

            case WIFI_EVENT_STA_AUTHMODE_CHANGE:
                // 认证模式改变
                break;
        }
    } else if (event_base == IP_EVENT) {
        if (event_id == IP_EVENT_STA_GOT_IP) {
            ip_event_got_ip_t* event = (ip_event_got_ip_t*)event_data;
            ESP_LOGI(TAG, "Got IP: " IPSTR, IP2STR(&event->ip_info.ip));
            wifi_state = WIFI_GOT_IP;

            // 启动MQTT连接
            mqtt_start();
        }
    }
}

// MQTT事件处理
static void mqtt_event_handler(void *handler_args, esp_event_base_t base,
                               int32_t event_id, void *event_data) {
    esp_mqtt_event_handle_t event = event_data;

    switch (event->event_id) {
        case MQTT_EVENT_CONNECTED:
            ESP_LOGI(TAG, "MQTT Connected");
            // 订阅主题
            esp_mqtt_client_subscribe(mqtt_client, "device/cmd", 0);
            break;

        case MQTT_EVENT_DISCONNECTED:
            ESP_LOGI(TAG, "MQTT Disconnected");
            break;

        case MQTT_EVENT_SUBSCRIBED:
            ESP_LOGI(TAG, "MQTT Subscribed");
            break;

        case MQTT_EVENT_PUBLISHED:
            ESP_LOGI(TAG, "MQTT Published");
            break;

        case MQTT_EVENT_DATA:
            // 收到数据
            ESP_LOGI(TAG, "Topic: %.*s", event->topic_len, event->topic);
            ESP_LOGI(TAG, "Data: %.*s", event->data_len, event->data);

            // 解析命令
            process_command(event->data, event->data_len);
            break;

        case MQTT_EVENT_ERROR:
            ESP_LOGI(TAG, "MQTT Error");
            break;
    }
}

void mqtt_start(void) {
    esp_mqtt_client_config_t mqtt_cfg = {
        .uri = "mqtt://broker.example.com:1883",
        .client_id = "esp32_device_001",
        .username = "device",
        .password = "secret",
        .keepalive = 60,
    };

    mqtt_client = esp_mqtt_client_init(&mqtt_cfg);
    esp_mqtt_client_register_event(mqtt_client, ESP_EVENT_ANY_ID,
                                   mqtt_event_handler, NULL);
    esp_mqtt_client_start(mqtt_client);
}

// 发布传感器数据
void publish_sensor_data(float temperature, float humidity) {
    if (wifi_state != WIFI_GOT_IP || mqtt_client == NULL) return;

    char payload[128];
    snprintf(payload, sizeof(payload),
             "{\"temp\":%.2f,\"humi\":%.2f,\"time\":%lu}",
             temperature, humidity, time(NULL));

    esp_mqtt_client_publish(mqtt_client, "device/data",
                            payload, 0, 0, 0);
}
```

---

## 十、多核与并发编程

### 10.1 RP2040双核通信

```c
// ============================================================
// RP2040 双核协作示例
// 核心0: 通信 + 控制
// 核心1: 实时数据处理
// ============================================================

#include "pico/multicore.h"
#include "hardware/irq.h"
#include "hardware/sync.h"

// 共享数据 (需要互斥保护)
typedef struct {
    int32_t sensor_value;
    uint32_t timestamp;
    bool data_ready;
} SharedData;

static SharedData shared_data;
static spin_lock_t *data_lock;

// 核心1入口
void core1_entry(void) {
    // 核心1初始化
    adc_init();
    adc_gpio_init(26);
    adc_select_input(0);

    while (1) {
        // 高频率采样
        int32_t sum = 0;
        for (int i = 0; i < 100; i++) {
            sum += adc_read();
            tight_loop_contents();
        }
        int32_t avg = sum / 100;

        // 写入共享数据 (加锁)
        uint32_t save = spin_lock_blocking(data_lock);
        shared_data.sensor_value = avg;
        shared_data.timestamp = time_us_32();
        shared_data.data_ready = true;
        spin_unlock(data_lock, save);

        // 通知核心0 (通过FIFO)
        multicore_fifo_push_blocking(0x01);
    }
}

// 核心0主程序
int main(void) {
    stdio_init_all();

    // 创建互斥锁
    data_lock = spin_lock_instance(spin_lock_claim_unused(true));

    // 启动核心1
    multicore_launch_core1(core1_entry);

    while (1) {
        // 等待核心1通知
        uint32_t msg = multicore_fifo_pop_blocking();

        if (msg == 0x01) {
            // 读取共享数据
            uint32_t save = spin_lock_blocking(data_lock);
            int32_t value = shared_data.sensor_value;
            uint32_t ts = shared_data.timestamp;
            shared_data.data_ready = false;
            spin_unlock(data_lock, save);

            // 处理并上传
            printf("Value: %ld at %u\n", value, ts);
        }
    }
}
```

---

## 十一、电源管理与低功耗

### 11.1 ESP32深度睡眠

```c
// ============================================================
// ESP32低功耗模式管理
// 用于电池供电的传感器节点
// ============================================================

#include "esp_sleep.h"
#include "esp32/ulp.h"
#include "driver/rtc_io.h"

// RTC内存中的数据 (睡眠保持)
RTC_DATA_ATTR static uint32_t boot_count = 0;
RTC_DATA_ATTR static uint32_t total_uptime = 0;

// 深度睡眠唤醒源配置
void setup_wakeup_sources(void) {
    // 1. 定时器唤醒 (每10分钟)
    esp_sleep_enable_timer_wakeup(10 * 60 * 1000000ULL);

    // 2. GPIO唤醒 (按键)
    esp_sleep_enable_ext0_wakeup(GPIO_NUM_0, 0);  // GPIO0低电平唤醒

    // 3. GPIO唤醒 (传感器中断)
    esp_sleep_enable_ext1_wakeup(
        (1ULL << GPIO_NUM_34),  // GPIO34
        ESP_EXT1_WAKEUP_ANY_HIGH  // 高电平唤醒
    );
}

// 进入深度睡眠
void enter_deep_sleep(void) {
    // 保存当前时间
    total_uptime += esp_timer_get_time() / 1000000;
    boot_count++;

    // 配置RTC外设保持 (GPIO状态)
    esp_sleep_pd_config(ESP_PD_DOMAIN_RTC_PERIPH, ESP_PD_OPTION_ON);

    // 进入深度睡眠
    esp_deep_sleep_start();
}

// 轻度睡眠 (Modem Sleep)
void enter_modem_sleep(void) {
    // 关闭WiFi射频，保持连接
    esp_wifi_set_ps(WIFI_PS_MAX_MODEM);

    // 或者完全关闭WiFi
    // esp_wifi_stop();
}

// ULP协处理器程序 (ULP在深度睡眠时继续运行)
// ULP可以监控GPIO/ADC，在需要时唤醒主CPU
const ulp_insn_t ulp_program[] = {
    I_MOVI(R3, 0),                    // 循环计数器

    // 读取ADC
    I_WR_REG_BIT(RTC_IO_TOUCH_PAD2_REG, RTC_IO_TOUCH_PAD2_MUX_SEL_S, 1),
    I_RD_REG(RTC_IO_TOUCH_PAD2_REG, RTC_IO_TOUCH_PAD2_FUN_IE_S, RTC_IO_TOUCH_PAD2_FUN_IE_S),

    // 与阈值比较
    // ...

    // 唤醒主CPU (如果需要)
    I_WAKE(),

    // 休眠一段时间
    I_HALT()
};
```

### 11.2 RP2040低功耗

```c
// ============================================================
// RP2040 dormont/sleep模式
// 注意: RP2040没有真正的RTC，需要外部RTC或定时唤醒
// ============================================================

#include "hardware/clocks.h"
#include "hardware/rosc.h"
#include "hardware/xosc.h"
#include "hardware/scb.h"

// 进入睡眠，等待中断
void sleep_until_interrupt(void) {
    // 等待中断 (WFI)
    __wfi();
}

// 降低系统时钟以节省功耗
void reduce_clock_speed(void) {
    // 切换到ROS C (内部RC振荡器)
    clocks_init();

    // 设置更低的系统频率
    set_sys_clock_khz(48000, false);  // 48MHz
    // 或者更低
    // set_sys_clock_khz(12000, false);  // 12MHz
}

// 恢复全速
void restore_full_speed(void) {
    set_sys_clock_khz(133000, true);  // 133MHz
}
```

---

## 十二、实战项目：传感器网关

```c
// ============================================================
// 完整项目: 多传感器无线网关
// 硬件: ESP32 + I2C传感器 + SPI Flash
// 功能: 采集→缓存→WiFi上传
// ============================================================

#include <stdio.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "nvs_flash.h"
#include "driver/i2c.h"
#include "driver/spi_master.h"

// ========== 配置 ==========
#define SENSOR_I2C_ADDR     0x76    // BME280
#define FLASH_CS_PIN        5
#define WIFI_SSID           "YourSSID"
#define WIFI_PASS           "YourPassword"
#define UPLOAD_URL          "http://api.example.com/data"

// ========== 数据结构 ==========
typedef struct {
    uint32_t timestamp;
    float temperature;
    float humidity;
    float pressure;
    uint16_t battery_mv;
} SensorData;

// 环形缓冲区
typedef struct {
    SensorData data[100];
    volatile uint16_t head;
    volatile uint16_t tail;
} DataBuffer;

static DataBuffer buffer;
static QueueHandle_t data_queue;

// ========== I2C传感器驱动 ==========
bool bme280_read(float *temp, float *humi, float *press) {
    uint8_t data[8];

    // 读取数据寄存器
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, (SENSOR_I2C_ADDR << 1) | I2C_MASTER_WRITE, true);
    i2c_master_write_byte(cmd, 0xF7, true);  // 数据起始寄存器
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, (SENSOR_I2C_ADDR << 1) | I2C_MASTER_READ, true);
    i2c_master_read(cmd, data, 8, I2C_MASTER_LAST_NACK);
    i2c_master_stop(cmd);

    esp_err_t ret = i2c_master_cmd_begin(I2C_NUM_0, cmd, pdMS_TO_TICKS(1000));
    i2c_cmd_link_delete(cmd);

    if (ret != ESP_OK) return false;

    // 解析数据 (简化，实际需要补偿计算)
    int32_t adc_p = (data[0] << 12) | (data[1] << 4) | (data[2] >> 4);
    int32_t adc_t = (data[3] << 12) | (data[4] << 4) | (data[5] >> 4);
    int32_t adc_h = (data[6] << 8) | data[7];

    *temp = adc_t / 100.0f;
    *humi = adc_h / 1024.0f;
    *press = adc_p / 256.0f;

    return true;
}

// ========== 任务 ==========

// 传感器采集任务
void sensor_task(void *pvParameters) {
    // 初始化I2C
    i2c_config_t conf = {
        .mode = I2C_MODE_MASTER,
        .sda_io_num = 21,
        .scl_io_num = 22,
        .sda_pullup_en = GPIO_PULLUP_ENABLE,
        .scl_pullup_en = GPIO_PULLUP_ENABLE,
        .master.clk_speed = 100000,
    };
    i2c_param_config(I2C_NUM_0, &conf);
    i2c_driver_install(I2C_NUM_0, conf.mode, 0, 0, 0);

    while (1) {
        SensorData reading;

        if (bme280_read(&reading.temperature, &reading.humidity, &reading.pressure)) {
            reading.timestamp = time(NULL);
            reading.battery_mv = read_battery();  // ADC读取电池电压

            // 发送到队列
            xQueueSend(data_queue, &reading, portMAX_DELAY);
        }

        vTaskDelay(pdMS_TO_TICKS(5000));  // 5秒采样间隔
    }
}

// 存储与上传任务
void storage_task(void *pvParameters) {
    SensorData data;

    while (1) {
        // 等待数据
        if (xQueueReceive(data_queue, &data, portMAX_DELAY)) {
            // 尝试立即上传
            if (wifi_connected()) {
                if (upload_data(&data)) {
                    continue;  // 上传成功
                }
            }

            // 上传失败，存入Flash缓存
            cache_to_flash(&data);
        }
    }
}

// WiFi连接任务
void wifi_task(void *pvParameters) {
    // WiFi初始化 (同前文)
    // ...

    while (1) {
        if (wifi_connected()) {
            // 尝试上传缓存数据
            flush_cached_data();
        }
        vTaskDelay(pdMS_TO_TICKS(30000));  // 30秒检查一次
    }
}

// ========== 主程序 ==========
void app_main(void) {
    // 初始化NVS
    ESP_ERROR_CHECK(nvs_flash_init());

    // 创建队列
    data_queue = xQueueCreate(20, sizeof(SensorData));

    // 创建任务
    xTaskCreate(sensor_task, "sensor", 4096, NULL, 5, NULL);
    xTaskCreate(storage_task, "storage", 4096, NULL, 4, NULL);
    xTaskCreate(wifi_task, "wifi", 8192, NULL, 3, NULL);
}
```

---

> **文档信息**
>
> - 创建时间: 2026-03-10
> - 覆盖平台: STM32F4/G0/H7, ESP32/S3/C3/C6, RP2040/RP2350
> - 协议: GPIO, I2C, SPI, UART, CAN, Modbus, WiFi, BLE
> - 应用领域: 工业控制、物联网、传感器网络、智能家居


---

## 深入理解

### 技术原理

深入探讨相关技术原理和实现细节。

### 实践指南

- 步骤1：理解基础概念
- 步骤2：掌握核心原理
- 步骤3：应用实践

### 相关资源

- 文档链接
- 代码示例
- 参考文章

---

> **最后更新**: 2026-03-21
> **维护者**: AI Code Review
