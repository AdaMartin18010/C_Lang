# 缓存优化实战指南

> **层级定位**: 02 Formal Semantics and Physics / 07 Microarchitecture
> **难度级别**: L4-L5
> **预估学习时间**: 6-8 小时

---

## 1. 缓存基础回顾

### 1.1 缓存层次结构

```
CPU核心
    │
    ├── L1缓存 (32-64KB)
    │   ├── L1d: 数据缓存
    │   └── L1i: 指令缓存
    │
    ├── L2缓存 (256KB-1MB)
    │   └── 统一缓存
    │
    └── L3缓存 (8-64MB共享)
        └── 多核心共享

访问延迟:
    L1: ~4 cycles
    L2: ~12 cycles
    L3: ~40 cycles
    内存: ~200+ cycles
```

### 1.2 缓存行与伪共享

```c
// 缓存行通常为64字节
#define CACHE_LINE_SIZE 64

// 伪共享示例
typedef struct {
    int counter1;  // CPU 0频繁更新
    int counter2;  // CPU 1频繁更新
} PaddedData;  // 同一缓存行！

// 解决方案：按缓存行对齐
typedef struct {
    int counter1;
    char padding[CACHE_LINE_SIZE - sizeof(int)];
} AlignedCounter;

AlignedCounter counters[2] __attribute__((aligned(CACHE_LINE_SIZE)));
```

---

## 2. 数据访问模式优化

### 2.1 行优先 vs 列优先

```c
// 二维数组访问模式
#define N 1024
int matrix[N][N];

// 行优先 - 缓存友好
void row_major() {
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            matrix[i][j] += 1;  // 顺序访问
        }
    }
}

// 列优先 - 缓存不友好
void col_major() {
    for (int j = 0; j < N; j++) {
        for (int i = 0; i < N; i++) {
            matrix[i][j] += 1;  // 跨行跳跃访问
        }
    }
}
// 性能差异: 行优先通常快5-10倍
```

### 2.2 循环分块 (Loop Tiling/Blocking)

```c
// 矩阵乘法 - 原始版本
void matmul_naive(float *A, float *B, float *C, int n) {
    for (int i = 0; i < n; i++)
        for (int j = 0; j < n; j++)
            for (int k = 0; k < n; k++)
                C[i*n+j] += A[i*n+k] * B[k*n+j];
}

// 分块优化版本
#define BLOCK_SIZE 64
void matmul_blocked(float *A, float *B, float *C, int n) {
    for (int ii = 0; ii < n; ii += BLOCK_SIZE)
        for (int jj = 0; jj < n; jj += BLOCK_SIZE)
            for (int kk = 0; kk < n; kk += BLOCK_SIZE)
                // 处理块内计算
                for (int i = ii; i < min(ii+BLOCK_SIZE, n); i++)
                    for (int j = jj; j < min(jj+BLOCK_SIZE, n); j++)
                        for (int k = kk; k < min(kk+BLOCK_SIZE, n); k++)
                            C[i*n+j] += A[i*n+k] * B[k*n+j];
}
```

### 2.3 结构体数组 vs 数组结构体

```c
// 数组结构体 (SoA) - 缓存友好
typedef struct {
    float x[1024];
    float y[1024];
    float z[1024];
} PointsSoA;

// 结构体数组 (AoS)
typedef struct {
    float x, y, z;
} Point;
Point points[1024];

// 如果只需要处理x坐标:
void process_soa(PointsSoA *p) {
    for (int i = 0; i < 1024; i++) {
        p->x[i] *= 2;  // 顺序访问，缓存高效
    }
}

void process_aos(Point *p) {
    for (int i = 0; i < 1024; i++) {
        p[i].x *= 2;  // 每12字节取4字节，浪费
    }
}
```

---

## 3. 预取技术

### 3.1 软件预取

```c
#include <xmmintrin.h>  // _mm_prefetch

void process_with_prefetch(float *data, int n) {
    for (int i = 0; i < n; i++) {
        // 预取未来数据
        if (i + 16 < n) {
            _mm_prefetch((const char*)&data[i + 16], _MM_HINT_T0);
        }
        
        // 处理当前数据
        data[i] = heavy_computation(data[i]);
    }
}
```

### 3.2 硬件预取友好模式

