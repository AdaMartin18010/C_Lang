# 5G MIMO信号检测

> **标准**: 3GPP TS 38.211 | **难度**: L5 | **更新**: 2026-03-15

---

## 概述

MIMO (Multiple-Input Multiple-Output) 是5G核心技术，通过多天线提升频谱效率。

```text
MIMO系统模型:
━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
y = Hx + n

y: 接收信号向量 (Nr × 1)
H: 信道矩阵 (Nr × Nt)
x: 发送信号向量 (Nt × 1)
n: 噪声向量 (Nr × 1)
━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
```

## 检测算法

### 1. MMSE检测

最小均方误差检测器：

```c
#include <complex.h>

// 复数矩阵运算
typedef float complex cfloat;

// MMSE检测
// x_hat = (H^H * H + sigma^2 * I)^-1 * H^H * y
void mmse_detect(cfloat* H,          // 信道矩阵 (Nr x Nt)
                 cfloat* y,          // 接收信号 (Nr x 1)
                 float sigma2,       // 噪声方差
                 int Nr, int Nt,     // 天线数
                 cfloat* x_hat) {    // 输出估计 (Nt x 1)

    // 计算 H^H * H
    cfloat HH_H[Nt][Nt];
    for (int i = 0; i < Nt; i++) {
        for (int j = 0; j < Nt; j++) {
            HH_H[i][j] = 0;
            for (int k = 0; k < Nr; k++) {
                HH_H[i][j] += conjf(H[k*Nt + i]) * H[k*Nt + j];
            }
        }
    }

    // 加噪声项: + sigma^2 * I
    for (int i = 0; i < Nt; i++) {
        HH_H[i][i] += sigma2;
    }

    // 计算 H^H * y
    cfloat HH_y[Nt];
    for (int i = 0; i < Nt; i++) {
        HH_y[i] = 0;
        for (int k = 0; k < Nr; k++) {
            HH_y[i] += conjf(H[k*Nt + i]) * y[k];
        }
    }

    // 解线性方程: (H^H*H + sigma^2*I) * x_hat = H^H * y
    // 使用高斯消元或Cholesky分解
    solve_linear_system((cfloat*)HH_H, HH_y, Nt, x_hat);
}
```

### 2. ZF检测

迫零检测器：

```c
// ZF检测: x_hat = (H^H * H)^-1 * H^H * y
void zf_detect(cfloat* H, cfloat* y, int Nr, int Nt, cfloat* x_hat) {
    cfloat HH_H[Nt][Nt];

    // H^H * H
    for (int i = 0; i < Nt; i++) {
        for (int j = 0; j < Nt; j++) {
            HH_H[i][j] = 0;
            for (int k = 0; k < Nr; k++) {
                HH_H[i][j] += conjf(H[k*Nt + i]) * H[k*Nt + j];
            }
        }
    }

    // 矩阵求逆
    cfloat inv_HH_H[Nt][Nt];
    matrix_inverse((cfloat*)HH_H, (cfloat*)inv_HH_H, Nt);

    // x_hat = inv(H^H*H) * H^H * y
    cfloat HH_y[Nt];
    for (int i = 0; i < Nt; i++) {
        HH_y[i] = 0;
        for (int k = 0; k < Nr; k++) {
            HH_y[i] += conjf(H[k*Nt + i]) * y[k];
        }
    }

    for (int i = 0; i < Nt; i++) {
        x_hat[i] = 0;
        for (int j = 0; j < Nt; j++) {
            x_hat[i] += inv_HH_H[i][j] * HH_y[j];
        }
    }
}
```

### 3. SIMD优化

```c
#include <immintrin.h>

// AVX2优化的复数矩阵乘法
void matmul_complex_avx2(cfloat* A, cfloat* B, cfloat* C,
                         int M, int N, int K) {
    for (int i = 0; i < M; i++) {
        for (int j = 0; j < N; j += 4) {
            __m256 sum_real = _mm256_setzero_ps();
            __m256 sum_imag = _mm256_setzero_ps();

            for (int k = 0; k < K; k++) {
                // A[i][k] 广播
                float ar = crealf(A[i*K + k]);
                float ai = cimagf(A[i*K + k]);
                __m256 a_r = _mm256_set1_ps(ar);
                __m256 a_i = _mm256_set1_ps(ai);

                // B[k][j:j+4] 加载
                __m256 b_real = _mm256_set_ps(
                    crealf(B[k*N + j+3]), crealf(B[k*N + j+2]),
                    crealf(B[k*N + j+1]), crealf(B[k*N + j]), 0, 0, 0, 0);
                __m256 b_imag = _mm256_set_ps(
                    cimagf(B[k*N + j+3]), cimagf(B[k*N + j+2]),
                    cimagf(B[k*N + j+1]), cimagf(B[k*N + j]), 0, 0, 0, 0);

                // 复数乘法: (a+ib)*(c+id) = (ac-bd) + i(ad+bc)
                sum_real = _mm256_fmadd_ps(a_r, b_real, sum_real);
                sum_real = _mm256_fnmadd_ps(a_i, b_imag, sum_real);
                sum_imag = _mm256_fmadd_ps(a_r, b_imag, sum_imag);
                sum_imag = _mm256_fmadd_ps(a_i, b_real, sum_imag);
            }

            // 存储结果
            // ...
        }
    }
}
```

## 性能指标

| 配置 | 复杂度 | 吞吐量 | BLER性能 |
|:-----|:-------|:-------|:---------|
| 2×2 MMSE | O(8) | 高 | 接近ML |
| 4×4 MMSE | O(64) | 高 | 接近ML |
| 8×8 MMSE | O(512) | 中 | 接近ML |
| 球形译码 | 可变 | 低 | 最优 |

---

> **状态**: ✅ 已完成


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
