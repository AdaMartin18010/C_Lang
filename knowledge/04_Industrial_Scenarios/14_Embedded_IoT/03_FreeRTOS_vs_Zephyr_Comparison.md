# FreeRTOS vs Zephyr RTOS 深度对比与迁移指南

## 目录

1. [概述与定位](#1-概述与定位)
2. [架构对比](#2-架构对比)
3. [任务/线程管理对比](#3-任务线程管理对比)
4. [同步原语对比](#4-同步原语对比)
5. [通信机制对比](#5-通信机制对比)
6. [中断处理对比](#6-中断处理对比)
7. [内存管理对比](#7-内存管理对比)
8. [设备驱动模型对比](#8-设备驱动模型对比)
9. [网络协议栈对比](#9-网络协议栈对比)
10. [安全特性对比](#10-安全特性对比)
11. [迁移指南](#11-迁移指南)
12. [决策树](#12-决策树)
13. [代码对比示例](#13-代码对比示例)

---

## 1. 概述与定位

### 1.1 FreeRTOS 概述

FreeRTOS 是由 Amazon Web Services (AWS) 维护的开源实时操作系统内核，发布于2003年，是嵌入式领域最受欢迎的RTOS之一。

**核心特点：**
- **轻量级**：最小内核仅需4-9KB ROM
- **可移植性**：支持35+处理器架构
- **许可证**：MIT许可证（2017年后）
- **生态系统**：Amazon FreeRTOS 提供云端集成

**典型应用场景：**
| 场景 | 说明 |
|------|------|
| 传感器节点 | 低功耗、简单任务调度 |
| 消费电子设备 | 可穿戴设备、智能家居 |
| 工业控制 | PLC、运动控制器 |
| 医疗设备 | 便携式医疗仪器 |
| 汽车电子 | ECU、车身控制模块 |

### 1.2 Zephyr RTOS 概述

Zephyr 是由 Linux 基金会托管的开源实时操作系统，2016年正式发布，旨在为资源受限设备提供安全、可扩展的解决方案。

**核心特点：**
- **现代架构**：基于设备的驱动模型
- **安全性**：内置安全启动、加密支持
- **网络原生**：完整的网络协议栈支持
- **许可证**：Apache 2.0

**典型应用场景：**
| 场景 | 说明 |
|------|------|
| IoT网关 | 边缘计算、协议转换 |
| 智能穿戴 | 复杂UI、传感器融合 |
| 工业物联网 | 确定性通信、安全连接 |
| 汽车电子 | 功能安全、网络安全 |
| 智能城市 | 大规模传感器网络 |

### 1.3 核心定位对比

| 特性 | FreeRTOS | Zephyr RTOS |
|------|----------|-------------|
| **设计哲学** | 极简内核，可选组件 | 全功能平台，开箱即用 |
| **目标市场** | 低端MCU (8-32位) | 中高端MCU (32位+) |
| **内存需求** | 4-9KB ROM | 8-512KB ROM |
| **启动时间** | < 1ms | 10-100ms |
| **功能安全认证** | 可选（需付费） | 内置支持 |
| **网络协议栈** | 可选第三方 | 内置完整支持 |
| **开发模型** | 库集成 | 完整SDK |
| **构建系统** | IDE集成 | CMake + Kconfig |

---

## 2. 架构对比

### 2.1 内核架构

#### FreeRTOS 内核架构

```
┌─────────────────────────────────────────────────────────────┐
│                      应用层 (Application)                    │
├─────────────────────────────────────────────────────────────┤
│  任务管理    │   队列    │   信号量   │   软件定时器          │
├─────────────────────────────────────────────────────────────┤
│                    内核抽象层 (Kernel)                       │
│  ┌──────────┐  ┌──────────┐  ┌──────────┐  ┌──────────┐    │
│  │ 任务调度 │  │ 内存管理 │  │ 中断管理 │  │ 时间管理 │    │
│  └──────────┘  └──────────┘  └──────────┘  └──────────┘    │
├─────────────────────────────────────────────────────────────┤
│                  硬件抽象层 (Port Layer)                     │
│  ┌──────────┐  ┌──────────┐  ┌──────────┐                  │
│  │ 上下文切换│  │  时钟节拍 │  │ 临界区   │                  │
│  └──────────┘  └──────────┘  └──────────┘                  │
├─────────────────────────────────────────────────────────────┤
│                      硬件 (Hardware)                         │
└─────────────────────────────────────────────────────────────┘
```

**设计特点：**
- 抢占式或协作式调度
- 单一地址空间
- 静态或动态任务创建
- 最小化上下文切换开销

#### Zephyr RTOS 架构

```
┌─────────────────────────────────────────────────────────────┐
│                      应用层 (Application)                    │
├─────────────────────────────────────────────────────────────┤
│  网络协议栈  │   文件系统   │   电源管理   │   安全服务     │
├─────────────────────────────────────────────────────────────┤
│                    内核服务层 (Kernel Services)              │
│  ┌──────────┐  ┌──────────┐  ┌──────────┐  ┌──────────┐    │
│  │ 线程管理 │  │ 同步原语 │  │ 内存管理 │  │ 中断管理 │    │
│  └──────────┘  └──────────┘  └──────────┘  └──────────┘    │
├─────────────────────────────────────────────────────────────┤
│                    设备驱动层 (Device Drivers)               │
│  ┌──────────┐  ┌──────────┐  ┌──────────┐  ┌──────────┐    │
│  │ GPIO/I2C │  │  UART/SPI │  │ 网络设备  │  │ 传感器   │    │
│  └──────────┘  └──────────┘  └──────────┘  └──────────┘    │
├─────────────────────────────────────────────────────────────┤
│                 板级支持包 (Board Support)                   │
│  ┌──────────┐  ┌──────────┐  ┌──────────┐                  │
│  │ 设备树   │  │ Kconfig  │  │ SoC定义  │                  │
│  └──────────┘  └──────────┘  └──────────┘                  │
├─────────────────────────────────────────────────────────────┤
│                      硬件 (Hardware)                         │
└─────────────────────────────────────────────────────────────┘
```

**设计特点：**
- 统一设备模型
- 设备树配置
- 模块化组件系统
- 多线程安全设计

### 2.2 架构对比表

| 架构特性 | FreeRTOS | Zephyr RTOS |
|----------|----------|-------------|
| **内核类型** | 微内核 | 宏内核 |
| **调度器** | 优先级抢占式 | 多调度器可选 |
| **任务/线程** | 任务(Task) | 线程(Thread) |
| **地址空间** | 单地址空间 | 支持MPU隔离 |
| **设备抽象** | 无（应用层实现） | 统一设备模型 |
| **配置方式** | FreeRTOSConfig.h | Kconfig + Devicetree |
| **模块化** | 可选组件 | 高度模块化 |
| **POSIX支持** | 有限 | 完整POSIX子集 |

### 2.3 内存模型对比

#### FreeRTOS 内存模型

```c
/* FreeRTOSConfig.h - 内存配置 */
#define configTOTAL_HEAP_SIZE           ((size_t)(32 * 1024))
#define configUSE_HEAP_SCHEME           4

/* 五种堆分配方案 */
/* Heap_1: 仅分配，不释放 - 最简单 */
/* Heap_2: 最佳匹配算法 - 已废弃 */
/* Heap_3: 使用标准库malloc/free - 线程安全 */
/* Heap_4: 首次适配算法 - 支持合并 */
/* Heap_5: Heap_4 + 跨多个内存区域 */
```

**内存布局：**
```
┌──────────────────┐ 高地址
│     栈空间        │ ← 向下增长
├──────────────────┤
│     堆空间        │ ← 向上增长
├──────────────────┤
│   .bss (未初始化) │
├──────────────────┤
│   .data (已初始化)│
├──────────────────┤
│   .text (代码)   │
└──────────────────┘ 低地址
```

#### Zephyr 内存模型

```c
/* prj.conf - 内存配置 */
CONFIG_MAIN_STACK_SIZE=2048
CONFIG_IDLE_STACK_SIZE=256
CONFIG_ISR_STACK_SIZE=2048
CONFIG_SYSTEM_WORKQUEUE_STACK_SIZE=2048
CONFIG_HEAP_MEM_POOL_SIZE=8192

/* 内存区域配置 */
CONFIG_SRAM_SIZE=256
CONFIG_FLASH_SIZE=512
```

**内存布局：**
```
┌─────────────────────────────┐ 高地址
│      中断栈 (ISR Stack)      │
├─────────────────────────────┤
│     系统工作队列栈           │
├─────────────────────────────┤
│     主线程栈                 │
├─────────────────────────────┤
│     应用线程栈区域           │
├─────────────────────────────┤
│     堆内存池 (Heap)          │
├─────────────────────────────┤
│     .noinit (不掉电保持)      │
├─────────────────────────────┤
│     .bss (未初始化数据)       │
├─────────────────────────────┤
│     .data (已初始化数据)      │
├─────────────────────────────┤
│     代码段 (.text)           │
├─────────────────────────────┤
│     只读数据 (.rodata)       │
└─────────────────────────────┘ 低地址
```

#### 内存模型对比表

| 内存特性 | FreeRTOS | Zephyr RTOS |
|----------|----------|-------------|
| **堆方案** | 5种可选 | 统一内存池 |
| **静态分配** | 支持 | 推荐 |
| **动态分配** | 支持 | 支持（可选） |
| **内存保护** | MPU可选 | MPU内置支持 |
| **内存区域** | 单一连续 | 多区域支持 |
| **栈溢出检测** | 可选 | 内置 |
| **堆溢出检测** | 可选 | 内置 |
| **内存碎片** | Heap_4/5处理 | 内存池管理 |

---

## 3. 任务/线程管理对比

### 3.1 核心概念对比

| 概念 | FreeRTOS | Zephyr RTOS |
|------|----------|-------------|
| **基本单位** | Task（任务） | Thread（线程） |
| **创建方式** | xTaskCreate / xTaskCreateStatic | k_thread_create |
| **删除方式** | vTaskDelete | k_thread_abort |
| **优先级范围** | 0 (最低) 至 configMAX_PRIORITIES-1 | -CONFIG_NUM_COOP_PRIORITIES 至 CONFIG_NUM_PREEMPT_PRIOPRIORITIES-1 |
| **默认优先级** | tskIDLE_PRIORITY (0) | K_PRIO_PREEMPT(0) |
| **调度策略** | 抢占式/协作式 | 合作式/抢占式/时间片 |
| **状态** | Running/Ready/Blocked/Suspended | Running/Ready/Pending/Suspended |

### 3.2 FreeRTOS 任务管理

#### 任务创建与删除

```c
/* 
 * FreeRTOS 任务管理完整示例
 * 文件: freertos_task_example.c
 * 编译: gcc -o freertos_task_example freertos_task_example.c -lfreertos
 */

#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include <stdio.h>
#include <string.h>

/* 任务参数结构体 - 用于传递多个参数 */
typedef struct {
    uint32_t task_id;
    uint32_t interval_ms;
    const char* name;
} TaskParams_t;

/* 静态任务分配 */
#define STATIC_TASK_STACK_SIZE  256
#define STATIC_TASK_PRIORITY    2

/* 静态任务TCB和栈 */
static StaticTask_t xIdleTaskTCB;
static StaticTask_t xTimerTaskTCB;
static StackType_t uxIdleTaskStack[configMINIMAL_STACK_SIZE];
static StackType_t uxTimerTaskStack[configTIMER_TASK_STACK_DEPTH];

/* 应用任务 */
static TaskHandle_t xTaskHandles[3] = {NULL};
static StaticTask_t xTaskTCBs[3];
static StackType_t xTaskStacks[3][256];

/* 同步信号量 */
static SemaphoreHandle_t xStartSemaphore = NULL;

/*
 * 任务1: 周期性任务
 * 功能: 周期性执行工作，演示基本任务结构
 */
static void vPeriodicTask(void *pvParameters)
{
    TaskParams_t *params = (TaskParams_t *)pvParameters;
    TickType_t xLastWakeTime;
    const TickType_t xFrequency = pdMS_TO_TICKS(params->interval_ms);
    uint32_t execution_count = 0;
    
    /* 等待启动信号 */
    xSemaphoreTake(xStartSemaphore, portMAX_DELAY);
    
    /* 初始化上次唤醒时间 */
    xLastWakeTime = xTaskGetTickCount();
    
    printf("[Task %s] Started, ID=%lu, Interval=%lu ms\n", 
           params->name, params->task_id, params->interval_ms);
    
    for (;;) {
        /* 周期性执行 - 使用vTaskDelayUntil确保精确周期 */
        vTaskDelayUntil(&xLastWakeTime, xFrequency);
        
        execution_count++;
        printf("[Task %s] Execution #%lu at tick %lu\n", 
               params->name, execution_count, xTaskGetTickCount());
        
        /* 模拟工作负载 */
        {
            volatile uint32_t i;
            for (i = 0; i < 10000; i++);
        }
        
        /* 优先级继承演示 */
        if (execution_count >= 10) {
            printf("[Task %s] Reached max executions, suspending\n", params->name);
            vTaskSuspend(NULL);  /* 挂起自身 */
        }
    }
}

/*
 * 任务2: 事件驱动任务
 * 功能: 等待通知后执行工作
 */
static void vEventDrivenTask(void *pvParameters)
{
    TaskParams_t *params = (TaskParams_t *)pvParameters;
    uint32_t ulNotificationValue;
    const TickType_t xMaxBlockTime = pdMS_TO_TICKS(5000);
    
    printf("[Task %s] Event-driven task started\n", params->name);
    
    for (;;) {
        /* 等待任务通知，带超时 */
        ulNotificationValue = ulTaskNotifyTake(pdTRUE, xMaxBlockTime);
        
        if (ulNotificationValue > 0) {
            printf("[Task %s] Received notification, value=%lu\n", 
                   params->name, ulNotificationValue);
            
            /* 处理事件 */
            switch (ulNotificationValue) {
                case 1:
                    printf("[Task %s] Processing EVENT_TYPE_A\n", params->name);
                    break;
                case 2:
                    printf("[Task %s] Processing EVENT_TYPE_B\n", params->name);
                    break;
                default:
                    printf("[Task %s] Processing UNKNOWN_EVENT\n", params->name);
                    break;
            }
        } else {
            printf("[Task %s] Timeout waiting for notification\n", params->name);
        }
    }
}

/*
 * 任务3: 一次性初始化任务
 * 功能: 执行系统初始化后删除自身
 */
static void vInitTask(void *pvParameters)
{
    (void)pvParameters;
    
    printf("[InitTask] System initialization started\n");
    
    /* 执行硬件初始化 */
    printf("[InitTask] Initializing peripherals...\n");
    vTaskDelay(pdMS_TO_TICKS(100));
    
    /* 初始化软件组件 */
    printf("[InitTask] Initializing software components...\n");
    vTaskDelay(pdMS_TO_TICKS(100));
    
    /* 释放启动信号量，允许其他任务运行 */
    printf("[InitTask] Releasing start semaphore\n");
    xSemaphoreGive(xStartSemaphore);
    
    printf("[InitTask] Initialization complete, deleting self\n");
    
    /* 删除自身 - 任务结束 */
    vTaskDelete(NULL);
}

/*
 * 空闲任务钩子
 * 功能: 在系统空闲时执行低优先级工作
 */
void vApplicationIdleHook(void)
{
    static uint32_t idle_count = 0;
    idle_count++;
    
    /* 每1000次空闲调用执行一次 */
    if (idle_count >= 1000) {
        idle_count = 0;
        /* 可执行节能操作或后台任务 */
    }
}

/*
 * 栈溢出钩子
 * 功能: 检测任务栈溢出时调用
 */
void vApplicationStackOverflowHook(TaskHandle_t xTask, char *pcTaskName)
{
    printf("[ERROR] Stack overflow in task: %s\n", pcTaskName);
    /* 在此可添加错误处理，如系统复位 */
    for (;;);
}

/*
 * 内存分配失败钩子
 */
void vApplicationMallocFailedHook(void)
{
    printf("[ERROR] Memory allocation failed\n");
    for (;;);
}

/*
 * 静态分配回调函数
 * 用于FreeRTOS使用静态分配时提供内存
 */
void vApplicationGetIdleTaskMemory(StaticTask_t **ppxIdleTaskTCBBuffer,
                                    StackType_t **ppxIdleTaskStackBuffer,
                                    uint32_t *pulIdleTaskStackSize)
{
    *ppxIdleTaskTCBBuffer = &xIdleTaskTCB;
    *ppxIdleTaskStackBuffer = uxIdleTaskStack;
    *pulIdleTaskStackSize = configMINIMAL_STACK_SIZE;
}

void vApplicationGetTimerTaskMemory(StaticTask_t **ppxTimerTaskTCBBuffer,
                                     StackType_t **ppxTimerTaskStackBuffer,
                                     uint32_t *pulTimerTaskStackSize)
{
    *ppxTimerTaskTCBBuffer = &xTimerTaskTCB;
    *ppxTimerTaskStackBuffer = uxTimerTaskStack;
    *pulTimerTaskStackSize = configTIMER_TASK_STACK_DEPTH;
}

/* 任务参数 */
static TaskParams_t xTaskParams[3] = {
    {1, 100, "Periodic100"},
    {2, 500, "Periodic500"},
    {3, 0,   "EventDriven"}
};

/*
 * 主函数
 */
int main(void)
{
    printf("\n=== FreeRTOS Task Management Demo ===\n\n");
    
    /* 创建启动信号量 */
    xStartSemaphore = xSemaphoreCreateBinary();
    configASSERT(xStartSemaphore != NULL);
    
    /* 创建初始化任务 - 最高优先级 */
    xTaskCreate(vInitTask,
                "Init",
                256,
                NULL,
                configMAX_PRIORITIES - 1,
                NULL);
    
    /* 创建周期任务1 - 静态分配 */
    xTaskHandles[0] = xTaskCreateStatic(
        vPeriodicTask,
        "Periodic100",
        256,
        &xTaskParams[0],
        3,
        xTaskStacks[0],
        &xTaskTCBs[0]
    );
    configASSERT(xTaskHandles[0] != NULL);
    
    /* 创建周期任务2 - 动态分配 */
    BaseType_t result = xTaskCreate(
        vPeriodicTask,
        "Periodic500",
        256,
        &xTaskParams[1],
        2,
        &xTaskHandles[1]
    );
    configASSERT(result == pdPASS);
    
    /* 创建事件驱动任务 */
    result = xTaskCreate(
        vEventDrivenTask,
        "EventTask",
        256,
        &xTaskParams[2],
        1,
        &xTaskHandles[2]
    );
    configASSERT(result == pdPASS);
    
    printf("[Main] All tasks created, starting scheduler...\n\n");
    
    /* 启动调度器 */
    vTaskStartScheduler();
    
    /* 不应该到达这里 */
    printf("[ERROR] Scheduler returned unexpectedly\n");
    return 1;
}
```

#### FreeRTOS 任务状态机

```
                    ┌─────────────┐
                    │   不存在    │
                    └──────┬──────┘
                           │ xTaskCreate
                           ▼
                    ┌─────────────┐
            ┌──────│    就绪     │◄──────────────┐
            │      │   Ready     │               │
            │      └──────┬──────┘               │
            │             │ 调度器选择           │
            │             ▼                     │
            │      ┌─────────────┐              │
            │      │    运行     │──────────────┘
            │      │  Running    │ 被抢占/yield
            │      └──────┬──────┘
            │             │ 等待事件/延时
            │             ▼
            │      ┌─────────────┐     ┌─────────────┐
            └─────►│    阻塞     │────►│   挂起      │
                   │  Blocked    │     │ Suspended   │
                   └─────────────┘     └─────────────┘
                           │                  │
                           │ 事件发生         │ vTaskResume
                           └──────────────────┘
```

### 3.3 Zephyr RTOS 线程管理

#### 线程创建与删除

```c
/* 
 * Zephyr RTOS 线程管理完整示例
 * 文件: zephyr_thread_example.c
 * 配置: prj.conf
 */

#include <zephyr/kernel.h>
#include <zephyr/sys/printk.h>
#include <zephyr/logging/log.h>

LOG_MODULE_REGISTER(thread_example, LOG_LEVEL_DBG);

/* 线程参数结构体 */
struct thread_params {
    uint32_t thread_id;
    uint32_t interval_ms;
    const char *name;
};

/* 线程栈大小定义 */
#define THREAD_STACK_SIZE       2048
#define THREAD_PRIORITY_HIGH    K_PRIO_PREEMPT(0)
#define THREAD_PRIORITY_NORMAL  K_PRIO_PREEMPT(5)
#define THREAD_PRIORITY_LOW     K_PRIO_PREEMPT(10)
#define THREAD_PRIORITY_COOP    K_PRIO_COOP(1)

/* 线程栈空间 - 必须使用K_THREAD_STACK_DEFINE */
K_THREAD_STACK_DEFINE(thread_stack_1, THREAD_STACK_SIZE);
K_THREAD_STACK_DEFINE(thread_stack_2, THREAD_STACK_SIZE);
K_THREAD_STACK_DEFINE(thread_stack_3, THREAD_STACK_SIZE);
K_THREAD_STACK_DEFINE(workq_stack, THREAD_STACK_SIZE);

/* 线程控制块 */
static struct k_thread thread_data[3];
static struct k_thread workq_thread;

/* 同步对象 */
static K_SEM_DEFINE(start_sem, 0, 1);
static K_EVENT_DEFINE(event_handle);
static K_MUTEX_DEFINE(resource_mutex);

/* 线程参数 */
static struct thread_params thread_params[3] = {
    {1, 100, "Periodic100"},
    {2, 500, "Periodic500"},
    {3, 0,   "EventDriven"}
};

/* 工作队列 */
static struct k_work_q my_work_q;

/*
 * 线程1: 周期性线程
 * 功能: 使用k_sleep实现精确周期
 */
static void periodic_thread(void *p1, void *p2, void *p3)
{
    struct thread_params *params = p1;
    uint32_t execution_count = 0;
    int64_t last_time, current_time;
    
    ARG_UNUSED(p2);
    ARG_UNUSED(p3);
    
    /* 等待启动信号 */
    k_sem_take(&start_sem, K_FOREVER);
    
    LOG_INF("[Thread %s] Started, ID=%u, Interval=%u ms", 
            params->name, params->thread_id, params->interval_ms);
    
    last_time = k_uptime_get();
    
    while (1) {
        /* 精确周期睡眠 */
        k_sleep(K_MSEC(params->interval_ms));
        
        current_time = k_uptime_get();
        execution_count++;
        
        LOG_INF("[Thread %s] Execution #%u at time %lld ms (jitter=%lld ms)", 
                params->name, execution_count, current_time,
                current_time - last_time - params->interval_ms);
        
        last_time = current_time;
        
        /* 模拟工作负载 */
        k_busy_wait(1000);  /* 1ms忙等待 */
        
        /* 条件挂起演示 */
        if (execution_count >= 10) {
            LOG_INF("[Thread %s] Reached max executions, aborting", params->name);
            k_thread_abort(k_current_get());
        }
    }
}

/*
 * 线程2: 事件驱动线程
 * 功能: 使用k_poll等待多个事件源
 */
static void event_driven_thread(void *p1, void *p2, void *p3)
{
    struct thread_params *params = p1;
    struct k_poll_event events[2];
    int rc;
    
    ARG_UNUSED(p2);
    ARG_UNUSED(p3);
    
    LOG_INF("[Thread %s] Event-driven thread started", params->name);
    
    /* 配置poll事件 */
    k_poll_event_init(&events[0], K_POLL_TYPE_SEM_AVAILABLE,
                      K_POLL_MODE_NOTIFY_ONLY, &start_sem);
    k_poll_event_init(&events[1], K_POLL_TYPE_FIFO_DATA_AVAILABLE,
                      K_POLL_MODE_NOTIFY_ONLY, NULL);
    
    while (1) {
        /* 等待多个事件源 */
        rc = k_poll(events, 1, K_MSEC(5000));
        
        if (rc == 0) {
            /* 处理事件 */
            if (events[0].state == K_POLL_STATE_SEM_AVAILABLE) {
                k_sem_take(&start_sem, K_NO_WAIT);
                LOG_INF("[Thread %s] Received semaphore event", params->name);
            }
            
            /* 重置事件状态 */
            events[0].state = K_POLL_STATE_NOT_READY;
        } else if (rc == -EAGAIN) {
            LOG_WRN("[Thread %s] Poll timeout", params->name);
        }
    }
}

/*
 * 线程3: 工作线程
 * 功能: 执行耗时后台任务
 */
static void worker_thread(void *p1, void *p2, void *p3)
{
    ARG_UNUSED(p1);
    ARG_UNUSED(p2);
    ARG_UNUSED(p3);
    
    LOG_INF("[WorkerThread] Started");
    
    while (1) {
        /* 等待工作 */
        k_sleep(K_MSEC(100));
        
        /* 获取互斥锁保护共享资源 */
        if (k_mutex_lock(&resource_mutex, K_MSEC(100)) == 0) {
            LOG_DBG("[WorkerThread] Accessing shared resource");
            
            /* 访问共享资源 */
            k_busy_wait(5000);  /* 5ms工作 */
            
            k_mutex_unlock(&resource_mutex);
        }
    }
}

/*
 * 系统工作队列处理函数
 */
static void work_handler(struct k_work *work)
{
    ARG_UNUSED(work);
    LOG_INF("[WorkQueue] Processing deferred work");
}

static K_WORK_DEFINE(my_work, work_handler);

/*
 * 初始化线程
 * 功能: 系统初始化后退出
 */
static void init_thread(void *p1, void *p2, void *p3)
{
    ARG_UNUSED(p1);
    ARG_UNUSED(p2);
    ARG_UNUSED(p3);
    
    LOG_INF("[InitThread] System initialization started");
    
    /* 执行初始化 */
    k_sleep(K_MSEC(100));
    LOG_INF("[InitThread] Hardware initialized");
    
    k_sleep(K_MSEC(100));
    LOG_INF("[InitThread] Software components initialized");
    
    /* 提交延迟工作 */
    k_work_submit(&my_work);
    
    /* 释放启动信号 */
    LOG_INF("[InitThread] Releasing start semaphore");
    k_sem_give(&start_sem);
    
    LOG_INF("[InitThread] Initialization complete");
    
    /* 线程自然退出 - 不需要显式abort */
}

/*
 * 线程监控回调
 * 功能: 当线程启动/终止时调用
 */
static void thread_monitor_callback(const struct k_thread *thread,
                                     enum k_thread_state state)
{
    const char *state_str;
    
    switch (state) {
        case K_THREAD_DEAD:
            state_str = "DEAD";
            break;
        case K_THREAD_SUSPENDED:
            state_str = "SUSPENDED";
            break;
        case K_THREAD_READY:
            state_str = "READY";
            break;
        case K_THREAD_RUNNING:
            state_str = "RUNNING";
            break;
        case K_THREAD_PENDING:
            state_str = "PENDING";
            break;
        default:
            state_str = "UNKNOWN";
            break;
    }
    
    LOG_DBG("[Monitor] Thread %p state changed to %s", thread, state_str);
}

/*
 * 主函数
 */
int main(void)
{
    printk("\n=== Zephyr RTOS Thread Management Demo ===\n\n");
    
    /* 创建初始化线程 - 最高优先级 */
    k_thread_create(&thread_data[0], init_thread_stack, THREAD_STACK_SIZE,
                    init_thread, NULL, NULL, NULL,
                    K_HIGHEST_THREAD_PRIO, 0, K_NO_WAIT);
    
    /* 设置线程名称 */
    k_thread_name_set(&thread_data[0], "init_thread");
    
    /* 创建周期线程1 */
    k_thread_create(&thread_data[0], thread_stack_1, THREAD_STACK_SIZE,
                    periodic_thread, &thread_params[0], NULL, NULL,
                    THREAD_PRIORITY_HIGH, 0, K_FOREVER);
    k_thread_name_set(&thread_data[0], "periodic_100");
    
    /* 创建周期线程2 */
    k_thread_create(&thread_data[1], thread_stack_2, THREAD_STACK_SIZE,
                    periodic_thread, &thread_params[1], NULL, NULL,
                    THREAD_PRIORITY_NORMAL, 0, K_FOREVER);
    k_thread_name_set(&thread_data[1], "periodic_500");
    
    /* 创建事件驱动线程 */
    k_thread_create(&thread_data[2], thread_stack_3, THREAD_STACK_SIZE,
                    event_driven_thread, &thread_params[2], NULL, NULL,
                    THREAD_PRIORITY_LOW, 0, K_FOREVER);
    k_thread_name_set(&thread_data[2], "event_driven");
    
    /* 启动工作队列 */
    k_work_q_start(&my_work_q, workq_stack, 
                   K_THREAD_STACK_SIZEOF(workq_stack),
                   THREAD_PRIORITY_COOP);
    
    /* 启动创建的线程 */
    k_thread_start(&thread_data[0]);
    k_thread_start(&thread_data[1]);
    k_thread_start(&thread_data[2]);
    
    LOG_INF("[Main] All threads created, entering main loop");
    
    /* 主线程保持运行 */
    while (1) {
        k_sleep(K_SECONDS(1));
        
        /* 打印线程统计信息 */
        for (int i = 0; i < 3; i++) {
            struct k_thread *thread = &thread_data[i];
            LOG_DBG("[Main] Thread %d state: %d", i, 
                    thread->base.thread_state);
        }
    }
    
    return 0;
}
```

#### Zephyr 线程状态机

```
                    ┌─────────────┐
                    │   未创建    │
                    └──────┬──────┘
                           │ k_thread_create
                           ▼
                    ┌─────────────┐
            ┌──────│    就绪     │◄────────────────────┐
            │      │   READY     │                     │
            │      └──────┬──────┘                     │
            │             │ 调度器选择                │
            │             ▼                           │
            │      ┌─────────────┐                    │
            │      │    运行     │────────────────────┘
            │      │  RUNNING    │ 被抢占/时间片用完
            │      └──────┬──────┘
            │             │ 等待资源/事件
            │             ▼
            │      ┌─────────────┐
            └─────►│    等待     │───────┐
                   │   PENDING   │       │ k_wakeup
                   └─────────────┘       │
                           │             │
                           │ 资源可用     │
                           ▼             │
                    ┌─────────────┐      │
                    │   挂起      │──────┘
                    │  SUSPENDED  │ k_thread_resume
                    └─────────────┘
                           │
                           │ k_thread_abort
                           ▼
                    ┌─────────────┐
                    │   终止      │
                    │    DEAD     │
                    └─────────────┘
```

### 3.4 任务/线程管理对比表

| 功能 | FreeRTOS API | Zephyr API | 说明 |
|------|-------------|-----------|------|
| **创建** | `xTaskCreate()` | `k_thread_create()` | Zephyr需预分配栈 |
| **删除** | `vTaskDelete()` | `k_thread_abort()` | Zephyr更强制 |
| **挂起** | `vTaskSuspend()` | `k_thread_suspend()` | 功能相同 |
| **恢复** | `vTaskResume()` | `k_thread_resume()` | 功能相同 |
| **延时** | `vTaskDelay()` | `k_sleep()` | Zephyr支持ns级 |
| **精确延时** | `vTaskDelayUntil()` | `k_sleep()` + 补偿 | FreeRTOS专用 |
| **获取ID** | `xTaskGetCurrentTaskHandle()` | `k_current_get()` | - |
| **优先级设置** | `vTaskPrioritySet()` | `k_thread_priority_set()` | - |
| **优先级获取** | `uxTaskPriorityGet()` | `k_thread_priority_get()` | - |
| **让出CPU** | `taskYIELD()` | `k_yield()` | - |
| **栈检查** | `uxTaskGetStackHighWaterMark()` | `k_thread_stack_space_get()` | - |
| **名称设置** | `pcTaskName`参数 | `k_thread_name_set()` | Zephyr独立API |

---

## 4. 同步原语对比

### 4.1 同步机制概览

| 同步原语 | FreeRTOS | Zephyr RTOS |
|----------|----------|-------------|
| **二值信号量** | `SemaphoreHandle_t` | `K_SEM_DEFINE` |
| **计数信号量** | `xSemaphoreCreateCounting()` | `k_sem_init()` |
| **互斥锁** | `SemaphoreHandle_t` (mutex) | `K_MUTEX_DEFINE` |
| **递归互斥锁** | `xSemaphoreCreateRecursiveMutex()` | 不支持 |
| **事件组** | `EventGroupHandle_t` | `K_EVENT_DEFINE` |
| **条件变量** | 无原生支持 | `k_condvar` |
| **读写锁** | 无 | `k_rwlock` |
| **屏障** | 无 | `k_barrier` |

### 4.2 信号量对比

#### FreeRTOS 信号量

```c
/* 
 * FreeRTOS 信号量完整示例
 * 文件: freertos_semaphore_example.c
 */

#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include <stdio.h>

/* 信号量句柄 */
static SemaphoreHandle_t xBinarySem = NULL;
static SemaphoreHandle_t xCountingSem = NULL;
static SemaphoreHandle_t xMutex = NULL;
static SemaphoreHandle_t xRecursiveMutex = NULL;

/* 资源计数 */
static volatile uint32_t shared_resource = 0;

/*
 * 二值信号量使用示例
 * 应用场景: 任务同步、中断通知
 */
static void vBinarySemaphoreTask(void *pvParameters)
{
    (void)pvParameters;
    const TickType_t xMaxBlockTime = pdMS_TO_TICKS(1000);
    
    printf("[BinarySemTask] Waiting for semaphore...\n");
    
    for (;;) {
        /* 等待信号量 - 可从中断或另一任务释放 */
        if (xSemaphoreTake(xBinarySem, xMaxBlockTime) == pdTRUE) {
            printf("[BinarySemTask] Semaphore received, processing...\n");
            
            /* 执行需要同步的操作 */
            vTaskDelay(pdMS_TO_TICKS(100));
            
            printf("[BinarySemTask] Processing complete\n");
        } else {
            printf("[BinarySemTask] Timeout waiting for semaphore\n");
        }
    }
}

/*
 * 计数信号量使用示例
 * 应用场景: 资源池管理、事件计数
 */
static void vCountingSemaphoreProducer(void *pvParameters)
{
    (void)pvParameters;
    uint32_t produced_count = 0;
    
    for (;;) {
        /* 模拟生产资源 */
        vTaskDelay(pdMS_TO_TICKS(200));
        produced_count++;
        
        /* 释放计数信号量 - 表示可用资源 */
        if (xSemaphoreGive(xCountingSem) == pdTRUE) {
            printf("[Producer] Produced item #%lu, sem count available\n", 
                   produced_count);
        } else {
            printf("[Producer] Semaphore full, item dropped\n");
        }
    }
}

static void vCountingSemaphoreConsumer(void *pvParameters)
{
    (void)pvParameters;
    uint32_t consumed_count = 0;
    
    for (;;) {
        /* 等待可用资源 */
        if (xSemaphoreTake(xCountingSem, portMAX_DELAY) == pdTRUE) {
            consumed_count++;
            printf("[Consumer] Consumed item #%lu\n", consumed_count);
            
            /* 处理资源 */
            vTaskDelay(pdMS_TO_TICKS(500));
        }
    }
}

/*
 * 互斥锁使用示例
 * 应用场景: 保护共享资源
 */
static void vMutexTaskA(void *pvParameters)
{
    (void)pvParameters;
    const TickType_t xDelay = pdMS_TO_TICKS(100);
    
    for (;;) {
        /* 获取互斥锁 - 带超时 */
        if (xSemaphoreTake(xMutex, xDelay) == pdTRUE) {
            printf("[MutexTaskA] Acquired mutex\n");
            
            /* 访问共享资源 */
            uint32_t temp = shared_resource;
            vTaskDelay(pdMS_TO_TICKS(10));
            shared_resource = temp + 1;
            printf("[MutexTaskA] Resource incremented to %lu\n", shared_resource);
            
            /* 释放互斥锁 */
            xSemaphoreGive(xMutex);
            printf("[MutexTaskA] Released mutex\n");
        } else {
            printf("[MutexTaskA] Failed to acquire mutex\n");
        }
        
        vTaskDelay(pdMS_TO_TICKS(200));
    }
}

static void vMutexTaskB(void *pvParameters)
{
    (void)pvParameters;
    
    for (;;) {
        /* 获取互斥锁 */
        if (xSemaphoreTake(xMutex, portMAX_DELAY) == pdTRUE) {
            printf("[MutexTaskB] Acquired mutex\n");
            
            /* 访问共享资源 */
            shared_resource *= 2;
            printf("[MutexTaskB] Resource doubled to %lu\n", shared_resource);
            
            xSemaphoreGive(xMutex);
            printf("[MutexTaskB] Released mutex\n");
        }
        
        vTaskDelay(pdMS_TO_TICKS(300));
    }
}

/*
 * 递归互斥锁使用示例
 * 应用场景: 递归调用保护
 */
static void recursive_function(int depth);

static void vRecursiveMutexTask(void *pvParameters)
{
    (void)pvParameters;
    
    for (;;) {
        printf("[RecursiveTask] Starting recursive operation\n");
        recursive_function(3);
        printf("[RecursiveTask] Recursive operation complete\n");
        
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

static void recursive_function(int depth)
{
    if (depth <= 0) {
        return;
    }
    
    /* 获取递归互斥锁 - 同一任务可多次获取 */
    xSemaphoreTakeRecursive(xRecursiveMutex, portMAX_DELAY);
    printf("[Recursive] Depth %d: Acquired recursive mutex\n", depth);
    
    /* 递归调用 */
    recursive_function(depth - 1);
    
    /* 释放递归互斥锁 - 必须与获取次数相同 */
    printf("[Recursive] Depth %d: Releasing recursive mutex\n", depth);
    xSemaphoreGiveRecursive(xRecursiveMutex);
}

/*
 * 中断中的信号量操作
 */
void vApplicationTickHook(void)
{
    static uint32_t tick_count = 0;
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;
    
    tick_count++;
    
    /* 每500个tick释放一次二值信号量 */
    if (tick_count >= 500) {
        tick_count = 0;
        xSemaphoreGiveFromISR(xBinarySem, &xHigherPriorityTaskWoken);
        
        /* 上下文切换（如果需要） */
        portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
    }
}

int main(void)
{
    printf("\n=== FreeRTOS Semaphore Demo ===\n\n");
    
    /* 创建二值信号量 */
    xBinarySem = xSemaphoreCreateBinary();
    configASSERT(xBinarySem != NULL);
    
    /* 创建计数信号量 - 最大10，初始0 */
    xCountingSem = xSemaphoreCreateCounting(10, 0);
    configASSERT(xCountingSem != NULL);
    
    /* 创建互斥锁 */
    xMutex = xSemaphoreCreateMutex();
    configASSERT(xMutex != NULL);
    
    /* 创建递归互斥锁 */
    xRecursiveMutex = xSemaphoreCreateRecursiveMutex();
    configASSERT(xRecursiveMutex != NULL);
    
    /* 创建任务 */
    xTaskCreate(vBinarySemaphoreTask, "BinarySem", 256, NULL, 2, NULL);
    xTaskCreate(vCountingSemaphoreProducer, "Producer", 256, NULL, 2, NULL);
    xTaskCreate(vCountingSemaphoreConsumer, "Consumer", 256, NULL, 2, NULL);
    xTaskCreate(vMutexTaskA, "MutexA", 256, NULL, 2, NULL);
    xTaskCreate(vMutexTaskB, "MutexB", 256, NULL, 2, NULL);
    xTaskCreate(vRecursiveMutexTask, "Recursive", 256, NULL, 2, NULL);
    
    vTaskStartScheduler();
    
    return 0;
}
```

#### Zephyr 信号量

```c
/* 
 * Zephyr RTOS 信号量完整示例
 * 文件: zephyr_semaphore_example.c
 */

#include <zephyr/kernel.h>
#include <zephyr/sys/printk.h>
#include <zephyr/logging/log.h>

LOG_MODULE_REGISTER(semaphore_example, LOG_LEVEL_DBG);

/* 定义信号量 */
static K_SEM_DEFINE(binary_sem, 0, 1);      /* 二值信号量 */
static K_SEM_DEFINE(counting_sem, 0, 10);   /* 计数信号量 */
static K_MUTEX_DEFINE(resource_mutex);      /* 互斥锁 */

/* 共享资源 */
static volatile uint32_t shared_resource = 0;

/*
 * 二值信号量线程
 */
static void binary_sem_thread(void *p1, void *p2, void *p3)
{
    ARG_UNUSED(p1);
    ARG_UNUSED(p2);
    ARG_UNUSED(p3);
    
    LOG_INF("[BinarySemThread] Waiting for semaphore...");
    
    while (1) {
        /* 等待信号量 - 支持超时 */
        int ret = k_sem_take(&binary_sem, K_MSEC(1000));
        
        if (ret == 0) {
            LOG_INF("[BinarySemThread] Semaphore received, processing...");
            
            /* 执行操作 */
            k_sleep(K_MSEC(100));
            
            LOG_INF("[BinarySemThread] Processing complete");
        } else {
            LOG_WRN("[BinarySemThread] Timeout waiting for semaphore");
        }
    }
}

/*
 * 计数信号量生产者线程
 */
static void counting_producer(void *p1, void *p2, void *p3)
{
    ARG_UNUSED(p1);
    ARG_UNUSED(p2);
    ARG_UNUSED(p3);
    
    uint32_t produced_count = 0;
    
    while (1) {
        k_sleep(K_MSEC(200));
        produced_count++;
        
        /* 释放计数信号量 */
        int ret = k_sem_give(&counting_sem);
        
        if (ret == 0) {
            LOG_INF("[Producer] Produced item #%u, count=%u", 
                    produced_count, k_sem_count_get(&counting_sem));
        } else {
            LOG_WRN("[Producer] Semaphore full, item dropped");
        }
    }
}

/*
 * 计数信号量消费者线程
 */
static void counting_consumer(void *p1, void *p2, void *p3)
{
    ARG_UNUSED(p1);
    ARG_UNUSED(p2);
    ARG_UNUSED(p3);
    
    uint32_t consumed_count = 0;
    
    while (1) {
        /* 等待可用资源 */
        k_sem_take(&counting_sem, K_FOREVER);
        
        consumed_count++;
        LOG_INF("[Consumer] Consumed item #%u", consumed_count);
        
        /* 处理资源 */
        k_sleep(K_MSEC(500));
    }
}

/*
 * 互斥锁保护线程A
 */
static void mutex_thread_a(void *p1, void *p2, void *p3)
{
    ARG_UNUSED(p1);
    ARG_UNUSED(p2);
    ARG_UNUSED(p3);
    
    while (1) {
        /* 获取互斥锁 - 带超时 */
        int ret = k_mutex_lock(&resource_mutex, K_MSEC(100));
        
        if (ret == 0) {
            LOG_INF("[MutexThreadA] Acquired mutex");
            
            /* 访问共享资源 */
            uint32_t temp = shared_resource;
            k_sleep(K_MSEC(10));
            shared_resource = temp + 1;
            LOG_INF("[MutexThreadA] Resource incremented to %u", shared_resource);
            
            /* 释放互斥锁 */
            k_mutex_unlock(&resource_mutex);
            LOG_INF("[MutexThreadA] Released mutex");
        } else {
            LOG_WRN("[MutexThreadA] Failed to acquire mutex: %d", ret);
        }
        
        k_sleep(K_MSEC(200));
    }
}

/*
 * 互斥锁保护线程B
 */
static void mutex_thread_b(void *p1, void *p2, void *p3)
{
    ARG_UNUSED(p1);
    ARG_UNUSED(p2);
    ARG_UNUSED(p3);
    
    while (1) {
        /* 获取互斥锁 */
        k_mutex_lock(&resource_mutex, K_FOREVER);
        
        LOG_INF("[MutexThreadB] Acquired mutex");
        
        /* 访问共享资源 */
        shared_resource *= 2;
        LOG_INF("[MutexThreadB] Resource doubled to %u", shared_resource);
        
        k_mutex_unlock(&resource_mutex);
        LOG_INF("[MutexThreadB] Released mutex");
        
        k_sleep(K_MSEC(300));
    }
}

/*
 * 递归锁替代方案 - 使用计数器
 * Zephyr不直接支持递归互斥锁
 */
struct recursive_lock {
    struct k_mutex mutex;
    k_tid_t owner;
    int count;
};

static struct recursive_lock my_recursive_lock;

static void recursive_lock_init(struct recursive_lock *lock)
{
    k_mutex_init(&lock->mutex);
    lock->owner = NULL;
    lock->count = 0;
}

static int recursive_lock_acquire(struct recursive_lock *lock)
{
    k_tid_t current = k_current_get();
    
    if (lock->owner == current) {
        /* 已持有锁，增加计数 */
        lock->count++;
        return 0;
    }
    
    /* 尝试获取锁 */
    int ret = k_mutex_lock(&lock->mutex, K_FOREVER);
    if (ret == 0) {
        lock->owner = current;
        lock->count = 1;
    }
    return ret;
}

static int recursive_lock_release(struct recursive_lock *lock)
{
    k_tid_t current = k_current_get();
    
    if (lock->owner != current) {
        return -EINVAL;  /* 不持有该锁 */
    }
    
    lock->count--;
    
    if (lock->count == 0) {
        lock->owner = NULL;
        return k_mutex_unlock(&lock->mutex);
    }
    
    return 0;
}

/* 递归函数演示 */
static void recursive_operation(int depth, struct recursive_lock *lock)
{
    if (depth <= 0) {
        return;
    }
    
    recursive_lock_acquire(lock);
    LOG_INF("[Recursive] Depth %d: Acquired lock", depth);
    
    recursive_operation(depth - 1, lock);
    
    LOG_INF("[Recursive] Depth %d: Releasing lock", depth);
    recursive_lock_release(lock);
}

static void recursive_thread(void *p1, void *p2, void *p3)
{
    ARG_UNUSED(p1);
    ARG_UNUSED(p2);
    ARG_UNUSED(p3);
    
    recursive_lock_init(&my_recursive_lock);
    
    while (1) {
        LOG_INF("[RecursiveThread] Starting recursive operation");
        recursive_operation(3, &my_recursive_lock);
        LOG_INF("[RecursiveThread] Recursive operation complete");
        
        k_sleep(K_SECONDS(1));
    }
}

/*
 * 中断中释放信号量
 */
static void timer_expiry_handler(struct k_timer *timer)
{
    ARG_UNUSED(timer);
    
    /* 在中断上下文释放信号量 */
    k_sem_give(&binary_sem);
    LOG_DBG("[ISR] Semaphore given from timer interrupt");
}

static K_TIMER_DEFINE(my_timer, timer_expiry_handler, NULL);

/* 线程栈定义 */
K_THREAD_STACK_DEFINE(binary_sem_stack, 2048);
K_THREAD_STACK_DEFINE(producer_stack, 2048);
K_THREAD_STACK_DEFINE(consumer_stack, 2048);
K_THREAD_STACK_DEFINE(mutex_a_stack, 2048);
K_THREAD_STACK_DEFINE(mutex_b_stack, 2048);
K_THREAD_STACK_DEFINE(recursive_stack, 2048);

static struct k_thread binary_sem_thread_data;
static struct k_thread producer_thread_data;
static struct k_thread consumer_thread_data;
static struct k_thread mutex_a_thread_data;
static struct k_thread mutex_b_thread_data;
static struct k_thread recursive_thread_data;

int main(void)
{
    printk("\n=== Zephyr RTOS Semaphore Demo ===\n\n");
    
    /* 创建线程 */
    k_thread_create(&binary_sem_thread_data, binary_sem_stack,
                    K_THREAD_STACK_SIZEOF(binary_sem_stack),
                    binary_sem_thread, NULL, NULL, NULL,
                    K_PRIO_PREEMPT(5), 0, K_NO_WAIT);
    
    k_thread_create(&producer_thread_data, producer_stack,
                    K_THREAD_STACK_SIZEOF(producer_stack),
                    counting_producer, NULL, NULL, NULL,
                    K_PRIO_PREEMPT(5), 0, K_NO_WAIT);
    
    k_thread_create(&consumer_thread_data, consumer_stack,
                    K_THREAD_STACK_SIZEOF(consumer_stack),
                    counting_consumer, NULL, NULL, NULL,
                    K_PRIO_PREEMPT(5), 0, K_NO_WAIT);
    
    k_thread_create(&mutex_a_thread_data, mutex_a_stack,
                    K_THREAD_STACK_SIZEOF(mutex_a_stack),
                    mutex_thread_a, NULL, NULL, NULL,
                    K_PRIO_PREEMPT(5), 0, K_NO_WAIT);
    
    k_thread_create(&mutex_b_thread_data, mutex_b_stack,
                    K_THREAD_STACK_SIZEOF(mutex_b_stack),
                    mutex_thread_b, NULL, NULL, NULL,
                    K_PRIO_PREEMPT(5), 0, K_NO_WAIT);
    
    k_thread_create(&recursive_thread_data, recursive_stack,
                    K_THREAD_STACK_SIZEOF(recursive_stack),
                    recursive_thread, NULL, NULL, NULL,
                    K_PRIO_PREEMPT(5), 0, K_NO_WAIT);
    
    /* 启动定时器 - 周期性释放信号量 */
    k_timer_start(&my_timer, K_MSEC(500), K_MSEC(500));
    
    /* 主循环 */
    while (1) {
        k_sleep(K_SECONDS(10));
        
        /* 打印统计信息 */
        LOG_INF("[Main] Binary sem count: %u", k_sem_count_get(&binary_sem));
        LOG_INF("[Main] Counting sem count: %u", k_sem_count_get(&counting_sem));
        LOG_INF("[Main] Shared resource: %u", shared_resource);
    }
    
    return 0;
}
```

### 4.3 事件与通知机制

#### FreeRTOS 事件组

```c
/* 
 * FreeRTOS 事件组示例
 * 文件: freertos_eventgroup_example.c
 */

#include "FreeRTOS.h"
#include "task.h"
#include "event_groups.h"
#include <stdio.h>

/* 事件位定义 */
#define EVENT_WIFI_CONNECTED    (1 << 0)    /* bit 0: WiFi连接 */
#define EVENT_WIFI_GOT_IP       (1 << 1)    /* bit 1: 获取IP */
#define EVENT_MQTT_CONNECTED    (1 << 2)    /* bit 2: MQTT连接 */
#define EVENT_SENSOR_READY      (1 << 3)    /* bit 3: 传感器就绪 */
#define EVENT_DATA_READY        (1 << 4)    /* bit 4: 数据就绪 */

/* 复合事件 */
#define EVENT_NETWORK_READY     (EVENT_WIFI_CONNECTED | EVENT_WIFI_GOT_IP)
#define EVENT_ALL_READY         (EVENT_NETWORK_READY | EVENT_MQTT_CONNECTED)

/* 事件组句柄 */
static EventGroupHandle_t xEventGroup = NULL;

/*
 * WiFi管理任务
 */
static void vWiFiTask(void *pvParameters)
{
    (void)pvParameters;
    
    for (;;) {
        printf("[WiFiTask] Connecting to WiFi...\n");
        vTaskDelay(pdMS_TO_TICKS(2000));
        
        /* WiFi连接成功 */
        xEventGroupSetBits(xEventGroup, EVENT_WIFI_CONNECTED);
        printf("[WiFiTask] WiFi connected\n");
        
        vTaskDelay(pdMS_TO_TICKS(1000));
        
        /* 获取IP地址 */
        xEventGroupSetBits(xEventGroup, EVENT_WIFI_GOT_IP);
        printf("[WiFiTask] Got IP address\n");
        
        /* 等待断开连接事件或其他操作 */
        vTaskDelay(pdMS_TO_TICKS(10000));
    }
}

/*
 * MQTT客户端任务
 */
static void vMQTTTask(void *pvParameters)
{
    (void)pvParameters;
    EventBits_t uxBits;
    const TickType_t xTicksToWait = pdMS_TO_TICKS(10000);
    
    for (;;) {
        printf("[MQTTTask] Waiting for network...\n");
        
        /* 等待网络就绪事件 - 需要WiFi连接且获取IP */
        uxBits = xEventGroupWaitBits(
            xEventGroup,
            EVENT_NETWORK_READY,      /* 等待的位 */
            pdFALSE,                   /* 退出时不清除 */
            pdTRUE,                    /* 必须所有位都设置 */
            xTicksToWait
        );
        
        if ((uxBits & EVENT_NETWORK_READY) == EVENT_NETWORK_READY) {
            printf("[MQTTTask] Network ready, connecting to MQTT broker...\n");
            vTaskDelay(pdMS_TO_TICKS(1000));
            
            /* MQTT连接成功 */
            xEventGroupSetBits(xEventGroup, EVENT_MQTT_CONNECTED);
            printf("[MQTTTask] MQTT connected\n");
        } else {
            printf("[MQTTTask] Network timeout\n");
        }
        
        vTaskDelay(pdMS_TO_TICKS(5000));
    }
}

/*
 * 传感器任务
 */
static void vSensorTask(void *pvParameters)
{
    (void)pvParameters;
    EventBits_t uxBits;
    
    for (;;) {
        printf("[SensorTask] Initializing sensors...\n");
        vTaskDelay(pdMS_TO_TICKS(500));
        
        /* 传感器就绪 */
        xEventGroupSetBits(xEventGroup, EVENT_SENSOR_READY);
        printf("[SensorTask] Sensors ready\n");
        
        /* 等待所有系统就绪 */
        uxBits = xEventGroupWaitBits(
            xEventGroup,
            EVENT_ALL_READY,
            pdFALSE,
            pdTRUE,
            portMAX_DELAY
        );
        
        if ((uxBits & EVENT_ALL_READY) == EVENT_ALL_READY) {
            printf("[SensorTask] All systems ready, starting data collection\n");
            
            for (int i = 0; i < 5; i++) {
                vTaskDelay(pdMS_TO_TICKS(1000));
                printf("[SensorTask] Collected sample #%d\n", i + 1);
                
                /* 数据就绪事件 */
                xEventGroupSetBits(xEventGroup, EVENT_DATA_READY);
            }
        }
        
        vTaskDelay(pdMS_TO_TICKS(5000));
    }
}

/*
 * 数据处理任务
 */
static void vDataProcessingTask(void *pvParameters)
{
    (void)pvParameters;
    EventBits_t uxBits;
    uint32_t data_count = 0;
    
    for (;;) {
        /* 等待数据就绪事件 */
        uxBits = xEventGroupWaitBits(
            xEventGroup,
            EVENT_DATA_READY,
            pdTRUE,                    /* 清除该位 */
            pdFALSE,
            portMAX_DELAY
        );
        
        if (uxBits & EVENT_DATA_READY) {
            data_count++;
            printf("[DataTask] Processing data packet #%lu\n", data_count);
        }
    }
}

/*
 * 系统监控任务
 */
static void vMonitorTask(void *pvParameters)
{
    (void)pvParameters;
    EventBits_t uxBits;
    
    for (;;) {
        vTaskDelay(pdMS_TO_TICKS(3000));
        
        /* 获取当前事件状态（不清除） */
        uxBits = xEventGroupGetBits(xEventGroup);
        
        printf("[Monitor] Event status:\n");
        printf("  WiFi Connected: %s\n", 
               (uxBits & EVENT_WIFI_CONNECTED) ? "YES" : "NO");
        printf("  Got IP: %s\n", 
               (uxBits & EVENT_WIFI_GOT_IP) ? "YES" : "NO");
        printf("  MQTT Connected: %s\n", 
               (uxBits & EVENT_MQTT_CONNECTED) ? "YES" : "NO");
        printf("  Sensor Ready: %s\n", 
               (uxBits & EVENT_SENSOR_READY) ? "YES" : "NO");
        printf("  Data Ready: %s\n", 
               (uxBits & EVENT_DATA_READY) ? "YES" : "NO");
    }
}

/*
 * 中断中使用事件组
 */
void vApplicationTickHook(void)
{
    static uint32_t tick_count = 0;
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;
    
    tick_count++;
    
    /* 模拟周期性中断事件 */
    if (tick_count >= 1000) {
        tick_count = 0;
        xEventGroupSetBitsFromISR(
            xEventGroup,
            EVENT_SENSOR_READY,
            &xHigherPriorityTaskWoken
        );
        portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
    }
}

/*
 * 同步点示例 - 多任务同步
 */
static void vSyncPointTask(void *pvParameters)
{
    uint32_t task_id = (uint32_t)pvParameters;
    
    printf("[SyncTask%u] Started, doing initialization...\n", task_id);
    
    /* 模拟不同初始化时间 */
    vTaskDelay(pdMS_TO_TICKS(500 * task_id));
    
    printf("[SyncTask%u] Initialization complete, waiting at barrier\n", task_id);
    
    /* 设置自己的完成位 */
    xEventGroupSync(
        xEventGroup,
        (1 << (5 + task_id)),      /* 设置自己的位 */
        (1 << 5) | (1 << 6) | (1 << 7), /* 等待所有任务 */
        portMAX_DELAY
    );
    
    printf("[SyncTask%u] All tasks reached barrier, continuing\n", task_id);
    
    vTaskDelete(NULL);
}

int main(void)
{
    printf("\n=== FreeRTOS Event Group Demo ===\n\n");
    
    /* 创建事件组 */
    xEventGroup = xEventGroupCreate();
    configASSERT(xEventGroup != NULL);
    
    /* 创建任务 */
    xTaskCreate(vWiFiTask, "WiFi", 256, NULL, 3, NULL);
    xTaskCreate(vMQTTTask, "MQTT", 256, NULL, 2, NULL);
    xTaskCreate(vSensorTask, "Sensor", 256, NULL, 2, NULL);
    xTaskCreate(vDataProcessingTask, "DataProc", 256, NULL, 1, NULL);
    xTaskCreate(vMonitorTask, "Monitor", 256, NULL, 1, NULL);
    
    /* 创建同步任务 */
    xTaskCreate(vSyncPointTask, "Sync1", 256, (void*)1, 2, NULL);
    xTaskCreate(vSyncPointTask, "Sync2", 256, (void*)2, 2, NULL);
    xTaskCreate(vSyncPointTask, "Sync3", 256, (void*)3, 2, NULL);
    
    vTaskStartScheduler();
    
    return 0;
}
```

#### Zephyr 事件与通知

```c
/* 
 * Zephyr RTOS 事件与通知示例
 * 文件: zephyr_event_example.c
 */

#include <zephyr/kernel.h>
#include <zephyr/sys/printk.h>
#include <zephyr/logging/log.h>

LOG_MODULE_REGISTER(event_example, LOG_LEVEL_DBG);

/* 事件定义 */
#define EVENT_WIFI_CONNECTED    BIT(0)
#define EVENT_WIFI_GOT_IP       BIT(1)
#define EVENT_MQTT_CONNECTED    BIT(2)
#define EVENT_SENSOR_READY      BIT(3)
#define EVENT_DATA_READY        BIT(4)

#define EVENT_NETWORK_READY     (EVENT_WIFI_CONNECTED | EVENT_WIFI_GOT_IP)
#define EVENT_ALL_READY         (EVENT_NETWORK_READY | EVENT_MQTT_CONNECTED)

/* 事件对象 */
static K_EVENT_DEFINE(system_events);

/* 轮询对象 */
static K_POLL_SIGNAL_DEFINE(wifi_signal);
static K_POLL_SIGNAL_DEFINE(mqtt_signal);

/*
 * WiFi管理线程
 */
static void wifi_thread(void *p1, void *p2, void *p3)
{
    ARG_UNUSED(p1);
    ARG_UNUSED(p2);
    ARG_UNUSED(p3);
    
    while (1) {
        LOG_INF("[WiFiThread] Connecting to WiFi...");
        k_sleep(K_SECONDS(2));
        
        /* WiFi连接成功 */
        k_event_post(&system_events, EVENT_WIFI_CONNECTED);
        LOG_INF("[WiFiThread] WiFi connected");
        
        /* 触发信号 */
        k_poll_signal_raise(&wifi_signal, 1);
        
        k_sleep(K_SECONDS(1));
        
        /* 获取IP地址 */
        k_event_post(&system_events, EVENT_WIFI_GOT_IP);
        LOG_INF("[WiFiThread] Got IP address");
        
        k_sleep(K_SECONDS(10));
    }
}

/*
 * MQTT客户端线程
 */
static void mqtt_thread(void *p1, void *p2, void *p3)
{
    ARG_UNUSED(p1);
    ARG_UNUSED(p2);
    ARG_UNUSED(p3);
    
    while (1) {
        LOG_INF("[MQTTThread] Waiting for network...");
        
        /* 等待网络就绪 - 使用事件 */
        uint32_t events = k_event_wait(&system_events,
                                        EVENT_NETWORK_READY,
                                        false,           /* 不消费 */
                                        K_SECONDS(10));
        
        if ((events & EVENT_NETWORK_READY) == EVENT_NETWORK_READY) {
            LOG_INF("[MQTTThread] Network ready, connecting to MQTT broker...");
            k_sleep(K_SECONDS(1));
            
            /* MQTT连接成功 */
            k_event_post(&system_events, EVENT_MQTT_CONNECTED);
            LOG_INF("[MQTTThread] MQTT connected");
            
            k_poll_signal_raise(&mqtt_signal, 1);
        } else {
            LOG_WRN("[MQTTThread] Network timeout");
        }
        
        k_sleep(K_SECONDS(5));
    }
}

/*
 * 传感器线程
 */
static void sensor_thread(void *p1, void *p2, void *p3)
{
    ARG_UNUSED(p1);
    ARG_UNUSED(p2);
    ARG_UNUSED(p3);
    
    while (1) {
        LOG_INF("[SensorThread] Initializing sensors...");
        k_sleep(K_MSEC(500));
        
        /* 传感器就绪 */
        k_event_post(&system_events, EVENT_SENSOR_READY);
        LOG_INF("[SensorThread] Sensors ready");
        
        /* 等待所有系统就绪 */
        uint32_t events = k_event_wait(&system_events,
                                        EVENT_ALL_READY,
                                        false,
                                        K_FOREVER);
        
        if ((events & EVENT_ALL_READY) == EVENT_ALL_READY) {
            LOG_INF("[SensorThread] All systems ready, starting data collection");
            
            for (int i = 0; i < 5; i++) {
                k_sleep(K_SECONDS(1));
                LOG_INF("[SensorThread] Collected sample #%d", i + 1);
                
                /* 数据就绪事件 */
                k_event_post(&system_events, EVENT_DATA_READY);
            }
        }
        
        k_sleep(K_SECONDS(5));
    }
}

/*
 * 数据处理线程
 */
static void data_processing_thread(void *p1, void *p2, void *p3)
{
    ARG_UNUSED(p1);
    ARG_UNUSED(p2);
    ARG_UNUSED(p3);
    
    uint32_t data_count = 0;
    
    while (1) {
        /* 等待数据就绪 - 使用带超时的等待 */
        uint32_t events = k_event_wait(&system_events,
                                        EVENT_DATA_READY,
                                        true,           /* 消费事件 */
                                        K_FOREVER);
        
        if (events & EVENT_DATA_READY) {
            data_count++;
            LOG_INF("[DataThread] Processing data packet #%u", data_count);
        }
    }
}

/*
 * 系统监控线程
 */
static void monitor_thread(void *p1, void *p2, void *p3)
{
    ARG_UNUSED(p1);
    ARG_UNUSED(p2);
    ARG_UNUSED(p3);
    
    while (1) {
        k_sleep(K_SECONDS(3));
        
        /* 获取当前事件状态（不消费） */
        uint32_t events = k_event_test(&system_events, 0);
        
        LOG_INF("[Monitor] Event status:");
        LOG_INF("  WiFi Connected: %s", 
                (events & EVENT_WIFI_CONNECTED) ? "YES" : "NO");
        LOG_INF("  Got IP: %s", 
                (events & EVENT_WIFI_GOT_IP) ? "YES" : "NO");
        LOG_INF("  MQTT Connected: %s", 
                (events & EVENT_MQTT_CONNECTED) ? "YES" : "NO");
        LOG_INF("  Sensor Ready: %s", 
                (events & EVENT_SENSOR_READY) ? "YES" : "NO");
        LOG_INF("  Data Ready: %s", 
                (events & EVENT_DATA_READY) ? "YES" : "NO");
    }
}

/*
 * 使用k_poll的多路等待
 */
static void poll_thread(void *p1, void *p2, void *p3)
{
    ARG_UNUSED(p1);
    ARG_UNUSED(p2);
    ARG_UNUSED(p3);
    
    struct k_poll_event events[2];
    
    /* 初始化轮询事件 */
    k_poll_event_init(&events[0], K_POLL_TYPE_SIGNAL,
                      K_POLL_MODE_NOTIFY_ONLY, &wifi_signal);
    k_poll_event_init(&events[1], K_POLL_TYPE_SIGNAL,
                      K_POLL_MODE_NOTIFY_ONLY, &mqtt_signal);
    
    while (1) {
        LOG_INF("[PollThread] Waiting for signals...");
        
        /* 等待任一信号 */
        int rc = k_poll(events, 2, K_SECONDS(30));
        
        if (rc == 0) {
            if (events[0].state == K_POLL_STATE_SIGNALED) {
                LOG_INF("[PollThread] WiFi signal received");
                k_poll_signal_reset(&wifi_signal);
                events[0].state = K_POLL_STATE_NOT_READY;
            }
            
            if (events[1].state == K_POLL_STATE_SIGNALED) {
                LOG_INF("[PollThread] MQTT signal received");
                k_poll_signal_reset(&mqtt_signal);
                events[1].state = K_POLL_STATE_NOT_READY;
            }
        }
    }
}

/*
 * 屏障同步示例
 */
static K_SEM_DEFINE(barrier_sem, 0, 3);
static atomic_t barrier_count;

static void barrier_thread(void *p1, void *p2, void *p3)
{
    uint32_t task_id = (uint32_t)p1;
    
    ARG_UNUSED(p2);
    ARG_UNUSED(p3);
    
    LOG_INF("[BarrierThread%u] Started, doing initialization...", task_id);
    
    /* 模拟不同初始化时间 */
    k_sleep(K_MSEC(500 * task_id));
    
    LOG_INF("[BarrierThread%u] Initialization complete, waiting at barrier", task_id);
    
    /* 增加计数 */
    atomic_inc(&barrier_count);
    
    /* 等待所有线程到达 */
    if (atomic_get(&barrier_count) == 3) {
        /* 最后一个线程释放所有等待 */
        for (int i = 0; i < 3; i++) {
            k_sem_give(&barrier_sem);
        }
    }
    
    k_sem_take(&barrier_sem, K_FOREVER);
    
    LOG_INF("[BarrierThread%u] All threads reached barrier, continuing", task_id);
}

/* 线程栈 */
K_THREAD_STACK_DEFINE(wifi_stack, 2048);
K_THREAD_STACK_DEFINE(mqtt_stack, 2048);
K_THREAD_STACK_DEFINE(sensor_stack, 2048);
K_THREAD_STACK_DEFINE(data_stack, 2048);
K_THREAD_STACK_DEFINE(monitor_stack, 2048);
K_THREAD_STACK_DEFINE(poll_stack, 2048);
K_THREAD_STACK_DEFINE(barrier_stack_1, 1024);
K_THREAD_STACK_DEFINE(barrier_stack_2, 1024);
K_THREAD_STACK_DEFINE(barrier_stack_3, 1024);

static struct k_thread wifi_thread_data;
static struct k_thread mqtt_thread_data;
static struct k_thread sensor_thread_data;
static struct k_thread data_thread_data;
static struct k_thread monitor_thread_data;
static struct k_thread poll_thread_data;
static struct k_thread barrier_thread_data[3];

int main(void)
{
    printk("\n=== Zephyr RTOS Event Demo ===\n\n");
    
    /* 创建线程 */
    k_thread_create(&wifi_thread_data, wifi_stack,
                    K_THREAD_STACK_SIZEOF(wifi_stack),
                    wifi_thread, NULL, NULL, NULL,
                    K_PRIO_PREEMPT(3), 0, K_NO_WAIT);
    
    k_thread_create(&mqtt_thread_data, mqtt_stack,
                    K_THREAD_STACK_SIZEOF(mqtt_stack),
                    mqtt_thread, NULL, NULL, NULL,
                    K_PRIO_PREEMPT(2), 0, K_NO_WAIT);
    
    k_thread_create(&sensor_thread_data, sensor_stack,
                    K_THREAD_STACK_SIZEOF(sensor_stack),
                    sensor_thread, NULL, NULL, NULL,
                    K_PRIO_PREEMPT(2), 0, K_NO_WAIT);
    
    k_thread_create(&data_thread_data, data_stack,
                    K_THREAD_STACK_SIZEOF(data_stack),
                    data_processing_thread, NULL, NULL, NULL,
                    K_PRIO_PREEMPT(1), 0, K_NO_WAIT);
    
    k_thread_create(&monitor_thread_data, monitor_stack,
                    K_THREAD_STACK_SIZEOF(monitor_stack),
                    monitor_thread, NULL, NULL, NULL,
                    K_PRIO_PREEMPT(1), 0, K_NO_WAIT);
    
    k_thread_create(&poll_thread_data, poll_stack,
                    K_THREAD_STACK_SIZEOF(poll_stack),
                    poll_thread, NULL, NULL, NULL,
                    K_PRIO_PREEMPT(2), 0, K_NO_WAIT);
    
    /* 创建屏障同步线程 */
    k_thread_create(&barrier_thread_data[0], barrier_stack_1,
                    K_THREAD_STACK_SIZEOF(barrier_stack_1),
                    barrier_thread, (void*)1, NULL, NULL,
                    K_PRIO_PREEMPT(2), 0, K_NO_WAIT);
    
    k_thread_create(&barrier_thread_data[1], barrier_stack_2,
                    K_THREAD_STACK_SIZEOF(barrier_stack_2),
                    barrier_thread, (void*)2, NULL, NULL,
                    K_PRIO_PREEMPT(2), 0, K_NO_WAIT);
    
    k_thread_create(&barrier_thread_data[2], barrier_stack_3,
                    K_THREAD_STACK_SIZEOF(barrier_stack_3),
                    barrier_thread, (void*)3, NULL, NULL,
                    K_PRIO_PREEMPT(2), 0, K_NO_WAIT);
    
    /* 主循环 */
    while (1) {
        k_sleep(K_SECONDS(30));
        LOG_INF("[Main] System running...");
    }
    
    return 0;
}
```

### 4.4 同步原语对比表

| 特性 | FreeRTOS | Zephyr RTOS |
|------|----------|-------------|
| **二值信号量** | `xSemaphoreCreateBinary()` | `K_SEM_DEFINE` / `k_sem_init()` |
| **计数信号量** | `xSemaphoreCreateCounting()` | `k_sem_init()` |
| **互斥锁** | `xSemaphoreCreateMutex()` | `K_MUTEX_DEFINE` |
| **递归互斥锁** | `xSemaphoreCreateRecursiveMutex()` | 需手动实现 |
| **事件组** | `xEventGroupCreate()` | `K_EVENT_DEFINE` |
| **任务通知** | `xTaskNotify()` / `ulTaskNotifyTake()` | `k_poll_signal` |
| **条件变量** | 不支持 | `k_condvar` |
| **读写锁** | 不支持 | `k_rwlock` |
| **获取超时** | `portMAX_DELAY` / 指定ticks | `K_FOREVER` / `K_MSEC()` |
| **ISR安全** | `FromISR`后缀函数 | 所有API ISR安全 |

---

## 5. 通信机制对比

### 5.1 队列与消息传递

| 特性 | FreeRTOS Queue | Zephyr FIFO | Zephyr Message Queue |
|------|----------------|-------------|---------------------|
| **数据类型** | 固定大小副本 | 指针传递 | 固定大小副本 |
| **容量** | 创建时指定 | 动态 | 创建时指定 |
| **阻塞操作** | 支持 | 支持 | 支持 |
| **多发送者** | 支持 | 支持 | 支持 |
| **多接收者** | 支持 | 支持 | 支持 |
| **覆盖模式** | 支持 | 不支持 | 不支持 |
| **查看功能** | `xQueuePeek()` | `k_fifo_peek_head()` | `k_msgq_peek()` |

#### FreeRTOS 队列

```c
/* 
 * FreeRTOS 队列完整示例
 * 文件: freertos_queue_example.c
 */

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include <stdio.h>
#include <string.h>

/* 消息类型定义 */
typedef enum {
    MSG_TYPE_SENSOR_DATA = 0,
    MSG_TYPE_CONTROL_CMD,
    MSG_TYPE_STATUS_REQ,
    MSG_TYPE_CONFIG_UPDATE,
    MSG_TYPE_MAX
} MessageType_t;

/* 传感器数据结构 */
typedef struct {
    uint32_t timestamp;
    float temperature;
    float humidity;
    float pressure;
    uint32_t sequence_num;
} SensorData_t;

/* 控制命令结构 */
typedef struct {
    uint32_t command_id;
    uint32_t param1;
    uint32_t param2;
    char description[32];
} ControlCommand_t;

/* 通用消息结构 */
typedef struct {
    MessageType_t type;
    union {
        SensorData_t sensor;
        ControlCommand_t command;
        uint32_t raw_data[16];
    } payload;
} Message_t;

/* 队列句柄 */
static QueueHandle_t xSensorQueue = NULL;
static QueueHandle_t xCommandQueue = NULL;
static QueueHandle_t xPriorityQueue = NULL;

/* 统计信息 */
static volatile uint32_t messages_sent = 0;
static volatile uint32_t messages_received = 0;
static volatile uint32_t queue_full_count = 0;

/*
 * 传感器数据采集任务
 * 功能: 采集传感器数据并发送到队列
 */
static void vSensorTask(void *pvParameters)
{
    (void)pvParameters;
    SensorData_t sensor_data;
    uint32_t sequence = 0;
    
    printf("[SensorTask] Started\n");
    
    for (;;) {
        /* 模拟传感器读取 */
        sequence++;
        sensor_data.timestamp = xTaskGetTickCount();
        sensor_data.temperature = 25.0f + (float)(sequence % 10);
        sensor_data.humidity = 50.0f + (float)(sequence % 20);
        sensor_data.pressure = 1013.25f + (float)(sequence % 5);
        sensor_data.sequence_num = sequence;
        
        /* 创建消息 */
        Message_t msg;
        msg.type = MSG_TYPE_SENSOR_DATA;
        memcpy(&msg.payload.sensor, &sensor_data, sizeof(SensorData_t));
        
        /* 发送到队列 - 带超时 */
        if (xQueueSend(xSensorQueue, &msg, pdMS_TO_TICKS(100)) == pdPASS) {
            messages_sent++;
            
            if (sequence % 10 == 0) {
                printf("[SensorTask] Sent sensor data #%lu (T=%.1f, H=%.1f)\n",
                       sequence, sensor_data.temperature, sensor_data.humidity);
            }
        } else {
            queue_full_count++;
            printf("[SensorTask] Queue full, data dropped\n");
        }
        
        /* 采集周期 */
        vTaskDelay(pdMS_TO_TICKS(100));
    }
}

/*
 * 命令处理任务
 * 功能: 从队列接收并执行命令
 */
static void vCommandTask(void *pvParameters)
{
    (void)pvParameters;
    Message_t msg;
    
    printf("[CommandTask] Started\n");
    
    for (;;) {
        /* 接收命令 - 永久等待 */
        if (xQueueReceive(xCommandQueue, &msg, portMAX_DELAY) == pdPASS) {
            messages_received++;
            
            if (msg.type == MSG_TYPE_CONTROL_CMD) {
                ControlCommand_t *cmd = &msg.payload.command;
                printf("[CommandTask] Executing command %lu: %s\n",
                       cmd->command_id, cmd->description);
                printf("  Parameters: %lu, %lu\n", cmd->param1, cmd->param2);
                
                /* 模拟命令执行 */
                vTaskDelay(pdMS_TO_TICKS(50));
                
                printf("[CommandTask] Command %lu completed\n", cmd->command_id);
            }
        }
    }
}

/*
 * 数据处理任务
 * 功能: 从多个队列接收数据（使用队列集）
 */
static QueueSetHandle_t xQueueSet = NULL;

static void vDataProcessingTask(void *pvParameters)
{
    (void)pvParameters;
    QueueSetMemberHandle_t xActivatedMember;
    Message_t msg;
    
    printf("[DataProcTask] Started\n");
    
    for (;;) {
        /* 等待任一队列有数据 */
        xActivatedMember = xQueueSelectFromSet(xQueueSet, portMAX_DELAY);
        
        if (xActivatedMember == (QueueSetMemberHandle_t)xSensorQueue) {
            /* 从传感器队列接收 */
            if (xQueueReceive(xSensorQueue, &msg, 0) == pdPASS) {
                messages_received++;
                
                /* 处理传感器数据 */
                float avg_temp = msg.payload.sensor.temperature;
                printf("[DataProcTask] Processing sensor data #%lu (avg_temp=%.2f)\n",
                       msg.payload.sensor.sequence_num, avg_temp);
                
                /* 温度告警检查 */
                if (avg_temp > 30.0f) {
                    printf("[DataProcTask] WARNING: High temperature detected!\n");
                }
            }
        }
        else if (xActivatedMember == (QueueSetMemberHandle_t)xCommandQueue) {
            /* 命令转发到专用处理任务 */
            if (xQueueReceive(xCommandQueue, &msg, 0) == pdPASS) {
                messages_received++;
                printf("[DataProcTask] Forwarding command to handler\n");
                /* 实际应用中可能转发到工作队列 */
            }
        }
    }
}

/*
 * 优先级命令任务
 * 功能: 发送高优先级命令（覆盖队列）
 */
static void vPriorityCommandTask(void *pvParameters)
{
    (void)pvParameters;
    uint32_t cmd_id = 0;
    
    printf("[PriorityTask] Started\n");
    
    for (;;) {
        vTaskDelay(pdMS_TO_TICKS(5000));
        
        /* 创建紧急命令 */
        Message_t msg;
        msg.type = MSG_TYPE_CONTROL_CMD;
        msg.payload.command.command_id = ++cmd_id;
        msg.payload.command.param1 = 0xFF;
        msg.payload.command.param2 = 0xFF;
        snprintf(msg.payload.command.description, 32, "EMERGENCY_STOP");
        
        /* 覆盖发送到优先级队列 */
        if (xQueueOverwrite(xPriorityQueue, &msg) == pdPASS) {
            printf("[PriorityTask] Emergency command %lu sent (overwritten)\n", cmd_id);
        }
    }
}

/*
 * 队列监控任务
 */
static void vQueueMonitorTask(void *pvParameters)
{
    (void)pvParameters;
    UBaseType_t uxMessagesWaiting;
    
    for (;;) {
        vTaskDelay(pdMS_TO_TICKS(5000));
        
        printf("\n[QueueMonitor] Statistics:\n");
        printf("  Messages sent: %lu\n", messages_sent);
        printf("  Messages received: %lu\n", messages_received);
        printf("  Queue full events: %lu\n", queue_full_count);
        
        uxMessagesWaiting = uxQueueMessagesWaiting(xSensorQueue);
        printf("  Sensor queue messages: %u\n", uxMessagesWaiting);
        
        uxMessagesWaiting = uxQueueMessagesWaiting(xCommandQueue);
        printf("  Command queue messages: %u\n", uxMessagesWaiting);
        
        UBaseType_t uxSpaces = uxQueueSpacesAvailable(xSensorQueue);
        printf("  Sensor queue spaces: %u\n", uxSpaces);
    }
}

/*
 * 使用队列查看功能
 */
static void vQueuePeekTask(void *pvParameters)
{
    (void)pvParameters;
    Message_t msg;
    
    for (;;) {
        vTaskDelay(pdMS_TO_TICKS(1000));
        
        /* 查看队列头部而不移除 */
        if (xQueuePeek(xSensorQueue, &msg, pdMS_TO_TICKS(100)) == pdPASS) {
            printf("[PeekTask] Next sensor data: seq=%lu, temp=%.1f\n",
                   msg.payload.sensor.sequence_num,
                   msg.payload.sensor.temperature);
        }
    }
}

/*
 * 中断中发送数据
 */
void vApplicationTickHook(void)
{
    static uint32_t tick_count = 0;
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;
    
    tick_count++;
    
    if (tick_count >= 1000) {
        tick_count = 0;
        
        Message_t msg;
        msg.type = MSG_TYPE_STATUS_REQ;
        msg.payload.raw_data[0] = xTaskGetTickCount();
        
        xQueueSendFromISR(xPriorityQueue, &msg, &xHigherPriorityTaskWoken);
        
        portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
    }
}

/*
 * 静态队列分配
 */
#define QUEUE_LENGTH    10
#define ITEM_SIZE       sizeof(Message_t)

static uint8_t ucQueueStorage[QUEUE_LENGTH * ITEM_SIZE];
static StaticQueue_t xStaticQueue;

static QueueHandle_t xStaticQueueHandle = NULL;

int main(void)
{
    printf("\n=== FreeRTOS Queue Demo ===\n\n");
    
    /* 创建传感器队列 - 深度20 */
    xSensorQueue = xQueueCreate(20, sizeof(Message_t));
    configASSERT(xSensorQueue != NULL);
    
    /* 创建命令队列 */
    xCommandQueue = xQueueCreate(10, sizeof(Message_t));
    configASSERT(xCommandQueue != NULL);
    
    /* 创建优先级队列（长度为1，用于覆盖） */
    xPriorityQueue = xQueueCreate(1, sizeof(Message_t));
    configASSERT(xPriorityQueue != NULL);
    
    /* 创建队列集 */
    xQueueSet = xQueueCreateSet(30);
    configASSERT(xQueueSet != NULL);
    
    /* 添加队列到队列集 */
    xQueueAddToSet(xSensorQueue, xQueueSet);
    xQueueAddToSet(xCommandQueue, xQueueSet);
    
    /* 创建静态队列 */
    xStaticQueueHandle = xQueueCreateStatic(
        QUEUE_LENGTH,
        ITEM_SIZE,
        ucQueueStorage,
        &xStaticQueue
    );
    configASSERT(xStaticQueueHandle != NULL);
    
    /* 创建任务 */
    xTaskCreate(vSensorTask, "Sensor", 512, NULL, 2, NULL);
    xTaskCreate(vCommandTask, "Command", 512, NULL, 2, NULL);
    xTaskCreate(vDataProcessingTask, "DataProc", 512, NULL, 3, NULL);
    xTaskCreate(vPriorityCommandTask, "Priority", 256, NULL, 4, NULL);
    xTaskCreate(vQueueMonitorTask, "Monitor", 256, NULL, 1, NULL);
    xTaskCreate(vQueuePeekTask, "Peek", 256, NULL, 1, NULL);
    
    vTaskStartScheduler();
    
    return 0;
}
```

#### Zephyr FIFO与消息队列

```c
/* 
 * Zephyr RTOS FIFO与消息队列示例
 * 文件: zephyr_queue_example.c
 */

#include <zephyr/kernel.h>
#include <zephyr/sys/printk.h>
#include <zephyr/logging/log.h>

LOG_MODULE_REGISTER(queue_example, LOG_LEVEL_DBG);

/* 消息类型 */
enum msg_type {
    MSG_TYPE_SENSOR_DATA = 0,
    MSG_TYPE_CONTROL_CMD,
    MSG_TYPE_STATUS_REQ,
    MSG_TYPE_CONFIG_UPDATE
};

/* 传感器数据 */
struct sensor_data {
    uint32_t timestamp;
    float temperature;
    float humidity;
    float pressure;
    uint32_t sequence_num;
};

/* 控制命令 */
struct control_command {
    uint32_t command_id;
    uint32_t param1;
    uint32_t param2;
    char description[32];
};

/* FIFO节点 - 用于FIFO方式传递 */
struct data_item {
    void *fifo_reserved;  /* FIFO内部使用 */
    enum msg_type type;
    union {
        struct sensor_data sensor;
        struct control_command command;
        uint32_t raw_data[16];
    } payload;
};

/* 定义FIFO */
K_FIFO_DEFINE(sensor_fifo);

/* 定义消息队列 - 用于传递数据的副本 */
K_MSGQ_DEFINE(sensor_msgq, sizeof(struct sensor_data), 20, 4);
K_MSGQ_DEFINE(command_msgq, sizeof(struct control_command), 10, 4);

/* 内存池 - 用于FIFO节点分配 */
K_MEM_SLAB_DEFINE(data_item_slab, sizeof(struct data_item), 30, 4);

/* 统计 */
static atomic_t messages_sent;
static atomic_t messages_received;
static atomic_t queue_full_count;

/*
 * 传感器数据采集线程 - 使用FIFO
 */
static void sensor_fifo_thread(void *p1, void *p2, void *p3)
{
    ARG_UNUSED(p1);
    ARG_UNUSED(p2);
    ARG_UNUSED(p3);
    
    uint32_t sequence = 0;
    
    LOG_INF("[SensorFIFOThread] Started");
    
    while (1) {
        /* 从内存池分配节点 */
        struct data_item *item;
        int ret = k_mem_slab_alloc(&data_item_slab, (void**)&item, K_MSEC(100));
        
        if (ret == 0) {
            /* 填充数据 */
            sequence++;
            item->type = MSG_TYPE_SENSOR_DATA;
            item->payload.sensor.timestamp = k_uptime_get_32();
            item->payload.sensor.temperature = 25.0f + (float)(sequence % 10);
            item->payload.sensor.humidity = 50.0f + (float)(sequence % 20);
            item->payload.sensor.pressure = 1013.25f + (float)(sequence % 5);
            item->payload.sensor.sequence_num = sequence;
            
            /* 放入FIFO */
            k_fifo_put(&sensor_fifo, item);
            atomic_inc(&messages_sent);
            
            if (sequence % 10 == 0) {
                LOG_INF("[SensorFIFOThread] Sent sensor data #%u (T=%.1f, H=%.1f)",
                        sequence, item->payload.sensor.temperature,
                        item->payload.sensor.humidity);
            }
        } else {
            atomic_inc(&queue_full_count);
            LOG_WRN("[SensorFIFOThread] Memory slab full, data dropped");
        }
        
        k_sleep(K_MSEC(100));
    }
}

/*
 * 传感器数据采集线程 - 使用消息队列
 */
static void sensor_msgq_thread(void *p1, void *p2, void *p3)
{
    ARG_UNUSED(p1);
    ARG_UNUSED(p2);
    ARG_UNUSED(p3);
    
    uint32_t sequence = 0;
    
    LOG_INF("[SensorMsgqThread] Started");
    
    while (1) {
        struct sensor_data data;
        
        /* 填充数据 */
        sequence++;
        data.timestamp = k_uptime_get_32();
        data.temperature = 25.0f + (float)(sequence % 10);
        data.humidity = 50.0f + (float)(sequence % 20);
        data.pressure = 1013.25f + (float)(sequence % 5);
        data.sequence_num = sequence;
        
        /* 发送到消息队列 */
        int ret = k_msgq_put(&sensor_msgq, &data, K_MSEC(100));
        
        if (ret == 0) {
            atomic_inc(&messages_sent);
        } else {
            atomic_inc(&queue_full_count);
            LOG_WRN("[SensorMsgqThread] Message queue full");
        }
        
        k_sleep(K_MSEC(100));
    }
}

/*
 * 数据处理线程 - 从FIFO接收
 */
static void data_fifo_thread(void *p1, void *p2, void *p3)
{
    ARG_UNUSED(p1);
    ARG_UNUSED(p2);
    ARG_UNUSED(p3);
    
    LOG_INF("[DataFIFOThread] Started");
    
    while (1) {
        /* 从FIFO获取 */
        struct data_item *item = k_fifo_get(&sensor_fifo, K_FOREVER);
        
        if (item != NULL) {
            atomic_inc(&messages_received);
            
            if (item->type == MSG_TYPE_SENSOR_DATA) {
                struct sensor_data *sensor = &item->payload.sensor;
                
                LOG_DBG("[DataFIFOThread] Processing sensor data #%u", 
                        sensor->sequence_num);
                
                /* 温度检查 */
                if (sensor->temperature > 30.0f) {
                    LOG_WRN("[DataFIFOThread] High temperature: %.2f",
                            sensor->temperature);
                }
            }
            
            /* 释放内存回slab */
            k_mem_slab_free(&data_item_slab, (void*)item);
        }
    }
}

/*
 * 数据处理线程 - 从消息队列接收
 */
static void data_msgq_thread(void *p1, void *p2, void *p3)
{
    ARG_UNUSED(p1);
    ARG_UNUSED(p2);
    ARG_UNUSED(p3);
    
    struct sensor_data data;
    
    LOG_INF("[DataMsgqThread] Started");
    
    while (1) {
        /* 从消息队列获取 - 数据被复制到data */
        int ret = k_msgq_get(&sensor_msgq, &data, K_FOREVER);
        
        if (ret == 0) {
            atomic_inc(&messages_received);
            
            LOG_DBG("[DataMsgqThread] Processing sensor data #%u (T=%.2f)",
                    data.sequence_num, data.temperature);
            
            /* 数据处理...
               数据是副本，可以直接修改 */
            float processed_temp = data.temperature * 1.8f + 32.0f;
            (void)processed_temp;  /* 仅演示 */
        }
    }
}

/*
 * 命令处理线程
 */
static void command_thread(void *p1, void *p2, void *p3)
{
    ARG_UNUSED(p1);
    ARG_UNUSED(p2);
    ARG_UNUSED(p3);
    
    struct control_command cmd;
    
    LOG_INF("[CommandThread] Started");
    
    while (1) {
        /* 接收命令 */
        int ret = k_msgq_get(&command_msgq, &cmd, K_FOREVER);
        
        if (ret == 0) {
            atomic_inc(&messages_received);
            
            LOG_INF("[CommandThread] Executing command %u: %s",
                    cmd.command_id, cmd.description);
            LOG_INF("  Parameters: 0x%X, 0x%X", cmd.param1, cmd.param2);
            
            /* 模拟执行 */
            k_sleep(K_MSEC(50));
            
            LOG_INF("[CommandThread] Command %u completed", cmd.command_id);
        }
    }
}

/*
 * 命令发送线程
 */
static void command_sender_thread(void *p1, void *p2, void *p3)
{
    ARG_UNUSED(p1);
    ARG_UNUSED(p2);
    ARG_UNUSED(p3);
    
    uint32_t cmd_id = 0;
    
    while (1) {
        k_sleep(K_SECONDS(3));
        
        struct control_command cmd;
        cmd.command_id = ++cmd_id;
        cmd.param1 = cmd_id * 10;
        cmd.param2 = cmd_id * 20;
        snprintf(cmd.description, 32, "TestCommand_%u", cmd_id);
        
        int ret = k_msgq_put(&command_msgq, &cmd, K_NO_WAIT);
        
        if (ret == 0) {
            atomic_inc(&messages_sent);
            LOG_INF("[CmdSender] Sent command %u", cmd_id);
        } else {
            LOG_WRN("[CmdSender] Failed to send command");
        }
    }
}

/*
 * 消息队列查看示例
 */
static void peek_thread(void *p1, void *p2, void *p3)
{
    ARG_UNUSED(p1);
    ARG_UNUSED(p2);
    ARG_UNUSED(p3);
    
    struct sensor_data data;
    
    while (1) {
        k_sleep(K_SECONDS(1));
        
        /* 查看队列头部而不移除 */
        int ret = k_msgq_peek(&sensor_msgq, &data);
        
        if (ret == 0) {
            LOG_INF("[PeekThread] Next in queue: seq=%u, temp=%.1f",
                    data.sequence_num, data.temperature);
        } else {
            LOG_DBG("[PeekThread] Queue empty");
        }
    }
}

/*
 * 队列监控线程
 */
static void monitor_thread(void *p1, void *p2, void *p3)
{
    ARG_UNUSED(p1);
    ARG_UNUSED(p2);
    ARG_UNUSED(p3);
    
    while (1) {
        k_sleep(K_SECONDS(5));
        
        LOG_INF("\n[Monitor] Statistics:");
        LOG_INF("  Messages sent: %u", atomic_get(&messages_sent));
        LOG_INF("  Messages received: %u", atomic_get(&messages_received));
        LOG_INF("  Queue full events: %u", atomic_get(&queue_full_count));
        LOG_INF("  Msgq used: %u/%u", k_msgq_num_used_get(&sensor_msgq),
                k_msgq_num_msgs_get(&sensor_msgq));
        
        /* 清理消息队列（演示） */
        if (k_msgq_num_used_get(&sensor_msgq) > 15) {
            LOG_WRN("[Monitor] Purging sensor message queue");
            k_msgq_purge(&sensor_msgq);
        }
    }
}

/* 线程栈 */
K_THREAD_STACK_DEFINE(sensor_fifo_stack, 2048);
K_THREAD_STACK_DEFINE(sensor_msgq_stack, 2048);
K_THREAD_STACK_DEFINE(data_fifo_stack, 2048);
K_THREAD_STACK_DEFINE(data_msgq_stack, 2048);
K_THREAD_STACK_DEFINE(command_stack, 2048);
K_THREAD_STACK_DEFINE(cmd_sender_stack, 2048);
K_THREAD_STACK_DEFINE(peek_stack, 2048);
K_THREAD_STACK_DEFINE(monitor_stack, 2048);

static struct k_thread sensor_fifo_thread_data;
static struct k_thread sensor_msgq_thread_data;
static struct k_thread data_fifo_thread_data;
static struct k_thread data_msgq_thread_data;
static struct k_thread command_thread_data;
static struct k_thread cmd_sender_thread_data;
static struct k_thread peek_thread_data;
static struct k_thread monitor_thread_data;

int main(void)
{
    printk("\n=== Zephyr RTOS Queue Demo ===\n\n");
    
    /* 创建线程 */
    k_thread_create(&sensor_fifo_thread_data, sensor_fifo_stack,
                    K_THREAD_STACK_SIZEOF(sensor_fifo_stack),
                    sensor_fifo_thread, NULL, NULL, NULL,
                    K_PRIO_PREEMPT(5), 0, K_NO_WAIT);
    
    k_thread_create(&sensor_msgq_thread_data, sensor_msgq_stack,
                    K_THREAD_STACK_SIZEOF(sensor_msgq_stack),
                    sensor_msgq_thread, NULL, NULL, NULL,
                    K_PRIO_PREEMPT(5), 0, K_NO_WAIT);
    
    k_thread_create(&data_fifo_thread_data, data_fifo_stack,
                    K_THREAD_STACK_SIZEOF(data_fifo_stack),
                    data_fifo_thread, NULL, NULL, NULL,
                    K_PRIO_PREEMPT(4), 0, K_NO_WAIT);
    
    k_thread_create(&data_msgq_thread_data, data_msgq_stack,
                    K_THREAD_STACK_SIZEOF(data_msgq_stack),
                    data_msgq_thread, NULL, NULL, NULL,
                    K_PRIO_PREEMPT(4), 0, K_NO_WAIT);
    
    k_thread_create(&command_thread_data, command_stack,
                    K_THREAD_STACK_SIZEOF(command_stack),
                    command_thread, NULL, NULL, NULL,
                    K_PRIO_PREEMPT(3), 0, K_NO_WAIT);
    
    k_thread_create(&cmd_sender_thread_data, cmd_sender_stack,
                    K_THREAD_STACK_SIZEOF(cmd_sender_stack),
                    command_sender_thread, NULL, NULL, NULL,
                    K_PRIO_PREEMPT(3), 0, K_NO_WAIT);
    
    k_thread_create(&peek_thread_data, peek_stack,
                    K_THREAD_STACK_SIZEOF(peek_stack),
                    peek_thread, NULL, NULL, NULL,
                    K_PRIO_PREEMPT(2), 0, K_NO_WAIT);
    
    k_thread_create(&monitor_thread_data, monitor_stack,
                    K_THREAD_STACK_SIZEOF(monitor_stack),
                    monitor_thread, NULL, NULL, NULL,
                    K_PRIO_PREEMPT(1), 0, K_NO_WAIT);
    
    /* 主循环 */
    while (1) {
        k_sleep(K_SECONDS(30));
    }
    
    return 0;
}
```

### 5.2 通信机制对比表

| 功能 | FreeRTOS API | Zephyr API | 说明 |
|------|-------------|-----------|------|
| **创建队列** | `xQueueCreate()` | `K_MSGQ_DEFINE()` | Zephyr静态定义 |
| **发送** | `xQueueSend()` | `k_msgq_put()` | 两者都支持超时 |
| **接收** | `xQueueReceive()` | `k_msgq_get()` | - |
| **覆盖发送** | `xQueueOverwrite()` | 不支持 | FreeRTOS特有 |
| **查看** | `xQueuePeek()` | `k_msgq_peek()` | - |
| **队列集** | `xQueueCreateSet()` | `k_poll()` | Zephyr更灵活 |
| **FIFO** | 需用队列模拟 | `K_FIFO_DEFINE()` | Zephyr原生支持 |
| **LIFO** | 需用队列模拟 | `K_LIFO_DEFINE()` | Zephyr原生支持 |
| **内存管理** | 队列分配 | `k_mem_slab` | Zephyr更细粒度 |
| **消息数量** | `uxQueueMessagesWaiting()` | `k_msgq_num_used_get()` | - |
| **清除队列** | `xQueueReset()` | `k_msgq_purge()` | - |

---

