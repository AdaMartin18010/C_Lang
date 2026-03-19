# 视频脚本：性能优化技巧

> **视频标题**: 性能优化技巧 - 让C代码飞起来
> **视频时长**: 35分钟
> **目标受众**: 有C语言基础，希望提升代码性能的开发者
> **前置知识**: C语言基础、计算机体系结构基础
> **关联文档**: 性能优化

---

## 开场 (0:00 - 1:30)

### 0:00 | 引入 | 画面：程序性能的重要性

**讲解词**:
性能优化是程序员的核心技能之一。无论是高频率交易系统、游戏引擎，还是嵌入式设备，性能都直接影响用户体验和成本。今天，我们将探索让C代码性能飞跃的各种技术。

### 0:40 | 优化原则 | 画面：优化策略的层次结构

**讲解词**:
优化有三个层次：算法层面——选择最优算法；架构层面——利用缓存和并行；微优化层面——指令级优化。本课程覆盖这三个层次。

### 1:10 | 主题预告 | 画面：课程大纲

**讲解词**:
今天我们将学习：性能分析方法、算法优化、内存访问优化、编译器优化、SIMD向量化、并行化、以及实战案例。

---

## 第一部分：性能分析 (1:30 - 6:00)

### 1:30 | 测量基础 | 画面：计时的精确方法

**讲解词**:
优化前必须能准确测量。使用高精度计时器，多次运行取平均，消除测量开销，都是基本要求。

**计时方法**:

```c
// Linux: 使用clock_gettime
#include <time.h>

struct timespec start, end;
clock_gettime(CLOCK_MONOTONIC, &start);

// 被测代码
function_to_measure();

clock_gettime(CLOCK_MONOTONIC, &end);
double elapsed = (end.tv_sec - start.tv_sec) +
                 (end.tv_nsec - start.tv_nsec) / 1e9;

// 或者使用RDTSC (x86)
#include <x86intrin.h>

uint64_t start = __rdtsc();
// 被测代码
uint64_t end = __rdtsc();
uint64_t cycles = end - start;
```

### 2:15 | perf工具 | 画面：perf的使用示例

**讲解词**:
perf是Linux上最强大的性能分析工具。它可以分析CPU周期、缓存未命中、分支预测失败等各种硬件事件。

**perf使用**:

```bash
# 基本性能统计
perf stat ./program

# 记录调用图
perf record -g ./program
perf report

# 分析特定事件
perf stat -e cycles,instructions,cache-misses,branch-misses ./program

# 火焰图
perf script | stackcollapse-perf.pl | flamegraph.pl > flame.svg

# 分析缓存行为
perf stat -e L1-dcache-loads,L1-dcache-load-misses,L1-dcache-stores ./program
```

### 3:00 | 火焰图解读 | 画面：火焰图的可视化

**讲解词**:
火焰图直观展示程序的调用栈和耗时分布。宽度表示时间比例，颜色无意义，纵向表示调用深度。

**火焰图分析**:

```
火焰图解读:
┌─────────────────────────────────────────┐
│ main()                                  │
│ ████████████████████████████████████   │
├─────────────────────────────────────────┤
│ process_data()    │  parse_input()      │
│ █████████████████ │  ████████████████   │
├───────────────────┼─────────────────────┤
│ heavy_compute()   │  strlen()           │
│ █████████████████ │  ████               │
└─────────────────────────────────────────┘

- 宽度 = 执行时间比例
- 纵向 = 调用栈深度
- 寻找"平顶" = 热点函数
- 瓶颈可能在宽的叶子节点
```

### 3:45 | 编译器优化报告 | 画面：GCC优化报告示例

**讲解词**:
编译器优化报告显示哪些优化被应用、哪些被跳过。使用-fopt-info选项可以获取详细信息。

**优化报告**:

```bash
# GCC优化报告
gcc -O3 -fopt-info-all-optall.log -c source.c

# 常见报告内容
# - loop vectorized: 循环向量化成功
# - loop unrolled: 循环展开
# - function inlined: 函数内联
# - missed: 优化机会未利用

# Clang优化报告
clang -O3 -Rpass=.* -Rpass-missed=.* -Rpass-analysis=.* source.c

# 查看汇编
gcc -O3 -S -fverbose-asm source.c
```

