# C语言矩阵运算与BLAS

> **目标**: 掌握C语言矩阵运算优化，使用BLAS加速
> **技术栈**: OpenBLAS, Intel MKL, 纯C实现
> **难度**: L3 | **预估时间**: 2-3小时

---

## 1. 基础矩阵结构

```c
#include <stdlib.h>
#include <string.h>
#include <math.h>

// 矩阵结构
typedef struct {
    int rows;
    int cols;
    double* data;  // 行优先存储
} Matrix;

// 创建矩阵
Matrix* mat_create(int rows, int cols) {
    Matrix* m = malloc(sizeof(Matrix));
    m->rows = rows;
    m->cols = cols;
    m->data = calloc(rows * cols, sizeof(double));
    return m;
}

// 释放矩阵
void mat_free(Matrix* m) {
    free(m->data);
    free(m);
}

// 获取元素
static inline double mat_get(const Matrix* m, int row, int col) {
    return m->data[row * m->cols + col];
}

// 设置元素
static inline void mat_set(Matrix* m, int row, int col, double val) {
    m->data[row * m->cols + col] = val;
}
```

---

## 2. 基础矩阵运算

```c
// 矩阵加法: C = A + B
void mat_add(const Matrix* A, const Matrix* B, Matrix* C) {
    int size = A->rows * A->cols;
    for (int i = 0; i < size; i++) {
        C->data[i] = A->data[i] + B->data[i];
    }
}

// 矩阵逐元素乘法(Hadamard积): C = A ⊙ B
void mat_hadamard(const Matrix* A, const Matrix* B, Matrix* C) {
    int size = A->rows * A->cols;
    for (int i = 0; i < size; i++) {
        C->data[i] = A->data[i] * B->data[i];
    }
}

// 标量乘法: B = a * A
void mat_scale(const Matrix* A, double scalar, Matrix* B) {
    int size = A->rows * A->cols;
    for (int i = 0; i < size; i++) {
        B->data[i] = A->data[i] * scalar;
    }
}

// 矩阵转置
void mat_transpose(const Matrix* A, Matrix* T) {
    for (int i = 0; i < A->rows; i++) {
        for (int j = 0; j < A->cols; j++) {
            mat_set(T, j, i, mat_get(A, i, j));
        }
    }
}
```

---

## 3. 矩阵乘法优化

### 3.1 基础实现

```c
// 基础矩阵乘法: C = A × B
void mat_mult_basic(const Matrix* A, const Matrix* B, Matrix* C) {
    int M = A->rows;
    int N = B->cols;
    int K = A->cols;

    memset(C->data, 0, M * N * sizeof(double));

    for (int i = 0; i < M; i++) {
        for (int k = 0; k < K; k++) {
            double a = mat_get(A, i, k);
            for (int j = 0; j < N; j++) {
                C->data[i * N + j] += a * mat_get(B, k, j);
            }
        }
    }
}
```

### 3.2 缓存优化(分块)

```c
#define BLOCK_SIZE 64

void mat_mult_blocked(const Matrix* A, const Matrix* B, Matrix* C) {
    int M = A->rows;
    int N = B->cols;
    int K = A->cols;

    memset(C->data, 0, M * N * sizeof(double));

    for (int i0 = 0; i0 < M; i0 += BLOCK_SIZE) {
        for (int j0 = 0; j0 < N; j0 += BLOCK_SIZE) {
            for (int k0 = 0; k0 < K; k0 += BLOCK_SIZE) {
                // 处理块
                int imax = (i0 + BLOCK_SIZE < M) ? i0 + BLOCK_SIZE : M;
                int jmax = (j0 + BLOCK_SIZE < N) ? j0 + BLOCK_SIZE : N;
                int kmax = (k0 + BLOCK_SIZE < K) ? k0 + BLOCK_SIZE : K;

                for (int i = i0; i < imax; i++) {
                    for (int k = k0; k < kmax; k++) {
                        double a = mat_get(A, i, k);
                        for (int j = j0; j < jmax; j++) {
                            C->data[i * N + j] += a * mat_get(B, k, j);
                        }
                    }
                }
            }
        }
    }
}
```

### 3.3 SIMD优化

```c
#include <immintrin.h>

// AVX2优化的矩阵乘法
void mat_mult_avx2(const Matrix* A, const Matrix* B, Matrix* C) {
    int M = A->rows;
    int N = B->cols;
    int K = A->cols;

    memset(C->data, 0, M * N * sizeof(double));

    for (int i = 0; i < M; i++) {
        for (int k = 0; k < K; k++) {
            double a = mat_get(A, i, k);
            __m256d a_vec = _mm256_set1_pd(a);

            int j = 0;
            // 每次处理4个元素
            for (; j <= N - 4; j += 4) {
                __m256d b_vec = _mm256_loadu_pd(&B->data[k * N + j]);
                __m256d c_vec = _mm256_loadu_pd(&C->data[i * N + j]);
                c_vec = _mm256_fmadd_pd(a_vec, b_vec, c_vec);
                _mm256_storeu_pd(&C->data[i * N + j], c_vec);
            }

            // 处理剩余元素
            for (; j < N; j++) {
                C->data[i * N + j] += a * mat_get(B, k, j);
            }
        }
    }
}
```

---

## 4. OpenBLAS使用

### 4.1 安装与配置

```bash
# Ubuntu/Debian
sudo apt-get install libopenblas-dev

# macOS
brew install openblas

# 编译时链接
gcc -o program program.c -lopenblas -lpthread
```

### 4.2 基本使用

