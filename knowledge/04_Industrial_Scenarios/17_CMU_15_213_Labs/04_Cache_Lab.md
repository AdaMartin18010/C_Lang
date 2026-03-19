# Lab 4: Cache Lab - 缓存模拟与优化实验

## 1. 实验目标

### 1.1 总体目标

Cache Lab旨在加深对缓存存储器层次结构的理解，包括缓存的组织、访问模式和性能优化。

| 目标 | 描述 |
|------|------|
| **缓存模拟** | 实现一个缓存模拟器 |
| **缓存分析** | 分析不同访问模式的缓存行为 |
| **性能优化** | 优化矩阵转置以提高缓存命中率 |
| **空间-时间权衡** | 理解局部性原理 |

### 1.2 实验组成

```
Part A: Cache Simulator
  - 实现一个LRU缓存模拟器
  - 解析valgrind内存访问轨迹
  - 计算命中、未命中和驱逐次数

Part B: Matrix Transpose Optimization
  - 优化矩阵转置函数
  - 最小化缓存未命中率
  - 针对不同缓存参数优化
```

### 1.3 评分标准

| 部分 | 任务 | 分值 |
|------|------|------|
| Part A | 缓存模拟器正确性 | 27分 |
| Part B | 32x32矩阵 | 8分 |
| Part B | 64x64矩阵 | 8分 |
| Part B | 61x67矩阵 | 10分 |

---

## 2. 背景知识

### 2.1 缓存基础

```
┌─────────────────────────────────────────┐
│              缓存组织结构                │
├─────────────────────────────────────────┤
│  地址划分: [标记][组索引][块偏移]        │
│                                         │
│  ┌─────────┐    ┌─────────┐    ┌─────┐ │
│  │  Tag    │    │  Set    │    │Block│ │
│  │ (t位)   │    │ (s位)   │    │(b位)│ │
│  └─────────┘    └─────────┘    └─────┘ │
└─────────────────────────────────────────┘

缓存参数:
- S = 2^s 组数
- E = 每组行数（关联度）
- B = 2^b 块大小（字节）
- m = 地址位数
```

### 2.2 缓存访问类型

```c
// 缓存命中 (Hit)
// 数据在缓存中，直接返回

// 缓存未命中 (Miss)
// - Cold Miss: 首次访问，缓存中不存在
// - Conflict Miss: 不同数据映射到同一缓存行
// - Capacity Miss: 缓存容量不足

// 驱逐 (Eviction)
// 缓存满时需要替换旧行（LRU策略）
```

### 2.3 局部性原理

```c
// 时间局部性：近期访问的数据很可能再次被访问
for (int i = 0; i < n; i++) {
    sum += a[0];  // 重复访问同一元素
}

// 空间局部性：访问某地址后，附近地址很可能被访问
for (int i = 0; i < n; i++) {
    sum += a[i];  // 顺序访问相邻元素
}
```

---

## 3. Part A: 缓存模拟器实现

### 3.1 数据结构

```c
// csim.c - 缓存模拟器
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>

// 缓存行结构
typedef struct {
    bool valid;      // 有效位
    uint64_t tag;    // 标记
    int lru_counter; // LRU计数器
} cache_line_t;

// 缓存组结构
typedef struct {
    cache_line_t *lines;  // 行数组
} cache_set_t;

// 缓存结构
typedef struct {
    cache_set_t *sets;    // 组数组
    int s, E, b;          // 参数
    int S, B;             // 计算值
    int hit_count;
    int miss_count;
    int eviction_count;
    int global_counter;   // 全局LRU计数器
} cache_t;
```

### 3.2 缓存初始化

```c
// 初始化缓存
cache_t* init_cache(int s, int E, int b) {
    cache_t *cache = malloc(sizeof(cache_t));
    cache->s = s;
    cache->E = E;
    cache->b = b;
    cache->S = 1 << s;    // 2^s
    cache->B = 1 << b;    // 2^b
    cache->hit_count = 0;
    cache->miss_count = 0;
    cache->eviction_count = 0;
    cache->global_counter = 0;

    // 分配组
    cache->sets = malloc(cache->S * sizeof(cache_set_t));
    for (int i = 0; i < cache->S; i++) {
        cache->sets[i].lines = malloc(E * sizeof(cache_line_t));
        for (int j = 0; j < E; j++) {
            cache->sets[i].lines[j].valid = false;
            cache->sets[i].lines[j].tag = 0;
            cache->sets[i].lines[j].lru_counter = 0;
        }
    }
    return cache;
}
```

### 3.3 地址解析与访问

