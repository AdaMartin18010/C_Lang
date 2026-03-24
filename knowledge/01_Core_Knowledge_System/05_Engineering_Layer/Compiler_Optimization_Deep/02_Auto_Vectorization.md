# C语言自动向量化深度解析

> **编译器**: GCC 12+ / Clang 15+
> **指令集**: SSE/AVX/AVX2/AVX-512/NEON/SVE
> **优化级别**: -O3 / -ftree-vectorize

---

## 1. 向量化基础

### 1.1 SIMD概念

单指令多数据 (SIMD) 允许一条指令同时处理多个数据：

```
标量操作:  1 + 1  (1次运算1个结果)
向量操作:  [1,2,3,4] + [5,6,7,8] = [6,8,10,12] (1次运算4个结果)
```

### 1.2 向量宽度

| 指令集 | 寄存器宽度 | int32并行度 | float32并行度 |
|:-------|:-----------|:------------|:--------------|
| SSE4.2 | 128-bit | 4 | 4 |
| AVX2 | 256-bit | 8 | 8 |
| AVX-512 | 512-bit | 16 | 16 |
| NEON (ARM) | 128-bit | 4 | 4 |
| SVE2 (ARM) | 可变 | 最多16 | 最多16 |

---

## 2. 可被向量化的模式

### 2.1 简单数组操作

```c
// 编译器可以自动向量化为 SIMD 指令
void add_arrays(float* restrict c,
                const float* restrict a,
                const float* restrict b,
                int n) {
    for (int i = 0; i < n; i++) {
        c[i] = a[i] + b[i];
    }
}

// GCC/Clang 生成类似:
// vmovups (%rsi), %ymm0      # 加载8个float
// vaddps (%rdx), %ymm0, %ymm0 # 加法
// vmovups %ymm0, (%rdi)      # 存储
```

### 2.2 条件赋值

```c
// 可以向量化为条件移动
void clamp_array(float* out, const float* in, int n, float min, float max) {
    for (int i = 0; i < n; i++) {
        float x = in[i];
        if (x < min) x = min;
        if (x > max) x = max;
        out[i] = x;
    }
}

// 向量化: 计算所有分支，然后混合结果
```

### 2.3 归约操作

```c
// 求和 (需要特殊处理)
float sum_array(const float* arr, int n) {
    float sum = 0.0f;
    for (int i = 0; i < n; i++) {
        sum += arr[i];
    }
    return sum;
}

// 编译器生成:
// 1. 向量累加多个部分和
// 2. 水平合并向量内的和
// 3. 处理剩余元素
```

---

## 3. 帮助编译器向量化

### 3.1 使用restrict

```c
// 告诉编译器指针不重叠
void good_vectorization(float* restrict out,
                        const float* restrict in1,
                        const float* restrict in2,
                        int n) {
    for (int i = 0; i < n; i++) {
        out[i] = in1[i] + in2[i];
    }
}
```

### 3.2 对齐数据

```c
#include <immintrin.h>  // 或 <arm_neon.h>

// 手动对齐分配
float* aligned_alloc(size_t count) {
    void* ptr = NULL;
    posix_memalign(&ptr, 32, count * sizeof(float));  // 32字节对齐
    return (float*)ptr;
}

// C11对齐分配
#include <stdalign.h>
#include <stdlib.h>

float* aligned_alloc_c11(size_t count) {
    return aligned_alloc(32, count * sizeof(float));
}

// 使用
void process_aligned(float* out, float* in, int n) {
    for (int i = 0; i < n; i += 8) {
        // 编译器知道对齐，可使用对齐加载指令
    }
}
```

### 3.3 避免指针别名

```c
// 可能无法向量化 (编译器不确定a和b是否重叠)
void maybe_aliased(int* a, int* b, int n) {
    for (int i = 0; i < n; i++) {
        a[i] = b[i] * 2;
    }
}

// 可以向量化的版本
void no_alias(int* restrict a, const int* restrict b, int n) {
    for (int i = 0; i < n; i++) {
        a[i] = b[i] * 2;
    }
}
```

