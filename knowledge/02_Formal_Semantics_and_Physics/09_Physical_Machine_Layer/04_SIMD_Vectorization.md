# SIMD 与向量化编程实战 (SIMD Intrinsics Programming)

> **层级定位**: 02 Formal Semantics and Physics / 09 Physical_Machine_Layer
> **主题**: x86-64 SSE/AVX 与 RISC-V V-ext intrinsics 编程
> **难度级别**: L4 分析 → L5 专家
> **预估学习时间**: 12-18 小时
> **对标资料**: Intel Intrinsics Guide / RISC-V V Spec / Agner Fog

## 概述

SIMD（Single Instruction Multiple Data，单指令多数据）是现代 CPU 最核心的并行计算机制。一条 SIMD 指令可以同时处理 4-64 个数据元素，理论加速比与数据宽度成正比。本模块从程序员视角出发，系统阐述 x86-64 SSE/AVX/AVX-512 和 RISC-V Vector 扩展的 intrinsics 编程技术，包含完整的代码示例和性能对比，对齐 Intel 官方 Intrinsics Guide 和 RISC-V V 规范。

---

## 1. SIMD 架构概览

| 扩展 | 寄存器宽度 | 典型数据类型 | 同时处理元素数 (float) |
|:-----|:-----------|:-------------|:----------------------|
| **SSE** | 128-bit | `__m128` | 4 × float32 |
| **SSE2** | 128-bit | `__m128i/d` | 2 × float64 / 16 × int8 |
| **AVX** | 256-bit | `__m256` | 8 × float32 |
| **AVX-512** | 512-bit | `__m512` | 16 × float32 |
| **RISC-V V** | 可变 (VLEN) | `vfloat32m1_t` | 取决于 VLEN (通常 4-16) |
| **ARM NEON** | 128-bit | `float32x4_t` | 4 × float32 |

### SIMD 寄存器文件 (x86-64)

```
XMM0-XMM15:  128-bit (SSE)    — 16 个寄存器
YMM0-YMM15:  256-bit (AVX)    — 低 128-bit 与 XMM 重叠
ZMM0-ZMM31:  512-bit (AVX-512) — 扩展至 32 个寄存器 (x86-64)
```

---

## 2. x86-64 AVX2 Intrinsics 编程

### 头文件与编译

```c
#include <immintrin.h>    // AVX2 + AVX-512

// 编译选项
gcc -O3 -mavx2 -mfma program.c -o program    # AVX2
 gcc -O3 -mavx512f -mavx512vl program.c      # AVX-512
```

### 基础数据类型

```c
__m256   a;   // 8 × float32
__m256d  b;   // 4 × float64
__m256i  c;   // 整数向量: 32 × int8, 16 × int16, 8 × int32, 4 × int64
```

### 核心操作示例：向量加法

```c
#include <immintrin.h>
#include <stdio.h>

void vec_add_avx2(const float *a, const float *b, float *out, int n) {
    // 每次处理 8 个 float (256-bit / 32-bit)
    int i = 0;
    for (; i + 8 <= n; i += 8) {
        __m256 va = _mm256_loadu_ps(&a[i]);   // 非对齐加载
        __m256 vb = _mm256_loadu_ps(&b[i]);
        __m256 vc = _mm256_add_ps(va, vb);    // 8 个并行加法
        _mm256_storeu_ps(&out[i], vc);
    }
    // 处理尾部不足 8 个的元素
    for (; i < n; i++)
        out[i] = a[i] + b[i];
}
```

### 点积计算 (FMA 优化)

```c
float dot_product_avx2(const float *a, const float *b, int n) {
    __m256 sum = _mm256_setzero_ps();
    int i = 0;
    for (; i + 8 <= n; i += 8) {
        __m256 va = _mm256_loadu_ps(&a[i]);
        __m256 vb = _mm256_loadu_ps(&b[i]);
        sum = _mm256_fmadd_ps(va, vb, sum);   // 乘加融合: a*b + sum
    }
    // 水平求和
    float result[8];
    _mm256_storeu_ps(result, sum);
    float total = 0;
    for (int j = 0; j < 8; j++) total += result[j];
    for (; i < n; i++) total += a[i] * b[i];
    return total;
}
```

### 矩阵乘法 (分块 + AVX2)

```c
void matmul_avx2(const float *A, const float *B, float *C,
                 int M, int N, int K) {
    for (int i = 0; i < M; i++) {
        for (int j = 0; j < N; j += 8) {
            __m256 cvec = _mm256_setzero_ps();
            for (int k = 0; k < K; k++) {
                __m256 a_broadcast = _mm256_set1_ps(A[i*K + k]);
                __m256 bvec = _mm256_loadu_ps(&B[k*N + j]);
                cvec = _mm256_fmadd_ps(a_broadcast, bvec, cvec);
            }
            _mm256_storeu_ps(&C[i*N + j], cvec);
        }
    }
}
```

---

## 3. RISC-V Vector (RVV) Intrinsics

### RISC-V V 的核心特性

- **可变向量长度 (VLEN)**: 硬件定义，软件不可见
- **向量长度寄存器 (vl)**: 运行时设置实际处理元素数
- **无固定向量宽度**: 同一代码可在 128-bit 到 4096-bit 向量处理器上运行