```c
// 解析地址，提取tag和set index
void parse_address(cache_t *cache, uint64_t addr,
                   uint64_t *tag, int *set_idx) {
    int b = cache->b;
    int s = cache->s;

    *set_idx = (addr >> b) & ((1 << s) - 1);
    *tag = addr >> (b + s);
}

// 访问缓存
void access_cache(cache_t *cache, uint64_t addr, char op) {
    uint64_t tag;
    int set_idx;
    parse_address(cache, addr, &tag, &set_idx);

    cache_set_t *set = &cache->sets[set_idx];
    cache->global_counter++;

    // 检查是否命中
    for (int i = 0; i < cache->E; i++) {
        if (set->lines[i].valid && set->lines[i].tag == tag) {
            // 命中
            cache->hit_count++;
            set->lines[i].lru_counter = cache->global_counter;

            // 如果是M操作，额外一次命中
            if (op == 'M') {
                cache->hit_count++;
            }
            return;
        }
    }

    // 未命中
    cache->miss_count++;

    // 查找空行
    for (int i = 0; i < cache->E; i++) {
        if (!set->lines[i].valid) {
            // 使用空行
            set->lines[i].valid = true;
            set->lines[i].tag = tag;
            set->lines[i].lru_counter = cache->global_counter;

            if (op == 'M') {
                cache->hit_count++;
            }
            return;
        }
    }

    // 需要驱逐
    cache->eviction_count++;

    // 找到LRU行
    int lru_idx = 0;
    int min_counter = set->lines[0].lru_counter;
    for (int i = 1; i < cache->E; i++) {
        if (set->lines[i].lru_counter < min_counter) {
            min_counter = set->lines[i].lru_counter;
            lru_idx = i;
        }
    }

    // 替换LRU行
    set->lines[lru_idx].tag = tag;
    set->lines[lru_idx].lru_counter = cache->global_counter;

    if (op == 'M') {
        cache->hit_count++;
    }
}
```

### 3.4 主函数与轨迹解析

```c
#include <getopt.h>

void print_usage() {
    printf("Usage: ./csim [-hv] -s <s> -E <E> -b <b> -t <tracefile>\n");
}

int main(int argc, char *argv[]) {
    int s = 0, E = 0, b = 0;
    char *tracefile = NULL;
    bool verbose = false;

    int opt;
    while ((opt = getopt(argc, argv, "hvs:E:b:t:")) != -1) {
        switch (opt) {
            case 'h':
                print_usage();
                return 0;
            case 'v':
                verbose = true;
                break;
            case 's':
                s = atoi(optarg);
                break;
            case 'E':
                E = atoi(optarg);
                break;
            case 'b':
                b = atoi(optarg);
                break;
            case 't':
                tracefile = optarg;
                break;
            default:
                print_usage();
                return 1;
        }
    }

    if (s == 0 || E == 0 || b == 0 || tracefile == NULL) {
        print_usage();
        return 1;
    }

    cache_t *cache = init_cache(s, E, b);

    // 读取轨迹文件
    FILE *fp = fopen(tracefile, "r');
    if (!fp) {
        perror("fopen");
        return 1;
    }

    char op;
    uint64_t addr;
    int size;

    while (fscanf(fp, " %c %lx,%d", &op, &addr, &size) == 3) {
        if (op == 'I') continue;  // 忽略指令访问

        if (verbose) {
            printf("%c %lx,%d ", op, addr, size);
        }

        access_cache(cache, addr, op);

        if (verbose) {
            // 打印详细结果
        }
    }

    fclose(fp);

    // 打印结果
    printSummary(cache->hit_count, cache->miss_count,
                 cache->eviction_count);

    // 释放内存
    // ...

    return 0;
}
```

### 3.5 测试

```bash
# 编译
make csim

# 测试（与参考实现对比）
./csim -s 4 -E 1 -b 4 -t traces/yi.trace -v
./test-csim

# 各种配置测试
./csim -s 1 -E 1 -b 1 -t traces/yi.trace
./csim -s 4 -E 2 -b 4 -t traces/yi.trace
./csim -s 2 -E 4 -b 3 -t traces/trans.trace
```

---

## 4. Part B: 矩阵转置优化

### 4.1 问题分析

```c
// 原始转置实现
void trans(int M, int N, int A[N][M], int B[M][N]) {
    int i, j, tmp;
    for (i = 0; i < N; i++) {
        for (j = 0; j < M; j++) {
            tmp = A[i][j];
            B[j][i] = tmp;
        }
    }
}
```

**问题**: 当矩阵较大时，B的列访问导致大量缓存未命中。

### 4.2 缓存参数

```
实验缓存配置:
- s = 5  → 32组 (S = 32)
- E = 1  → 直接映射
- b = 5  → 32字节块 (B = 32, 可存8个int)

总缓存大小: 32 × 1 × 32 = 1024字节
```

### 4.3 32x32矩阵优化

#### 冲突分析

