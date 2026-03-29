# DMA (直接内存访问) 深度解析

> **核心特性**: 无需CPU干预的数据传输
> **关键指标**: 吞吐量、延迟、CPU占用率
> **架构**: 总线矩阵、通道仲裁、FIFO缓冲
> **最后更新**: 2026-03-29

---

## 1. DMA架构形式化定义

### 1.1 DMA系统拓扑

```
┌─────────────────────────────────────────────────────────────────────────────┐
│                          DMA系统拓扑结构                                      │
├─────────────────────────────────────────────────────────────────────────────┤
│                                                                             │
│   ┌──────────┐                                                             │
│   │   CPU    │──────┐                                                       │
│   │ (Cortex) │      │                                                       │
│   └──────────┘      │         总线矩阵 (Bus Matrix)                        │
│                     │        ┌─────────────────────────┐                   │
│   ┌──────────┐      └───────→│                         │←────────┐        │
│   │  D-Code  │                │   主设备 (Masters)      │         │        │
│   │  总线    │───────────────→│   - Cortex-M D-Bus      │         │        │
│   └──────────┘                │   - DMA1                │         │        │
│                               │   - DMA2                │         │        │
│   ┌──────────┐                │   - Ethernet DMA        │         │        │
│   │  System  │                │                         │         │        │
│   │  总线    │───────────────→│   从设备 (Slaves)       │         │        │
│   └──────────┘                │   - Flash (代码)        │         │        │
│                               │   - SRAM1, SRAM2        │         │        │
│   ┌──────────┐                │   - AHB1外设           │         │        │
│   │   DMA    │                │   - AHB2外设           │         │        │
│   │ 控制器   │←──────────────→│   - APB外设            │         │        │
│   └──────────┘                │                         │         │        │
│        │                      └─────────────────────────┘         │        │
│        │                                    │                     │        │
│        ↓                                    ↓                     │        │
│   ┌─────────────────────────────────────────────────────────┐    │        │
│   │                    DMA控制器内部结构                     │    │        │
│   │  ┌─────────────────────────────────────────────────────┐ │    │        │
│   │  │  仲裁器 (Arbiter)                                   │ │    │        │
│   │  │  - 固定优先级 / 轮询仲裁                            │ │    │        │
│   │  └─────────────────────────────────────────────────────┘ │    │        │
│   │                           │                              │    │        │
│   │  ┌─────────┐  ┌─────────┐ │  ┌─────────┐  ┌─────────┐  │    │        │
│   │  │ Channel │  │ Channel │ │  │ Channel │  │ Channel │  │    │        │
│   │  │    0    │  │    1    │←┘  │   ...   │  │    N    │  │    │        │
│   │  │         │  │         │    │         │  │         │  │    │        │
│   │  │ • 源地址 │  │ • 源地址 │    │ • 源地址 │  │ • 源地址 │  │    │        │
│   │  │ • 目标地址│  │ • 目标地址│    │ • 目标地址│  │ • 目标地址│  │    │        │
│   │  │ • 计数器 │  │ • 计数器 │    │ • 计数器 │  │ • 计数器 │  │    │        │
│   │  │ • 配置   │  │ • 配置   │    │ • 配置   │  │ • 配置   │  │    │        │
│   │  └─────────┘  └─────────┘    └─────────┘  └─────────┘  │    │        │
│   └─────────────────────────────────────────────────────────┘    │        │
│         │              │              │              │            │        │
│         ↓              ↓              ↓              ↓            │        │
│   ┌──────────┐  ┌──────────┐  ┌──────────┐  ┌──────────┐        │        │
│   │  外设0   │  │  外设1   │  │  外设2   │  │  外设3   │        │        │
│   │ (ADC)    │  │ (USART)  │  │ (SPI)    │  │ (TIM)    │        │        │
│   └──────────┘  └──────────┘  └──────────┘  └──────────┘        │        │
│                                                                 │        │
└─────────────────────────────────────────────────────────────────┘        │
                                                                           │
   总线带宽分配公式:                                                        │
   ─────────────────                                                        │
   总线占用率 = Σ(通道i带宽需求) / 总线带宽 × 100%                          │
                                                                            │
   约束: Σ(通道i带宽) ≤ 总线带宽 × 0.8 (留20%余量给CPU)                     │
                                                                            │
└───────────────────────────────────────────────────────────────────────────┘
```