---

## 4. 手动SIMD (当自动向量化不够时)

### 4.1 x86 AVX2示例

```c
#include <immintrin.h>

// 手动AVX2向量化
void add_arrays_avx2(float* out, const float* a, const float* b, int n) {
    int i = 0;

    // 每次处理8个float (256-bit)
    for (; i <= n - 8; i += 8) {
        __m256 va = _mm256_loadu_ps(&a[i]);
        __m256 vb = _mm256_loadu_ps(&b[i]);
        __m256 vc = _mm256_add_ps(va, vb);
        _mm256_storeu_ps(&out[i], vc);
    }

    // 剩余元素
    for (; i < n; i++) {
        out[i] = a[i] + b[i];
    }
}

// 整数版本
void add_int_arrays_avx2(int* out, const int* a, const int* b, int n) {
    int i = 0;

    for (; i <= n - 8; i += 8) {
        __m256i va = _mm256_loadu_si256((__m256i*)&a[i]);
        __m256i vb = _mm256_loadu_si256((__m256i*)&b[i]);
        __m256i vc = _mm256_add_epi32(va, vb);
        _mm256_storeu_si256((__m256i*)&out[i], vc);
    }

    for (; i < n; i++) {
        out[i] = a[i] + b[i];
    }
}
```

### 4.2 ARM NEON示例

```c
#include <arm_neon.h>

void add_arrays_neon(float* out, const float* a, const float* b, int n) {
    int i = 0;

    // NEON每次处理4个float (128-bit)
    for (; i <= n - 4; i += 4) {
        float32x4_t va = vld1q_f32(&a[i]);
        float32x4_t vb = vld1q_f32(&b[i]);
        float32x4_t vc = vaddq_f32(va, vb);
        vst1q_f32(&out[i], vc);
    }

    for (; i < n; i++) {
        out[i] = a[i] + b[i];
    }
}
```

---

## 5. 编译器选项

### 5.1 GCC/Clang

```bash
# 基础向量化
---

## 🔗 文档关联

### 核心关联
| 文档 | 关系类型 | 说明 |
|:-----|:---------|:-----|
| [内存管理](../../../../01_Core_Knowledge_System/02_Core_Layer/02_Memory_Management.md) | 核心关联 | 内存管理基础 |
| [指针深度](../../../../01_Core_Knowledge_System/02_Core_Layer/01_Pointer_Depth.md) | 核心关联 | 指针深度基础 |
| [并发编程](../../../../03_System_Technology_Domains/14_Concurrency_Parallelism/readme.md) | 核心关联 | 并发编程基础 |
| [数据类型](../../../../01_Core_Knowledge_System/01_Basic_Layer/02_Data_Type_System.md) | 核心关联 | 数据类型基础 |
| [数组与指针](../../../../01_Core_Knowledge_System/02_Core_Layer/05_Arrays_Pointers.md) | 核心关联 | 数组与指针基础 |

### 扩展阅读
| 文档 | 关系类型 | 说明 |
|:-----|:---------|:-----|
| [软件工程](../../../../01_Core_Knowledge_System/05_Engineering_Layer/readme.md) | 核心关联 | 软件工程基础 |
| [形式语义](../../../../02_Formal_Semantics_and_Physics/readme.md) | 核心关联 | 形式语义基础 |
| [系统技术](../../../../03_System_Technology_Domains/readme.md) | 核心关联 | 系统技术基础 |
| [工业场景](../../../../04_Industrial_Scenarios/readme.md) | 核心关联 | 工业场景基础 |
| [思维表征](../../../../06_Thinking_Representation/readme.md) | 核心关联 | 思维表征基础 |
gcc -O3 -march=native -o program program.c

# 指定架构
gcc -O3 -mavx2 -o program program.c
gcc -O3 -mavx512f -o program program.c

# 查看向量化报告
gcc -O3 -fopt-info-vec -c program.c
gcc -O3 -fopt-info-vec-missed -c program.c  # 查看失败原因

# 仅向量化
gcc -O2 -ftree-vectorize -c program.c
```

