# 缓存友好编程 (Cache-Friendly Programming)

> **层级定位**: 02 Formal Semantics and Physics / 07 Microarchitecture
> **主题**: 从程序员视角理解内存层次并编写缓存友好代码
> **难度级别**: L3 应用 → L4 分析
> **预估学习时间**: 8-12 小时
> **对标课程**: CMU 15-213 CSAPP Ch.6

## 概述

现代 CPU 的速度远超主内存。缓存（Cache）是弥合这一差距的关键硬件机制。理解缓存的工作原理，并据此编写"缓存友好"的代码，是系统性能优化中最具杠杆效应的技能之一。本模块从程序员视角出发，系统阐述内存层次结构、缓存映射策略、局部性原理，以及编写缓存友好代码的具体技术，对齐 CMU CSAPP 第六章的经典教学体系。

---

## 1. 内存层次结构回顾

```
寄存器 (1 cycle)
    │
    ▼
L1 缓存 (4 cycles, 32-64 KB)
    │
    ▼
L2 缓存 (10 cycles, 256-512 KB)
    │
    ▼
L3 缓存 (40 cycles, 8-32 MB, 共享)
    │
    ▼
主内存 (200 cycles, GB 级)
    │
    ▼
磁盘/SSD (百万 cycles)
```

### 关键数据（典型 x86-64 桌面 CPU）

| 层级 | 延迟 | 大小 | 关联度 |
|:-----|:-----|:-----|:-------|
| L1d | ~4 cycles | 48 KB | 12-way |
| L1i | ~4 cycles | 32 KB | 8-way |
| L2 | ~12 cycles | 1.25 MB | 10-way |
| L3 | ~40 cycles | 24 MB | 16-way (共享) |
| DRAM | ~200 cycles | 16+ GB | — |

> **核心洞察**: 缓存未命中 (cache miss) 的代价是命中的 50-100 倍。编写缓存友好代码的本质是**最大化缓存命中率**。

---

## 2. 局部性原理 (Locality)

### 时间局部性 (Temporal Locality)
>
> 如果一个数据项被访问，它很可能在不久的将来再次被访问。

```c
// ✅ 时间局部性好：重复访问 sum
int sum_array(int a[], int n) {
    int sum = 0;          // sum 在寄存器/L1 中
    for (int i = 0; i < n; i++)
        sum += a[i];
    return sum;
}
```

### 空间局部性 (Spatial Locality)
>
> 如果一个数据项被访问，它附近的数据项也很可能被访问。

```c
// ✅ 空间局部性好：顺序访问数组
for (int i = 0; i < n; i++)
    process(a[i]);        // a[i+1] 可能已在缓存行中
```

---

## 3. 缓存映射与行 (Cache Line)

### 缓存行 (Cache Line)

缓存的最小传输单元是**缓存行**（通常为 64 字节），而非单个字节。

```
访问 a[0] → 加载 a[0]~a[15] (假设 int=4B, 64B 缓存行)
访问 a[1] → 命中！已在缓存中
```

### 直接映射 vs 组相联

| 策略 | 原理 | 冲突问题 |
|:-----|:-----|:---------|
| **直接映射** | 每个内存地址映射到唯一缓存行 | 容易冲突 (stride = 2^n × cache_size) |
| **组相联** | 每组有 N 个缓存行可选 (N-way) | 冲突减少，但比较复杂 |
| **全相联** | 任意缓存行可存放任意地址 | 无冲突，但硬件成本高 |

---

## 4. 缓存友好的核心模式

### 模式 1: 优先遍历行 (Row-Major Order)

```c
// ❌ 缓存不友好：列优先访问
for (int j = 0; j < n; j++)
    for (int i = 0; i < n; i++)
        sum += a[i][j];     // 每次访问跨行，跳过整个缓存行

// ✅ 缓存友好：行优先访问
for (int i = 0; i < n; i++)
    for (int j = 0; j < n; j++)
        sum += a[i][j];     // 顺序访问，缓存行完全利用
```