### 4:30 | 微基准测试 | 画面：Google Benchmark的使用

**讲解词**:
Google Benchmark是一个C++库，但也可用于测试C代码。它自动处理重复运行、统计分析和计时精度。

**基准测试**:

```c
#include <benchmark/benchmark.h>

// 被测函数
void sort_array(int *arr, int n);

static void BM_Sort(benchmark::State &state) {
    int n = state.range(0);
    int *arr = new int[n];

    for (auto _ : state) {
        // 设置数据
        for (int i = 0; i < n; i++) {
            arr[i] = n - i;  // 逆序
        }

        // 计时开始
        sort_array(arr, n);

        // 防止优化掉结果
        benchmark::DoNotOptimize(arr);
    }

    // 报告吞吐量
    state.SetItemsProcessed(state.iterations() * n);
    state.SetBytesProcessed(state.iterations() * n * sizeof(int));

    delete[] arr;
}

BENCHMARK(BM_Sort)->Range(8, 8<<10);

BENCHMARK_MAIN();
```

---

## 第二部分：算法优化 (6:00 - 11:00)

### 6:00 | 复杂度分析 | 画面：大O表示法的可视化

**讲解词**:
算法选择是优化最重要的层面。从O(n²)到O(n log n)的提升，往往比微优化更有价值。

**算法对比**:

```
算法复杂度对比 (n=1,000,000):
┌────────────────┬───────────────┬────────────────┐
│ 算法           │ 复杂度         │ 操作次数        │
├────────────────┼───────────────┼────────────────┤
│ 线性查找       │ O(n)          │ 1,000,000      │
│ 二分查找       │ O(log n)      │ 20             │
│ 冒泡排序       │ O(n²)         │ 1,000,000,000, │
│ 快速排序       │ O(n log n)    │ 20,000,000     │
│ 朴素字符串匹配 │ O(nm)         │ 1,000,000,000  │
│ KMP算法        │ O(n+m)        │ 2,000,000      │
└────────────────┴───────────────┴────────────────┘
```

### 6:40 | 数据结构选择 | 画面：不同操作的复杂度对比

**讲解词**:
选择合适的数据结构是优化的关键。不同操作（插入、查找、删除）在不同数据结构上的性能差异巨大。

**数据结构对比**:

```
操作复杂度:
┌─────────────┬──────────┬──────────┬──────────┬──────────┐
│ 数据结构     │ 插入      │ 查找      │ 删除      │ 遍历      │
├─────────────┼──────────┼──────────┼──────────┼──────────┤
│ 数组         │ O(n)     │ O(n)     │ O(n)     │ O(n)     │
│ 有序数组      │ O(n)     │ O(log n) │ O(n)     │ O(n)     │
│ 链表         │ O(1)     │ O(n)     │ O(n)     │ O(n)     │
│ 哈希表       │ O(1)*    │ O(1)*    │ O(1)*    │ O(n)     │
│ 二叉搜索树    │ O(log n) │ O(log n) │ O(log n) │ O(n)     │
│ 跳表         │ O(log n) │ O(log n) │ O(log n) │ O(n)     │
└─────────────┴──────────┴──────────┴──────────┴──────────┘
*平均情况

场景选择:
- 频繁查找 → 哈希表
- 需要顺序 → 树
- 频繁插入删除 → 链表
- 缓存友好 → 数组
```

### 7:25 | 查找优化 | 画面：各种查找算法的对比

**讲解词**:
查找是最常见的操作。根据数据特点选择最优算法：小数据用线性查找、有序数据用二分、字符串用Trie。

**查找优化**:

```c
// 优化1: 线性查找的哨兵优化
int linear_search(int *arr, int n, int key) {
    int last = arr[n - 1];
    arr[n - 1] = key;  // 哨兵

    int i = 0;
    while (arr[i] != key) i++;

    arr[n - 1] = last;
    return (i < n - 1 || last == key) ? i : -1;
}

// 优化2: 二分查找 (有序数据)
int binary_search(int *arr, int n, int key) {
    int lo = 0, hi = n - 1;
    while (lo <= hi) {
        int mid = lo + (hi - lo) / 2;
        if (arr[mid] < key) lo = mid + 1;
        else if (arr[mid] > key) hi = mid - 1;
        else return mid;
    }
    return -1;
}

// 优化3: 插值查找 (均匀分布数据)
int interpolation_search(int *arr, int n, int key) {
    int lo = 0, hi = n - 1;
    while (lo <= hi && key >= arr[lo] && key <= arr[hi]) {
        int pos = lo + ((key - arr[lo]) * (hi - lo)) / (arr[hi] - arr[lo]);
        if (arr[pos] == key) return pos;
        if (arr[pos] < key) lo = pos + 1;
        else hi = pos - 1;
    }
    return -1;
}
```

