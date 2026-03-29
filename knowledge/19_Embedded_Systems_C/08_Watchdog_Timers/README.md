# Watchdog (看门狗) 机制深度解析

> **核心功能**: 系统故障检测与恢复
> **安全机制**: 独立时钟、窗口喂狗、早期预警
> **可靠性指标**: MTBF、故障覆盖率、恢复时间
> **最后更新**: 2026-03-29

---

## 1. Watchdog类型与架构

### 1.1 Watchdog分类层次

```
┌─────────────────────────────────────────────────────────────────────────────┐
│                       Watchdog分类层次结构                                    │
├─────────────────────────────────────────────────────────────────────────────┤
│                                                                             │
│  Watchdog Timer (WDT)                                                        │
│  │                                                                          │
│  ├── 硬件看门狗 (Hardware WDT)                                               │
│  │   ├── 片内看门狗 (On-chip)                                                │
│  │   │   ├── 独立看门狗 (IWDG)                                               │
│  │   │   │   ├── 专用低速时钟 (LSI)                                          │
│  │   │   │   ├── 复位域独立                                                  │
│  │   │   │   └── 最可靠                                                      │
│  │   │   │                                                                  │
│  │   │   └── 窗口看门狗 (WWDG)                                               │
│  │   │       ├── 系统时钟驱动                                                │
│  │   │       ├── 早期中断预警                                                │
│  │   │       └── 时间窗口约束                                                │
│  │   │                                                                      │
│  │   └── 外部看门狗 (External)                                               │
│  │       ├── 专用WDT芯片 (如MAX6369)                                         │
│  │       ├── 监控多路电源                                                    │
│  │       └── 适用于安全关键系统                                              │
│  │                                                                          │
│  ├── 软件看门狗 (Software WDT)                                               │
│  │   ├── 任务级监控                                                          │
│  │   │   └── RTOS任务看门狗                                                  │
│  │   │       ├── 各任务独立喂狗                                              │
│  │   │       ├── 检测任务死锁                                                │
│  │   │       └── 定位故障任务                                                │
│  │   │                                                                      │
│  │   └── 算法级监控                                                          │
│  │       ├── 程序流监控                                                      │
│  │       ├── 数据一致性检查                                                  │
│  │       └── 心跳机制                                                        │
│  │                                                                          │
│  └── 复合看门狗 (Compound WDT)                                               │
│      └── 多级监控体系                                                        │
│          ├── 硬件WDT → 系统级复位                                            │
│          ├── 软件WDT → 任务级恢复                                            │
│          └── 外部WDT → 电源级控制                                            │
│                                                                             │
└─────────────────────────────────────────────────────────────────────────────┘
```

### 1.2 看门狗对比矩阵

| 特性 | IWDG | WWDG | 外部WDT | 软件WDT |
|------|------|------|---------|---------|
| **时钟源** | 独立LSI | 系统时钟 | 外部晶振 | CPU时钟 |
| **时钟故障免疫** | 是 | 否 | 是 | 否 |
| **早期预警** | 否 | 是 | 可选 | 是 |
| **窗口约束** | 否 | 是 | 可选 | 可配置 |
| **调试支持** | 可冻结 | 可冻结 | 外部控制 | 可冻结 |
| **复位类型** | 系统复位 | 系统复位 | 系统/电源 | 任务级 |
| **成本** | 内置 | 内置 | 外部元件 | 软件开销 |

---

## 2. 独立看门狗 (IWDG) 深度分析

### 2.1 IWDG形式化定义

**定义 2.1 (独立看门狗)**

```
IWDG = (Prescaler, ReloadValue, DownCounter, EnableControl)

超时时间计算:
  T_timeout = (Prescaler × ReloadValue) / f_LSI

  其中:
    f_LSI = 32kHz (典型值, 范围17-47kHz)
    Prescaler ∈ {/4, /8, /16, /32, /64, /128, /256}
    ReloadValue ∈ [0x000, 0xFFF] (12-bit)

超时范围:
  最小: (4 × 1) / 32000 = 125μs
  最大: (256 × 4096) / 32000 ≈ 32.8s

喂狗操作:
  向KR寄存器写入0xAAAA序列
  操作必须原子性执行

启动机制:
  软件启动: 向KR写入0xCCCC
  硬件启动: 选项字节配置

  安全建议: 硬件强制启动，不可软件禁用

关键寄存器:
  KR (Key Register): 0xCCCC=启动, 0xAAAA=喂狗, 0x5555=解锁PR/RLR
  PR (Prescaler): 预分频配置
  RLR (Reload Register): 重载值配置
  SR (Status): 更新状态指示
```