```c
// 顺序访问 - 硬件预取工作良好
void sequential_access(int *arr, int n) {
    for (int i = 0; i < n; i++) {
        arr[i]++;  // 硬件预取有效
    }
}

// 步长访问 - 需要特定步长
void strided_access(int *arr, int n) {
    for (int i = 0; i < n; i += 4) {
        arr[i]++;  // 固定步长，预取有效
    }
}

// 随机访问 - 预取无效
void random_access(int *arr, int *indices, int n) {
    for (int i = 0; i < n; i++) {
        arr[indices[i]]++;  // 硬件无法预测
    }
}
```

---

## 4. 多核缓存一致性

### 4.1 MESI协议实战

```c
// 无效化风暴示例
typedef struct {
    volatile int counter;
} SharedCounter;

SharedCounter counters[4];  // 4个核心各自更新

// 高竞争版本
void increment_naive(int cpu_id) {
    for (int i = 0; i < 1000000; i++) {
        counters[cpu_id].counter++;  // 仍在同一缓存行！
    }
}

// 缓存行对齐版本
typedef struct {
    volatile int counter;
    char padding[60];  // 填充到64字节
} PaddedCounter __attribute__((aligned(64)));

PaddedCounter counters_aligned[4];
```

### 4.2 锁与缓存

```c
// 假共享的锁
typedef struct {
    pthread_mutex_t lock;
    int data;
} LockedData;  // 锁和数据可能在同一缓存行

// 优化：分离锁和数据
typedef struct {
    pthread_mutex_t lock __attribute__((aligned(64)));
    char padding[64];
    int data;
} OptimizedLockedData;
```

---

## 5. 实际优化案例

### 5.1 链表遍历优化

```c
// 常规链表 - 缓存不友好
typedef struct Node {
    int data;
    struct Node *next;
} Node;

// 优化：将节点数据连续存储
// 使用数组索引代替指针
typedef struct {
    int data;
    int next_index;  // -1表示结束
} ArrayNode;

ArrayNode nodes[10000];
int head_index;

void traverse_optimized() {
    for (int i = head_index; i != -1; i = nodes[i].next_index) {
        process(nodes[i].data);  // 可能顺序访问
    }
}
```

### 5.2 哈希表优化

```c
// 开放寻址法 - 更好的缓存局部性
typedef struct {
    uint64_t key;
    void *value;
    bool occupied;
} HashEntry;

typedef struct {
    HashEntry *entries;
    size_t capacity;
    size_t size;
} LinearHashTable;

// 线性探测 - 缓存友好
void *lookup(LinearHashTable *ht, uint64_t key) {
    size_t idx = hash(key) & (ht->capacity - 1);
    
    while (ht->entries[idx].occupied) {
        if (ht->entries[idx].key == key) {
            return ht->entries[idx].value;
        }
        idx = (idx + 1) & (ht->capacity - 1);  // 顺序访问
    }
    return NULL;
}
```

---

## 6. 性能测量

### 6.1 使用perf分析缓存

```bash
# 测量缓存未命中
perf stat -e cache-misses,cache-references,L1-dcache-load-misses ./program

# 详细缓存统计
perf stat -e \
    L1-dcache-loads,L1-dcache-load-misses,\
    L1-dcache-stores,L1-dcache-store-misses,\
    LLC-loads,LLC-load-misses \
    ./program

# 记录缓存事件
perf record -e cache-misses ./program
perf report
```

### 6.2 PAPI库测量

```c
#include <papi.h>

void measure_cache() {
    int Events[2] = {PAPI_L1_DCM, PAPI_L2_DCM};  // L1/L2数据缓存未命中
    long long values[2];
    
    PAPI_start_counters(Events, 2);
    
    // 被测代码
    workload();
    
    PAPI_stop_counters(values, 2);
    
    printf("L1 misses: %lld\n", values[0]);
    printf("L2 misses: %lld\n", values[1]);
}
```

---

## 关联导航

### 前置知识
- [微架构](../README.md)
- [性能优化](../../../01_Core_Knowledge_System/05_Engineering_Layer/03_Performance_Optimization.md)
- [数组与指针](../../../01_Core_Knowledge_System/02_Core_Layer/05_Arrays_Pointers.md)

### 后续延伸
- [SIMD优化](./04_SIMD_Optimization.md)
- [并发编程](../../../03_System_Technology_Domains/14_Concurrency_Parallelism/README.md)
