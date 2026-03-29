# 定时器与PWM深度解析

> **核心功能**: 时间基准、波形生成、输入捕获
> **关键指标**: 分辨率、精度、抖动
> **应用场景**: 电机控制、通信时序、测量系统
> **最后更新**: 2026-03-29

---

## 1. 定时器架构形式化定义

### 1.1 定时器分类层次结构

```
┌─────────────────────────────────────────────────────────────────────────────┐
│                       定时器分类层次结构                                      │
├─────────────────────────────────────────────────────────────────────────────┤
│                                                                             │
│  定时器 (Timers)                                                            │
│  │                                                                          │
│  ├── 基本定时器 (Basic Timers)                                              │
│  │   └── TIM6, TIM7                                                        │
│  │       ├── 功能: 时间基准、触发DAC                                        │
│  │       ├── 分辨率: 16-bit                                                │
│  │       └── 特性: 无输入/输出通道                                          │
│  │                                                                          │
│  ├── 通用定时器 (General Purpose Timers)                                    │
│  │   ├── 2通道: TIM9, TIM12                                               │
│  │   │   └── 2×输入捕获/输出比较/PWM                                       │
│  │   │                                                                      │
│  │   └── 4通道: TIM2-TIM5, TIM10-TIM11                                     │
│  │       ├── 4×输入捕获/输出比较/PWM                                       │
│  │       ├── 编码器接口                                                     │
│  │       ├── 霍尔传感器接口                                                 │
│  │       └── 分辨率: 16/32-bit                                              │
│  │                                                                          │
│  └── 高级定时器 (Advanced Timers)                                           │
│      └── TIM1, TIM8                                                        │
│          ├── 通用定时器全部功能                                             │
│          ├── 互补PWM输出 (死区插入)                                         │
│          ├── 刹车输入 (Break Input)                                         │
│          ├── 重复计数器                                                     │
│          └── 电机控制专用功能                                               │
│                                                                             │
└─────────────────────────────────────────────────────────────────────────────┘
```

### 1.2 定时器核心参数形式化定义

**定义 1.1 (定时器时间基准)**

```
定时器时钟 = CK_CNT = CK_PSC / (PSC + 1)

其中:
  CK_PSC = 定时器外设时钟 (如 72MHz, 168MHz)
  PSC = 预分频器值 (Prescaler)
  PSC ∈ [0, 65535]

计数器周期 = (ARR + 1) × T_CNT
           = (ARR + 1) × (PSC + 1) / CK_PSC

其中:
  ARR = 自动重装载值 (Auto-Reload Register)
  ARR ∈ [0, 65535] 或 [0, 4294967295] (32-bit)

PWM频率公式:
  f_PWM = CK_PSC / ((PSC + 1) × (ARR + 1))

PWM占空比公式:
  D = CCR / (ARR + 1)

  其中:
    CCR = 捕获/比较寄存器值
    D ∈ [0, 1]
```

### 1.3 参数配置矩阵

| 目标PWM频率 | CK_PSC | PSC | ARR | 分辨率 | 适用场景 |
|-------------|--------|-----|-----|--------|----------|
| 50 Hz (伺服) | 72MHz | 71 | 19999 | 0.005% | 舵机控制 |
| 1 kHz (LED) | 72MHz | 0 | 71999 | 0.0014% | LED调光 |
| 20 kHz (电机) | 72MHz | 0 | 3599 | 0.028% | BLDC控制 |
| 1 MHz (高速) | 168MHz | 0 | 167 | 0.6% | 高速测量 |

---

## 2. PWM生成深度分析

### 2.1 PWM模式比较矩阵