```
32x32矩阵，每行32个int = 128字节 = 4个缓存块

A[i][j]和B[j][i]的地址关系:
- A[i][j]映射到组: (base_A + i*128 + j*4) >> 5
- B[j][i]映射到组: (base_B + j*128 + i*4) >> 5

问题: 当i=j时，A[i][i]和B[i][i]可能映射到同一缓存行！
```

#### 分块优化

```c
// 8x8分块，利用局部性
void transpose_submit(int M, int N, int A[N][M], int B[M][N]) {
    int i, j, k, l;
    int tmp0, tmp1, tmp2, tmp3, tmp4, tmp5, tmp6, tmp7;

    for (i = 0; i < N; i += 8) {
        for (j = 0; j < M; j += 8) {
            for (k = i; k < i + 8; k++) {
                // 一次性读取A的一行8个元素
                tmp0 = A[k][j];
                tmp1 = A[k][j+1];
                tmp2 = A[k][j+2];
                tmp3 = A[k][j+3];
                tmp4 = A[k][j+4];
                tmp5 = A[k][j+5];
                tmp6 = A[k][j+6];
                tmp7 = A[k][j+7];

                // 写入B的列（在分块内按行存储）
                B[j][k] = tmp0;
                B[j+1][k] = tmp1;
                B[j+2][k] = tmp2;
                B[j+3][k] = tmp3;
                B[j+4][k] = tmp4;
                B[j+5][k] = tmp5;
                B[j+6][k] = tmp6;
                B[j+7][k] = tmp7;
            }
        }
    }
}
```

#### 对角线特殊处理

```c
// 处理对角线上的冲突
void transpose_32x32(int M, int N, int A[N][M], int B[M][N]) {
    int i, j, k;
    int tmp0, tmp1, tmp2, tmp3, tmp4, tmp5, tmp6, tmp7;

    for (i = 0; i < N; i += 8) {
        for (j = 0; j < M; j += 8) {
            for (k = i; k < i + 8; k++) {
                // 读取A的一行
                tmp0 = A[k][j];
                tmp1 = A[k][j+1];
                tmp2 = A[k][j+2];
                tmp3 = A[k][j+3];
                tmp4 = A[k][j+4];
                tmp5 = A[k][j+5];
                tmp6 = A[k][j+6];
                tmp7 = A[k][j+7];

                // 如果是同一块（对角线），需要特殊处理
                if (i == j) {
                    // 先存储到B的同一行（避免冲突）
                    B[k][j] = tmp0;
                    B[k][j+1] = tmp1;
                    B[k][j+2] = tmp2;
                    B[k][j+3] = tmp3;
                    B[k][j+4] = tmp4;
                    B[k][j+5] = tmp5;
                    B[k][j+6] = tmp6;
                    B[k][j+7] = tmp7;
                } else {
                    B[j][k] = tmp0;
                    B[j+1][k] = tmp1;
                    B[j+2][k] = tmp2;
                    B[j+3][k] = tmp3;
                    B[j+4][k] = tmp4;
                    B[j+5][k] = tmp5;
                    B[j+6][k] = tmp6;
                    B[j+7][k] = tmp7;
                }
            }

            // 对角线块需要原地转置
            if (i == j) {
                for (k = i; k < i + 8; k++) {
                    for (int l = k + 1; l < i + 8; l++) {
                        int tmp = B[k][l];
                        B[k][l] = B[l][k];
                        B[l][k] = tmp;
                    }
                }
            }
        }
    }
}
```

### 4.4 64x64矩阵优化

#### 冲突分析

```
64x64矩阵，每行64个int = 256字节 = 8个缓存块

问题更严重: 由于行大小256字节是缓存大小(1024)的1/4，
导致A和B的对应元素更容易冲突。
```

#### 4x4分块 + 特殊处理