### 8:20 | 排序优化 | 画面：排序算法的性能对比

**讲解词**:
排序是经典问题。标准库的qsort通常足够好，但特定场景可以做得更好。

**排序优化**:

```c
// 优化1: 对小数组使用插入排序
void introsort(int *arr, int n);

// 优化2: 计数排序 (整数、范围小)
void counting_sort(uint8_t *arr, int n) {
    int count[256] = {0};
    for (int i = 0; i < n; i++) count[arr[i]]++;

    int k = 0;
    for (int i = 0; i < 256; i++) {
        while (count[i]-- > 0) arr[k++] = i;
    }
}

// 优化3: 基数排序 (整数)
void radix_sort(uint32_t *arr, int n);

// 优化4: 针对几乎有序数据的TimSort
// (现代标准库已实现)
```

### 9:10 | 字符串处理 | 画面：字符串算法的优化

**讲解词**:
字符串处理在很多应用中占主导地位。KMP、Boyer-Moore、Rabin-Karp等算法可以显著提升性能。

**字符串优化**:

```c
// KMP算法 (线性时间字符串匹配)
void compute_lps(const char *pat, int m, int *lps);

void kmp_search(const char *txt, const char *pat) {
    int n = strlen(txt);
    int m = strlen(pat);
    int lps[m];
    compute_lps(pat, m, lps);

    int i = 0, j = 0;
    while (i < n) {
        if (pat[j] == txt[i]) {
            i++; j++;
        }
        if (j == m) {
            printf("Found at %d\n", i - j);
            j = lps[j - 1];
        } else if (i < n && pat[j] != txt[i]) {
            if (j != 0) j = lps[j - 1];
            else i++;
        }
    }
}

// Boyer-Moore (更快的平均情况)
void boyer_moore_search(const char *txt, const char *pat);
```

### 10:00 | 空间换时间 | 画面：预计算和缓存的示例

**讲解词**:
空间换时间是经典优化策略。预计算、查找表、缓存都可以减少运行时计算。

**空间换时间**:

```c
// 优化1: 三角函数查找表
static const float sin_table[360] = {
    0.000000f, 0.017452f, 0.034899f, // ...
};

float fast_sin(int degrees) {
    return sin_table[degrees % 360];
}

// 优化2: CRC查找表
static const uint32_t crc32_table[256] = { /* ... */ };

uint32_t fast_crc32(const uint8_t *data, size_t len) {
    uint32_t crc = 0xFFFFFFFF;
    for (size_t i = 0; i < len; i++) {
        crc = crc32_table[(crc ^ data[i]) & 0xFF] ^ (crc >> 8);
    }
    return ~crc;
}

// 优化3: 记忆化 (递归转DP)
int fib(int n) {
    static int memo[100] = {0};
    if (n <= 1) return n;
    if (memo[n] != 0) return memo[n];
    return memo[n] = fib(n - 1) + fib(n - 2);
}
```

---

## 第三部分：内存优化 (11:00 - 17:00)

### 11:00 | 缓存层次结构 | 画面：多级缓存的可视化

**讲解词**:
现代CPU有多级缓存。L1缓存最快但最小，L3最慢但最大。理解缓存层次是内存优化的基础。

**缓存层次**:

```
内存层次 (典型x86-64服务器):
┌──────────────┬─────────┬────────────┬──────────┐
│ 层次         │ 大小     │ 访问延迟    │ 速度     │
├──────────────┼─────────┼────────────┼──────────┤
│ L1d缓存      │ 32KB    │ 4 cycles   │ ~1x      │
│ L1i缓存      │ 32KB    │ 4 cycles   │ ~1x      │
│ L2缓存       │ 256KB   │ 12 cycles  │ ~3x      │
│ L3缓存       │ 16MB    │ 40 cycles  │ ~10x     │
│ 主内存       │ GB级    │ 200+ cycles│ ~50x     │
│ SSD          │ TB级    │ 10us       │ ~2500x   │
│ 网络         │ -       │ 1ms        │ ~250000x │
└──────────────┴─────────┴────────────┴──────────┘

缓存行: 64字节 (x86), 128字节 (ARM)
预取: 硬件自动检测顺序访问模式
```