```
┌─────────────────────────────────────────────────────────────────────────────┐
│                       PWM模式比较矩阵                                        │
├─────────────────────────────────────────────────────────────────────────────┤
│                                                                             │
│  模式                PWM Mode 1              PWM Mode 2                     │
│  ─────────────────────────────────────────────────────────────────────────  │
│                                                                             │
│  描述               向上计数:                 向上计数:                      │
│                     计数 < CCR: 有效         计数 < CCR: 无效               │
│                     计数 ≥ CCR: 无效         计数 ≥ CCR: 有效               │
│                                                                             │
│  波形特性           占空比 ∝ CCR              占空比 ∝ 1-CCR                 │
│                                                                             │
│  适用场景           通用PWM控制               需要反向逻辑的应用             │
│                                                                             │
│  互补输出           支持死区                  支持死区                       │
│                                                                             │
│  中间对齐模式       支持                      支持                           │
│  (Center-aligned)   双边缘对称PWM            双边缘对称PWM (反向)            │
│                                                                             │
└─────────────────────────────────────────────────────────────────────────────┘
```

### 2.2 PWM波形生成状态机

```
PWM生成状态机 (向上计数模式)

状态定义:
  S0: IDLE     - 空闲，计数器停止
  S1: COUNTING - 正在计数 0 → ARR
  S2: OVERFLOW - 计数器溢出，重置

状态转换:
  S0 ──[使能计数器]──→ S1
                        │
                        │ 计数 < CCR
                        ↓
                       S1 ──[CNT < CCR]──→ OCxREF = Active
                        │
                        │ 计数 ≥ CCR
                        ↓
                       S1 ──[CNT ≥ CCR]──→ OCxREF = Inactive
                        │
                        │ 计数 = ARR
                        ↓
                       S2 ──[溢出中断]──→ S1 (重置)

形式化输出函数:
  OCxREF = { Active,   if CNT < CCR
           { Inactive, if CNT ≥ CCR

其中:
  CNT = 当前计数值
  CCR = 捕获/比较寄存器值

有效电平定义 (CCxP位):
  Active = { High,  if CCxP = 0
           { Low,   if CCxP = 1
```

### 2.3 高级PWM：死区插入与互补输出

```
┌─────────────────────────────────────────────────────────────────────────────┐
│                      互补PWM与死区插入                                        │
├─────────────────────────────────────────────────────────────────────────────┤
│                                                                             │
│  应用场景: H桥驱动、三相逆变器                                              │
│                                                                             │
│  信号时序图:                                                                │
│                                                                             │
│  原始PWM (OCxREF)    ┌──┐  ┌──┐  ┌──┐  ┌──┐  ┌──┐                          │
│                      │  │  │  │  │  │  │  │  │  │                          │
│                     ─┘  └──┘  └──┘  └──┘  └──┘  └─                         │
│                      ↑  ↑    ↑  ↑                                           │
│                      │  │    │  └── 死区时间                                │
│                      │  │    └──── 死区时间                                 │
│                      │  └───────── 有效高电平                               │
│                      └──────────── 有效高电平                               │
│                                                                             │
│  正向输出 (OCx)      ┌──┐     ┌──┐     ┌──┐                                │
│  (高边开关)          │  │     │  │     │  │                                │
│                     ─┘  └─────┘  └─────┘  └─────                           │
│                         ↑  ↑     ↑  ↑                                       │
│                         │  │     │  │                                       │
│                         └──┴─────┴──┴── 死区 (两路均为低)                   │
│                                                                             │
│  反向输出 (OCxN)     ─┐     ┌─────┐     ┌─────                             │
│  (低边开关)           │     │     │     │                                  │
│                      └─────┘     └─────┘                                   │
│                      ↑  ↑        ↑  ↑                                       │
│                      │  │        │  │                                       │
│                      └──┴────────┴──┘  死区 (两路均为低)                    │
│                                                                             │
│  死区时间计算:                                                              │
│  ────────────                                                               │
│  DT = DTG[7:5] × DTG[4:0] × T_DTG                                         │
│                                                                             │
│  其中 DTG[7:0] 是死区时间寄存器值                                           │
│                                                                             │
└─────────────────────────────────────────────────────────────────────────────┘
```

### 2.4 死区时间计算代码