### 2.2 IWDG可靠性定理

**定理 2.1 (IWDG故障覆盖率)**

```
定理: IWDG能检测以下故障类型:

可检测故障:
  1. 程序跑飞 (PC指向无效地址)
  2. 死循环 (while(1)等)
  3. 任务死锁 (RTOS中)
  4. 栈溢出 (导致代码执行异常)
  5. 时钟故障 (主时钟停止，LSI继续)
  6. 部分硬件故障 (导致代码卡死)

不可检测故障:
  1. 周期性喂狗但功能异常
  2. 喂狗代码被意外执行
  3. 时序问题 (偶发性错误在喂狗间隔内恢复)
  4. 安全关键计算错误 (但程序流正常)

覆盖率提升策略:
  - 合理设置超时时间 (典型100ms-1s)
  - 多位置喂狗 (关键路径检查)
  - 窗口看门狗约束喂狗时机
  - 软件检查配合 (任务健康度)

故障覆盖率公式:
  Coverage = 1 - (T_critical / T_timeout)

  其中 T_critical = 故障导致危害的最大允许时间

  示例:
    T_critical = 50ms (电机失控前的安全时间)
    T_timeout = 100ms
    Coverage = 1 - (50/100) = 50%

    T_timeout = 20ms
    Coverage = 1 - (50/20) = -150% (不够，需更快检测)

    T_timeout = 10ms
    Coverage = 60% (需结合其他安全措施)
```

### 2.3 IWDG喂狗策略决策树

```
IWDG喂狗策略选择
│
├── 裸机程序?
│   ├── 是
│   │   ├── 主循环喂狗
│   │   │   └── 放在主循环最后
│   │   │   └── 确保所有关键任务完成
│   │   │
│   │   └── 中断中不喂狗
│   │       └── 防止中断风暴导致主循环饿死
│   │
│   └── 否 (RTOS)
│       └── 专用喂狗任务
│           ├── 最高优先级任务
│           ├── 检查所有任务健康
│           └── 条件喂狗
│
├── 安全关键系统?
│   ├── 是 (SIL/ASIL)
│   │   ├── 多位置喂狗检查
│   │   │   ├── 程序流检查
│   │   │   ├── 数据一致性
│   │   │   └── 通信心跳
│   │   │
│   │   └── 窗口看门狗配合
│   │       └── 防止过快喂狗
│   │
│   └── 否 (一般应用)
│       └── 单点喂狗
│           └── 主循环末尾
│
└── 调试需求?
    ├── 是
    │   └── 调试模式禁用
    │       └── 断点不触发复位
    │
    └── 否
        └── 强制使能
            └── 生产环境必须开启
```

---

## 3. 窗口看门狗 (WWDG) 深度分析

### 3.1 WWDG形式化定义

**定义 3.1 (窗口看门狗)**

```
WWDG = (Prescaler, WindowValue, CounterValue, EarlyInterrupt)

时间窗口定义:
  T_window_min = (Prescaler × (0x40 - WindowValue)) / f_PCLK
  T_window_max = (Prescaler × 0x40) / f_PCLK

  其中:
    f_PCLK = APB时钟频率
    Prescaler ∈ {/1, /2, /4, /8}
    WindowValue ∈ [0x40, 0x7F]
    CounterValue ∈ [0x40, 0x7F]

喂狗约束:
  必须在 T_window_min < t < T_window_max 内喂狗

  过早喂狗 (t < T_window_min) → 复位
  过晚喂狗 (t > T_window_max) → 复位

早期预警中断 (EWI):
  当 Counter = 0x40 时触发中断
  允许在复位前执行安全操作
  可用于数据保存或 graceful shutdown

计数器特性:
  7-bit递减计数器 (0x7F → 0x3F)
  0x40是MSB清零阈值
  0x3F以下触发复位

超时计算示例 (PCLK=48MHz):
  Prescaler = /8
  Window = 0x60
  Counter初始 = 0x7F

  T_max = (8 × 64) / 48MHz = 10.67ms
  T_min = (8 × (64 - (0x7F-0x60))) / 48MHz = 5.33ms

  有效窗口: 5.33ms ~ 10.67ms
```

### 3.2 WWDG状态机