### 11:50 | 数据局部性 | 画面：行优先vs列优先访问对比

**讲解词**:
数据局部性决定缓存效率。空间局部性——访问相邻数据；时间局部性——重复使用数据。

**局部性优化**:

```c
// 不好: 列优先访问 (缓存不友好)
for (int j = 0; j < cols; j++) {
    for (int i = 0; i < rows; i++) {
        sum += matrix[i][j];  // 跳跃访问
    }
}

// 好: 行优先访问 (缓存友好)
for (int i = 0; i < rows; i++) {
    for (int j = 0; j < cols; j++) {
        sum += matrix[i][j];  // 顺序访问
    }
}

// 结构体数组 vs 数组结构体
// 不好
struct Particle {
    float x, y, z;
    float vx, vy, vz;
} particles[1000];

// 可能更好 (如果只访问位置)
struct Particles {
    float x[1000], y[1000], z[1000];
    float vx[1000], vy[1000], vz[1000];
} particles;
```

### 12:45 | 循环优化 | 画面：循环变换的可视化

**讲解词**:
循环是优化的重点。循环交换、循环展开、循环分块、循环融合都是常用技术。

**循环优化**:

```c
// 优化1: 循环展开
// 原始
for (int i = 0; i < n; i++) {
    sum += arr[i];
}

// 展开4次
for (int i = 0; i < n; i += 4) {
    sum += arr[i] + arr[i+1] + arr[i+2] + arr[i+3];
}

// 优化2: 循环分块 (矩阵乘法)
void matmul_blocked(float *C, float *A, float *B, int n) {
    const int BLOCK = 32;  // 适应L1缓存

    for (int ii = 0; ii < n; ii += BLOCK) {
        for (int jj = 0; jj < n; jj += BLOCK) {
            for (int kk = 0; kk < n; kk += BLOCK) {
                // 块内计算
                for (int i = ii; i < ii + BLOCK; i++) {
                    for (int j = jj; j < jj + BLOCK; j++) {
                        float sum = C[i*n+j];
                        for (int k = kk; k < kk + BLOCK; k++) {
                            sum += A[i*n+k] * B[k*n+j];
                        }
                        C[i*n+j] = sum;
                    }
                }
            }
        }
    }
}

// 优化3: 循环不变量外提
// 不好
for (int i = 0; i < n; i++) {
    int x = strlen(str);  // 每次都计算
    arr[i] = x;
}

// 好
int x = strlen(str);
for (int i = 0; i < n; i++) {
    arr[i] = x;
}
```

### 14:00 | 对齐和填充 | 画面：结构体内存布局

**讲解词**:
数据对齐影响缓存行利用率和SIMD效率。正确对齐可以显著提升性能。

**对齐优化**:

```c
// 结构体内存布局优化
// 不好 (24字节)
struct Bad {
    char a;      // 1 + 3 padding
    int b;       // 4
    char c;      // 1 + 3 padding
    int d;       // 4
    char e;      // 1 + 3 padding
};

// 好 (12字节)
struct Good {
    int b;       // 4
    int d;       // 4
    char a;      // 1
    char c;      // 1
    char e;      // 1 + 1 padding
};

// 显式对齐
#include <stdalign.h>

alignas(64) float buffer[1024];  // 64字节对齐 (缓存行)

// 动态对齐
float *ptr = aligned_alloc(64, size);

// 结构体对齐
struct Aligned {
    char data;
    alignas(64) char cache_line[64];  // 单独对齐
};
```

### 14:55 | 预取 | 画面：硬件和软件预取

**讲解词**:
预取是在数据需要之前将其加载到缓存。硬件自动预取检测顺序访问，软件预取用于不规则访问模式。

**预取使用**:

```c
// GCC内置预取
#include <xmmintrin.h>

void process_with_prefetch(float *arr, int n) {
    for (int i = 0; i < n; i++) {
        // 预取未来要访问的数据
        if (i + 16 < n) {
            _mm_prefetch((char *)&arr[i + 16], _MM_HINT_T0);
        }

        // 处理当前数据
        process(arr[i]);
    }
}

// 或者使用GCC内置
void prefetch(void *addr) {
    __builtin_prefetch(addr, 0, 3);  // 读, 高局部性
}
```

### 15:40 | 避免伪共享 | 画面：多线程缓存行冲突

**讲解词**:
伪共享发生在不同线程修改同一缓存行的不同变量时。虽然逻辑不冲突，但会导致缓存反复同步。

**伪共享解决**:

```c
// 不好: 可能共享缓存行
struct Counter {
    atomic_int count;
};

struct Counter counters[8];  // 8个线程的计数器

// 好: 填充到缓存行大小
#define CACHE_LINE_SIZE 64

struct PaddedCounter {
    atomic_int count;
    char padding[CACHE_LINE_SIZE - sizeof(atomic_int)];
};

struct PaddedCounter counters[8];
// 确保每个计数器在不同缓存行
```

---

## 第四部分：编译器和指令优化 (17:00 - 24:00)

### 17:00 | 编译器优化级别 | 画面：-O0到-O3的对比

**讲解词**:
GCC/Clang提供多个优化级别。-O0用于调试，-O2是发布版本的平衡选择，-O3启用激进优化但可能增加代码大小。

**优化级别**:

```
优化级别对比:
┌──────────┬──────────────────────────────────────────────┐
│ 级别     │ 特点                                         │
├──────────┼──────────────────────────────────────────────┤
│ -O0      │ 无优化, 最快编译, 最好调试                    │
│ -O1      │ 基本优化, 保持调试能力                        │
│ -O2      │ 全面优化 (推荐发布版本)                       │
│ -O3      │ 激进优化, 可能增大代码                        │
│ -Os      │ 优化代码大小                                  │
│ -Ofast   │ -O3 + 忽略标准符合性 (危险)                   │
│ -Og      │ 优化保持调试能力 (推荐开发)                   │
└──────────┴──────────────────────────────────────────────┘

常用选项:
-O2 -march=native -flto  (链接时优化)
```

### 17:45 | 自动向量化 | 画面：编译器向量化的条件

**讲解词**:
编译器可以自动将循环转换为SIMD代码。理解向量化条件可以写出编译器更容易优化的代码。

**向量化条件**:

```c
// 可以向量化的循环
void good_vectorization(float *a, float *b, float *c, int n) {
    for (int i = 0; i < n; i++) {
        c[i] = a[i] + b[i];  // 简单, 无依赖
    }
}

// 阻碍向量化的因素
void bad_vectorization(float *a, int n) {
    for (int i = 0; i < n; i++) {
        a[i] = a[i-1] + 1;   // 循环依赖!
    }

    for (int i = 0; i < n; i++) {
        if (a[i] > 0)        // 条件分支
            a[i] = sqrt(a[i]);
    }

    float sum = 0;
    for (int i = 0; i < n; i++) {
        sum += a[i];         // 归约, 需要特殊处理
    }
}

// 帮助编译器
#pragma GCC ivdep  // 忽略向量依赖 (确保安全!)
for (int i = 0; i < n; i++) {
    a[i] = b[i] * c[i];
}
```

### 18:40 | SIMD编程 | 画面：SSE/AVX指令示例

**讲解词**:
当自动向量化不够时，可以手动使用SIMD指令。SSE处理128位，AVX处理256位，AVX-512处理512位。

**SIMD编程**:

```c
#include <immintrin.h>

// SSE向量加法
void vec_add_sse(float *a, float *b, float *c, int n) {
    for (int i = 0; i < n; i += 4) {
        __m128 va = _mm_loadu_ps(&a[i]);
        __m128 vb = _mm_loadu_ps(&b[i]);
        __m128 vc = _mm_add_ps(va, vb);
        _mm_storeu_ps(&c[i], vc);
    }
}

// AVX向量加法 (256位, 8个float)
void vec_add_avx(float *a, float *b, float *c, int n) {
    for (int i = 0; i < n; i += 8) {
        __m256 va = _mm256_loadu_ps(&a[i]);
        __m256 vb = _mm256_loadu_ps(&b[i]);
        __m256 vc = _mm256_add_ps(va, vb);
        _mm256_storeu_ps(&c[i], vc);
    }
}

// 使用Intel Intrinsics Guide查询指令
```