```c
/* 死区时间配置形式化定义 */

typedef struct {
    uint16_t deadtime_ns;       /* 死区时间 (纳秒) */
    uint32_t timer_clock_hz;    /* 定时器时钟频率 */
} DeadTimeConfig;

/* 死区时间计算
 *
 * 定时器时钟周期: T_dtg = 1 / CK_INT
 *
 * 根据 DTG[7:5] 值不同，计算方式不同:
 *
 * 情况1: DTG[7:5] = 0xx
 *   DT = DTG[7:0] × T_dtg
 *   范围: 0 ~ 127 × T_dtg
 *
 * 情况2: DTG[7:5] = 10x
 *   DT = (64 + DTG[5:0]) × 2 × T_dtg
 *   范围: 128 ~ 254 × T_dtg
 *
 * 情况3: DTG[7:5] = 110
 *   DT = (32 + DTG[4:0]) × 8 × T_dtg
 *   范围: 256 ~ 504 × T_dtg
 *
 * 情况4: DTG[7:5] = 111
 *   DT = (32 + DTG[4:0]) × 16 × T_dtg
 *   范围: 512 ~ 1008 × T_dtg
 */
uint8_t calculate_deadtime_register(uint32_t timer_clock_hz,
                                    uint16_t deadtime_ns) {
    /* 计算所需的时钟周期数 */
    uint32_t t_dtg_ns = 1000000000UL / timer_clock_hz;
    uint32_t dtg_ticks = deadtime_ns / t_dtg_ns;

    uint8_t dtg_reg;

    if (dtg_ticks <= 127) {
        /* 情况1 */
        dtg_reg = (uint8_t)dtg_ticks;
    } else if (dtg_ticks <= 254) {
        /* 情况2 */
        dtg_reg = 0x80 | (((uint8_t)dtg_ticks / 2 - 64) & 0x3F);
    } else if (dtg_ticks <= 504) {
        /* 情况3 */
        dtg_reg = 0xC0 | (((uint8_t)dtg_ticks / 8 - 32) & 0x1F);
    } else if (dtg_ticks <= 1008) {
        /* 情况4 */
        dtg_reg = 0xE0 | (((uint8_t)dtg_ticks / 16 - 32) & 0x1F);
    } else {
        /* 超出范围 */
        dtg_reg = 0xFF;  /* 最大死区 */
    }

    return dtg_reg;
}

/* 示例计算 (168MHz定时器, 100ns死区):
 *   T_dtg = 1/168MHz = 5.95ns
 *   dtg_ticks = 100 / 5.95 = 16.8 → 17
 *   DTG = 17 (情况1)
 *   实际死区 = 17 × 5.95 = 101.15ns
 */
```

---

## 3. 输入捕获模式

### 3.1 输入捕获时序分析

```
┌─────────────────────────────────────────────────────────────────────────────┐
│                      输入捕获时序分析                                         │
├─────────────────────────────────────────────────────────────────────────────┤
│                                                                             │
│  输入信号 (TI1)      ┌────┐    ┌────┐    ┌────┐                            │
│                     │    │    │    │    │    │                            │
│                  ───┘    └────┘    └────┘    └────                         │
│                     ↑         ↑         ↑                                  │
│                     │         │         │                                  │
│  捕获事件           C1        C2        C3                                 │
│                                                                             │
│  计数值 (CNT)       V1        V2        V3  (被锁存到CCR1)                  │
│                                                                             │
│  测量应用:                                                                  │
│  ────────                                                                   │
│  1. 脉冲宽度:  Period = C2 - C1 (如果捕获上升沿)                           │
│  2. 频率:       F = CK_CNT / (C2 - C1)                                     │
│  3. 占空比:     D = (C2 - C1) / (C3 - C1) (需要双通道)                     │
│                                                                             │
│  滤波与边沿检测:                                                            │
│  ──────────────                                                             │
│  滤波器: f_SAMPLING = CK_INT / (2^N × N)                                   │
│  其中 N = 滤波器配置值                                                      │
│                                                                             │
└─────────────────────────────────────────────────────────────────────────────┘
```

### 3.2 输入捕获模式矩阵

| 捕获模式 | 触发边沿 | 预分频 | 适用场景 | 精度 |
|----------|----------|--------|----------|------|
| **直接** | 上升沿 | 1:1 | 频率测量 | 1时钟 |
| **间接** | 下降沿 | 1:1 | 脉宽测量 | 1时钟 |
| **PWM输入** | 双沿 | 1:1 | 周期+占空比 | 1时钟 |
| **预分频** | 上升沿 | 1:N | 低频信号 | N时钟 |