```
┌─────────────────────────────────────────────────────────────────────────────┐
│                       WWDG窗口约束状态机                                      │
├─────────────────────────────────────────────────────────────────────────────┤
│                                                                             │
│  状态定义:                                                                   │
│  ┌──────────────┐  ┌──────────────┐  ┌──────────────┐  ┌──────────────┐    │
│  │   IDLE       │  │   EARLY      │  │   WINDOW     │  │   LATE       │    │
│  │   未启动     │  │   过早区     │  │   有效窗口   │  │   过晚区     │    │
│  └──────────────┘  └──────────────┘  └──────────────┘  └──────────────┘    │
│                                                                             │
│  状态转换:                                                                   │
│                                                                             │
│  IDLE ──[启动]──→ EARLY ──[计数递减]──→ WINDOW ──[继续计数]──→ LATE        │
│                      │                    │                      │         │
│                      │ 喂狗              │ 喂狗               │ 超时      │
│                      ▼                    ▼                      ▼         │
│                   [复位]              [正常]                  [复位]       │
│                                                                             │
│  时间线:                                                                     │
│  ─────────                                                                   │
│                                                                             │
│  Counter:   0x7F → 0x60 ─────────────────────────→ 0x40 → 0x3F → [复位]    │
│              │      │                              │      │                │
│              │      └── T_min                      │      └── T_max        │
│              │         (窗口开始)                  │         (窗口结束)    │
│              │                                      │                       │
│              └── 启动点                            └── EWI中断点            │
│                                                                             │
│  有效喂狗窗口: [T_min, T_max]                                               │
│                                                                             │
│  喂狗时机决策:                                                               │
│  ────────────                                                                │
│  1. 等待EWI中断 (推荐)                                                        │
│     - 在中断服务程序中喂狗                                                    │
│     - 确保在窗口晚期喂狗                                                      │
│     - 最大化故障检测时间                                                      │
│                                                                             │
│  2. 轮询计数器值                                                              │
│     - 检查 Counter < WindowValue                                              │
│     - 然后执行喂狗                                                            │
│     - 增加软件开销                                                            │
│                                                                             │
└─────────────────────────────────────────────────────────────────────────────┘
```

---

## 4. 多级看门狗架构

### 4.1 复合看门狗决策矩阵

```
多级看门狗架构设计
│
├── 系统级监控 (硬件IWDG)
│   ├── 超时时间: 100ms-1s
│   ├── 复位类型: 系统硬复位
│   └── 故障覆盖:
│       ├── 程序跑飞
│       ├── 死循环
│       └── 主时钟故障
│
├── 任务级监控 (软件WDT)
│   ├── 超时时间: 10-100ms
│   ├── 恢复类型: 任务重启/系统复位
│   └── 故障覆盖:
│       ├── 任务死锁
│       ├── 任务饿死
│       └── 优先级反转
│
├── 通信级监控
│   ├── 心跳机制
│   ├── 超时时间: 协议定义
│   └── 故障覆盖:
│       ├── 通信中断
│       ├── 数据超时
│       └── 对端故障
│
└── 外部监控 (外部WDT芯片)
    ├── 监控范围: 电源/温度/复位
    ├── 超时时间: 可配置
    └── 故障覆盖:
        ├── 电源故障
        ├── 过热
        └── 软件禁用看门狗

层级响应策略:
  Level 1 (软件)  → 尝试恢复任务
  Level 2 (WWDG)  → 保存关键数据后复位
  Level 3 (IWDG)  → 立即系统复位
  Level 4 (外部)  → 断电重启
```

### 4.2 RTOS任务看门狗实现

