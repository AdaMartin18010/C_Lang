# C语言循环优化深度解析

> **编译器**: GCC 12+ / Clang 15+
> **优化级别**: -O2, -O3
> **相关选项**: -funroll-loops, -floop-nest-optimize

---

## 1. 循环优化概述

循环是程序中最耗时的部分，编译器通过各种技术提升循环性能：

```
循环不变量外提 (LICM)
循环展开 (Unrolling)
循环向量化 (Vectorization)
强度削减 (Strength Reduction)
归纳变量优化 (Induction Variables)
```

---

## 2. 循环不变量外提 (LICM)

### 2.1 基本原理

将循环内不依赖于循环变量的计算移到循环外。

```c
// 优化前
for (int i = 0; i < n; i++) {
    int x = a * b;      // 每次循环都计算相同值
    arr[i] = x + i;
}

// 优化后 (编译器自动)
int x = a * b;          // 外提
for (int i = 0; i < n; i++) {
    arr[i] = x + i;
}
```

### 2.2 数组索引优化

```c
// 优化前
for (int i = 0; i < n; i++) {
    arr[i * stride + offset] = i;  // 每次计算地址
}

// 优化后
int* ptr = arr + offset;           // 计算基地址
for (int i = 0; i < n; i++) {
    ptr[i * stride] = i;
}

// 进一步优化 (stride=1时)
int* ptr = arr + offset;
for (int i = 0; i < n; i++) {
    ptr[i] = i;                    // 直接使用索引
}
```

### 2.3 函数调用外提

```c
// 优化前
for (int i = 0; i < n; i++) {
    int len = strlen(str);         // 每次调用strlen
    arr[i] = len + i;
}

// 优化后
int len = strlen(str);             // 外提
for (int i = 0; i < n; i++) {
    arr[i] = len + i;
}
```

---

## 3. 循环展开 (Loop Unrolling)

### 3.1 完全展开

```c
// 原始循环
for (int i = 0; i < 4; i++) {
    sum += arr[i];
}

// 完全展开后
sum += arr[0];
sum += arr[1];
sum += arr[2];
sum += arr[3];
```

### 3.2 部分展开

```c
// 原始循环
for (int i = 0; i < n; i++) {
    arr[i] = i * 2;
}

// 展开2次
for (int i = 0; i < n; i += 2) {
    arr[i] = i * 2;
    arr[i+1] = (i+1) * 2;  // 注意边界检查
}

// 编译器实际生成 (展开4次)
for (int i = 0; i < n - 3; i += 4) {  // 主体
    arr[i] = i * 2;
    arr[i+1] = (i+1) * 2;
    arr[i+2] = (i+2) * 2;
    arr[i+3] = (i+3) * 2;
}
for (; i < n; i++) {                   // 余数处理
    arr[i] = i * 2;
}
```

### 3.3 展开的收益

| 优势 | 说明 |
|:-----|:-----|
| 减少循环开销 | 每次迭代减少分支判断 |
| 增加指令级并行 | 更多独立指令可同时执行 |
| 更好的寄存器利用 | 多个值可同时驻留寄存器 |
| 向量化机会 | 更容易形成向量操作 |

### 3.4 手动控制展开

```c
// GCC/Clang pragma
#pragma unroll 4
for (int i = 0; i < n; i++) {
    arr[i] = i * 2;
}

// 或完全展开
#pragma unroll
for (int i = 0; i < 8; i++) {  // 小循环完全展开
    arr[i] = i * 2;
}
```

---

## 4. 强度削减 (Strength Reduction)

### 4.1 乘法转加法

```c
// 优化前
for (int i = 0; i < n; i++) {
    arr[i] = i * 5;  // 每次乘法
}

// 优化后
int val = 0;
for (int i = 0; i < n; i++) {
    arr[i] = val;
    val += 5;        // 加法代替乘法
}
```

### 4.2 数组索引优化

```c
// 优化前
for (int i = 0; i < n; i++) {
    process(&arr[i * 8]);  // 乘法计算偏移
}

// 优化后
int* ptr = arr;
for (int i = 0; i < n; i++) {
    process(ptr);
    ptr += 8;              // 指针加法
}
```

---

## 5. 归纳变量优化

### 5.1 多个归纳变量合并

```c
// 优化前
for (int i = 0; i < n; i++) {
    arr[i] = i * 2 + j * 3;  // 两个归纳变量
}

// 优化后
int val = j * 3;
for (int i = 0; i < n; i++) {
    arr[i] = val;
    val += 2;                  // 单归纳变量
}
```

### 5.2 指针与索引

```c
// 编译器通常将索引优化为指针
// 源代码
for (int i = 0; i < n; i++) {
    arr[i] = 0;
}

// 生成的汇编类似
// ptr = arr
// end = arr + n
// while (ptr < end) {
//     *ptr = 0
//     ptr++
// }
```

---

## 6. 循环交换 (Loop Interchange)

### 6.1 缓存优化