### 19:50 | 分支预测 | 画面：分支预测失败的代价

**讲解词**:
现代CPU使用分支预测推测执行。预测失败会清空流水线，代价高昂。通过likely/unlikely提示可以帮助预测。

**分支优化**:

```c
#include <stdbool.h>

// GCC/Clang 分支预测提示
#define likely(x)   __builtin_expect(!!(x), 1)
#define unlikely(x) __builtin_expect(!!(x), 0)

void process_with_hints(int *data, int n) {
    for (int i = 0; i < n; i++) {
        // 假设大多数值为正
        if (likely(data[i] >= 0)) {
            process_positive(data[i]);
        } else {
            process_negative(data[i]);
        }

        // 假设错误很少发生
        if (unlikely(check_error(data[i]))) {
            handle_error();
        }
    }
}

// 优化: 排序后处理 (减少分支)
void process_sorted(int *data, int n) {
    // 先排序
    qsort(data, n, sizeof(int), compare);

    // 分支预测更准
    for (int i = 0; i < n; i++) {
        if (data[i] < threshold) {
            // 连续多个可能都满足
        }
    }
}
```

### 20:45 | 内联函数 | 画面：内联的利弊

**讲解词**:
内联消除函数调用开销，但会增加代码大小。小函数适合内联，大函数可能适得其反。

**内联优化**:

```c
// 总是内联
static inline int max(int a, int b) {
    return a > b ? a : b;
}

// GCC强制内联
#define FORCE_INLINE __attribute__((always_inline))

static FORCE_INLINE int fast_max(int a, int b) {
    return a > b ? a : b;
}

// 禁止内联 (大函数或递归)
#define NO_INLINE __attribute__((noinline))

NO_INLINE void big_function(void);

// 编译器会权衡，通常不用手动干预
// 除非 profiling 显示需要
```

### 21:30 | 链接时优化 | 画面：LTO的工作流程

**讲解词**:
链接时优化(LTO)允许跨编译单元优化。编译器在链接阶段可以看到整个程序，进行更激进的优化。

**LTO使用**:

```bash
# 编译时启用LTO
gcc -O3 -flto -c file1.c
gcc -O3 -flto -c file2.c
gcc -O3 -flto file1.o file2.o -o program

# 或者全程序优化
gcc -O3 -fwhole-program file1.c file2.c -o program

# Clang ThinLTO (更快, 类似效果)
clang -O3 -flto=thin -c file1.c
clang -O3 -flto=thin -c file2.c
clang -O3 -flto=thin file1.o file2.o -o program
```

---

## 第五部分：并行优化 (24:00 - 29:00)

### 24:00 | 多线程并行 | 画面：OpenMP并行化示例

**讲解词**:
多核CPU需要并行代码才能充分发挥性能。OpenMP是简单的并行化方案，只需添加编译指令。

**OpenMP使用**:

```c
#include <omp.h>

// 并行for循环
void parallel_sum(float *arr, int n) {
    float sum = 0;

    #pragma omp parallel for reduction(+:sum)
    for (int i = 0; i < n; i++) {
        sum += arr[i];
    }

    printf("Sum: %f\n", sum);
}

// 并行sections
#pragma omp parallel sections
{
    #pragma omp section
    process_part1();

    #pragma omp section
    process_part2();
}

// 编译
// gcc -O3 -fopenmp program.c
```

### 25:00 | 任务并行 | 画面：任务并行的可视化

**讲解词**:
任务并行适合不规则计算。使用线程池或任务队列，可以动态分配工作。

**任务并行**:

```c
// 线程池任务并行
#include <pthread.h>

typedef struct {
    void (*func)(void *);
    void *arg;
} Task;

void parallel_process_tree(Node *root) {
    if (root == NULL) return;

    // 大任务并行化
    if (root->size > THRESHOLD) {
        pthread_t left_thread;
        pthread_create(&left_thread, NULL,
                       (void*)parallel_process_tree, root->left);

        parallel_process_tree(root->right);

        pthread_join(left_thread, NULL);
    } else {
        // 小任务串行
        parallel_process_tree(root->left);
        parallel_process_tree(root->right);
    }

    process_node(root);
}
```