```c
/* RTOS任务看门狗形式化实现 */

#define MAX_TASKS 8
#define TASK_WDG_TIMEOUT_MS 500

typedef struct {
    TaskHandle_t task_handle;
    char task_name[16];
    uint32_t last_feed_time;
    uint8_t is_alive;
    uint8_t timeout_ms;
} TaskWatchdogEntry;

typedef struct {
    TaskWatchdogEntry tasks[MAX_TASKS];
    uint8_t task_count;
    SemaphoreHandle_t mutex;
    TimerHandle_t check_timer;
} TaskWatchdogManager;

TaskWatchdogManager twdgm;

/* 初始化任务看门狗管理器 */
void task_wdg_init(void) {
    twdgm.mutex = xSemaphoreCreateMutex();
    twdgm.task_count = 0;

    /* 创建检查定时器 */
    twdgm.check_timer = xTimerCreate(
        "TaskWDG",
        pdMS_TO_TICKS(100),  /* 100ms检查周期 */
        pdTRUE,              /* 自动重载 */
        NULL,
        task_wdg_check_callback
    );

    xTimerStart(twdgm.check_timer, 0);
}

/* 注册任务到看门狗 */
BaseType_t task_wdg_register(TaskHandle_t task, const char *name, uint8_t timeout) {
    xSemaphoreTake(twdgm.mutex, portMAX_DELAY);

    if (twdgm.task_count >= MAX_TASKS) {
        xSemaphoreGive(twdgm.mutex);
        return pdFAIL;
    }

    TaskWatchdogEntry *entry = &twdgm.tasks[twdgm.task_count++];
    entry->task_handle = task;
    strncpy(entry->task_name, name, 15);
    entry->task_name[15] = '\0';
    entry->last_feed_time = xTaskGetTickCount();
    entry->is_alive = pdTRUE;
    entry->timeout_ms = timeout;

    xSemaphoreGive(twdgm.mutex);
    return pdPASS;
}

/* 任务喂狗 */
void task_wdg_feed(TaskHandle_t task) {
    xSemaphoreTake(twdgm.mutex, portMAX_DELAY);

    for (int i = 0; i < twdgm.task_count; i++) {
        if (twdgm.tasks[i].task_handle == task) {
            twdgm.tasks[i].last_feed_time = xTaskGetTickCount();
            twdgm.tasks[i].is_alive = pdTRUE;
            break;
        }
    }

    xSemaphoreGive(twdgm.mutex);
}

/* 看门狗检查回调 */
void task_wdg_check_callback(TimerHandle_t xTimer) {
    uint32_t current_time = xTaskGetTickCount();
    uint8_t all_alive = pdTRUE;

    xSemaphoreTake(twdgm.mutex, portMAX_DELAY);

    for (int i = 0; i < twdgm.task_count; i++) {
        TaskWatchdogEntry *entry = &twdgm.tasks[i];

        uint32_t elapsed = (current_time - entry->last_feed_time) * portTICK_PERIOD_MS;

        if (elapsed > entry->timeout_ms) {
            /* 任务超时 */
            entry->is_alive = pdFALSE;
            all_alive = pdFALSE;

            /* 记录故障 */
            log_error("Task %s watchdog timeout (%d ms)",
                      entry->task_name, elapsed);

            /* 尝试重启任务 */
            task_wdg_restart_task(entry);
        }
    }

    xSemaphoreGive(twdgm.mutex);

    /* 如果有任务超时，可选触发硬件看门狗 */
    if (!all_alive && is_critical_system()) {
        /* 不喂硬件看门狗，触发系统复位 */
    } else {
        /* 喂硬件看门狗 */
        iwdg_feed();
    }
}

/* 任务喂狗宏 */
#define TASK_FEED_WDG() task_wdg_feed(xTaskGetCurrentTaskHandle())

/* 任务实现示例 */
void sensor_task(void *pvParameters) {
    /* 注册到看门狗 */
    task_wdg_register(xTaskGetCurrentTaskHandle(), "Sensor", 200);

    for (;;) {
        /* 读取传感器 */
        read_sensors();

        /* 处理数据 */
        process_data();

        /* 关键: 喂狗 */
        TASK_FEED_WDG();

        vTaskDelay(pdMS_TO_TICKS(50));
    }
}
```

---

## 5. 看门狗设计最佳实践

### 5.1 超时时间选择决策矩阵

| 应用场景 | 推荐超时 | 理由 |
|----------|----------|------|
| **电机控制** | 10-50ms | 快速失控保护 |
| **通信网关** | 100-500ms | 允许帧处理时间 |
| **传感器节点** | 1-10s | 低功耗长周期 |
| **安全关键** | 10-100ms | SIL/ASIL要求 |
| **用户界面** | 500ms-2s | 允许用户操作 |

### 5.2 看门狗测试策略

```
看门狗验证测试
│
├── 功能测试
│   ├── 正常喂狗
│   │   └── 系统不应复位
│   │
│   ├── 停止喂狗
│   │   └── 应在超时后复位
│   │
│   └── 窗口测试 (WWDG)
│       ├── 过早喂狗 → 复位
│       └── 过晚喂狗 → 复位
│
├── 故障注入
│   ├── 时钟故障
│   │   └── IWDG应能检测 (LSI独立)
│   │
│   ├── 中断风暴
│   │   └── 看门狗不应被饿死
│   │
│   └── 栈破坏
│       └── 应触发看门狗复位
│
└── 边界测试
    ├── 最小超时
    │   └── 验证精度
    │
    ├── 最大超时
    │   └── 验证范围
    │
    └── 温度影响
        └── 验证时钟漂移
```

---

## 6. 参考资源

- **STM32 Reference Manual**: IWDG and WWDG chapters
- **IEC 61508**: Functional Safety - Watchdog requirements
- **ISO 26262**: Automotive safety - Watchdog timer specifications
- **Cortex-M Devices Generic User Guide**: Watchdog timer implementation
