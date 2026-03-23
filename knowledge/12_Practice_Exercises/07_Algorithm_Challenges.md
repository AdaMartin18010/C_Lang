
---

## 🔗 全面知识关联体系

### 【全局层】知识库导航

| 维度 | 目标文档 | 导航作用 |
|:-----|:---------|:---------|
| **总索引** | [../00_GLOBAL_INDEX.md](../00_GLOBAL_INDEX.md) | 完整知识图谱入口，全局视角 |
| **本模块** | [../README.md](../README.md) | 模块总览与目录导航 |
| **学习路径** | [../06_Thinking_Representation/06_Learning_Paths/README.md](../06_Thinking_Representation/06_Learning_Paths/README.md) | 阶段化学习路线规划 |
| **概念映射** | [../06_Thinking_Representation/05_Concept_Mappings/README.md](../06_Thinking_Representation/05_Concept_Mappings/README.md) | 核心概念等价关系图 |

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
| **理论基础** | [../02_Formal_Semantics_and_Physics/00_Core_Semantics_Foundations/README.md](../02_Formal_Semantics_and_Physics/00_Core_Semantics_Foundations/README.md) | 语义学理论基础 |
| **核心机制** | [../01_Core_Knowledge_System/02_Core_Layer/README.md](../01_Core_Knowledge_System/02_Core_Layer/README.md) | C语言核心机制 |
| **标准接口** | [../01_Core_Knowledge_System/04_Standard_Library_Layer/README.md](../01_Core_Knowledge_System/04_Standard_Library_Layer/README.md) | 标准库API |
| **系统实现** | [../03_System_Technology_Domains/README.md](../03_System_Technology_Domains/README.md) | 系统级实现 |

### 【局部层】横向关联网

| 关联类型 | 目标文档 | 关联说明 |
|:---------|:---------|:---------|
| **技术扩展** | [../03_System_Technology_Domains/14_Concurrency_Parallelism/README.md](../03_System_Technology_Domains/14_Concurrency_Parallelism/README.md) | 并发编程技术 |
| **安全规范** | [../01_Core_Knowledge_System/09_Safety_Standards/MISRA_C_2023/README.md](../01_Core_Knowledge_System/09_Safety_Standards/MISRA_C_2023/README.md) | 安全编码标准 |
| **工具支持** | [../07_Modern_Toolchain/README.md](../07_Modern_Toolchain/README.md) | 现代开发工具链 |
| **实践案例** | [../04_Industrial_Scenarios/README.md](../04_Industrial_Scenarios/README.md) | 工业实践场景 |

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
| **问题导向** | 06决策树 → 相关模块 | [决策树目录](../06_Thinking_Representation/01_Decision_Trees/README.md) |
| **项目驱动** | 04案例 → 所需知识 | [工业场景](../04_Industrial_Scenarios/README.md) |
| **深入研究** | 02形式语义 → 11CompCert | [形式语义](../02_Formal_Semantics_and_Physics/README.md) |

---

# 算法挑战 (Algorithm Challenges)

> **难度**: L3-L5 | **预计时间**: 25-40小时

---

## 挑战1: 快速排序优化

### 描述

实现工业级快速排序，超越标准库qsort性能。

### 要求

1. **三数取中法**选择pivot
2. **三路划分**处理重复元素
3. **尾递归优化**（循环代替递归）
4. **小数组切换**到插入排序（阈值16）
5. **并行化**（OpenMP或pthreads）

### 接口

```c
typedef int (*CompareFunc)(const void *, const void *);

void quicksort(void *base, size_t nmemb, size_t size, CompareFunc cmp);
```

### 基准测试

```c
// 测试数据
int random_arr[1000000];      // 随机数据
int sorted_arr[1000000];      // 已排序
int reverse_arr[1000000];     // 逆序
int duplicate_arr[1000000];   // 大量重复
```

### 性能目标