### 26:00 | GPU卸载 | 画面：CUDA/OpenCL的简介

**讲解词**:
对于大规模并行计算，GPU可以提供数量级的加速。CUDA(NVIDIA)和OpenCL(跨平台)是主流方案。

**GPU编程**:

```c
// CUDA示例
__global__ void vec_add_gpu(float *a, float *b, float *c, int n) {
    int i = blockIdx.x * blockDim.x + threadIdx.x;
    if (i < n) {
        c[i] = a[i] + b[i];
    }
}

void launch_gpu_kernel(float *a, float *b, float *c, int n) {
    float *d_a, *d_b, *d_c;
    cudaMalloc(&d_a, n * sizeof(float));
    cudaMalloc(&d_b, n * sizeof(float));
    cudaMalloc(&d_c, n * sizeof(float));

    cudaMemcpy(d_a, a, n * sizeof(float), cudaMemcpyHostToDevice);
    cudaMemcpy(d_b, b, n * sizeof(float), cudaMemcpyHostToDevice);

    int threads = 256;
    int blocks = (n + threads - 1) / threads;
    vec_add_gpu<<<blocks, threads>>>(d_a, d_b, d_c, n);

    cudaMemcpy(c, d_c, n * sizeof(float), cudaMemcpyDeviceToHost);

    cudaFree(d_a);
    cudaFree(d_b);
    cudaFree(d_c);
}
```

### 27:00 | 锁优化 | 画面：无锁数据结构的简介

**讲解词**:
锁竞争是多线程的性能杀手。使用无锁数据结构、读-复制-更新(RCU)、或者细粒度锁可以提升并发性能。

**锁优化**:

```c
// 读多写少: 读写锁
pthread_rwlock_t rwlock;

void read_data() {
    pthread_rwlock_rdlock(&rwlock);  // 多个读者
    // 读取
    pthread_rwlock_unlock(&rwlock);
}

void write_data() {
    pthread_rwlock_wrlock(&rwlock);  // 独占
    // 写入
    pthread_rwlock_unlock(&rwlock);
}

// 或者使用RCU (Linux内核风格)
rcu_read_lock();
p = rcu_dereference(global_ptr);  // 获取指针
// 使用p (只读)
rcu_read_unlock();

// 写入端
new_ptr = kmalloc(...);
*new_ptr = new_data;
rcu_assign_pointer(global_ptr, new_ptr);
synchronize_rcu();  // 等待读者退出
kfree(old_ptr);
```

---

## 第六部分：实战案例 (29:00 - 33:30)

### 29:00 | 案例：图像处理优化 | 画面：图像滤波优化前后的对比

**讲解词**:
让我们看一个图像处理的完整优化案例。原始代码经过多轮优化，性能提升超过100倍。

**图像处理优化**:

```c
// 优化1: 原始代码 (基线)
void blur_naive(uint8_t *src, uint8_t *dst, int w, int h) {
    for (int y = 0; y < h; y++) {
        for (int x = 0; x < w; x++) {
            int sum = 0;
            for (int dy = -1; dy <= 1; dy++) {
                for (int dx = -1; dx <= 1; dx++) {
                    sum += src[(y+dy)*w + (x+dx)];
                }
            }
            dst[y*w + x] = sum / 9;
        }
    }
}

// 优化2: 分离滤波器 (O(n²) -> O(n))
void blur_separable(uint8_t *src, uint8_t *dst, int w, int h) {
    uint8_t *tmp = malloc(w * h);

    // 水平滤波
    for (int y = 0; y < h; y++) {
        for (int x = 0; x < w; x++) {
            tmp[y*w + x] = (src[y*w + x-1] + src[y*w + x] + src[y*w + x+1]) / 3;
        }
    }

    // 垂直滤波
    for (int y = 0; y < h; y++) {
        for (int x = 0; x < w; x++) {
            dst[y*w + x] = (tmp[(y-1)*w + x] + tmp[y*w + x] + tmp[(y+1)*w + x]) / 3;
        }
    }

    free(tmp);
}

// 优化3: SIMD
#include <immintrin.h>

void blur_simd(uint8_t *src, uint8_t *dst, int w, int h);

// 优化4: 多线程
void blur_parallel(uint8_t *src, uint8_t *dst, int w, int h) {
    #pragma omp parallel for
    for (int y = 0; y < h; y++) {
        // 处理一行
    }
}
```

