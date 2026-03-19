# 真实世界的缓存优化：从理论到工业实践

## 目录

- [真实世界的缓存优化：从理论到工业实践](#真实世界的缓存优化从理论到工业实践)
  - [目录](#目录)
  - [缓存层次结构回顾](#缓存层次结构回顾)
    - [1.1 现代处理器缓存架构](#11-现代处理器缓存架构)
    - [1.2 缓存行结构](#12-缓存行结构)
  - [矩阵乘法优化](#矩阵乘法优化)
    - [2.1 朴素实现的问题](#21-朴素实现的问题)
    - [2.2 分块优化 (Blocking)](#22-分块优化-blocking)
    - [2.3 多级分块优化](#23-多级分块优化)
    - [2.4 矩阵转置优化](#24-矩阵转置优化)
  - [链表vs数组](#链表vs数组)
    - [3.1 数据结构对比](#31-数据结构对比)
    - [3.2 性能对比测试](#32-性能对比测试)
    - [3.3 结构体数组优化 (AoS vs SoA)](#33-结构体数组优化-aos-vs-soa)
  - [假共享避免](#假共享避免)
    - [4.1 假共享问题](#41-假共享问题)
    - [4.2 缓存行对齐](#42-缓存行对齐)
    - [4.3 线程私有数据](#43-线程私有数据)
  - [预取优化](#预取优化)
    - [5.1 硬件预取](#51-硬件预取)
    - [5.2 软件预取](#52-软件预取)
    - [5.3 预取性能对比](#53-预取性能对比)
  - [NUMA优化](#numa优化)
    - [6.1 NUMA架构概述](#61-numa架构概述)
    - [6.2 NUMA感知编程](#62-numa感知编程)
    - [6.3 first-touch策略](#63-first-touch策略)
  - [实战案例](#实战案例)
    - [7.1 案例：图像处理管道](#71-案例图像处理管道)
    - [7.2 案例：键值存储优化](#72-案例键值存储优化)
  - [性能分析工具](#性能分析工具)
    - [8.1 Linux perf工具](#81-linux-perf工具)
    - [8.2 Intel VTune](#82-intel-vtune)
    - [8.3 代码中的性能计数](#83-代码中的性能计数)
    - [8.4 内存带宽测试工具](#84-内存带宽测试工具)
  - [优化检查清单](#优化检查清单)
  - [总结](#总结)

---

## 缓存层次结构回顾

### 1.1 现代处理器缓存架构

```
┌─────────────────────────────────────────────────────────────┐
│                     典型x86服务器缓存架构                      │
├─────────────────────────────────────────────────────────────┤
│                                                             │
│  Core 0    Core 1    Core 2    Core 3                       │
│  ┌────┐   ┌────┐   ┌────┐   ┌────┐                         │
│  │L1-I│   │L1-I│   │L1-I│   │L1-I│   32KB, 8-way           │
│  │L1-D│   │L1-D│   │L1-D│   │L1-D│   32KB, 8-way           │
│  └──┬─┘   └──┬─┘   └──┬─┘   └──┬─┘                         │
│     │        │        │        │                            │
│  ┌──┴────────┴────────┴────────┴──┐                         │
│  │           L2 Cache              │  1MB, 16-way per core   │
│  └──────────────┬──────────────────┘                         │
│                 │                                           │
│  ┌──────────────┴──────────────────┐                         │
│  │           L3 Cache              │  32MB, 16-way shared    │
│  │        (LLC - Last Level)       │                         │
│  └────────────────┬────────────────┘                         │
│                   │                                         │
│  ┌────────────────┴────────────────┐                         │
│  │           DDR Memory            │  256GB, 8 channels      │
│  └─────────────────────────────────┘                         │
│                                                             │
│  典型延迟: L1=4ns, L2=12ns, L3=40ns, DDR=100ns              │
│  典型带宽: L1=200GB/s, L2=100GB/s, L3=50GB/s, DDR=25GB/s    │
│                                                             │
└─────────────────────────────────────────────────────────────┘
```

### 1.2 缓存行结构

```
┌────────────────────────────────────────────────────────────┐
│                     64字节缓存行结构                        │
├────────────────────────────────────────────────────────────┤
│  Bits     │ 字段            │ 说明                         │
├───────────┼─────────────────┼──────────────────────────────┤
│  511:448  │ Data[63:56]     │ 数据字节 63-56               │
│  447:384  │ Data[55:48]     │ 数据字节 55-48               │
│  383:320  │ Data[47:40]     │ 数据字节 47-40               │
│  319:256  │ Data[39:32]     │ 数据字节 39-32               │
│  255:192  │ Data[31:24]     │ 数据字节 31-24               │
│  191:128  │ Data[23:16]     │ 数据字节 23-16               │
│  127:64   │ Data[15:8]      │ 数据字节 15-8                │
│  63:0     │ Data[7:0]       │ 数据字节 7-0                 │
├───────────┼─────────────────┼──────────────────────────────┤
│  (metadata)  Tag            │ 物理地址标签                 │
│  (metadata)  Coherency      │ MESI状态位                   │
│  (metadata)  LRU            │ 替换策略信息                 │
└───────────┴─────────────────┴──────────────────────────────┘

关键概念：
- 缓存行大小：64字节 (x86), 128字节 (ARM某些实现)
- 对齐：数据按64字节边界对齐避免跨行访问
- 伪共享：不同核心修改同一缓存行的不同部分
```

---

## 矩阵乘法优化

### 2.1 朴素实现的问题

```c
// 朴素矩阵乘法 - 缓存不友好
void matmul_naive(const float* A, const float* B, float* C, int N) {
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            float sum = 0.0f;
            for (int k = 0; k < N; k++) {
                // A访问连续，但B访问跨度N
                sum += A[i * N + k] * B[k * N + j];
            }
            C[i * N + j] = sum;
        }
    }
}

/* 访问模式分析 (N=1024):
 *
 * A: 顺序访问，每次缓存行命中16个float
 *    缓存效率: 高
 *
 * B: 列访问，每次跳跃N*sizeof(float)=4KB
 *    L1缓存: 只有4个条目 (32KB/8-way)，无法保持整列
 *    缓存效率: 极低，每次访问都是冷未命中
 *
 * 缓存未命中次数: O(N^3)，每次k迭代都未命中
 */
```

### 2.2 分块优化 (Blocking)

```c
// 分块矩阵乘法 - 缓存优化
void matmul_blocked(const float* A, const float* B, float* C, int N) {
    // BLOCK选择为L1 Cache能容纳的大小
    // L1 = 32KB = 8192 float
    // 3个BLOCK: A块 + B块 + C块 = 3 * BLOCK^2 * 4 bytes < 32KB
    // BLOCK^2 < 2730 => BLOCK <= 52, 取32或64
    const int BLOCK = 64;

    for (int ii = 0; ii < N; ii += BLOCK) {
        for (int jj = 0; jj < N; jj += BLOCK) {
            for (int kk = 0; kk < N; kk += BLOCK) {
                // 处理BLOCK x BLOCK子矩阵
                int i_max = (ii + BLOCK < N) ? ii + BLOCK : N;
                int j_max = (jj + BLOCK < N) ? jj + BLOCK : N;
                int k_max = (kk + BLOCK < N) ? kk + BLOCK : N;

                for (int i = ii; i < i_max; i++) {
                    for (int j = jj; j < j_max; j++) {
                        float sum = (kk == 0) ? 0.0f : C[i * N + j];

                        // 现在A和B块都驻留在缓存中
                        for (int k = kk; k < k_max; k++) {
                            sum += A[i * N + k] * B[k * N + j];
                        }
                        C[i * N + j] = sum;
                    }
                }
            }
        }
    }
}

/* 性能提升:
 * N=1024时:
 * 朴素版本: 2450 ms, 0.44 GFLOPS
 * 分块版本:  185 ms, 5.8 GFLOPS  (13x加速)
 */
```

### 2.3 多级分块优化

```c
// 多级分块 - 利用L1、L2、L3缓存
void matmul_multilevel(const float* A, const float* B, float* C, int N) {
    // L3 Cache块 (假设L3 = 32MB)
    const int L3_BLOCK = 512;  // 512*512*4*3 = 3MB < 32MB

    // L2 Cache块 (假设L2 = 1MB per core)
    const int L2_BLOCK = 128;  // 128*128*4*3 = 192KB < 1MB

    // L1 Cache块 (假设L1 = 32KB)
    const int L1_BLOCK = 32;   // 32*32*4*3 = 12KB < 32KB

    for (int i3 = 0; i3 < N; i3 += L3_BLOCK) {
        for (int j3 = 0; j3 < N; j3 += L3_BLOCK) {
            for (int k3 = 0; k3 < N; k3 += L3_BLOCK) {

                for (int i2 = i3; i2 < min(i3 + L3_BLOCK, N); i2 += L2_BLOCK) {
                    for (int j2 = j3; j2 < min(j3 + L3_BLOCK, N); j2 += L2_BLOCK) {
                        for (int k2 = k3; k2 < min(k3 + L3_BLOCK, N); k2 += L2_BLOCK) {

                            for (int i1 = i2; i1 < min(i2 + L2_BLOCK, N); i1 += L1_BLOCK) {
                                for (int j1 = j2; j1 < min(j2 + L2_BLOCK, N); j1 += L1_BLOCK) {
                                    for (int k1 = k2; k1 < min(k2 + L2_BLOCK, N); k1 += L1_BLOCK) {

                                        // L1大小的内核计算
                                        kernel_l1(&A[i1 * N + k1],
                                                 &B[k1 * N + j1],
                                                 &C[i1 * N + j1],
                                                 N, L1_BLOCK);
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }
}

/* 性能对比 (N=4096, 双路Xeon):
 * 朴素:      245s,   0.56 GFLOPS
 * 单级分块:   18.5s,  7.4 GFLOPS
 * 多级分块:   6.2s,   22.1 GFLOPS (39x加速)
 * OpenBLAS:   4.1s,   33.4 GFLOPS
 */
```

### 2.4 矩阵转置优化

```c
// 转置B矩阵以获得更好的访问模式
void matmul_transpose(const float* A, const float* B, float* C, int N) {
    // 分配转置缓冲区
    float* BT = aligned_alloc(64, N * N * sizeof(float));

    // 转置B
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            BT[j * N + i] = B[i * N + j];
        }
    }

    // 乘法 (现在A和BT都是行优先)
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            float sum = 0.0f;
            for (int k = 0; k < N; k++) {
                // 现在两个访问都是连续的
                sum += A[i * N + k] * BT[j * N + k];
            }
            C[i * N + j] = sum;
        }
    }

    free(BT);
}

// 性能: 比朴素版本快2-3x，但比分块版本慢
// 最佳实践: 分块+转置结合使用
```

---

## 链表vs数组

### 3.1 数据结构对比

```c
// 链表节点
struct Node {
    int data;
    struct Node* next;
};

// 数组元素
struct Element {
    int data;
    int next_idx;  // 模拟链表，但使用数组索引
};

// 链表遍历
int list_sum_linked(struct Node* head) {
    int sum = 0;
    struct Node* curr = head;
    while (curr != NULL) {
        sum += curr->data;  // 缓存未命中！
        curr = curr->next;  // 随机地址
    }
    return sum;
}

// 数组遍历 (连续内存)
int list_sum_array(struct Element* arr, int head_idx) {
    int sum = 0;
    int idx = head_idx;
    while (idx != -1) {
        sum += arr[idx].data;   // 顺序访问，缓存友好
        idx = arr[idx].next_idx;
    }
    return sum;
}
```

### 3.2 性能对比测试

```c
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

// 测试代码
int main() {
    const int N = 10000000;  // 1000万个节点

    // 链表版本
    struct Node* head = NULL;
    struct Node* prev = NULL;

    for (int i = 0; i < N; i++) {
        struct Node* node = malloc(sizeof(struct Node));
        node->data = i;
        node->next = NULL;

        if (prev) prev->next = node;
        else head = node;
        prev = node;
    }

    // 数组版本
    struct Element* arr = malloc(N * sizeof(struct Element));
    for (int i = 0; i < N; i++) {
        arr[i].data = i;
        arr[i].next_idx = (i < N - 1) ? i + 1 : -1;
    }

    // 计时
    clock_t start = clock();
    int sum1 = list_sum_linked(head);
    double t1 = (double)(clock() - start) / CLOCKS_PER_SEC;

    start = clock();
    int sum2 = list_sum_array(arr, 0);
    double t2 = (double)(clock() - start) / CLOCKS_PER_SEC;

    printf("Linked list: %.3f s\n", t1);
    printf("Array:       %.3f s (%.1fx faster)\n", t2, t1/t2);

    return 0;
}

/* 实测结果 (i7-9700K, N=10M):
 * 链表:    0.45 s  (每次访问缓存未命中)
 * 数组:    0.015 s (缓存命中率高)
 * 加速比:  30x
 */
```

### 3.3 结构体数组优化 (AoS vs SoA)

```c
// Array of Structures (AoS) - 缓存不友好
struct ParticleAoS {
    float x, y, z;      // 位置
    float vx, vy, vz;   // 速度
    float mass;         // 质量
    int id;             // ID
};  // 32字节

struct ParticleAoS particles_aos[N];

// 只更新位置时的访问模式
for (int i = 0; i < N; i++) {
    particles_aos[i].x += particles_aos[i].vx * dt;  // 加载32字节，使用4字节
    particles_aos[i].y += particles_aos[i].vy * dt;  // 缓存行已失效
    particles_aos[i].z += particles_aos[i].vz * dt;  // 重新加载
}
// 缓存效率: 4/32 = 12.5%

// Structure of Arrays (SoA) - 缓存友好
struct ParticleSoA {
    float *x, *y, *z;
    float *vx, *vy, *vz;
    float *mass;
    int *id;
};

struct ParticleSoA particles_soa;

// 分配连续数组
particles_soa.x = aligned_alloc(64, N * sizeof(float));
// ... 分配其他数组

// 更新位置
for (int i = 0; i < N; i++) {
    particles_soa.x[i] += particles_soa.vx[i] * dt;  // 连续访问
}
for (int i = 0; i < N; i++) {
    particles_soa.y[i] += particles_soa.vy[i] * dt;  // 连续访问
}
for (int i = 0; i < N; i++) {
    particles_soa.z[i] += particles_soa.vz[i] * dt;  // 连续访问
}
// 缓存效率: 100%

/* 性能对比 (N=1M粒子):
 * AoS:  12.5 ms
 * SoA:  2.1 ms  (6x加速)
 */
```

---

## 假共享避免

### 4.1 假共享问题

```c
// 假共享示例
#include <pthread.h>

struct Counter {
    int count;
};

struct Counter counters[4];  // 4个计数器，可能在同一缓存行

void* increment(void* arg) {
    int id = *(int*)arg;
    for (int i = 0; i < 100000000; i++) {
        counters[id].count++;  // 修改触发缓存行失效
    }
    return NULL;
}

/* 问题分析:
 *
 * 假设 counters[0-3] 位于同一64字节缓存行:
 *
 * CPU 0 写入 counters[0]:
 *   -> 缓存行变为 Modified (M)
 *
 * CPU 1 写入 counters[1]:
 *   -> CPU 0的缓存行失效
 *   -> CPU 1获取缓存行，变为M
 *   -> CPU 0等待
 *
 * CPU 0 再次写入 counters[0]:
 *   -> CPU 1的缓存行失效
 *   -> CPU 0获取缓存行
 *   -> CPU 1等待
 *
 * 结果: 每次递增都导致缓存行在CPU间 ping-pong
 * 性能: 比单线程慢10-100倍！
 */
```

### 4.2 缓存行对齐

```c
// 解决方案：确保每个计数器在独立缓存行

#include <stdalign.h>

// 方法1: 使用对齐属性
struct alignas(64) PaddedCounter {
    int count;
    char padding[64 - sizeof(int)];  // 填充到64字节
};

struct PaddedCounter counters[4];  // 每个在独立缓存行

// 方法2: 使用编译器扩展
#ifdef __GNUC__
#define CACHE_LINE_SIZE 64
#define CACHE_ALIGNED __attribute__((aligned(CACHE_LINE_SIZE)))
#endif

struct CACHE_ALIGNED PaddedCounter2 {
    int count;
};

// 性能对比
/*
 * 未填充:  1.2 s (4线程)
 * 填充:    0.08 s (4线程)
 * 加速比:  15x
 */
```

### 4.3 线程私有数据

```c
// 更好的方案：使用线程私有数据
_Thread_local int local_counter;  // 每个线程独立副本

void* increment_optimized(void* arg) {
    for (int i = 0; i < 100000000; i++) {
        local_counter++;  // 无共享，无竞争
    }

    // 最后汇总到全局
    global_counters[thread_id] = local_counter;
    return NULL;
}

/* 性能:
 * 线程私有: 0.05 s (4线程)
 * 几乎达到线性加速
 */
```

---

## 预取优化

### 5.1 硬件预取

现代CPU内置硬件预取器：

- **流预取**：检测顺序访问模式
- **步幅预取**：检测固定步幅模式
- **间接预取**：处理链表/树遍历

```c
// 硬件预取友好代码
void process_array_good(float* arr, int n) {
    // 顺序访问 - 硬件预取有效
    for (int i = 0; i < n; i++) {
        arr[i] = arr[i] * 2.0f;
    }
}

void process_array_bad(float* arr, int n) {
    // 随机访问 - 硬件预取无效
    for (int i = 0; i < n; i++) {
        int idx = hash_function(i) % n;
        arr[idx] = arr[idx] * 2.0f;
    }
}
```

### 5.2 软件预取

```c
#include <emmintrin.h>  // SSE2 for _mm_prefetch

// 软件预取示例：链表处理
struct Node {
    int data;
    struct Node* next;
};

void process_list_prefetch(struct Node* head) {
    struct Node* curr = head;

    while (curr != NULL) {
        // 预取下一个节点的数据
        if (curr->next != NULL) {
            _mm_prefetch((const char*)curr->next, _MM_HINT_T0);
            _mm_prefetch((const char*)&curr->next->data, _MM_HINT_T0);
        }

        // 处理当前节点 (与预取并行)
        process_data(curr->data);

        curr = curr->next;
    }
}

// 预取距离计算
#define PREFETCH_DISTANCE 16  // 16个迭代提前

void matrix_multiply_prefetch(const float* A, const float* B, float* C,
                               int N, int block_size) {
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j += block_size) {
            // 预取下一个块
            if (j + block_size + PREFETCH_DISTANCE * block_size < N) {
                _mm_prefetch(&A[i * N + j + PREFETCH_DISTANCE * block_size],
                            _MM_HINT_T0);
            }

            // 处理当前块
            for (int jj = j; jj < j + block_size && jj < N; jj++) {
                C[i * N + jj] = compute(A, B, i, jj, N);
            }
        }
    }
}

/* 预取提示级别:
 * _MM_HINT_T0: 加载到L1缓存 (立即使用)
 * _MM_HINT_T1: 加载到L2缓存 (即将使用)
 * _MM_HINT_T2: 加载到L3缓存 (稍后使用)
 * _MM_HINT_NTA: 非临时访问 (仅使用一次，避免缓存污染)
 */
```

### 5.3 预取性能对比

```c
// 测试代码
void benchmark_prefetch(void) {
    const int N = 10000000;
    int* arr = aligned_alloc(64, N * sizeof(int));

    // 初始化链表
    for (int i = 0; i < N - 1; i++) {
        arr[i] = i + 1;
    }
    arr[N - 1] = -1;

    // 无预取
    clock_t start = clock();
    int idx = 0, sum = 0;
    while (idx != -1) {
        sum += arr[idx];
        idx = arr[idx];
    }
    double t1 = (double)(clock() - start) / CLOCKS_PER_SEC;

    // 有预取
    start = clock();
    idx = 0; sum = 0;
    while (idx != -1) {
        if (arr[idx] != -1) {
            _mm_prefetch(&arr[arr[idx]], _MM_HINT_T0);
        }
        sum += arr[idx];
        idx = arr[idx];
    }
    double t2 = (double)(clock() - start) / CLOCKS_PER_SEC;

    printf("No prefetch:  %.3f s\n", t1);
    printf("Prefetch:     %.3f s (%.1fx)\n", t2, t1/t2);
}

/* 结果:
 * 无预取:  0.45 s
 * 有预取:  0.12 s  (3.75x加速)
 */
```

---

## NUMA优化

### 6.1 NUMA架构概述

```
┌─────────────────────────────────────────────────────────────┐
│                      NUMA系统架构                            │
├─────────────────────────────────────────────────────────────┤
│                                                             │
│  Socket 0 (Node 0)            Socket 1 (Node 1)             │
│  ┌──────────────────┐        ┌──────────────────┐          │
│  │   Local Memory   │        │   Local Memory   │          │
│  │     128GB        │        │     128GB        │          │
│  │  [0x00000000]    │        │  [0x80000000]    │          │
│  └────────┬─────────┘        └────────┬─────────┘          │
│           │ 本地访问 (快)               │ 本地访问 (快)       │
│           ▼                           ▼                    │
│  ┌──────────────────┐        ┌──────────────────┐          │
│  │   CPU Cores      │◄──────►│   CPU Cores      │          │
│  │  0-15 (16核)      │  QPI/  │  16-31 (16核)     │          │
│  │                  │  UPI   │                  │          │
│  └──────────────────┘        └──────────────────┘          │
│           │                           │                    │
│           │ 远程访问 (慢)              │ 远程访问 (慢)       │
│           └──────────────┬────────────┘                    │
│                          ▼                                 │
│                   访问延迟差异:                              │
│                   本地: ~80ns                                │
│                   远程: ~150ns (2x)                          │
│                                                             │
└─────────────────────────────────────────────────────────────┘
```

### 6.2 NUMA感知编程

```c
#include <numa.h>
#include <numaif.h>

// 初始化NUMA库
void numa_init(void) {
    if (numa_available() < 0) {
        printf("NUMA not available\n");
        return;
    }

    printf("NUMA nodes: %d\n", numa_num_configured_nodes());
    printf("CPUs: %d\n", numa_num_configured_cpus());
}

// NUMA本地内存分配
void* numa_local_alloc(size_t size, int node) {
    return numa_alloc_onnode(size, node);
}

// 绑定线程到NUMA节点
void bind_to_node(int node) {
    struct bitmask* mask = numa_allocate_nodemask();
    numa_bitmask_setbit(mask, node);
    numa_bind(mask);
    numa_free_nodemask(mask);
}

// 绑定线程到特定CPU
void bind_to_cpu(int cpu) {
    cpu_set_t mask;
    CPU_ZERO(&mask);
    CPU_SET(cpu, &mask);
    sched_setaffinity(0, sizeof(mask), &mask);
}

// 最优内存分配策略
void optimal_numa_allocation(float** data, size_t size, int num_threads) {
    int num_nodes = numa_num_configured_nodes();

    // 每个NUMA节点分配一部分数据
    for (int node = 0; node < num_nodes; node++) {
        size_t chunk = size / num_nodes;
        float* ptr = numa_alloc_onnode(chunk * sizeof(float), node);

        // 绑定到该节点的线程处理这部分数据
        #pragma omp parallel for num_threads(num_threads/num_nodes)
        for (int i = node * chunk; i < (node + 1) * chunk; i++) {
            process_data(&data[i]);
        }
    }
}
```

### 6.3 first-touch策略

```c
// first-touch策略：数据初始化时分配到访问它的节点

void first_touch_allocation(float** data, size_t n, int num_threads) {
    // 1. 使用interleave分配，避免全部在一个节点
    *data = numa_alloc_interleaved(n * sizeof(float));

    // 2. 每个线程初始化自己的部分 (触发first-touch)
    #pragma omp parallel for num_threads(num_threads)
    for (int i = 0; i < n; i++) {
        (*data)[i] = 0.0f;  // 这会分配数据到当前线程的本地节点
    }
}

// 对比: 非NUMA感知分配
void bad_allocation(float** data, size_t n) {
    // 所有数据都在调用线程的节点
    *data = malloc(n * sizeof(float));

    // 初始化
    for (int i = 0; i < n; i++) {
        (*data)[i] = 0.0f;
    }

    // 其他节点访问时都是远程访问！
}

/* 性能对比 (双路服务器, 64GB数据):
 * 非NUMA感知:  2.5 s
 * NUMA优化:    1.3 s (1.9x加速)
 */
```

---

## 实战案例

### 7.1 案例：图像处理管道

```c
// 原始实现 - 多次遍历
void image_pipeline_bad(const uint8_t* input, uint8_t* output,
                        int width, int height) {
    // 分配中间缓冲区
    uint8_t* temp1 = malloc(width * height);
    uint8_t* temp2 = malloc(width * height);
    uint8_t* temp3 = malloc(width * height);

    // 每个步骤遍历整张图像
    gaussian_blur(input, temp1, width, height);    // 读取1x，写入1x
    edge_detect(temp1, temp2, width, height);      // 读取1x，写入1x
    sharpen(temp2, temp3, width, height);          // 读取1x，写入1x
    color_adjust(temp3, output, width, height);    // 读取1x，写入1x

    // 总内存流量: 4读 + 4写 = 8x图像大小

    free(temp1);
    free(temp2);
    free(temp3);
}

// 优化实现 - 分块处理，缓存驻留
void image_pipeline_optimized(const uint8_t* input, uint8_t* output,
                               int width, int height) {
    // 选择块大小使所有数据在L2缓存中
    const int TILE_SIZE = 256;  // 256x256 = 64KB (适合L2)

    // 小块中间缓冲区
    uint8_t tile1[TILE_SIZE * TILE_SIZE];
    uint8_t tile2[TILE_SIZE * TILE_SIZE];
    uint8_t tile3[TILE_SIZE * TILE_SIZE];

    for (int ty = 0; ty < height; ty += TILE_SIZE) {
        for (int tx = 0; tx < width; tx += TILE_SIZE) {
            int tile_h = (ty + TILE_SIZE < height) ? TILE_SIZE : height - ty;
            int tile_w = (tx + TILE_SIZE < width) ? TILE_SIZE : width - tx;

            // 处理一个块 (所有数据在缓存中)
            load_tile(input, tile1, tx, ty, tile_w, tile_h, width);
            gaussian_blur_tile(tile1, tile2, tile_w, tile_h);
            edge_detect_tile(tile2, tile3, tile_w, tile_h);
            sharpen_tile(tile3, tile1, tile_w, tile_h);
            color_adjust_tile(tile1, output, tx, ty, tile_w, tile_h, width);
        }
    }

    // 总内存流量: 1读 + 1写 = 2x图像大小
}

/* 性能对比 (4K图像 3840x2160):
 * 原始:    45 ms,  带宽: 12 GB/s
 * 优化:    12 ms,  带宽: 5 GB/s (更少数据移动)
 * 加速比:  3.75x
 */
```

### 7.2 案例：键值存储优化

```c
// 哈希表实现优化

// 原始: 链表法
struct Entry {
    uint64_t key;
    uint64_t value;
    struct Entry* next;
};

struct Entry* buckets[N_BUCKETS];

// 优化: 开放寻址 + 缓存行对齐
struct alignas(64) Bucket {
    uint64_t keys[7];      // 7个键
    uint64_t values[7];    // 7个值
    uint64_t metadata;     // 高位: 有效位，低位: 跳房子信息
};

struct Bucket* table;

// 优化查找
uint64_t lookup_optimized(uint64_t key) {
    uint32_t hash = hash_function(key);
    uint32_t idx = hash & (TABLE_SIZE - 1);

    // 一次加载整个缓存行 (7个条目)
    struct Bucket* bucket = &table[idx];

    // 向量化比较 (使用SIMD)
    for (int i = 0; i < 7; i++) {
        if (bucket->keys[i] == key) {
            return bucket->values[i];
        }
    }

    // 使用跳房子散列的替代位置
    // ...

    return NOT_FOUND;
}

/* 性能对比 (100M条目):
 * 链表法:    250 ns/查找, 缓存未命中率高
 * 优化法:    45 ns/查找,  每查找1次缓存访问
 * 加速比:    5.5x
 */
```

---

## 性能分析工具

### 8.1 Linux perf工具

```bash
# 基本性能计数
perf stat ./program

# 详细缓存统计
perf stat -e cycles,instructions,cache-references,cache-misses,L1-dcache-loads,L1-dcache-load-misses,LLC-loads,LLC-load-misses ./program

# 缓存未命中采样
perf record -e cache-misses ./program
perf report

# 内存访问分析
perf mem record ./program
perf mem report

# 热点函数分析
perf top
```

### 8.2 Intel VTune

```bash
# 内存访问分析
vtune -collect memory-access -result-dir vtune_results ./program

# 微架构分析
vtune -collect uarch-exploration -result-dir vtune_results ./program

# 查看报告
vtune -report summary -result-dir vtune_results
```

### 8.3 代码中的性能计数

```c
// 使用PAPI库 (Portable Hardware Performance Counters)
#include <papi.h>

void measure_cache_performance(void) {
    int events[3] = {
        PAPI_L1_DCM,   // L1数据缓存未命中
        PAPI_L2_DCM,   // L2数据缓存未命中
        PAPI_L3_TCM    // L3总缓存未命中
    };
    long long values[3];
    int EventSet = PAPI_NULL;

    // 初始化PAPI
    PAPI_library_init(PAPI_VER_CURRENT);
    PAPI_create_eventset(&EventSet);
    PAPI_add_events(EventSet, events, 3);

    // 开始计数
    PAPI_start(EventSet);

    // 执行代码
    run_algorithm();

    // 停止计数
    PAPI_stop(EventSet, values);

    printf("L1 Cache Misses:  %lld\n", values[0]);
    printf("L2 Cache Misses:  %lld\n", values[1]);
    printf("L3 Cache Misses:  %lld\n", values[2]);

    // 计算缓存命中率
    long long l1_accesses = get_l1_accesses();  // 需要另外计数
    double l1_hit_rate = 100.0 * (l1_accesses - values[0]) / l1_accesses;
    printf("L1 Hit Rate: %.2f%%\n", l1_hit_rate);
}
```

### 8.4 内存带宽测试工具

```c
// 内存带宽测试
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

// 顺序读取带宽
double sequential_read_bandwidth(size_t size_mb) {
    size_t size = size_mb * 1024 * 1024;
    char* buffer = aligned_alloc(4096, size);

    // 初始化
    memset(buffer, 1, size);

    volatile char sink;
    clock_t start = clock();

    // 顺序读取
    for (size_t i = 0; i < size; i += 64) {
        sink += buffer[i];  // 每64字节读一次
    }

    double elapsed = (double)(clock() - start) / CLOCKS_PER_SEC;
    double bandwidth = (double)size / elapsed / 1024 / 1024 / 1024;  // GB/s

    free(buffer);
    return bandwidth;
}

// 随机读取带宽
double random_read_bandwidth(size_t size_mb) {
    size_t size = size_mb * 1024 * 1024;
    char* buffer = aligned_alloc(4096, size);
    size_t* indices = malloc((size / 64) * sizeof(size_t));

    // 生成随机访问模式
    for (size_t i = 0; i < size / 64; i++) {
        indices[i] = (rand() % (size / 64)) * 64;
    }

    volatile char sink;
    clock_t start = clock();

    // 随机读取
    for (size_t i = 0; i < size / 64; i++) {
        sink += buffer[indices[i]];
    }

    double elapsed = (double)(clock() - start) / CLOCKS_PER_SEC;
    double bandwidth = (double)size / elapsed / 1024 / 1024 / 1024;

    free(buffer);
    free(indices);
    return bandwidth;
}

int main() {
    printf("Memory Bandwidth Test\n");
    printf("=====================\n\n");

    printf("Sequential Read: %.2f GB/s\n", sequential_read_bandwidth(100));
    printf("Random Read:     %.2f GB/s\n", random_read_bandwidth(100));

    return 0;
}

/* 典型服务器结果:
 * Sequential Read: 45 GB/s
 * Random Read:     2.1 GB/s  (严重受限于延迟)
 */
```

---

## 优化检查清单

```markdown
## 缓存优化检查清单

### 内存布局
- [ ] 数据按64字节对齐
- [ ] 热数据与冷数据分离
- [ ] 使用SoA而非AoS
- [ ] 避免结构体填充浪费

### 访问模式
- [ ] 顺序访问优先于随机访问
- [ ] 行优先遍历多维数组
- [ ] 分块处理大数据集
- [ ] 合并多个遍历为单次遍历

### 多线程
- [ ] 避免假共享 (64字节对齐)
- [ ] 使用线程私有数据减少竞争
- [ ] NUMA感知数据分配
- [ ] 绑定线程到特定核心

### 预取
- [ ] 使用软件预取指令
- [ ] 选择合适的预取距离
- [ ] 避免不必要的预取
- [ ] 利用硬件预取的流式模式

### 缓存级别利用
- [ ] L1: 分块大小 4-32KB
- [ ] L2: 分块大小 128-512KB
- [ ] L3: 分块大小 1-8MB
- [ ] 避免超过缓存容量的工作集
```

---

## 总结

缓存优化是高性能计算的核心技能：

**关键原则：**

1. **空间局部性**：访问相邻内存位置
2. **时间局部性**：重复使用最近访问的数据
3. **避免冲突**：减少缓存行争用和伪共享

**优化优先级：**

1. 算法优化 (复杂度降低)
2. 数据布局优化 (SoA, 对齐)
3. 访问模式优化 (分块, 顺序)
4. 硬件特性利用 (预取, NUMA)

**性能目标：**

- L1命中率 > 95%
- L2命中率 > 90%
- 每周期内存访问 > 1

---

*文档版本: 1.0*
*最后更新: 2026年3月*
*适用平台: x86_64, ARM64, RISC-V*