### 头文件与编译

```c
#include <riscv_vector.h>

// 编译
gcc -O3 -march=rv64gcv -o program program.c
```

### 向量加法 (RISC-V V)

```c
void vec_add_rvv(const float *a, const float *b, float *out, size_t n) {
    size_t i = 0;
    while (i < n) {
        // 设置向量长度: 处理 min(n-i, 硬件最大) 个元素
        size_t vl = __riscv_vsetvl_e32m8(n - i);

        vfloat32m8_t va = __riscv_vle32_v_f32m8(&a[i], vl);
        vfloat32m8_t vb = __riscv_vle32_v_f32m8(&b[i], vl);
        vfloat32m8_t vc = __riscv_vfadd_vv_f32m8(va, vb, vl);

        __riscv_vse32_v_f32m8(&out[i], vc, vl);
        i += vl;
    }
}
```

### 点积 (RISC-V V with widening)

```c
float dot_product_rvv(const float *a, const float *b, size_t n) {
    size_t vlmax = __riscv_vsetvlmax_e32m1();
    vfloat32m1_t vsum = __riscv_vfmv_v_f_f32m1(0.0f, vlmax);

    size_t i = 0;
    while (i < n) {
        size_t vl = __riscv_vsetvl_e32m8(n - i);
        vfloat32m8_t va = __riscv_vle32_v_f32m8(&a[i], vl);
        vfloat32m8_t vb = __riscv_vle32_v_f32m8(&b[i], vl);

        // 部分点积
        vfloat32m1_t vred = __riscv_vfredusum_vs_f32m8_f32m1(
            __riscv_vfmul_vv_f32m8(va, vb, vl), vsum, vl);
        vsum = vred;
        i += vl;
    }

    float result;
    __riscv_vse32_v_f32m1(&result, vsum, 1);
    return result;
}
```

---

## 4. 性能对比

### 环境设置

```bash
# 检查 CPU SIMD 支持
gcc -dM -E - < /dev/null | grep -E "(SSE|AVX|RISCV)"
cat /proc/cpuinfo | grep flags | head -1
```

### 典型加速比 (理论 vs 实际)

| 操作 | Scalar | SSE (4-wide) | AVX2 (8-wide) | AVX-512 (16-wide) | 实际加速比 |
|:-----|:-------|:-------------|:--------------|:------------------|:-----------|
| float 加法 | 1× | 4× | 8× | 16× | 3-7× (AVX2) |
| float 乘加 (FMA) | 1× | 4× | 8× | 16× | 4-12× (AVX2) |
| int8 点积 | 1× | 16× | 32× | 64× | 8-20× (AVX2) |
| 矩阵乘法 | 1× | ~3× | ~6× | ~10× | 4-8× (AVX2) |

> **注意**: 实际加速比受内存带宽、缓存效率、数据依赖和指令发射限制影响，通常远低于理论峰值。

---

## 5. 关键优化技巧

### 5.1 内存对齐

```c
// ❌ 非对齐加载可能慢 2-10 倍
__m256 a = _mm256_loadu_ps(ptr);   // unaligned

// ✅ 对齐加载（需要 32-byte 对齐）
float *ptr alignas(32) = ...;
__m256 a = _mm256_load_ps(ptr);    // aligned
```

### 5.2 避免 SIMD ↔ Scalar 混用

```c
// ❌ 频繁在 SIMD 和标量间切换
for (...) {
    __m256 v = _mm256_loadu_ps(p);
    v = _mm256_add_ps(v, ...);
    if (v[0] > threshold) ...   // 提取标量 → 性能杀手
}
```

### 5.3 循环展开 + 多累加器

```c
// ✅ 多累加器隐藏延迟
__m256 sum0 = _mm256_setzero_ps();
__m256 sum1 = _mm256_setzero_ps();
for (int i = 0; i < n; i += 16) {
    sum0 = _mm256_fmadd_ps(_mm256_loadu_ps(&a[i]), _mm256_loadu_ps(&b[i]), sum0);
    sum1 = _mm256_fmadd_ps(_mm256_loadu_ps(&a[i+8]), _mm256_loadu_ps(&b[i+8]), sum1);
}
sum0 = _mm256_add_ps(sum0, sum1);
```

---

## 6. 权威参考

- **Intel Intrinsics Guide**: <https://www.intel.com/content/www/us/en/docs/intrinsics-guide/index.html>
- **RISC-V Vector Spec**: <https://github.com/riscv/riscv-v-spec>
- **RISC-V C Intrinsics**: <https://github.com/riscv/riscv-c-api-doc>
- **Agner Fog's Manual 1**: Instruction tables — 每条指令的延迟和吞吐
- **"Modern Parallel Programming"** (Intel): SIMD 优化最佳实践

## 待补充内容

- [ ] AVX-512 掩码寄存器编程与向量化循环
- [ ] ARM NEON/SVE intrinsics 对比
- [ ] 自动向量化：如何编写编译器友好代码
- [ ] 性能剖析：使用 Intel VTune 分析 SIMD 效率

---

*最后更新: 2026-05-10*