```c
void transpose_64x64(int M, int N, int A[N][M], int B[M][N]) {
    int i, j, k;
    int tmp0, tmp1, tmp2, tmp3, tmp4, tmp5, tmp6, tmp7;

    for (i = 0; i < N; i += 8) {
        for (j = 0; j < M; j += 8) {
            // 处理上半4行
            for (k = i; k < i + 4; k++) {
                tmp0 = A[k][j];
                tmp1 = A[k][j+1];
                tmp2 = A[k][j+2];
                tmp3 = A[k][j+3];
                tmp4 = A[k][j+4];
                tmp5 = A[k][j+5];
                tmp6 = A[k][j+6];
                tmp7 = A[k][j+7];

                // 先存到B的左上和右上
                B[j][k] = tmp0;
                B[j+1][k] = tmp1;
                B[j+2][k] = tmp2;
                B[j+3][k] = tmp3;
                B[j][k+4] = tmp4;    // 暂存位置
                B[j+1][k+4] = tmp5;
                B[j+2][k+4] = tmp6;
                B[j+3][k+4] = tmp7;
            }

            // 处理下半4行，同时完成上半的转置
            for (k = i + 4; k < i + 8; k++) {
                // 读取A的下半
                tmp0 = A[k][j];
                tmp1 = A[k][j+1];
                tmp2 = A[k][j+2];
                tmp3 = A[k][j+3];
                tmp4 = A[k][j+4];
                tmp5 = A[k][j+5];
                tmp6 = A[k][j+6];
                tmp7 = A[k][j+7];

                // 读取之前暂存的数据
                int t0 = B[j][k];
                int t1 = B[j+1][k];
                int t2 = B[j+2][k];
                int t3 = B[j+3][k];

                // 写入正确位置
                B[j][k] = tmp0;
                B[j+1][k] = tmp1;
                B[j+2][k] = tmp2;
                B[j+3][k] = tmp3;
                B[j+4][k-4] = t0;
                B[j+5][k-4] = t1;
                B[j+6][k-4] = t2;
                B[j+7][k-4] = t3;

                B[j+4][k] = tmp4;
                B[j+5][k] = tmp5;
                B[j+6][k] = tmp6;
                B[j+7][k] = tmp7;
            }
        }
    }
}
```

### 4.5 61x67不规则矩阵

```c
void transpose_61x67(int M, int N, int A[N][M], int B[M][N]) {
    int i, j, k, l;
    int block_size = 16;  // 使用16x16分块

    for (i = 0; i < N; i += block_size) {
        for (j = 0; j < M; j += block_size) {
            // 处理每个块，注意边界
            for (k = i; k < i + block_size && k < N; k++) {
                for (l = j; l < j + block_size && l < M; l++) {
                    B[l][k] = A[k][l];
                }
            }
        }
    }
}
```

### 4.6 完整提交代码

```c
// trans.c
#include <stdio.h>
#include "cachelab.h"

int is_transpose(int M, int N, int A[N][M], int B[M][N]);

// 32x32矩阵转置
void transpose_32x32(int M, int N, int A[N][M], int B[M][N]) {
    int i, j, k;
    int tmp0, tmp1, tmp2, tmp3, tmp4, tmp5, tmp6, tmp7;

    for (i = 0; i < N; i += 8) {
        for (j = 0; j < M; j += 8) {
            for (k = i; k < i + 8; k++) {
                tmp0 = A[k][j];
                tmp1 = A[k][j+1];
                tmp2 = A[k][j+2];
                tmp3 = A[k][j+3];
                tmp4 = A[k][j+4];
                tmp5 = A[k][j+5];
                tmp6 = A[k][j+6];
                tmp7 = A[k][j+7];

                if (i == j) {
                    B[k][j] = tmp0;
                    B[k][j+1] = tmp1;
                    B[k][j+2] = tmp2;
                    B[k][j+3] = tmp3;
                    B[k][j+4] = tmp4;
                    B[k][j+5] = tmp5;
                    B[k][j+6] = tmp6;
                    B[k][j+7] = tmp7;
                } else {
                    B[j][k] = tmp0;
                    B[j+1][k] = tmp1;
                    B[j+2][k] = tmp2;
                    B[j+3][k] = tmp3;
                    B[j+4][k] = tmp4;
                    B[j+5][k] = tmp5;
                    B[j+6][k] = tmp6;
                    B[j+7][k] = tmp7;
                }
            }

            if (i == j) {
                for (k = i; k < i + 8; k++) {
                    for (int l = k + 1; l < i + 8; l++) {
                        int tmp = B[k][l];
                        B[k][l] = B[l][k];
                        B[l][k] = tmp;
                    }
                }
            }
        }
    }
}

// 64x64矩阵转置
void transpose_64x64(int M, int N, int A[N][M], int B[M][N]) {
    // ... 实现同上
}

// 主提交函数
void transpose_submit(int M, int N, int A[N][M], int B[M][N]) {
    if (M == 32 && N == 32) {
        transpose_32x32(M, N, A, B);
    } else if (M == 64 && N == 64) {
        transpose_64x64(M, N, A, B);
    } else if (M == 61 && N == 67) {
        transpose_61x67(M, N, A, B);
    }
}
```

---

## 5. 测试与评分

### 5.1 测试命令

```bash
# 测试Part A
make test-csim

# 测试Part B
driver.py

# 手动测试特定配置
./test-trans -M 32 -N 32
./tracegen -M 64 -N 64
```

### 5.2 评分标准

| 矩阵 | 最大 misses | 满分 misses |
|------|------------|------------|
| 32x32 | 300 | 259 |
| 64x64 | 2000 | 1225 |
| 61x67 | 3000 | 1995 |

---

**创建日期**: 2026-03-19


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