### 1.2 DMA操作形式化定义

**定义 1.1 (DMA传输)**

```
DMA传输 = (Source, Destination, Count, Config, Callback)

Source: 源地址 ∈ 内存空间
Destination: 目标地址 ∈ 内存空间
Count: 传输单元数 ∈ ℕ
Config: 配置参数
  - Direction: 方向 ∈ {M2M, M2P, P2M, P2P}
  - Mode: 模式 ∈ {Normal, Circular, Double-Buffer}
  - Width: 数据宽度 ∈ {Byte, HalfWord, Word}
  - Increment: 地址增量 ∈ {Enable, Disable}
Callback: 完成回调函数

传输时间模型:
  T_transfer = T_setup + Count × T_unit + T_teardown

其中:
  T_setup ≈ 2-3 总线周期 (配置加载)
  T_unit ≈ 1-2 总线周期 (每个数据单元)
  T_teardown ≈ 1-2 总线周期 (中断/标志设置)

CPU开销对比:
  - 无DMA: CPU占用率 ≈ 100% (全程参与)
  - 有DMA: CPU占用率 ≈ (T_setup + T_teardown) / T_total < 5%
```

### 1.3 传输方向矩阵

| 方向 | 源 | 目标 | 典型应用 | 带宽效率 |
|------|-----|------|----------|----------|
| **M2M** | RAM | RAM | 内存拷贝、填充实例 | 高 |
| **M2P** | RAM | 外设 | DAC输出、GPIO批量操作 | 高 |
| **P2M** | 外设 | RAM | ADC采集、串口接收 | 极高 |
| **P2P** | 外设 | 外设 | 外设直连 (较少用) | 高 |

---

## 2. DMA通道管理

### 2.1 通道分配决策树

```
DMA通道分配策略
│
├── 外设固定映射 (如STM32F1)
│   └── 每个外设信号固定到特定通道
│       ├── USART1_TX → Channel 4
│       ├── USART1_RX → Channel 5
│       ├── ADC1      → Channel 1
│       └── 优点: 简单明确
│       └── 缺点: 通道冲突无法避免
│
└── 灵活映射 (如STM32F4/H7)
    └── 通过DMA请求路由器选择
        ├── 所有外设可映射到任意通道
        ├── 由DMAMUX/DMAMUX管理
        └── 优点: 避免通道冲突
```

### 2.2 通道仲裁算法

```c
/* DMA通道仲裁形式化定义 */

typedef enum {
    ARB_FIXED_PRIORITY = 0,     /* 固定优先级仲裁 */
    ARB_ROUND_ROBIN = 1         /* 轮询仲裁 */
} ArbitrationMode;

/* 固定优先级 */
ChannelPriority priority_table[] = {
    [0] = PRIORITY_VERY_HIGH,
    [1] = PRIORITY_HIGH,
    [2] = PRIORITY_MEDIUM,
    [3] = PRIORITY_LOW
};

/* 仲裁决策函数 */
int arbitrate_fixed(ChannelStatus channels[], int n) {
    /* 选择优先级最高且请求传输的通道 */
    int selected = -1;
    ChannelPriority highest = PRIORITY_LOW;

    for (int i = 0; i < n; i++) {
        if (channels[i].request &&
            channels[i].priority > highest) {
            highest = channels[i].priority;
            selected = i;
        }
    }
    return selected;
}

/* 轮询仲裁 */
int arbitrate_round_robin(ChannelStatus channels[], int n, int *last) {
    /* 从last+1开始查找下一个请求的通道 */
    for (int i = 1; i <= n; i++) {
        int idx = (*last + i) % n;
        if (channels[idx].request) {
            *last = idx;
            return idx;
        }
    }
    return -1;  /* 无请求 */
}

/* 定理: 轮询仲裁保证公平性
 * 证明:
 *   - 每个通道平均等待时间 = (n-1) × T_transfer
 *   - 无饿死可能
 *   - 但高优先级任务可能延迟
 */
```