### 3.3 PWM输入捕获实现

```c
/* PWM输入捕获形式化实现
 *
 * 使用两个通道测量频率和占空比:
 * - 通道1: 捕获周期 (上升沿)
 * - 通道2: 捕获占空比 (下降沿)
 */

typedef struct {
    uint32_t period;        /* PWM周期 (计数器值) */
    uint32_t duty;          /* 高电平时间 (计数器值) */
    uint32_t frequency;     /* 计算频率 */
    uint8_t  capture_done;  /* 捕获完成标志 */
} PWMInputCapture;

volatile PWMInputCapture pwm_capture;

/* 定时器中断处理 */
void TIM_IRQHandler(void) {
    if (TIM_GetITStatus(TIMx, TIM_IT_CC1)) {
        /* 通道1: 上升沿捕获 (周期结束) */
        static uint32_t last_ccr1 = 0;
        uint32_t current_ccr1 = TIM_GetCapture1(TIMx);

        pwm_capture.period = current_ccr1 - last_ccr1;
        last_ccr1 = current_ccr1;

        /* 通道2已捕获下降沿，计算占空比 */
        if (pwm_capture.duty > 0) {
            pwm_capture.capture_done = 1;
        }

        TIM_ClearITPendingBit(TIMx, TIM_IT_CC1);
    }

    if (TIM_GetITStatus(TIMx, TIM_IT_CC2)) {
        /* 通道2: 下降沿捕获 (高电平结束) */
        uint32_t ccr2 = TIM_GetCapture2(TIMx);
        pwm_capture.duty = ccr2;

        TIM_ClearITPendingBit(TIMx, TIM_IT_CC2);
    }
}

/* 频率计算 */
void calculate_pwm_frequency(void) {
    if (pwm_capture.capture_done) {
        uint32_t timer_clock = SystemCoreClock / (TIMx->PSC + 1);
        pwm_capture.frequency = timer_clock / pwm_capture.period;

        float duty_ratio = (float)pwm_capture.duty / pwm_capture.period;

        printf("Frequency: %lu Hz\n", pwm_capture.frequency);
        printf("Duty: %.2f%%\n", duty_ratio * 100);

        pwm_capture.capture_done = 0;
    }
}

/* 精度分析:
 * 时间分辨率 = 1 / CK_CNT
 *
 * 示例 (72MHz, PSC=0):
 *   分辨率 = 1/72MHz = 13.89ns
 *
 *   对于1kHz PWM:
 *     周期计数 = 72000
 *     频率误差 = 1/72000 = 0.0014%
 *
 *   对于1MHz PWM:
 *     周期计数 = 72
 *     频率误差 = 1/72 = 1.4% (需要提高时钟或使用预分频)
 */
```

---

## 4. 编码器接口模式

### 4.1 正交编码器接口

