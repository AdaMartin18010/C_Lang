# 性能优化练习 (Performance Optimization)

> **难度**: L4-L6 | **预计时间**: 15-20小时

---

## 练习1: SIMD向量化

### 目标

使用AVX2/SSE优化矩阵运算。

### 矩阵加法

```c
#include <immintrin.h>

// 普通实现
void mat_add_scalar(float *a, float *b, float *c, int n) {
    for (int i = 0; i < n * n; i++) {
        c[i] = a[i] + b[i];
    }
}

// AVX2实现 (256位向量，8个float)
void mat_add_avx2(float *a, float *b, float *c, int n) {
    int total = n * n;
    int i = 0;

    // 对齐处理
    for (; i < total && ((uintptr_t)(a + i) % 32 != 0); i++) {
        c[i] = a[i] + b[i];
    }

    // AVX2主循环
    for (; i <= total - 8; i += 8) {
        __m256 va = _mm256_load_ps(&a[i]);
        __m256 vb = _mm256_load_ps(&b[i]);
        __m256 vc = _mm256_add_ps(va, vb);
        _mm256_store_ps(&c[i], vc);
    }

    // 剩余元素
    for (; i < total; i++) {
        c[i] = a[i] + b[i];
    }
}
```

### 矩阵乘法 (分块 + AVX2)

```c
void matmul_blocked_avx2(float *a, float *b, float *c, int n) {
    const int BLOCK = 32;  // 缓存块大小

    for (int ii = 0; ii < n; ii += BLOCK) {
        for (int jj = 0; jj < n; jj += BLOCK) {
            for (int kk = 0; kk < n; kk += BLOCK) {
                // 处理BLOCK x BLOCK子矩阵
                for (int i = ii; i < ii + BLOCK && i < n; i++) {
                    for (int j = jj; j < jj + BLOCK && j < n; j += 8) {
                        __m256 c_vec = _mm256_load_ps(&c[i*n + j]);

                        for (int k = kk; k < kk + BLOCK && k < n; k++) {
                            __m256 a_val = _mm256_set1_ps(a[i*n + k]);
                            __m256 b_vec = _mm256_load_ps(&b[k*n + j]);
                            c_vec = _mm256_fmadd_ps(a_val, b_vec, c_vec);
                        }

                        _mm256_store_ps(&c[i*n + j], c_vec);
                    }
                }
            }
        }
    }
}
```

### 性能对比

```
矩阵大小: 1024x1024
Scalar:     2.5 GFLOPS
AVX2:      18.5 GFLOPS
Speedup:    7.4x
```

---


---

## 📑 目录