```c
#include <cblas.h>

// 矩阵乘法使用OpenBLAS
void mat_mult_blas(const Matrix* A, const Matrix* B, Matrix* C) {
    // C = alpha * A * B + beta * C
    double alpha = 1.0;
    double beta = 0.0;

    cblas_dgemm(CblasRowMajor,    // 存储方式
                CblasNoTrans,     // A不转置
                CblasNoTrans,     // B不转置
                A->rows,          // M
                B->cols,          // N
                A->cols,          // K
                alpha,
                A->data,          // A
                A->cols,          // lda
                B->data,          // B
                B->cols,          // ldb
                beta,
                C->data,          // C
                C->cols);         // ldc
}
```

### 4.3 批量矩阵乘法

```c
// 批量矩阵乘法(用于神经网络批量处理)
void batch_mat_mult_blas(const Matrix** A, const Matrix** B,
                         Matrix** C, int batch_size) {
    for (int i = 0; i < batch_size; i++) {
        mat_mult_blas(A[i], B[i], C[i]);
    }
}
```

---

## 5. Intel MKL使用

```c
#ifdef USE_MKL
#include <mkl.h>

// MKL矩阵乘法
void mat_mult_mkl(const Matrix* A, const Matrix* B, Matrix* C) {
    double alpha = 1.0;
    double beta = 0.0;

    cblas_dgemm(CblasRowMajor,
                CblasNoTrans,
                CblasNoTrans,
                A->rows,
                B->cols,
                A->cols,
                alpha,
                A->data,
                A->cols,
                B->data,
                B->cols,
                beta,
                C->data,
                C->cols);
}

// MKL内存对齐分配
Matrix* mat_create_aligned(int rows, int cols) {
    Matrix* m = malloc(sizeof(Matrix));
    m->rows = rows;
    m->cols = cols;
    m->data = mkl_malloc(rows * cols * sizeof(double), 64);
    memset(m->data, 0, rows * cols * sizeof(double));
    return m;
}

void mat_free_aligned(Matrix* m) {
    mkl_free(m->data);
    free(m);
}
#endif
```

---

## 6. 性能对比

```c
#include <time.h>

void benchmark() {
    int sizes[] = {128, 256, 512, 1024, 2048};
    int n_sizes = sizeof(sizes) / sizeof(sizes[0]);

    printf("Matrix Multiplication Benchmark\n");
    printf("Size\t\tBasic\t\tBlocked\t\tAVX2\t\tOpenBLAS\n");
    printf("----------------------------------------------------------------\n");

    for (int s = 0; s < n_sizes; s++) {
        int n = sizes[s];
        Matrix* A = mat_create(n, n);
        Matrix* B = mat_create(n, n);
        Matrix* C = mat_create(n, n);

        // 初始化随机数据
        for (int i = 0; i < n * n; i++) {
            A->data[i] = (double)rand() / RAND_MAX;
            B->data[i] = (double)rand() / RAND_MAX;
        }

        // 测试各种实现
        clock_t start, end;
        double t_basic = 0, t_blocked = 0, t_avx2 = 0, t_blas = 0;

        // Basic
        start = clock();
        mat_mult_basic(A, B, C);
        end = clock();
        t_basic = (double)(end - start) / CLOCKS_PER_SEC;

        // Blocked
        start = clock();
        mat_mult_blocked(A, B, C);
        end = clock();
        t_blocked = (double)(end - start) / CLOCKS_PER_SEC;

        // AVX2
        start = clock();
        mat_mult_avx2(A, B, C);
        end = clock();
        t_avx2 = (double)(end - start) / CLOCKS_PER_SEC;

        // OpenBLAS
        start = clock();
        mat_mult_blas(A, B, C);
        end = clock();
        t_blas = (double)(end - start) / CLOCKS_PER_SEC;

        printf("%dx%d\t\t%.3fs\t\t%.3fs\t\t%.3fs\t\t%.3fs\n",
               n, n, t_basic, t_blocked, t_avx2, t_blas);

        mat_free(A);
        mat_free(B);
        mat_free(C);
    }
}
```

---

## 7. 实用工具函数

```c
// 随机初始化
void mat_random_init(Matrix* m) {
    int size = m->rows * m->cols;
    for (int i = 0; i < size; i++) {
        m->data[i] = ((double)rand() / RAND_MAX - 0.5) * 2.0;
    }
}

// Xavier初始化(适用于神经网络)
void mat_xavier_init(Matrix* m) {
    double scale = sqrt(6.0 / (m->rows + m->cols));
    int size = m->rows * m->cols;
    for (int i = 0; i < size; i++) {
        m->data[i] = ((double)rand() / RAND_MAX - 0.5) * 2.0 * scale;
    }
}

// He初始化(ReLU激活函数)
void mat_he_init(Matrix* m) {
    double scale = sqrt(2.0 / m->cols);
    int size = m->rows * m->cols;
    for (int i = 0; i < size; i++) {
        m->data[i] = ((double)rand() / RAND_MAX) * scale;
    }
}

// 打印矩阵
void mat_print(const Matrix* m, const char* name) {
    printf("%s (%dx%d):\n", name, m->rows, m->cols);
    for (int i = 0; i < m->rows && i < 6; i++) {
        for (int j = 0; j < m->cols && j < 6; j++) {
            printf("%8.4f ", mat_get(m, i, j));
        }
        if (m->cols > 6) printf("...");
        printf("\n");
    }
    if (m->rows > 6) printf("...\n");
}
```

---

> **核心洞察**: 矩阵乘法是深度学习的计算瓶颈。使用OpenBLAS/MKL可获得10-100倍加速，而SIMD优化可在不依赖外部库的情况下获得3-5倍提升。

---

*最后更新: 2026-03-14*


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