> **性能差异**: 对于大数组，行优先可能比列优先快 **10-50 倍**。

### 模式 2: 分块/平铺 (Blocking/Tiling)

当工作集超过缓存容量时，将数据分块处理以适配缓存。

```c
// ❌ 缓存不友好：整个矩阵同时访问
for (int i = 0; i < n; i++)
    for (int j = 0; j < n; j++)
        for (int k = 0; k < n; k++)
            C[i][j] += A[i][k] * B[k][j];

// ✅ 缓存友好：分块矩阵乘法 (Tiling)
#define BLOCK 64
for (int ii = 0; ii < n; ii += BLOCK)
    for (int jj = 0; jj < n; jj += BLOCK)
        for (int kk = 0; kk < n; kk += BLOCK)
            for (int i = ii; i < ii + BLOCK; i++)
                for (int j = jj; j < jj + BLOCK; j++)
                    for (int k = kk; k < kk + BLOCK; k++)
                        C[i][j] += A[i][k] * B[k][j];
```

### 模式 3: 结构体数组 vs 数组结构体 (SoA vs AoS)

```c
// ❌ AoS: 访问 x 时 y,z 也被加载但可能不需要
struct Particle { float x, y, z; };
struct Particle particles[1000];
for (int i = 0; i < 1000; i++)
    sum_x += particles[i].x;

// ✅ SoA: 仅加载 x，缓存行完全利用
struct {
    float x[1000], y[1000], z[1000];
} particles;
for (int i = 0; i < 1000; i++)
    sum_x += particles.x[i];
```

### 模式 4: 避免伪共享 (False Sharing)

多线程中，不同 CPU 核心修改同一缓存行的不同变量会导致缓存失效。

```c
// ❌ 伪共享：counter[i] 可能在同一缓存行
int counter[8];           // 8 × 4B = 32B < 64B 缓存行
void* thread_fn(void* arg) {
    int id = *(int*)arg;
    for (int i = 0; i < 1000000; i++)
        counter[id]++;    // 所有线程竞争同一缓存行！
}

// ✅ 消除伪共享：填充到缓存行大小
struct PaddedCounter {
    int value;
    char pad[60];         // 64B - sizeof(int) = 60B 填充
} counter[8];             // 每个 counter 独占一个缓存行
```

---

## 5. 性能测量：Cache Miss 分析

### 使用 `perf` 分析缓存行为

```bash
# 测量缓存命中率
perf stat -e cache-references,cache-misses,L1-dcache-loads,L1-dcache-load-misses ./program

# 典型输出分析
cache-references:        1,234,567
cache-misses:              123,456   (10% miss rate)
L1-dcache-loads:         5,000,000
L1-dcache-load-misses:     250,000   (5% L1 miss rate)
```

### Cachegrind (Valgrind)

```bash
valgrind --tool=cachegrind ./program
cg_annotate cachegrind.out.*
```

---

## 6. 硬件预取器友好代码

现代 CPU 有硬件预取器，自动识别流式访问模式：

```c
// ✅ 硬件预取器友好：固定步长、顺序访问
for (int i = 0; i < n; i += 4)
    process(arr[i]);

// ❌ 硬件预取器不友好：随机/不规则访问
for (int i = 0; i < n; i++)
    process(arr[random_index[i]]);
```

---

## 7. 权威参考

- **CSAPP Ch.6**: The Memory Hierarchy — 缓存友好的权威教材
- **Agner Fog's Optimization Manuals**: Cache and memory access patterns
- **Intel 64 and IA-32 Optimization Reference Manual**: Cache optimization guidelines
- **"What Every Programmer Should Know About Memory"** (Ulrich Drepper): 深入内存子系统

## 待补充内容

- [ ] NUMA 架构下的缓存友好编程
- [ ] GPU 共享内存的缓存-like 优化
- [ ] 非临时存储 (Non-temporal stores) 与流式数据
- [ ] 缓存行大小探测与自适应算法

---

*最后更新: 2026-05-10*