```c
// 优化前 (缓存不友好)
for (int j = 0; j < n; j++) {
    for (int i = 0; i < n; i++) {
        arr[i][j]++;  // 列访问，跳跃大
    }
}

// 优化后 (缓存友好)
for (int i = 0; i < n; i++) {
    for (int j = 0; j < n; j++) {
        arr[i][j]++;  // 行访问，连续
    }
}
```

### 6.2 分块 (Tiling)

```c
// 矩阵乘法分块优化
void matmul_blocked(int n, double A[n][n], double B[n][n], double C[n][n]) {
    const int BLOCK = 32;  // 适合L1缓存大小

    for (int ii = 0; ii < n; ii += BLOCK) {
        for (int jj = 0; jj < n; jj += BLOCK) {
            for (int kk = 0; kk < n; kk += BLOCK) {
                // 块内计算
                for (int i = ii; i < ii + BLOCK && i < n; i++) {
                    for (int j = jj; j < jj + BLOCK && j < n; j++) {
                        for (int k = kk; k < kk + BLOCK && k < n; k++) {
                            C[i][j] += A[i][k] * B[k][j];
                        }
                    }
                }
            }
        }
    }
}
```

---

## 7. 查看编译器优化

### 7.1 GCC优化报告

```bash
# 查看循环优化
---

## 🔗 文档关联

### 核心关联
| 文档 | 关系类型 | 说明 |
|:-----|:---------|:-----|
| [内存管理](../../../../01_Core_Knowledge_System/02_Core_Layer/02_Memory_Management.md) | 核心关联 | 内存管理基础 |
| [指针深度](../../../../01_Core_Knowledge_System/02_Core_Layer/01_Pointer_Depth.md) | 核心关联 | 指针深度基础 |
| [并发编程](../../../../03_System_Technology_Domains/14_Concurrency_Parallelism/README.md) | 核心关联 | 并发编程基础 |
| [数据类型](../../../../01_Core_Knowledge_System/01_Basic_Layer/02_Data_Type_System.md) | 核心关联 | 数据类型基础 |
| [数组与指针](../../../../01_Core_Knowledge_System/02_Core_Layer/05_Arrays_Pointers.md) | 核心关联 | 数组与指针基础 |

### 扩展阅读
| 文档 | 关系类型 | 说明 |
|:-----|:---------|:-----|
| [软件工程](../../../../01_Core_Knowledge_System/05_Engineering_Layer/README.md) | 核心关联 | 软件工程基础 |
| [形式语义](../../../../02_Formal_Semantics_and_Physics/README.md) | 核心关联 | 形式语义基础 |
| [系统技术](../../../../03_System_Technology_Domains/README.md) | 核心关联 | 系统技术基础 |
| [工业场景](../../../../04_Industrial_Scenarios/README.md) | 核心关联 | 工业场景基础 |
| [思维表征](../../../../06_Thinking_Representation/README.md) | 核心关联 | 思维表征基础 |
gcc -O3 -fopt-info-vec -c loop.c

# 详细优化报告
gcc -O3 -fopt-info-all-optall -c loop.c 2>&1 | head -50
```

### 7.2 汇编对比

```bash
# 无优化
gcc -O0 -S loop.c -o loop_O0.s

# 优化后
gcc -O3 -S loop.c -o loop_O3.s

# 对比
diff loop_O0.s loop_O3.s
```

### 7.3 使用Godbolt Compiler Explorer

```c
// 示例代码用于查看优化
void sum_array(int* arr, int n, long* result) {
    long sum = 0;
    for (int i = 0; i < n; i++) {
        sum += arr[i];
    }
    *result = sum;
}
```

---

## 8. 最佳实践

### 8.1 帮助编译器优化

```c
// 1. 使用restrict关键字
void process(int* restrict out, const int* restrict in, int n) {
    for (int i = 0; i < n; i++) {
        out[i] = in[i] * 2;
    }
}

// 2. 数组大小显式
typedef struct {
    int data[64];  // 固定大小，便于展开
} FixedArray;

// 3. 循环次数常量
#define UNROLL_FACTOR 4

// 4. 避免循环内分支
for (int i = 0; i < n; i++) {
    // 不推荐: 循环内有条件
    if (i % 2 == 0) {
        arr[i] = 0;
    } else {
        arr[i] = 1;
    }
}

// 推荐: 分开循环或向量化
```

### 8.2 性能测试

```c
#include <time.h>
#include <stdio.h>

// 计时宏
#define TIME_LOOP(name, code) do { \
    clock_t start = clock(); \
    code; \
    clock_t end = clock(); \
    printf("%s: %f ms\n", name, 1000.0 * (end - start) / CLOCKS_PER_SEC); \
} while(0)

// 测试不同优化
void benchmark(void) {
    int arr[1000000];

    TIME_LOOP("Loop A", {
        for (int i = 0; i < 1000000; i++) {
            arr[i] = i * 2;
        }
    });
}
```

---

## 9. 参考

- **GCC文档**: <https://gcc.gnu.org/onlinedocs/gcc/Optimize-Options.html>
- **LLVM文档**: <https://llvm.org/docs/Vectorizers.html>
- **Agner Fog**: <https://agner.org/optimize/>

---

**最后更新**: 2026-03-24
