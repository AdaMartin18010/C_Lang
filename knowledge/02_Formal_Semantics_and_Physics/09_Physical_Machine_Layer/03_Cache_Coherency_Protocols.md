# 缓存一致性协议深度解析

> **层级定位**: 02 Formal Semantics and Physics / 09 Physical Machine Layer
> **难度级别**: L5 专家
> **预估学习时间**: 8-10 小时

---

## 目录

- [缓存一致性协议深度解析](#缓存一致性协议深度解析)
  - [目录](#目录)
  - [1. 缓存一致性问题](#1-缓存一致性问题)
    - [1.1 多核缓存挑战](#11-多核缓存挑战)
    - [1.2 一致性与连贯性](#12-一致性与连贯性)
  - [2. MESI协议](#2-mesi协议)
    - [2.1 MESI状态](#21-mesi状态)
    - [2.2 状态转换图](#22-状态转换图)
    - [2.3 状态转换示例](#23-状态转换示例)
    - [2.4 总线事务](#24-总线事务)
  - [3. MESIF协议 (Intel)](#3-mesif协议-intel)
    - [3.1 MESIF改进](#31-mesif改进)
  - [4. MOESI协议 (AMD)](#4-moesi协议-amd)
    - [4.1 MOESI状态](#41-moesi状态)
  - [5. 伪共享与避免](#5-伪共享与避免)
    - [5.1 伪共享机制](#51-伪共享机制)
    - [5.2 避免伪共享](#52-避免伪共享)
  - [6. 内存屏障与排序](#6-内存屏障与排序)
    - [6.1 存储缓冲区与失效队列](#61-存储缓冲区与失效队列)
    - [6.2 x86内存模型](#62-x86内存模型)
    - [6.3 ARM内存模型](#63-arm内存模型)
  - [7. 性能优化](#7-性能优化)
    - [7.1 缓存行优化](#71-缓存行优化)
    - [7.2 预取与提示](#72-预取与提示)
  - [关联导航](#关联导航)
    - [前置知识](#前置知识)
    - [后续延伸](#后续延伸)
    - [参考](#参考)

## 1. 缓存一致性问题

### 1.1 多核缓存挑战

```
CPU 0                    CPU 1
┌─────────┐             ┌─────────┐
│  L1缓存  │             │  L1缓存  │
│ x = 5   │             │ x = ?   │ ← 可能是旧值！
└────┬────┘             └────┬────┘
     │                       │
     └───────────┬───────────┘
                 │
            ┌────┴────┐
            │ L3缓存   │
            │ x = 5   │
            └─────────┘
                 │
                 ▼
               内存
            x = 5

问题：CPU 1读取x时，可能从自己的L1缓存读到旧值
```

### 1.2 一致性与连贯性

- **一致性 (Coherence)**: 单个地址的串行化访问
- **连贯性 (Consistency)**: 所有内存访问的全局顺序

```
一致性要求：
P0: write x=1  ──────────────────→
P1:          read x  ───→ 1 (必须看到1，不能是0)

时间 ─────────────────────────────────────────────→
```

---

## 2. MESI协议

### 2.1 MESI状态

| 状态 | 名称 | 含义 | 允许操作 |
|:-----|:-----|:-----|:---------|
| **M** | Modified | 已修改，独占 | 读写，写回后变I |
| **E** | Exclusive | 独占，未修改 | 读写，可静音转为M |
| **S** | Shared | 共享，只读 | 只读，可变为I |
| **I** | Invalid | 无效 | 无，需重新获取 |

### 2.2 状态转换图

```
                    CPU读（其他核无）
                    ┌──────────────┐
                    │              ▼
Invalid(I) ──────► Exclusive(E)
    │                  │
    │ CPU读（其他核有）│ CPU写
    │                  ▼
    └──────────────► Modified(M)
           ▲             │
           │             │ 写回/监听
           └─────────────┘
                  │
                  │ 其他核读
                  ▼
               Shared(S)
                  │
                  │ 其他核写/写回
                  ▼
               Invalid(I)
```

### 2.3 状态转换示例

```
场景1：独占写入
P0: read x (其他核无) → E状态
P0: write x=1        → 静音转为M状态（无总线事务）

场景2：共享到修改
P0: read x (P1也有)  → S状态
P0: write x=1        → 发送Invalidate，等待确认，转为M

场景3：监听失效
P0: M状态x=1
P1: read x           → P0写回内存并转为S，P1获得S
```

### 2.4 总线事务

```
处理器请求：
- PrRd:   处理器读
- PrWr:   处理器写

总线请求：
- BusRd:      读缓存行
- BusRdX:     读并独占（写前）
- BusUpgr:    升级（Shared→Modified，不读内存）
- Flush:      写回内存
- FlushOpt:   写回内存并传递给请求者

监听操作：
- 其他核的M/E状态写回
- 使其他核的S状态失效
```

---

## 3. MESIF协议 (Intel)

### 3.1 MESIF改进

增加了**F (Forward)** 状态：作为共享数据的分发者

```
场景：多个核共享只读数据
MESI: 所有S状态核都可以响应读请求
MESIF: 只有一个F状态核响应，减少总线竞争

状态转换：
P0: read x → E（唯一）
P1: read x → P0: E→F, P1: S（P0作为Forwarder）
P2: read x → P0(F)响应，P2: S
```

---

## 4. MOESI协议 (AMD)

### 4.1 MOESI状态

增加**O (Owned)** 状态：脏数据所有者，但可被共享

```
优势：减少写回次数

场景：
P0: M状态x=1
P1: read x

MESI: P0写回内存，P0→S，P1→S（内存访问）
MOESI: P0→O，P1→S，P0保留脏数据（无内存访问）

后续P2读x：P0(O)直接提供，无需内存访问
```

---

## 5. 伪共享与避免

### 5.1 伪共享机制

```
缓存行：64字节
Core 0修改x，Core 1修改y

|  x (4B)  | y (4B) | ... |  <- 同一缓存行！

Core 0写x → M状态
Core 1写y → Core 0写回 → I状态
Core 0再写x → 重复上述 → 乒乓效应
```

### 5.2 避免伪共享

```c
// 按缓存行对齐
struct padded_counter {
    atomic_int value;
    char padding[60];  // 64 - 4 = 60
} __attribute__((aligned(64)));

struct padded_counter counters[NUM_CPUS];

// C11方式
#include <stdalign.h>

alignas(64) atomic_int counter_per_cpu[NUM_CPUS];

// 检测伪共享
// perf c2c (Cache Coherence)
perf c2c record ./program
perf c2c report
```

---

## 6. 内存屏障与排序

### 6.1 存储缓冲区与失效队列

```
问题：存储缓冲区导致的乱序

Core 0:        Core 1:
store x=1      store y=1
load y (0?)    load x (0?)

可能结果：x=0, y=0（违反直觉）

原因：
1. store进入存储缓冲区，立即继续
2. 从存储缓冲区转发给后续load
3. 实际的store对其他核不可见

解决：内存屏障
```

### 6.2 x86内存模型

```
x86-TSO (Total Store Order):
- 所有核心看到相同的存储顺序
- Store在存储缓冲区中，对其他核不可见直到提交
- Load可以读取自己的store（存储缓冲区转发）

保证：
- Loads are not reordered with other loads
- Stores are not reordered with other stores
- Stores are not reordered with earlier loads
- In a multiprocessor system, memory ordering obeys causality

不保证：
- Loads may be reordered with earlier stores
  (读可能重排到写之前)
```

### 6.3 ARM内存模型

```
ARM更宽松：
- 任意重排序（数据依赖除外）
- 需要显式屏障

屏障指令：
- DMB (Data Memory Barrier)
- DSB (Data Synchronization Barrier)
- ISB (Instruction Synchronization Barrier)
```

---

## 7. 性能优化

### 7.1 缓存行优化

```c
// 结构体填充
struct hot_data {
    int frequently_accessed;
    // 避免将不相关数据放在同一缓存行
    char pad[60];
};

// 数组分块（避免竞争）
#define CACHE_LINE_SIZE 64
#define PADDING (CACHE_LINE_SIZE / sizeof(int))

int counters[NUM_THREADS][PADDING];  // 每线程一行
```

### 7.2 预取与提示

```c
#include <xmmintrin.h>

// 软件预取
void process_with_prefetch(int *data, int n) {
    for (int i = 0; i < n; i++) {
        // 预取未来数据
        if (i + 16 < n) {
            _mm_prefetch((const char*)&data[i + 16], _MM_HINT_T0);
        }
        process(data[i]);
    }
}
```

---

## 关联导航

### 前置知识

- [微架构](../readme.md)
- [并发编程](../../../03_System_Technology_Domains/14_Concurrency_Parallelism/readme.md)
- [C11内存模型](../../01_Game_Semantics/02_C11_Memory_Model.md)

### 后续延伸

- [缓存优化实战](../../07_Microarchitecture/03_Cache_Optimization_Practical.md)
- [性能分析硬件](../../16_Performance_Analysis_Hardware/readme.md)

### 参考

- 《计算机体系结构：量化研究方法》
- [MESI Protocol](https://en.wikipedia.org/wiki/MESI_protocol)