### 5.2 MSVC

```bash
# 自动向量化 (默认开启于/O2)
cl /O2 /arch:AVX2 program.c

# 查看向量化报告
cl /O2 /Qvec-report:2 program.c
```

---

## 6. 向量化失败常见原因

### 6.1 无法确定循环次数

```c
// 可能失败
void unknown_trip_count(int* a, int n) {
    for (int i = 0; i < n; i++) {
        a[i] *= 2;
    }
}

// 帮助编译器
void known_trip_count(int* a) {
    // 固定大小数组
    for (int i = 0; i < 1024; i++) {
        a[i] *= 2;
    }
}
```

### 6.2 复杂控制流

```c
// 难以向量化
void complex_control(int* a, int n) {
    for (int i = 0; i < n; i++) {
        if (a[i] < 0) continue;
        if (a[i] > 100) break;
        a[i] *= 2;
    }
}

// 简化版本
void simpler_control(int* a, int n) {
    for (int i = 0; i < n; i++) {
        int x = a[i];
        x = (x < 0) ? x : x * 2;  // 条件移动
        a[i] = (x > 100) ? x : x;
    }
}
```

### 6.3 数据依赖

```c
// 无法向量化 (存在依赖)
void dependency(int* a, int n) {
    for (int i = 1; i < n; i++) {
        a[i] = a[i-1] + 1;  // 依赖前一次迭代
    }
}

// 可以向量化的版本
void no_dependency(int* a, int n) {
    for (int i = 0; i < n; i++) {
        a[i] = i + 1;  // 独立计算
    }
}
```

---

## 7. 性能测试

### 7.1 基准测试框架

```c
#include <stdio.h>
#include <time.h>
#include <stdlib.h>

// 避免优化掉计算
volatile double dummy_sink;

static inline double get_time(void) {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return ts.tv_sec + ts.tv_nsec * 1e-9;
}

void benchmark_vectorization(void) {
    const int N = 10000000;
    float* a = aligned_alloc(32, N * sizeof(float));
    float* b = aligned_alloc(32, N * sizeof(float));
    float* c = aligned_alloc(32, N * sizeof(float));

    // 初始化
    for (int i = 0; i < N; i++) {
        a[i] = (float)i;
        b[i] = (float)(N - i);
    }

    // 预热缓存
    for (int i = 0; i < N; i++) {
        c[i] = a[i] + b[i];
    }

    // 测试
    double start = get_time();
    for (int iter = 0; iter < 100; iter++) {
        for (int i = 0; i < N; i++) {
            c[i] = a[i] + b[i];
        }
        dummy_sink = c[N-1];  // 防止优化
    }
    double end = get_time();

    printf("Time: %.3f ms\n", (end - start) * 10);
    printf("Throughput: %.2f GFLOPS\n",
           (double)N * 100 / ((end - start) * 1e9));

    free(a);
    free(b);
    free(c);
}
```

### 7.2 典型加速比

| 操作 | 标量 | AVX2 (8-wide) | 加速比 |
|:-----|:-----|:--------------|:-------|
| 数组加法 | 1x | 7-8x | ~8x |
| 乘法累加 | 1x | 7-8x | ~8x |
| 条件操作 | 1x | 4-6x | ~5x |
| 归约求和 | 1x | 4-6x | ~5x |

---

## 8. 参考

- **Intel Intrinsics Guide**: <https://www.intel.com/content/www/us/en/docs/intrinsics-guide/index.html>
- **ARM NEON**: <https://developer.arm.com/architectures/instruction-sets/intrinsics/>
- **GCC Vectorization**: <https://gcc.gnu.org/projects/tree-ssa/vectorization.html>

---

**最后更新**: 2026-03-24