| 数据类型 | 目标性能 |
|:---------|:---------|
| 随机数据 | 比qsort快20% |
| 已排序 | O(n)时间 |
| 逆序 | O(n log n) |
| 重复多 | 接近O(n) |

---

## 挑战2: K路归并

### 描述

合并K个已排序的数组/文件。

### 场景

- 外部排序的中间步骤
- 数据库归并连接
- 日志合并

### 实现

```c
typedef struct {
    const int *data;
    size_t size;
    size_t pos;
} SortedStream;

typedef struct {
    SortedStream *streams;
    int k;
    // 最小堆
    struct HeapNode *heap;
} KWayMerger;

KWayMerger* kway_create(SortedStream *streams, int k);
void kway_destroy(KWayMerger *merger);
bool kway_next(KWayMerger *merger, int *value);
```

### 进阶

- 支持外部排序（文件太大无法装入内存）
- 多线程并行归并

---

## 挑战3: 字符串匹配算法

### 描述

实现多种字符串匹配算法并对比性能。

### 要求

实现以下算法：

```c
// 暴力匹配
int brute_force(const char *text, const char *pattern);

// KMP算法
int kmp_search(const char *text, const char *pattern);

// Boyer-Moore算法
int bm_search(const char *text, const char *pattern);

// Rabin-Karp算法
int rk_search(const char *text, const char *pattern);

// Aho-Corasick算法（多模式匹配）
typedef struct ACNode ACNode;
typedef struct {
    ACNode *root;
    int pattern_count;
} ACAutomaton;

ACAutomaton* ac_build(const char **patterns, int n);
void ac_search(ACAutomaton *ac, const char *text,
               void (*callback)(int pattern_id, int pos));
```

### 测试文本

- 英文小说（《战争与和平》）
- DNA序列
- 二进制数据
- 攻击模式（重复字符）

---

## 挑战4: 最长公共子序列 (LCS)

### 描述

计算两个序列的最长公共子序列。

### 基础实现（动态规划）

```c
// 返回LCS长度
int lcs_length(const char *s1, const char *s2);

// 返回LCS字符串（需要malloc）
char* lcs_construct(const char *s1, const char *s2);
```

### 进阶要求

1. **空间优化**: O(min(m,n))空间
2. **Hirschberg算法**: 分治O(log n)空间
3. **Myers算法**: O((m+n)d)时间（d为编辑距离）
4. **diff工具**: 实现类似Unix diff的功能

### diff输出格式

```text
2,4c2,4
< This is old line 2
< This is old line 3
< This is old line 4
---
> This is new line 2
> This is new line 3
> This is new line 4
```

---

## 挑战5: 最短路算法全家桶

### 描述

实现并对比各种最短路算法。

### 算法实现

```c
typedef struct {
    int from, to;
    double weight;
} Edge;

typedef struct {
    int n, m;
    Edge *edges;
} Graph;

// Dijkstra - 单源，非负权
void dijkstra(const Graph *g, int src, double *dist, int *prev);

// Bellman-Ford - 单源，可负权，可检测负环
bool bellman_ford(const Graph *g, int src, double *dist, int *prev);

// SPFA - Bellman-Ford优化
bool spfa(const Graph *g, int src, double *dist, int *prev);

// Floyd-Warshall - 全源
void floyd_warshall(const Graph *g, double **dist, int **next);

// Johnson - 稀疏图全源
void johnson(const Graph *g, double **dist);
```

### 性能对比

| 算法 | 时间复杂度 | 空间复杂度 | 适用场景 |
|:-----|:-----------|:-----------|:---------|
| Dijkstra | O((V+E)log V) | O(V) | 非负权，单源 |
| Bellman-Ford | O(VE) | O(V) | 负权，稀疏图 |
| SPFA | O(VE)最坏 | O(V) | 实际快很多 |
| Floyd-Warshall | O(V³) | O(V²) | 稠密图，全源 |
| Johnson | O(V² log V + VE) | O(V²) | 稀疏图，全源 |