### 30:30 | 案例：网络服务器优化 | 画面：服务器架构的优化

**讲解词**:
网络服务器的优化涉及I/O多路复用、零拷贝、内存池等多个方面。

**服务器优化**:

```c
// 优化1: epoll代替select/poll
int epoll_fd = epoll_create1(0);
struct epoll_event ev, events[MAX_EVENTS];

ev.events = EPOLLIN;
ev.data.fd = listen_fd;
epoll_ctl(epoll_fd, EPOLL_CTL_ADD, listen_fd, &ev);

while (1) {
    int nfds = epoll_wait(epoll_fd, events, MAX_EVENTS, -1);
    for (int i = 0; i < nfds; i++) {
        if (events[i].data.fd == listen_fd) {
            // 接受新连接
        } else {
            // 处理I/O
        }
    }
}

// 优化2: 零拷贝发送
sendfile(out_fd, in_fd, &offset, count);

// 优化3: 内存池
void *pool_alloc(Pool *p, size_t size);
void pool_free(Pool *p, void *ptr);

// 优化4: 无锁队列
lockfree_queue_push(queue, item);
lockfree_queue_pop(queue, &item);
```

### 31:45 | 优化检查清单 | 画面：优化前后的检查表

**讲解词**:
优化完成后，使用检查清单确保没有引入问题。

**优化检查清单**:

```
优化检查清单:
□ 正确性验证
  □ 单元测试通过
  □ 集成测试通过
  □ 边界情况测试

□ 性能验证
  □ 基准测试对比
  □ 多平台测试
  □ 不同负载测试

□ 资源检查
  □ 内存使用合理
  □ 无内存泄漏
  □ 线程安全

□ 代码质量
  □ 可读性未严重下降
  □ 注释说明优化原因
  □ 可维护性
```

---

## 总结 (33:30 - 35:00)

### 33:30 | 核心内容回顾 | 画面：知识体系的思维导图

**讲解词**:
让我们回顾今天的内容：性能分析工具和方法、算法优化、内存访问优化、编译器和指令优化、并行优化、实战案例。

**知识总结**:

```
性能优化
├── 分析
│   ├── 计时测量
│   ├── perf工具
│   ├── 火焰图
│   └── 基准测试
├── 算法
│   ├── 复杂度分析
│   ├── 数据结构选择
│   ├── 查找优化
│   ├── 排序优化
│   └── 空间换时间
├── 内存
│   ├── 缓存优化
│   ├── 数据局部性
│   ├── 循环优化
│   ├── 对齐填充
│   └── 伪共享
├── 指令
│   ├── 编译器优化
│   ├── 向量化
│   ├── SIMD
│   ├── 分支预测
│   └── LTO
└── 并行
    ├── 多线程
    ├── OpenMP
    ├── 任务并行
    └── GPU
```

### 34:30 | 黄金法则 | 画面：优化的黄金法则

**讲解词**:
记住性能优化的黄金法则：先测量再优化；优化热点代码；保持代码可读性；考虑维护成本。

**黄金法则**:

1. **先测量** - 找到真正的瓶颈
2. **优化算法** - 大O改进最有效
3. **关注局部性** - 缓存友好是关键
4. **帮助编译器** - 写出可优化的代码
5. **验证正确性** - 优化不能破坏功能

### 34:55 | 结束 | 画面：资源链接

**讲解词**:
感谢观看！本视频代码示例在GitHub上。下一集我们将学习形式化验证入门。

---

## 技术备注

### 工具安装

```bash
# perf (Linux)
sudo apt install linux-tools-common linux-tools-generic

# Google Benchmark
git clone https://github.com/google/benchmark.git
cd benchmark && mkdir build && cd build
cmake .. -DBENCHMARK_DOWNLOAD_DEPENDENCIES=ON
make && sudo make install
```

### 参考文档

- [What Every Programmer Should Know About Memory](https://people.freebsd.org/~lstewart/articles/cpumemory.pdf)
- [Agner Fog的优化手册](https://agner.org/optimize/)

---

*脚本版本: 1.0*
*最后更新: 2026-03-19*


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