```
┌─────────────────────────────────────────────────────────────────────────────┐
│                      正交编码器接口原理                                       │
├─────────────────────────────────────────────────────────────────────────────┤
│                                                                             │
│  编码器信号:                                                                │
│                                                                             │
│  通道A (TI1)       ┌──┐  ┌──┐  ┌──┐  ┌──┐  ┌──┐  ┌──┐                     │
│                 ───┘  └──┘  └──┘  └──┘  └──┘  └──┘  └──                   │
│                    ↑     ↑     ↑     ↑     ↑     ↑                         │
│  通道B (TI2)     ─┐  ┌──┐  ┌──┐  ┌──┐  ┌──┐  ┌──┐  ┌─                     │
│                   └──┘  └──┘  └──┘  └──┘  └──┘  └──┘                       │
│                                                                             │
│  方向判断:                                                                  │
│  ────────                                                                   │
│  顺时针: A领先B 90° → 计数器递增                                            │
│  逆时针: B领先A 90° → 计数器递减                                            │
│                                                                             │
│  分辨率提升:                                                                │
│  ──────────                                                                 │
│  1X模式: 每周期计数1次 (仅TI1边沿)                                          │
│  2X模式: 每周期计数2次 (TI1+TI2边沿)                                        │
│  4X模式: 每周期计数4次 (TI1+TI2双沿) ← 最高分辨率                           │
│                                                                             │
│  示例 (1000线编码器):
 *   1X: 1000 counts/rev
 *   2X: 2000 counts/rev
 *   4X: 4000 counts/rev
 */

typedef struct {
    int32_t position;           /* 当前位置 */
    int32_t last_counter;       /* 上次计数器值 */
    int32_t overflow_count;     /* 溢出计数 */
    float velocity;             /* 速度 (counts/s) */
    uint32_t last_time;         /* 上次更新时间 */
} EncoderState;

void encoder_init(TIM_TypeDef *TIMx) {
    /* 配置编码器接口模式3 (TI1+TI2, 双沿检测) */
    TIM_EncoderInterfaceConfig(TIMx,
                               TIM_EncoderMode_TI12,
                               TIM_ICPolarity_Rising,
                               TIM_ICPolarity_Rising);

    /* 使能计数器 */
    TIM_Cmd(TIMx, ENABLE);
}

int32_t encoder_get_position(EncoderState *enc, TIM_TypeDef *TIMx) {
    int16_t current = (int16_t)TIM_GetCounter(TIMx);
    int16_t delta = current - enc->last_counter;

    /* 处理溢出 */
    if (delta > 32768) {
        enc->overflow_count--;
    } else if (delta < -32768) {
        enc->overflow_count++;
    }

    enc->last_counter = current;
    enc->position = (int32_t)enc->overflow_count * 65536 + current;

    return enc->position;
}

/* 速度计算 (周期性调用) */
void encoder_update_velocity(EncoderState *enc, uint32_t current_time_ms) {
    int32_t current_pos = encoder_get_position(enc, TIMx);
    uint32_t dt = current_time_ms - enc->last_time;

    if (dt > 0) {
        enc->velocity = (float)(current_pos - enc->position) * 1000.0f / dt;
    }

    enc->last_time = current_time_ms;
}
```

---

## 5. 定时器同步与主从模式

### 5.1 主从模式矩阵

| 模式 | 功能 | 触发信号 | 应用场景 |
|------|------|----------|----------|
| **复位模式** | 从计数器复位 | 主更新/比较 | 同步启动 |
| **门控模式** | 从计数器使能 | 主电平 | 使能控制 |
| **触发模式** | 从计数器启动 | 主边沿 | 延迟启动 |
| **外部时钟1** | 从时钟源 | 主输出 | 级联时钟 |

### 5.2 多定时器同步架构

```
┌─────────────────────────────────────────────────────────────────────────────┐
│                      多定时器同步架构                                         │
├─────────────────────────────────────────────────────────────────────────────┤
│                                                                             │
│  主定时器 (TIM1)                                                            │
│  ┌─────────────────┐                                                        │
│  │  计数器        │                                                        │
│  │  TRGO (更新)   │────────┬────────────────┐                              │
│  └─────────────────┘        │                │                              │
│                             ↓                ↓                              │
│  从定时器1 (TIM2)       从定时器2 (TIM3)                                    │
│  ┌─────────────────┐    ┌─────────────────┐                                │
│  │  模式: 触发模式  │    │  模式: 触发模式  │                                │
│  │  启动延迟: 0    │    │  启动延迟: 0    │                                │
│  │                 │    │                 │                                │
│  │  PWM输出        │    │  PWM输出        │                                │
│  │  (相位0°)       │    │  (相位120°)     │                                │
│  │                 │    │                 │                                │
│  └─────────────────┘    └─────────────────┘                                │
│                                                                             │
│  应用场景: 三相PWM控制                                                      │
│  - 三个定时器严格同步启动                                                   │
│  - 通过不同CCR值实现相位差                                                  │
│  - 主定时器统一控制频率                                                     │
│                                                                             │
└─────────────────────────────────────────────────────────────────────────────┘
```

---

## 6. 参考资源

- **STM32 Reference Manual**: TIM定时器章节
- **AN4776**: General-purpose timer cookbook
- **RM0090**: Advanced timer configuration
- **IEC 61800**: 可调速电气传动系统标准