### 2.3 通道优先级矩阵

| 优先级级别 | 值 | 适用场景 | 延迟保证 |
|------------|-----|----------|----------|
| **Very High** | 3 | ADC采样、关键控制 | < 1μs |
| **High** | 2 | 串口通信、定时器 | < 5μs |
| **Medium** | 1 | 显示刷新、日志 | < 20μs |
| **Low** | 0 | 后台数据传输 | 不保证 |

---

## 3. DMA传输模式深度分析

### 3.1 模式对比决策矩阵

```
┌─────────────────────────────────────────────────────────────────────────────┐
│                        DMA传输模式对比矩阵                                   │
├─────────────────────────────────────────────────────────────────────────────┤
│                                                                             │
│  模式              正常模式        循环模式        双缓冲模式                │
│  ─────────────────────────────────────────────────────────────────────────  │
│                                                                             │
│  描述              传输一次后停止   传输完成后自动  两个缓冲区交替使用        │
│                                   从头部重新开始                              │
│                                                                             │
│  适用场景          一次性数据块     连续流式数据   实时处理无间断             │
│                    传输            采集/输出      数据采集                    │
│                                                                             │
│  CPU干预           每次需重新       零干预        每半周期处理               │
│                    配置启动                                                 │
│                                                                             │
│  延迟              重新配置         零延迟        半周期处理时间              │
│                    时间                                                     │
│                                                                             │
│  实现复杂度        简单              简单          中等 (需管理两个缓冲)      │
│                                                                             │
│  应用实例          文件传输         音频播放      实时信号处理                │
│                    配置加载         ADC连续采样   视频帧捕获                  │
│                                                                             │
└─────────────────────────────────────────────────────────────────────────────┘
```

### 3.2 双缓冲模式状态机

```
双缓冲模式状态机 (Double Buffering State Machine)

状态定义:
  S0: IDLE           - 空闲状态
  S1: BUFFER0_ACTIVE - 正在填充Buffer0
  S2: BUFFER1_ACTIVE - 正在填充Buffer1
  S3: ERROR          - 错误状态

状态转换:
  S0 ──[配置启动]──→ S1
                    │
                    │ (Buffer0半满/全满中断)
                    ↓
                   S2 ──→ [处理Buffer0数据]
                    │
                    │ (Buffer1半满/全满中断)
                    ↓
                   S1 ──→ [处理Buffer1数据]
                    │ ↑
                    └─┘ (循环)

形式化定义:
  当前缓冲区索引: idx ∈ {0, 1}
  缓冲区状态: status[2] ∈ {EMPTY, FILLING, FULL, PROCESSING}

  不变量:
    1. status[0] = FILLING → status[1] ≠ FILLING
    2. status[idx] = FULL → DMA自动切换到 status[1-idx] = FILLING
    3. 至少一个缓冲区始终可用

实现要点:
  - HTIF (Half Transfer) 中断: 处理前半缓冲区
  - TCIF (Transfer Complete) 中断: 处理后半缓冲区
  - 确保处理速度 > DMA填充速度
```

### 3.3 双缓冲实现代码

