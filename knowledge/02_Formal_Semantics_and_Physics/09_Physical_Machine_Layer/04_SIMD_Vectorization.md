# SIMD向量化编程深度指南

> **层级定位**: 02 Formal Semantics and Physics / 09 Physical Machine Layer
> **难度级别**: L4-L5
> **预估学习时间**: 10-12 小时

---

## 目录

- [SIMD向量化编程深度指南](#simd向量化编程深度指南)
  - [目录](#目录)
  - [1. SIMD概述](#1-simd概述)
    - [1.1 什么SIMD](#11-什么simd)
    - [1.2 向量指令集演进](#12-向量指令集演进)
  - [2. x86 SIMD编程](#2-x86-simd编程)
    - [2.1 SSE/AVX内建函数](#21-sseavx内建函数)
    - [2.2 向量化循环](#22-向量化循环)
    - [2.3 水平操作与归约](#23-水平操作与归约)
  - [3. ARM NEON编程](#3-arm-neon编程)
    - [3.1 NEON基础](#31-neon基础)
    - [3.2 NEON矩阵乘法](#32-neon矩阵乘法)
  - [4. 自动向量化](#4-自动向量化)
    - [4.1 GCC/Clang自动向量化](#41-gccclang自动向量化)
    - [4.2 帮助编译器向量化](#42-帮助编译器向量化)
  - [5. 性能考量](#5-性能考量)
    - [5.1 向量化最佳实践](#51-向量化最佳实践)
    - [5.2 性能测量](#52-性能测量)
  - [关联导航](#关联导航)
    - [前置知识](#前置知识)
    - [后续延伸](#后续延伸)
    - [参考](#参考)

## 1. SIMD概述

### 1.1 什么SIMD

```
SISD (传统):                    SIMD (向量化):
┌─────────┐                     ┌─────────┐
│  数据1   │                     │ 数据1-8 │
└────┬────┘                     │ 数据2-8 │
     │                         │ 数据3-8 │
     ▼                         │ 数据4-8 │
┌─────────┐                     │ 数据5-8 │
│  运算    │                     │ 数据6-8 │
└────┬────┘                     │ 数据7-8 │
     │                         │ 数据8-8 │
     ▼                         └────┬────┘
┌─────────┐                          │
│  结果    │                          ▼
└─────────┘                     ┌─────────┐
                                │  8x运算  │
                                └────┬────┘
                                     │
                                     ▼
                                ┌─────────┐
                                │  8x结果  │
                                └─────────┘
```

### 1.2 向量指令集演进

| 指令集 | 位宽 | 寄存器 | 数据类型 | 架构 |
|:-------|:-----|:-------|:---------|:-----|
| MMX | 64-bit | 8 | int | x86 |
| SSE | 128-bit | 8/16 | float/int | x86 |
| AVX | 256-bit | 16 | float/int | x86 |
| AVX-512 | 512-bit | 32 | float/int | x86 |
| NEON | 128-bit | 32 | float/int | ARM |
| SVE | 可变 | 可变 | float/int | ARM |

---

## 2. x86 SIMD编程

### 2.1 SSE/AVX内建函数

```c
#include <immintrin.h>

// SSE: 128-bit (4个float或2个double)
__m128 vec_a = _mm_set_ps(4.0, 3.0, 2.0, 1.0);  // 倒序！
__m128 vec_b = _mm_set_ps(8.0, 7.0, 6.0, 5.0);
__m128 result = _mm_add_ps(vec_a, vec_b);       // [12,10,8,6]

// AVX: 256-bit (8个float或4个double)
__m256 avx_a = _mm256_set_ps(8.0, 7.0, 6.0, 5.0, 4.0, 3.0, 2.0, 1.0);
__m256 avx_b = _mm256_set1_ps(2.0);             // 广播
__m256 avx_res = _mm256_mul_ps(avx_a, avx_b);   // [16,14,12,10,8,6,4,2]

// 加载/存储
float data[8] __attribute__((aligned(32)));
__m256 loaded = _mm256_load_ps(data);           // 对齐加载
_mm256_store_ps(data, avx_res);                 // 对齐存储

// 未对齐加载
__m256 unaligned = _mm256_loadu_ps(data);
```

### 2.2 向量化循环

```c
// 普通循环
void saxpy_normal(float *y, const float *x, float a, int n) {
    for (int i = 0; i < n; i++) {
        y[i] = a * x[i] + y[i];
    }
}

// AVX向量化 (一次处理8个float)
void saxpy_avx(float *y, const float *x, float a, int n) {
    __m256 va = _mm256_set1_ps(a);  // 广播a到所有元素

    int i = 0;
    // 主循环：每次8个
    for (; i <= n - 8; i += 8) {
        __m256 vx = _mm256_loadu_ps(&x[i]);
        __m256 vy = _mm256_loadu_ps(&y[i]);
        vy = _mm256_add_ps(_mm256_mul_ps(va, vx), vy);
        _mm256_storeu_ps(&y[i], vy);
    }

    // 处理尾部
    for (; i < n; i++) {
        y[i] = a * x[i] + y[i];
    }
}
```

### 2.3 水平操作与归约

```c
// 水平求和（将8个float相加为1个）
float hsum_avx(__m256 v) {
    // v = [a,b,c,d,e,f,g,h]
    __m128 lo = _mm256_extractf128_ps(v, 0);  // [a,b,c,d]
    __m128 hi = _mm256_extractf128_ps(v, 1);  // [e,f,g,h]
    lo = _mm_add_ps(lo, hi);                   // [a+e,b+f,c+g,d+h]

    // 继续折叠
    __m128 shuf = _mm_movehdup_ps(lo);         // [b+f,d+h,b+f,d+h]
    lo = _mm_add_ps(lo, shuf);                 // [a+e+b+f, ..., ...]
    shuf = _mm_movehl_ps(shuf, lo);            // [c+g+d+h, ...]
    lo = _mm_add_ss(lo, shuf);                 // 最终和

    return _mm_cvtss_f32(lo);
}

// AVX-512简化版本
float hsum_avx512(__m512 v) {
    return _mm512_reduce_add_ps(v);
}
```

---

## 3. ARM NEON编程

### 3.1 NEON基础

```c
#include <arm_neon.h>

// 128-bit向量类型
float32x4_t vec4;    // 4个float
int32x4_t ivec4;     // 4个int32
float64x2_t dvec2;   // 2个double

// 加载和存储
float data[4];
float32x4_t v = vld1q_f32(data);  // 加载4个float
vst1q_f32(data, v);               // 存储

// 算术运算
float32x4_t a, b, c;
c = vaddq_f32(a, b);  // c[i] = a[i] + b[i]
c = vmulq_f32(a, b);  // c[i] = a[i] * b[i]
c = vfmaq_f32(c, a, b);  // c = c + a * b (乘累加)

// 广播
float32x4_t ones = vdupq_n_f32(1.0f);
```

### 3.2 NEON矩阵乘法

```c
// 4x4矩阵乘法向量化
void matmul_4x4_neon(float *C, const float *A, const float *B) {
    // 加载B的列
    float32x4_t b0 = vld1q_f32(B);
    float32x4_t b1 = vld1q_f32(B + 4);
    float32x4_t b2 = vld1q_f32(B + 8);
    float32x4_t b3 = vld1q_f32(B + 12);

    for (int i = 0; i < 4; i++) {
        // 加载A的行并广播每个元素
        float32x4_t a0 = vdupq_n_f32(A[i * 4 + 0]);
        float32x4_t a1 = vdupq_n_f32(A[i * 4 + 1]);
        float32x4_t a2 = vdupq_n_f32(A[i * 4 + 2]);
        float32x4_t a3 = vdupq_n_f32(A[i * 4 + 3]);

        // 计算点积
        float32x4_t c = vmulq_f32(a0, b0);
        c = vfmaq_f32(c, a1, b1);
        c = vfmaq_f32(c, a2, b2);
        c = vfmaq_f32(c, a3, b3);

        vst1q_f32(C + i * 4, c);
    }
}
```

---

## 4. 自动向量化

### 4.1 GCC/Clang自动向量化

```bash
# 编译选项
-O3                    # 最高优化级别
-ftree-vectorize       # 启用向量化（-O3默认开启）
-mavx2                 # 目标AVX2
-march=native          # 针对本地CPU
-fopt-info-vec         # 输出向量化信息
-fopt-info-vec-missed  # 输出未向量化的原因

# 示例
gcc -O3 -mavx2 -fopt-info-vec program.c
```

### 4.2 帮助编译器向量化

```c
// 1. 对齐
float data[1024] __attribute__((aligned(32)));

// 2. 使用 restrict 消除别名
void foo(float * restrict a, const float * restrict b, int n);

// 3. 循环展开提示
#pragma GCC unroll 4
for (int i = 0; i < n; i++) {
    // ...
}

// 4. 向量化断言
#pragma GCC ivdep  // 忽略数据依赖（需谨慎）
for (int i = 0; i < n; i++) {
    a[i] = a[i-1] + b[i];  // 编译器可能不敢向量化
}

// 5. OpenMP SIMD
#pragma omp simd
for (int i = 0; i < n; i++) {
    y[i] = a * x[i] + y[i];
}
```

---

## 5. 性能考量

### 5.1 向量化最佳实践

```
1. 内存对齐
   - 32字节对齐用于AVX
   - 64字节对齐用于AVX-512

2. 数据布局
   - SOA (Structure of Arrays) 优于 AOS
   - AOS: {x,y,z,x,y,z} - 难以向量化
   - SOA: {x,x,x,y,y,y,z,z,z} - 容易向量化

3. 避免水平操作
   - 水平求和代价高
   - 尽量保持垂直操作

4. 注意加载/存储模式
   - 连续访问最优
   - 步长访问次之
   - 随机访问最差
```

### 5.2 性能测量

```c
#include <time.h>

double measure_simd(void (*func)(float*, const float*, float, int),
                    float *y, const float *x, float a, int n,
                    int iterations) {
    struct timespec start, end;

    // 预热
    func(y, x, a, n);

    clock_gettime(CLOCK_MONOTONIC, &start);
    for (int i = 0; i < iterations; i++) {
        func(y, x, a, n);
    }
    clock_gettime(CLOCK_MONOTONIC, &end);

    double secs = (end.tv_sec - start.tv_sec) +
                  (end.tv_nsec - start.tv_nsec) * 1e-9;
    return secs / iterations;
}

// 计算带宽和FLOPS
void report_performance(double time_sec, int n, int flops_per_element) {
    double bytes_accessed = n * sizeof(float) * 2;  // 读+写
    double flops = n * flops_per_element;

    printf("带宽: %.2f GB/s\n", bytes_accessed / time_sec / 1e9);
    printf("FLOPS: %.2f GFLOPS\n", flops / time_sec / 1e9);
}
```

---

## 关联导航

### 前置知识

- [计算机体系结构](../README.md)
- [性能优化](../../07_Microarchitecture/03_Cache_Optimization_Practical.md)

### 后续延伸

- [GPU计算](../../../03_System_Technology_Domains/06_High_Performance_Computing/02_GPU_Computing.md)
- [矩阵运算优化](../../../07_Microarchitecture/07_Matrix_Operation_Optimization.md)

### 参考

- Intel Intrinsics Guide: <https://www.intel.com/content/www/us/en/docs/intrinsics-guide/index.html>
- ARM NEON Reference: <https://developer.arm.com/architectures/instruction-sets/simd-isas/neon>