---

## 挑战6: 正则表达式引擎

### 描述

实现简化版正则表达式引擎。

### 支持的语法

```text
.       匹配任意字符
*       零次或多次
+       一次或多次
?       零次或一次
^       行首
$       行尾
[abc]   字符类
[^abc]  否定字符类
()      分组
|       或
```

### 实现方式

1. **NFA构造**: Thompson构造法
2. **NFA转DFA**: 子集构造法
3. **DFA最小化**: Hopcroft算法
4. **匹配执行**: DFA模拟或回溯

### 接口

```c
typedef struct Regex Regex;

// 编译正则表达式
Regex* regex_compile(const char *pattern);

// 匹配整个字符串
bool regex_match(const Regex *re, const char *text);

// 搜索子串
bool regex_search(const Regex *re, const char *text,
                  int *match_start, int *match_end);

// 释放资源
void regex_free(Regex *re);
```

---

## 挑战7: 数值算法

### 描述

实现高精度数值计算。

### 大整数运算

```c
typedef struct {
    uint32_t *digits;  // 小端序，每元素存32位
    size_t size;       // 有效位数
    size_t capacity;
    bool negative;
} BigInt;

BigInt* bigint_from_int(int64_t n);
BigInt* bigint_from_string(const char *str);
char* bigint_to_string(const BigInt *n);
void bigint_free(BigInt *n);

BigInt* bigint_add(const BigInt *a, const BigInt *b);
BigInt* bigint_sub(const BigInt *a, const BigInt *b);
BigInt* bigint_mul(const BigInt *a, const BigInt *b);
BigInt* bigint_div(const BigInt *a, const BigInt *b, BigInt **remainder);
BigInt* bigint_pow(const BigInt *base, uint64_t exp);
BigInt* bigint_mod_pow(const BigInt *base, const BigInt *exp,
                       const BigInt *mod);  // RSA需要
```

### 矩阵运算

```c
typedef struct {
    size_t rows, cols;
    double *data;
} Matrix;

Matrix* matrix_create(size_t rows, size_t cols);
void matrix_free(Matrix *m);

Matrix* matrix_add(const Matrix *a, const Matrix *b);
Matrix* matrix_mul(const Matrix *a, const Matrix *b);
Matrix* matrix_transpose(const Matrix *m);

// 高斯消元
bool matrix_solve(const Matrix *a, const double *b, double *x);
double matrix_determinant(const Matrix *m);
Matrix* matrix_inverse(const Matrix *m);

// Strassen快速矩阵乘法
Matrix* matrix_mul_strassen(const Matrix *a, const Matrix *b);
```

---

## 挑战8: 贪心与动态规划

### 经典问题实现

```c
// 活动选择问题
int activity_selection(int *start, int *finish, int n, int *selected);

// 分数背包问题
double fractional_knapsack(double *values, double *weights,
                           int n, double capacity);

// 0-1背包问题
int knapsack_01(int *values, int *weights, int n, int capacity);

// 最长递增子序列
int lis_length(const int *arr, int n);

// 编辑距离
int edit_distance(const char *s1, const char *s2);

// 最长回文子串
char* longest_palindrome(const char *s);

// 矩阵链乘法
int matrix_chain_order(int *dims, int n, int **split_points);

// 最优二叉搜索树
void optimal_bst(double *keys, double *dummy_keys, int n,
                 double **root_cost, int **roots);
```

---

## 代码质量检查清单

- [ ] 所有算法有时间和空间复杂度分析
- [ ] 有全面的基准测试
- [ ] 边界情况处理（空输入、极大值等）
- [ ] 内存无泄漏
- [ ] 代码有清晰注释说明算法思路

---

> **返回导航**: [练习模块总览](./README.md) | [知识库总览](../README.md)


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