```c
/* 双缓冲DMA实现 */

#define BUFFER_SIZE 1024
#define HALF_BUFFER_SIZE (BUFFER_SIZE / 2)

typedef struct {
    uint16_t buffer[2][BUFFER_SIZE];
    volatile uint8_t active_buffer;     /* DMA正在写入的缓冲区 */
    volatile uint8_t processing_buffer; /* 待处理的缓冲区 */
    volatile uint8_t ready[2];          /* 缓冲区就绪标志 */
} DoubleBuffer;

DoubleBuffer adc_double_buf;

/* DMA半传输中断 (Half Transfer Complete) */
void DMA_IRQHandler_HT(void) {
    /* DMA正在填充后半部分，前半部分已就绪 */
    adc_double_buf.ready[0] = 1;
    adc_double_buf.processing_buffer = 0;
}

/* DMA传输完成中断 (Transfer Complete) */
void DMA_IRQHandler_TC(void) {
    /* DMA已回到前半部分，后半部分已就绪 */
    adc_double_buf.ready[1] = 1;
    adc_double_buf.processing_buffer = 1;
}

/* 主循环处理 */
void process_data(void) {
    for (int i = 0; i < 2; i++) {
        if (adc_double_buf.ready[i]) {
            /* 处理缓冲区i的数据 */
            process_buffer(adc_double_buf.buffer[i], BUFFER_SIZE);

            adc_double_buf.ready[i] = 0;
        }
    }
}

/* 实时性分析:
 *
 * 约束条件: T_process < T_fill_half
 *
 * 其中:
 *   T_fill_half = (BUFFER_SIZE/2) / SampleRate
 *
 * 示例 (音频采样):
 *   SampleRate = 48kHz
 *   BUFFER_SIZE = 1024 samples
 *   T_fill_half = 512 / 48000 = 10.67ms
 *
 *   要求: 处理1024个样本 < 10.67ms
 *        每个样本处理时间 < 10.4μs
 */
```

---

## 4. DMA性能优化

### 4.1 带宽计算定理

**定理 4.1 (DMA带宽上限)**

```
定理: DMA最大带宽 ≤ min(总线带宽, 外设带宽, 内存带宽)

计算:
  总线带宽 = 总线时钟 × 数据宽度

  示例 (AHB总线):
    时钟 = 168 MHz
    数据宽度 = 32 bits = 4 bytes
    理论带宽 = 168M × 4 = 672 MB/s

  实际带宽 (考虑等待周期):
    实际带宽 = 理论带宽 × 效率系数

    效率系数取决于:
      - 仲裁延迟
      - 外设等待状态
      - 内存访问延迟

    典型值: 0.6 - 0.8

多通道并发:
  总带宽需求 = Σ(通道i数据率)

  约束: 总带宽需求 ≤ 总线带宽 × 0.8 (安全余量)
```

### 4.2 优化策略决策树

```
DMA性能优化
│
├── 数据宽度优化
│   ├── 使用最大可能的数据宽度
│   │   ├── Byte (8-bit)  → 4次传输每32-bit字
│   │   ├── HalfWord (16) → 2次传输每32-bit字
│   │   └── Word (32)     → 1次传输 (最优)
│   │
│   └── 数据对齐
│       ├── 源和目标地址对齐到数据宽度
│       └── 未对齐访问导致额外周期
│
├── 突发传输 (Burst)
│   ├── 单次触发传输多个数据单元
│   │   └── 减少仲裁开销
│   │
│   └── 使用FIFO缓冲
│       ├── 积累数据到FIFO
│       └── 突发写入目标
│
├── 通道调度优化
│   ├── 高优先级给实时外设
│   ├── 避免通道冲突
│   └── 合理分配传输时机
│
└── 内存访问优化
    ├── 使用SRAM而非Flash (更快)
    ├── 避免与CPU频繁访问同一区域
    └── 使用DMA缓冲区专用区域
```

### 4.3 FIFO深度与突发配置

