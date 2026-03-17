# 缓存行优化

> **层级定位**: 04 Industrial Scenarios / 03 High Frequency Trading
> **对应标准**: x86/x64/ARM缓存架构, MESI协议
> **难度级别**: L4
> **预估学习时间**: 6-8小时

---

## 📋 本节概要

| 属性 | 内容 |
|:-----|:-----|
| **核心概念** | 缓存行、伪共享、缓存对齐、预取、NUMA优化 |
| **前置知识** | CPU架构、内存层次、并发编程 |
| **后续延伸** | 无锁编程、DPDK、高性能计算 |
| **权威来源** | Intel SDM, AMD APM, Ulrich Drepper "What Every Programmer Should Know About Memory" |

---


---

## 📑 目录

- [缓存行优化](#缓存行优化)
  - [📋 本节概要](#-本节概要)
  - [📑 目录](#-目录)
  - [📖 1. CPU缓存架构](#-1-cpu缓存架构)
    - [1.1 缓存层次](#11-缓存层次)
    - [1.2 缓存行结构](#12-缓存行结构)
  - [📖 2. 伪共享 (False Sharing)](#-2-伪共享-false-sharing)
    - [2.1 问题描述](#21-问题描述)
    - [2.2 伪共享的影响](#22-伪共享的影响)
    - [2.3 解决方案：缓存行对齐](#23-解决方案缓存行对齐)
  - [📖 3. 缓存友好数据结构](#-3-缓存友好数据结构)
    - [3.1 结构体数组(AoS) vs 数组结构体(SoA)](#31-结构体数组aos-vs-数组结构体soa)
    - [3.2 矩阵布局优化](#32-矩阵布局优化)
  - [📖 4. 预取技术](#-4-预取技术)
    - [4.1 软件预取](#41-软件预取)
    - [4.2 GCC内置预取](#42-gcc内置预取)
  - [📖 5. NUMA优化](#-5-numa优化)
    - [5.1 NUMA架构](#51-numa架构)
    - [5.2 NUMA感知编程](#52-numa感知编程)
  - [⚠️ 常见陷阱](#️-常见陷阱)
    - [陷阱 CACHE01: 忽略缓存行边界](#陷阱-cache01-忽略缓存行边界)
    - [陷阱 CACHE02: 过度预取](#陷阱-cache02-过度预取)
  - [✅ 质量验收清单](#-质量验收清单)


---

## 📖 1. CPU缓存架构

### 1.1 缓存层次

```text
┌─────────────────────────────────────────────────────────────┐
│                      CPU 核心                               │
├─────────────────────────────────────────────────────────────┤
│  ┌──────────────┐                                          │
│  │  L1 Data     │  32KB/core, 4 cycles latency             │
│  │  64-byte line│                                          │
│  ├──────────────┤                                          │
│  │  L1 Inst     │  32KB/core, 4 cycles latency             │
│  └──────────────┘                                          │
│         ↓                                                   │
│  ┌──────────────┐                                          │
│  │  L2 Unified  │  256KB-1MB/core, 12 cycles               │
│  │  64-byte line│                                          │
│  └──────────────┘                                          │
│         ↓                                                   │
│  ┌──────────────┐                                          │
│  │  L3 Shared   │  8-64MB/socket, 30-40 cycles             │
│  │  64-byte line│                                          │
│  └──────────────┘                                          │
└──────────────────────┬──────────────────────────────────────┘
                       │
                       ↓  60-100ns (~200-300 cycles)
┌─────────────────────────────────────────────────────────────┐
│                    Main Memory (DRAM)                       │
│                    ~16-512GB                                │
└─────────────────────────────────────────────────────────────┘
```

### 1.2 缓存行结构

```text
缓存行 (64 bytes on x86)
┌─────────────────────────────────────────────────────────────┐
│  Tag (20-24 bits) │ Index (6-12 bits) │ Offset (6 bits)   │
├─────────────────────────────────────────────────────────────┤
│  Valid │ Dirty │ Coherency State │ Data (64 bytes)        │
└─────────────────────────────────────────────────────────────┘

地址分解示例 (64-bit地址):
┌──────────────┬──────────────┬──────────────┐
│  Tag (40bit) │ Index (18bit)│ Offset (6bit)│
└──────────────┴──────────────┴──────────────┘
              ↓              ↓
         选择缓存组    选择字节
```

---

## 📖 2. 伪共享 (False Sharing)

### 2.1 问题描述

```c
// 伪共享示例 - 两个线程频繁修改同一缓存行
struct SharedData {
    int counter1;  // 线程1修改
    int counter2;  // 线程2修改
    // 两个变量在同一缓存行(64字节)内！
} data;

// 线程1
void thread1_work() {
    for (int i = 0; i < 1000000; i++) {
        data.counter1++;  // 使整个缓存行失效
    }
}

// 线程2
void thread2_work() {
    for (int i = 0; i < 1000000; i++) {
        data.counter2++;  // 使整个缓存行失效
    }
}
```

### 2.2 伪共享的影响

```text
时间线 (MESI协议):

T1: T1读取counter1 -> 缓存行进入E(独占)状态
T2: T2读取counter2 -> 缓存行变为S(共享)状态
T3: T1修改counter1 -> 缓存行变为M(修改)，使T2的副本失效
T4: T2修改counter2 -> 重新加载缓存行，T1的副本失效
T5: T1修改counter1 -> 重新加载缓存行，T2的副本失效
...

结果: 频繁的缓存失效和总线流量，性能下降10-100倍！
```

### 2.3 解决方案：缓存行对齐

```c
#include <stdalign.h>
#include <pthread.h>
#include <stdio.h>

// 方法1: 使用C11对齐
#define CACHE_LINE_SIZE 64

struct PaddedData {
    alignas(CACHE_LINE_SIZE) int counter1;
    alignas(CACHE_LINE_SIZE) int counter2;
};

// 方法2: 手动填充 (GCC/Clang)
struct PaddedData_GNU {
    int counter1;
    char padding[60];  // 64 - sizeof(int) = 60
    int counter2;
    char padding2[60];
} __attribute__((aligned(64)));

// 方法3: MSVC
#ifdef _MSC_VER
__declspec(align(64)) struct PaddedData_MSVC {
    int counter1;
    char padding[60];
    int counter2;
    char padding2[60];
};
#endif

// 性能测试
struct SharedData bad;
struct PaddedData good;

void* increment_bad(void* arg) {
    int* counter = (int*)arg;
    for (int i = 0; i < 10000000; i++) {
        (*counter)++;
    }
    return NULL;
}

int main() {
    pthread_t t1, t2;

    // 测试伪共享版本
    bad.counter1 = 0;
    bad.counter2 = 0;

    pthread_create(&t1, NULL, increment_bad, &bad.counter1);
    pthread_create(&t2, NULL, increment_bad, &bad.counter2);
    pthread_join(t1, NULL);
    pthread_join(t2, NULL);

    printf("Bad: counter1=%d, counter2=%d\n", bad.counter1, bad.counter2);

    return 0;
}
```

---

## 📖 3. 缓存友好数据结构

### 3.1 结构体数组(AoS) vs 数组结构体(SoA)

```c
// AoS - Array of Structs (面向对象风格)
struct ParticleAoS {
    float x, y, z;      // 位置
    float vx, vy, vz;   // 速度
    float mass;         // 质量
    int id;             // ID
};  // 32 bytes

struct ParticleAoS particles_aos[1000];

// 访问模式 (只更新位置)
for (int i = 0; i < 1000; i++) {
    particles_aos[i].x += particles_aos[i].vx * dt;
    particles_aos[i].y += particles_aos[i].vy * dt;
    particles_aos[i].z += particles_aos[i].vz * dt;
    // 加载32字节，只使用12字节，浪费50%缓存带宽！
}

// SoA - Structure of Arrays (数据导向设计)
struct ParticleSoA {
    float x[1000];
    float y[1000];
    float z[1000];
    float vx[1000];
    float vy[1000];
    float vz[1000];
    float mass[1000];
    int id[1000];
};

struct ParticleSoA particles_soa;

// 访问模式
for (int i = 0; i < 1000; i++) {
    particles_soa.x[i] += particles_soa.vx[i] * dt;
    particles_soa.y[i] += particles_soa.vy[i] * dt;
    particles_soa.z[i] += particles_soa.vz[i] * dt;
    // 连续访问x数组，缓存利用率100%！
}
```

### 3.2 矩阵布局优化

```c
// 行优先 (C默认) - 行访问快
float matrix_row_major[N][N];

// 快: 行访问，缓存友好
for (int i = 0; i < N; i++) {
    for (int j = 0; j < N; j++) {
        sum += matrix_row_major[i][j];  // 连续访问
    }
}

// 慢: 列访问，缓存不友好
for (int j = 0; j < N; j++) {
    for (int i = 0; i < N; i++) {
        sum += matrix_row_major[i][j];  // 跨行访问，步长大
    }
}

// 解决方案：分块访问
#define BLOCK_SIZE 64
for (int ii = 0; ii < N; ii += BLOCK_SIZE) {
    for (int jj = 0; jj < N; jj += BLOCK_SIZE) {
        for (int i = ii; i < ii + BLOCK_SIZE && i < N; i++) {
            for (int j = jj; j < jj + BLOCK_SIZE && j < N; j++) {
                sum += matrix_row_major[i][j];
            }
        }
    }
}
```

---

## 📖 4. 预取技术

### 4.1 软件预取

```c
#include <xmmintrin.h>  // SSE intrinsics

void prefetch_example(float* data, int n) {
    float sum = 0;
    int prefetch_distance = 16;  // 预取距离

    for (int i = 0; i < n; i++) {
        // 预取未来的数据
        if (i + prefetch_distance < n) {
            _mm_prefetch((const char*)&data[i + prefetch_distance],
                        _MM_HINT_T0);  // L1缓存
        }

        sum += data[i];
    }
}

// 不同类型的预取提示
// _MM_HINT_T0 - L1/L2/L3
// _MM_HINT_T1 - L2/L3 (绕过L1)
// _MM_HINT_T2 - L3 (绕过L1/L2)
// _MM_HINT_NTA - 非临时访问 (直接到L1，快过期)
```

### 4.2 GCC内置预取

```c
void gcc_prefetch(float* data, int n) {
    float sum = 0;

    for (int i = 0; i < n; i++) {
        // GCC内置函数
        __builtin_prefetch(&data[i + 64], 0, 3);
        // 参数: 地址, 读写(0=读,1=写), 局部性(0-3)

        sum += data[i];
    }
}
```

---

## 📖 5. NUMA优化

### 5.1 NUMA架构

```
NUMA (Non-Uniform Memory Access)

┌─────────────────────┐     ┌─────────────────────┐
│     Socket 0        │     │     Socket 1        │
│  ┌───────────────┐  │     │  ┌───────────────┐  │
│  │   CPU 0-3     │  │     │  │   CPU 4-7     │  │
│  │   L1/L2/L3    │  │     │  │   L1/L2/L3    │  │
│  └───────────────┘  │     │  └───────────────┘  │
│         │           │     │         │           │
│  ┌───────────────┐  │     │  ┌───────────────┐  │
│  │  Memory Ctrl  │──┼─────┼──┤  Memory Ctrl  │  │
│  │   Local RAM   │  │ QPI │  │   Local RAM   │  │
│  │   (fast)      │  │     │  │   (fast)      │  │
│  └───────────────┘  │     │  └───────────────┘  │
└─────────────────────┘     └─────────────────────┘

Socket 0访问Socket 1的内存: ~100ns (慢)
Socket 0访问本地内存: ~60ns (快)
```

### 5.2 NUMA感知编程

```c
#define _GNU_SOURCE
#include <numa.h>
#include <sched.h>
#include <stdio.h>

void numa_optimization() {
    if (numa_available() < 0) {
        printf("NUMA not available\n");
        return;
    }

    int num_nodes = numa_num_configured_nodes();
    printf("NUMA nodes: %d\n", num_nodes);

    // 分配NUMA本地内存
    int size = 1024 * 1024 * 100;  // 100MB
    int node = 0;  // 目标NUMA节点

    void* local_mem = numa_alloc_onnode(size, node);

    // 绑定线程到特定CPU
    cpu_set_t cpuset;
    CPU_ZERO(&cpuset);
    CPU_SET(0, &cpuset);  // 绑定到CPU 0
    sched_setaffinity(0, sizeof(cpuset), &cpuset);

    // 使用内存...

    numa_free(local_mem, size);
}

// 第一触摸策略 (First Touch)
void first_touch_policy() {
    int n = 1000000;
    double* data = malloc(n * sizeof(double));

    // 重要: 在将使用数据的线程中初始化
    // 这样内存会分配在该线程的本地NUMA节点
    #pragma omp parallel for
    for (int i = 0; i < n; i++) {
        data[i] = 0.0;  // 第一触摸决定NUMA位置
    }
}
```

---

## ⚠️ 常见陷阱

### 陷阱 CACHE01: 忽略缓存行边界

```c
// 错误: 可能跨越缓存行边界
struct __attribute__((packed)) BadLayout {
    char a;
    int b;  // 可能跨越两个缓存行！
    char c;
};

// 正确: 自然对齐
struct GoodLayout {
    int b;   // 4字节
    char a;  // 1字节
    char c;  // 1字节
    // 2字节填充
};
```

### 陷阱 CACHE02: 过度预取

```c
// 错误: 预取太提前或太多
for (int i = 0; i < n; i++) {
    _mm_prefetch(&data[i + 10000], _MM_HINT_T0);  // 太远了！
    _mm_prefetch(&data[i + 1], _MM_HINT_T0);       // 太近了！
    sum += data[i];
}

// 正确: 适度预取距离
#define PREFETCH_DISTANCE 64
for (int i = 0; i < n; i++) {
    if (i + PREFETCH_DISTANCE < n) {
        _mm_prefetch(&data[i + PREFETCH_DISTANCE], _MM_HINT_T0);
    }
    sum += data[i];
}
```

---

## ✅ 质量验收清单

- [x] CPU缓存层次结构
- [x] 缓存行结构详解
- [x] 伪共享问题与解决方案
- [x] 缓存对齐技术
- [x] AoS vs SoA数据布局
- [x] 矩阵分块优化
- [x] 软件预取技术
- [x] NUMA架构与优化
- [x] 常见陷阱分析

---

> **更新记录**
>
> - 2025-03-09: 重写充实，从模板63行扩展至完整技术文档