- [性能优化练习 (Performance Optimization)](#性能优化练习-performance-optimization)
  - [练习1: SIMD向量化](#练习1-simd向量化)
    - [目标](#目标)
    - [矩阵加法](#矩阵加法)
    - [矩阵乘法 (分块 + AVX2)](#矩阵乘法-分块--avx2)
    - [性能对比](#性能对比)
  - [📑 目录](#-目录)
  - [练习2: 缓存优化](#练习2-缓存优化)
    - [目标](#目标-1)
    - [矩阵转置](#矩阵转置)
    - [预取优化](#预取优化)
  - [练习3: 分支预测优化](#练习3-分支预测优化)
    - [目标](#目标-2)
    - [排序优化](#排序优化)
    - [查找优化](#查找优化)
  - [练习4: 锁-free编程](#练习4-锁-free编程)
    - [目标](#目标-3)
    - [无锁队列 (Michael-Scott Queue)](#无锁队列-michael-scott-queue)
  - [练习5: 内存布局优化](#练习5-内存布局优化)
    - [目标](#目标-4)
    - [结构体字段重排](#结构体字段重排)
    - [数组结构体(AoS) vs 结构体数组(SoA)](#数组结构体aos-vs-结构体数组soa)
  - [练习6: 编译器优化技巧](#练习6-编译器优化技巧)
    - [目标](#目标-5)
    - [属性使用](#属性使用)
    - [Likely/Unlikely宏](#likelyunlikely宏)
  - [综合项目: 图像卷积优化](#综合项目-图像卷积优化)
    - [目标](#目标-6)
    - [基准实现](#基准实现)
    - [优化步骤](#优化步骤)
    - [性能目标](#性能目标)
  - [性能分析工具](#性能分析工具)
    - [使用perf](#使用perf)
    - [使用Intel VTune (或AMD uProf)](#使用intel-vtune-或amd-uprof)
    - [使用Cachegrind](#使用cachegrind)
  - [深入理解](#深入理解)
    - [核心原理](#核心原理)
    - [实践应用](#实践应用)
    - [最佳实践](#最佳实践)


---

## 练习2: 缓存优化

### 目标

优化矩阵转置和遍历以提高缓存命中率。

### 矩阵转置

```c
// 缓存不友好的实现
void transpose_naive(int *src, int *dst, int n) {
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            dst[j*n + i] = src[i*n + j];  // 写入跨步大
        }
    }
}

// 分块优化 (缓存友好)
void transpose_blocked(int *src, int *dst, int n) {
    const int BLOCK = 64;  // 适合L1缓存

    for (int ii = 0; ii < n; ii += BLOCK) {
        for (int jj = 0; jj < n; jj += BLOCK) {
            for (int i = ii; i < ii + BLOCK && i < n; i++) {
                for (int j = jj; j < jj + BLOCK && j < n; j++) {
                    dst[j*n + i] = src[i*n + j];
                }
            }
        }
    }
}
```

### 预取优化

```c
void prefetch_example(float *data, int n) {
    for (int i = 0; i < n; i++) {
        // 预取未来数据
        if (i + 16 < n) {
            __builtin_prefetch(&data[i + 16], 0, 3);
        }
        process(data[i]);
    }
}
```

---

## 练习3: 分支预测优化

### 目标

减少分支预测失败，提高流水线效率。

### 排序优化

```c
// 分支多的实现
int sum_positive_branchy(int *arr, int n) {
    int sum = 0;
    for (int i = 0; i < n; i++) {
        if (arr[i] > 0) {  // 分支预测可能失败
            sum += arr[i];
        }
    }
    return sum;
}

// 分支少的实现 (使用条件移动)
int sum_positive_branchless(int *arr, int n) {
    int sum = 0;
    for (int i = 0; i < n; i++) {
        int mask = arr[i] >> 31;  // 负数时为-1，正数时为0
        sum += arr[i] & ~mask;     // 负数贡献0
    }
    return sum;
}
```

### 查找优化

```c
// 二分查找 (分支多)
int binary_search_branchy(int *arr, int n, int key) {
    int low = 0, high = n - 1;
    while (low <= high) {
        int mid = (low + high) >> 1;
        if (arr[mid] == key) return mid;
        if (arr[mid] < key) low = mid + 1;
        else high = mid - 1;
    }
    return -1;
}

// 无分支二分查找
int binary_search_branchless(int *arr, int n, int key) {
    int low = 0;
    int len = n;
    while (len > 1) {
        int half = len >> 1;
        len -= half;
        low = (arr[low + half - 1] < key) ? low + half : low;
    }
    return (arr[low] == key) ? low : -1;
}
```

---

## 练习4: 锁-free编程

### 目标

实现无锁数据结构。

### 无锁队列 (Michael-Scott Queue)

```c
#include <stdatomic.h>

typedef struct Node {
    void *data;
    _Atomic(struct Node *) next;
} Node;

typedef struct {
    _Atomic(Node *) head;
    _Atomic(Node *) tail;
} LockFreeQueue;

void lfqueue_init(LockFreeQueue *q) {
    Node *dummy = calloc(1, sizeof(Node));
    atomic_store(&q->head, dummy);
    atomic_store(&q->tail, dummy);
}

void lfqueue_enqueue(LockFreeQueue *q, void *data) {
    Node *node = calloc(1, sizeof(Node));
    node->data = data;

    while (1) {
        Node *tail = atomic_load(&q->tail);
        Node *next = atomic_load(&tail->next);

        if (tail == atomic_load(&q->tail)) {
            if (next == NULL) {
                if (atomic_compare_exchange_weak(&tail->next, &next, node)) {
                    atomic_compare_exchange_weak(&q->tail, &tail, node);
                    return;
                }
            } else {
                atomic_compare_exchange_weak(&q->tail, &tail, next);
            }
        }
    }
}

void* lfqueue_dequeue(LockFreeQueue *q) {
    while (1) {
        Node *head = atomic_load(&q->head);
        Node *tail = atomic_load(&q->tail);
        Node *next = atomic_load(&head->next);

        if (head == atomic_load(&q->head)) {
            if (head == tail) {
                if (next == NULL) return NULL;  // 空队列
                atomic_compare_exchange_weak(&q->tail, &tail, next);
            } else {
                void *data = next->data;
                if (atomic_compare_exchange_weak(&q->head, &head, next)) {
                    free(head);
                    return data;
                }
            }
        }
    }
}
```

---

## 练习5: 内存布局优化

### 目标

优化数据结构内存布局减少缓存缺失。

### 结构体字段重排

```c
// 内存布局差 (填充多)
typedef struct {
    char a;      // 1 byte + 7 padding
    double b;    // 8 bytes
    char c;      // 1 byte + 7 padding
    double d;    // 8 bytes
} BadLayout;     // 32 bytes

// 内存布局好 (紧凑)
typedef struct {
    double b;    // 8 bytes
    double d;    // 8 bytes
    char a;      // 1 byte
    char c;      // 1 byte + 6 padding
} GoodLayout;    // 24 bytes
```

### 数组结构体(AoS) vs 结构体数组(SoA)

```c
// Array of Structs (AoS) - 面向对象风格
typedef struct {
    float x, y, z;
    float vx, vy, vz;
    float mass;
} Particle;
Particle particles[1000];

// Struct of Arrays (SoA) - 数据导向设计
typedef struct {
    float x[1000], y[1000], z[1000];
    float vx[1000], vy[1000], vz[1000];
    float mass[1000];
} ParticlesSoA;

// SoA更适合SIMD处理
void update_positions_soa(ParticlesSoA *p, int n) {
    for (int i = 0; i < n; i += 8) {
        __m256 vx = _mm256_load_ps(&p->vx[i]);
        __m256 x = _mm256_load_ps(&p->x[i]);
        x = _mm256_add_ps(x, vx);
        _mm256_store_ps(&p->x[i], x);
        // ...
    }
}
```

---

## 练习6: 编译器优化技巧

### 目标

使用编译器扩展和属性优化。

### 属性使用

```c
// 热函数内联
__attribute__((always_inline)) static inline int hot_function(int x);

// 冷函数（错误处理）
__attribute__((cold)) void error_handler(const char *msg);

// 纯函数（无副作用）
__attribute__((pure)) int square(int x);

// 常量函数
__attribute__((const)) int max(int a, int b);

// 热/冷标签
__attribute__((hot)) void frequently_called(void);
__attribute__((cold)) void rarely_called(void);

// 对齐
__attribute__((aligned(64))) float cache_line_aligned[16];

// 打包结构体
__attribute__((packed)) struct Compact {
    char a;
    int b;
    char c;
};
```

### Likely/Unlikely宏

```c
#define likely(x)   __builtin_expect(!!(x), 1)
#define unlikely(x) __builtin_expect(!!(x), 0)

void process(int *data, int n) {
    for (int i = 0; i < n; i++) {
        if (unlikely(data[i] < 0)) {  // 告知编译器这种情况很少
            handle_error();
            continue;
        }
        process_normal(data[i]);
    }
}
```

---

## 综合项目: 图像卷积优化

### 目标

优化图像卷积（高斯模糊）性能。

### 基准实现

```c
void convolve2d_scalar(float *input, float *output, float *kernel,
                       int width, int height, int ksize) {
    int pad = ksize / 2;
    for (int y = pad; y < height - pad; y++) {
        for (int x = pad; x < width - pad; x++) {
            float sum = 0;
            for (int ky = 0; ky < ksize; ky++) {
                for (int kx = 0; kx < ksize; kx++) {
                    float pixel = input[(y + ky - pad) * width + (x + kx - pad)];
                    float weight = kernel[ky * ksize + kx];
                    sum += pixel * weight;
                }
            }
            output[y * width + x] = sum;
        }
    }
}
```

### 优化步骤

1. **分离卷积核** (可分离时): O(n²) -> O(2n)
2. **SIMD向量化**: 每次处理8像素
3. **分块处理**: 提高缓存命中率
4. **多线程**: OpenMP并行

### 性能目标

```
图像: 4096x4096, 核: 15x15
Scalar:     500 ms
分离核:     70 ms
SIMD:       15 ms
多线程(8核): 3 ms
Total: 166x speedup
```

---

## 性能分析工具

### 使用perf

```bash
# 编译带调试信息

---

## 🔗 全面知识关联体系

### 【全局层】知识库导航
| 维度 | 目标文档 | 导航作用 |
|:-----|:---------|:---------|
| **总索引** | [../00_GLOBAL_INDEX.md](../00_GLOBAL_INDEX.md) | 完整知识图谱入口，全局视角 |
| **本模块** | [../readme.md](../readme.md) | 模块总览与目录导航 |
| **学习路径** | [../06_Thinking_Representation/06_Learning_Paths/readme.md](../06_Thinking_Representation/06_Learning_Paths/readme.md) | 阶段化学习路线规划 |
| **概念映射** | [../06_Thinking_Representation/05_Concept_Mappings/readme.md](../06_Thinking_Representation/05_Concept_Mappings/readme.md) | 核心概念等价关系图 |

### 【阶段层】学习定位
**当前模块**: 知识库
**难度等级**: L1-L6
**前置依赖**: 核心知识体系
**后续延伸**: 持续学习

```

学习阶段金字塔:
    L6 专家层 [形式验证、编译器]
    L5 高级层 [并发、系统编程] ⬅️ 可能在此
    L4 进阶层 [指针、内存管理]
    L3 基础层 [函数、结构体]
    L2 入门层 [语法、数据类型]
    L1 零基础 [环境搭建]

```

### 【层次层】纵向知识链
| 层级 | 关联文档 | 层次关系 |
|:-----|:---------|:---------|
| **理论基础** | [../02_Formal_Semantics_and_Physics/00_Core_Semantics_Foundations/readme.md](../02_Formal_Semantics_and_Physics/00_Core_Semantics_Foundations/readme.md) | 语义学理论基础 |
| **核心机制** | [../01_Core_Knowledge_System/02_Core_Layer/readme.md](../01_Core_Knowledge_System/02_Core_Layer/readme.md) | C语言核心机制 |
| **标准接口** | [../01_Core_Knowledge_System/04_Standard_Library_Layer/readme.md](../01_Core_Knowledge_System/04_Standard_Library_Layer/readme.md) | 标准库API |
| **系统实现** | [../03_System_Technology_Domains/readme.md](../03_System_Technology_Domains/readme.md) | 系统级实现 |

### 【局部层】横向关联网
| 关联类型 | 目标文档 | 关联说明 |
|:---------|:---------|:---------|
| **技术扩展** | [../03_System_Technology_Domains/14_Concurrency_Parallelism/readme.md](../03_System_Technology_Domains/14_Concurrency_Parallelism/readme.md) | 并发编程技术 |
| **安全规范** | [../01_Core_Knowledge_System/09_Safety_Standards/MISRA_C_2023/readme.md](../01_Core_Knowledge_System/09_Safety_Standards/MISRA_C_2023/readme.md) | 安全编码标准 |
| **工具支持** | [../07_Modern_Toolchain/readme.md](../07_Modern_Toolchain/readme.md) | 现代开发工具链 |
| **实践案例** | [../04_Industrial_Scenarios/readme.md](../04_Industrial_Scenarios/readme.md) | 工业实践场景 |

### 【总体层】知识体系架构
```

┌─────────────────────────────────────────────────────────────┐
│                     总体知识体系架构                          │
├─────────────────────────────────────────────────────────────┤
│  01 Core Knowledge          → 核心概念与机制                  │
│  02 Formal Semantics        → 理论与物理基础                  │
│  03 System Technology       → 系统级技术领域                  │
│  04 Industrial Scenarios    → 工业应用场景                    │
│  05 Deep Structure          → 深层结构与元物理                │
│  06 Thinking Representation → 思维表征与学习                  │
│  07 Modern Toolchain        → 现代工具链                      │
└─────────────────────────────────────────────────────────────┘

```

### 【决策层】学习路径选择
| 目标 | 推荐路径 | 关键文档 |
|:-----|:---------|:---------|
| **系统学习** | 01 → 02 → 03 → 04 | 按顺序阅读各模块 |
| **问题导向** | 06决策树 → 相关模块 | [决策树目录](../06_Thinking_Representation/01_Decision_Trees/readme.md) |
| **项目驱动** | 04案例 → 所需知识 | [工业场景](../04_Industrial_Scenarios/readme.md) |
| **深入研究** | 02形式语义 → 11CompCert | [形式语义](../02_Formal_Semantics_and_Physics/readme.md) |

---

gcc -O3 -g program.c -o program

# 记录性能数据
perf record ./program

# 查看热点
perf report

# 详细分析
perf annotate
```

### 使用Intel VTune (或AMD uProf)

- 微架构分析
- 内存访问分析
- 向量化分析

### 使用Cachegrind

```bash
valgrind --tool=cachegrind ./program
cg_annotate cachegrind.out.*
```

---

> **返回导航**: [练习模块总览](./readme.md) | [知识库总览](../readme.md)


---

## 深入理解

### 核心原理

深入探讨技术原理和实现细节。

### 实践应用

- 应用场景1
- 应用场景2
- 应用场景3

### 最佳实践

1. 理解基础概念
2. 掌握核心机制
3. 应用到实际项目

---

> **最后更新**: 2026-03-21
> **维护者**: AI Code Review