```c
/* FIFO和突发配置 */

typedef enum {
    BURST_SINGLE = 0,       /* 单次传输 */
    BURST_INCR4 = 1,        /* 4节拍突发 */
    BURST_INCR8 = 2,        /* 8节拍突发 */
    BURST_INCR16 = 3        /* 16节拍突发 */
} BurstSize;

typedef enum {
    FIFO_1_4 = 0,           /* FIFO 1/4满触发 */
    FIFO_1_2 = 1,           /* FIFO 1/2满触发 */
    FIFO_3_4 = 2,           /* FIFO 3/4满触发 */
    FIFO_FULL = 3           /* FIFO满触发 */
} FIFOThreshold;

/* 突发传输配置决策表 */
void configure_burst_dma(DMA_Channel *ch,
                         size_t transfer_size,
                         uint32_t src_width,
                         uint32_t dst_width) {

    /* 决策1: 突发大小 */
    BurstSize burst;
    if (transfer_size >= 64) {
        burst = BURST_INCR16;   /* 大批量: 最大突发减少仲裁 */
    } else if (transfer_size >= 32) {
        burst = BURST_INCR8;
    } else if (transfer_size >= 16) {
        burst = BURST_INCR4;
    } else {
        burst = BURST_SINGLE;   /* 小量: 单次更灵活 */
    }

    /* 决策2: FIFO阈值 */
    FIFOThreshold threshold;
    if (dst_width > src_width) {
        /* 目标更宽: 积累更多数据 */
        threshold = FIFO_3_4;
    } else if (dst_width < src_width) {
        /* 目标更窄: 减少积累 */
        threshold = FIFO_1_4;
    } else {
        /* 等宽: 中等积累 */
        threshold = FIFO_1_2;
    }

    ch->burst_size = burst;
    ch->fifo_threshold = threshold;
}

/* 性能提升计算:
 *
 * 无突发 (单次):
 *   每个数据单元 = 1次总线请求 + 1次传输
 *   开销/数据 = 高
 *
 * 4节拍突发:
 *   每4个数据单元 = 1次总线请求 + 4次传输
 *   开销/数据 = 降低75%
 *
 * 理论加速比 = 1 / (1 - (burst_size-1)/burst_size × 开销比例)
 */
```

---

## 5. DMA错误处理与调试

### 5.1 错误类型矩阵

| 错误类型 | 原因 | 检测 | 恢复策略 |
|----------|------|------|----------|
| **传输错误 (TE)** | 总线错误、无效地址 | 硬件标志 | 停止DMA，检查地址 |
| **FIFO错误 (FE)** | FIFO溢出/下溢 | 硬件标志 | 调整FIFO阈值 |
| **直接模式错误 (DME)** | 直接模式配置冲突 | 硬件标志 | 检查配置 |

### 5.2 调试技术

```c
/* DMA调试与监控 */

typedef struct {
    uint32_t transfer_count;
    uint32_t error_count;
    uint32_t tc_interrupts;
    uint32_t ht_interrupts;
    uint32_t te_interrupts;
    uint32_t max_transfer_time;     /* us */
    uint32_t min_transfer_time;
} DMAMetrics;

void dma_monitor_transfer(DMA_Channel *ch,
                          DMAMetrics *metrics) {
    uint32_t start_time = get_micros();

    /* 启动传输 */
    dma_start_transfer(ch);

    /* 等待完成 */
    while (!dma_get_flag(ch, DMA_FLAG_TC)) {
        if (dma_get_flag(ch, DMA_FLAG_TE)) {
            metrics->error_count++;
            metrics->te_interrupts++;
            dma_clear_flags(ch);
            return;
        }
    }

    uint32_t elapsed = get_micros() - start_time;

    metrics->transfer_count++;
    metrics->tc_interrupts++;

    if (elapsed > metrics->max_transfer_time) {
        metrics->max_transfer_time = elapsed;
    }
    if (elapsed < metrics->min_transfer_time) {
        metrics->min_transfer_time = elapsed;
    }
}

/* 性能报告 */
void dma_print_metrics(DMAMetrics *m) {
    printf("DMA Transfer Count: %lu\n", m->transfer_count);
    printf("Error Count: %lu (%.2f%%)\n",
           m->error_count,
           100.0 * m->error_count / m->transfer_count);
    printf("Transfer Time: min=%lu us, max=%lu us\n",
           m->min_transfer_time, m->max_transfer_time);
    printf("Throughput: %.2f MB/s\n",
           (double)m->transfer_count * BUFFER_SIZE /
           (get_millis() / 1000.0) / 1024 / 1024);
}
```

---

## 6. 参考资源

- **STM32 Reference Manual**: DMA控制器章节
- **ARM CoreLink DMA-330**: 高性能DMA控制器
- **Cortex-M4 Technical Reference**: 总线矩阵架构
- **Application Note AN4031**: DMA性能优化指南
