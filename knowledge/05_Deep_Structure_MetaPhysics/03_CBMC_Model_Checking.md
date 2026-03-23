# CBMC模型检测


---

## 📑 目录

- [CBMC模型检测](#cbmc模型检测)
  - [📑 目录](#-目录)
  - [概述](#概述)
  - [CBMC 工具介绍](#cbmc-工具介绍)
    - [核心原理](#核心原理)
    - [安装与使用](#安装与使用)
    - [命令行选项](#命令行选项)
  - [有界模型检测](#有界模型检测)
    - [循环展开](#循环展开)
    - [展开界限选择](#展开界限选择)
    - [示例：验证循环不变量](#示例验证循环不变量)
  - [内存安全检查](#内存安全检查)
    - [数组边界检查](#数组边界检查)
    - [指针有效性检查](#指针有效性检查)
    - [内存泄漏检测](#内存泄漏检测)
  - [并发验证](#并发验证)
    - [数据竞争检测](#数据竞争检测)
    - [死锁检测](#死锁检测)
  - [断言验证](#断言验证)
    - [功能正确性验证](#功能正确性验证)
    - [算术属性验证](#算术属性验证)
  - [实际应用案例](#实际应用案例)
    - [验证嵌入式驱动](#验证嵌入式驱动)
  - [与其他工具对比](#与其他工具对比)
  - [最佳实践](#最佳实践)
  - [总结](#总结)


---

## 概述

CBMC（C Bounded Model Checker）是专门用于 C/C++ 程序的 bounded model checking 工具。它由卡内基梅隆大学开发，能够自动检测程序中的多种类型错误，包括内存安全违规、算术溢出、并发问题和断言失败。

## CBMC 工具介绍

### 核心原理

CBMC 基于 **有界模型检测（Bounded Model Checking）** 技术：

```
┌─────────────┐     ┌─────────────┐     ┌─────────────┐
│   C 程序    │ ──→ │  控制流图   │ ──→ │  逻辑公式   │
│   源代码    │     │   (CFG)     │     │   (SAT/SMT) │
└─────────────┘     └─────────────┘     └──────┬──────┘
                                               │
                                               ↓
                                        ┌─────────────┐
                                        │  SAT/SMT    │
                                        │  求解器     │
                                        │ (MiniSat/   │
                                        │  Z3)        │
                                        └──────┬──────┘
                                               │
                              ┌────────────────┼────────────────┐
                              ↓                ↓                ↓
                         UNSATISFIABLE    SATISFIABLE      TIMEOUT
                         (无错误)        (发现反例)       (达到界限)
```

### 安装与使用

```bash
# Ubuntu/Debian 安装

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

---

## 🔗 文档关联

### 核心关联
| 文档 | 关系类型 | 说明 |
|:-----|:---------|:-----|
| [内存管理](../../01_Core_Knowledge_System/02_Core_Layer/02_Memory_Management.md) | 核心关联 | 内存管理基础 |
| [指针深度](../../01_Core_Knowledge_System/02_Core_Layer/01_Pointer_Depth.md) | 核心关联 | 指针深度基础 |
| [并发编程](../../03_System_Technology_Domains/14_Concurrency_Parallelism/README.md) | 核心关联 | 并发编程基础 |
| [数据类型](../../01_Core_Knowledge_System/01_Basic_Layer/02_Data_Type_System.md) | 核心关联 | 数据类型基础 |
| [数组与指针](../../01_Core_Knowledge_System/02_Core_Layer/05_Arrays_Pointers.md) | 核心关联 | 数组与指针基础 |

### 扩展阅读
| 文档 | 关系类型 | 说明 |
|:-----|:---------|:-----|
| [软件工程](../../01_Core_Knowledge_System/05_Engineering_Layer/README.md) | 核心关联 | 软件工程基础 |
| [形式语义](../../02_Formal_Semantics_and_Physics/README.md) | 核心关联 | 形式语义基础 |
| [系统技术](../../03_System_Technology_Domains/README.md) | 核心关联 | 系统技术基础 |
| [工业场景](../../04_Industrial_Scenarios/README.md) | 核心关联 | 工业场景基础 |
| [思维表征](../../06_Thinking_Representation/README.md) | 核心关联 | 思维表征基础 |
sudo apt-get install cbmc

# macOS 安装
brew install cbmc

# 基本用法
cbmc program.c
cbmc program.c --function main
cbmc program.c --unwind 10 --bounds-check
```

### 命令行选项

| 选项 | 描述 | 示例 |
|------|------|------|
| `--bounds-check` | 数组边界检查 | `cbmc file.c --bounds-check` |
| `--pointer-check` | 指针有效性检查 | `cbmc file.c --pointer-check` |
| `--memory-leak-check` | 内存泄漏检测 | `cbmc file.c --memory-leak-check` |
| `--div-by-zero-check` | 除零检查 | `cbmc file.c --div-by-zero-check` |
| `--signed-overflow-check` | 有符号溢出检查 | `cbmc file.c --signed-overflow-check` |
| `--unsigned-overflow-check` | 无符号溢出检查 | `cbmc file.c --unsigned-overflow-check` |
| `--unwind N` | 设置展开界限 | `cbmc file.c --unwind 10` |
| `--unwinding-assertions` | 添加展开断言 | `cbmc file.c --unwinding-assertions` |

## 有界模型检测

### 循环展开

CBMC 通过展开循环来进行分析：

```c
// 原始代码
int sum_array(int *arr, int n) {
    int sum = 0;
    for (int i = 0; i < n; i++) {
        sum += arr[i];  // 需要展开 n 次
    }
    return sum;
}
```

展开后的逻辑（假设 n=3）：

```c
// 概念性展开
int sum_array_unrolled(int *arr, int n) {
    int sum = 0;
    int i = 0;

    // 迭代 1
    if (i < n) { sum += arr[i]; i++; }
    // 迭代 2
    if (i < n) { sum += arr[i]; i++; }
    // 迭代 3
    if (i < n) { sum += arr[i]; i++; }

    // 断言：循环已完成
    assert(i >= n);

    return sum;
}
```

### 展开界限选择

```bash
# 自动展开（尝试确定界限）
cbmc program.c --unwindset "loop1:10,loop2:5"

# 使用展开断言确保充分性
cbmc program.c --unwind 20 --unwinding-assertions

# 仅检查特定函数
cbmc program.c --function process_data --unwind 50
```

### 示例：验证循环不变量

```c
#include <assert.h>

// 计算阶乘
unsigned long factorial(unsigned n) {
    unsigned long result = 1;
    for (unsigned i = 1; i <= n; i++) {
        result *= i;
    }
    return result;
}

// 带不变量检查的版本
unsigned long factorial_verified(unsigned n) {
    __CPROVER_assume(n <= 20);  // 假设输入范围

    unsigned long result = 1;
    for (unsigned i = 1; i <= n; i++) {
        // 循环不变量：result = (i-1)!
        __CPROVER_loop_invariant(
            result == (i == 1 ? 1 : result / (i-1) * (i-1))
        );
        result *= i;
    }

    // 验证结果
    if (n == 0) __CPROVER_assert(result == 1, "0! = 1");
    if (n == 5) __CPROVER_assert(result == 120, "5! = 120");

    return result;
}

int main() {
    unsigned n;
    __CPROVER_havoc_object(&n);  // 非确定性输入
    factorial_verified(n);
    return 0;
}
```

验证命令：

```bash
cbmc factorial.c --function main --unwind 25 --signed-overflow-check
```

## 内存安全检查

### 数组边界检查

```c
#include <assert.h>
#include <stdlib.h>

void safe_buffer_access() {
    int buffer[10];
    size_t index;

    // 非确定性索引
    __CPROVER_havoc_object(&index);
    __CPROVER_assume(index < 10);  // 约束：有效索引

    // 安全访问
    buffer[index] = 42;

    // 验证：即使使用非确定性索引，也在边界内
    assert(buffer[index] == 42);
}

void unsafe_buffer_access() {
    int buffer[10];
    size_t index;

    __CPROVER_havoc_object(&index);
    // 未约束的索引 - 可能导致缓冲区溢出

    buffer[index] = 42;  // CBMC 将报告此处的潜在错误
}

int main() {
    safe_buffer_access();
    // unsafe_buffer_access();  // 取消注释以查看错误报告
    return 0;
}
```

验证命令：

```bash
# 检查安全版本（应通过）
cbmc buffer_safe.c --function main --bounds-check

# 检查不安全版本（应报告错误）
cbmc buffer_unsafe.c --function main --bounds-check
```

### 指针有效性检查

```c
#include <stdlib.h>
#include <assert.h>

struct Node {
    int data;
    struct Node *next;
};

// 链表操作 - 带验证
void list_append(struct Node **head, int value) {
    struct Node *new_node = malloc(sizeof(struct Node));
    __CPROVER_assume(new_node != NULL);  // 假设分配成功

    new_node->data = value;
    new_node->next = NULL;

    if (*head == NULL) {
        *head = new_node;
    } else {
        struct Node *current = *head;
        // 查找尾节点
        while (current->next != NULL) {
            __CPROVER_loop_invariant(current != NULL);
            current = current->next;
        }
        current->next = new_node;
    }
}

// 验证空指针解引用保护
void safe_dereference(struct Node *node) {
    if (node != NULL) {
        int val = node->data;  // 安全
        (void)val;
    }
}

int main() {
    struct Node *list = NULL;
    list_append(&list, 1);
    list_append(&list, 2);
    safe_dereference(list);
    return 0;
}
```

### 内存泄漏检测

```c
#include <stdlib.h>

// 无泄漏版本
void no_leak_function() {
    int *ptr = malloc(sizeof(int) * 10);
    if (ptr != NULL) {
        // 使用内存...
        *ptr = 42;
        free(ptr);  // 正确释放
    }
}

// 有泄漏版本
void leak_function() {
    int *ptr = malloc(sizeof(int) * 10);
    if (ptr != NULL) {
        *ptr = 42;
        // 忘记 free(ptr) - 内存泄漏！
    }
}

int main() {
    no_leak_function();
    // leak_function();  // CBMC 将报告内存泄漏
    return 0;
}
```

验证命令：

```bash
cbmc memory_leak.c --memory-leak-check
```

## 并发验证

### 数据竞争检测

```c
#include <pthread.h>
#include <assert.h>

int shared_counter = 0;
pthread_mutex_t mutex;

// 线程安全版本（使用互斥锁）
void *thread_safe_increment(void *arg) {
    pthread_mutex_lock(&mutex);
    shared_counter++;  // 受保护的访问
    pthread_mutex_unlock(&mutex);
    return NULL;
}

// 非线程安全版本
void *thread_unsafe_increment(void *arg) {
    shared_counter++;  // 数据竞争！
    return NULL;
}

int main() {
    pthread_t t1, t2;
    pthread_mutex_init(&mutex, NULL);

    pthread_create(&t1, NULL, thread_safe_increment, NULL);
    pthread_create(&t2, NULL, thread_safe_increment, NULL);

    pthread_join(t1, NULL);
    pthread_join(t2, NULL);

    pthread_mutex_destroy(&mutex);
    return 0;
}
```

验证命令：

```bash
# 检测数据竞争
cbmc concurrent.c --pointer-check --data-races-check
```

### 死锁检测

```c
#include <pthread.h>

pthread_mutex_t m1 = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t m2 = PTHREAD_MUTEX_INITIALIZER;

// 可能导致死锁的代码
void *thread_a(void *arg) {
    pthread_mutex_lock(&m1);
    pthread_mutex_lock(&m2);  // 可能阻塞
    pthread_mutex_unlock(&m2);
    pthread_mutex_unlock(&m1);
    return NULL;
}

void *thread_b(void *arg) {
    pthread_mutex_lock(&m2);
    pthread_mutex_lock(&m1);  // 可能阻塞 - 死锁！
    pthread_mutex_unlock(&m1);
    pthread_mutex_unlock(&m2);
    return NULL;
}

// 正确顺序版本
void *thread_c(void *arg) {
    pthread_mutex_lock(&m1);  // 统一顺序
    pthread_mutex_lock(&m2);
    pthread_mutex_unlock(&m2);
    pthread_mutex_unlock(&m1);
    return NULL;
}

int main() {
    pthread_t t1, t2;
    pthread_create(&t1, NULL, thread_a, NULL);
    pthread_create(&t2, NULL, thread_c, NULL);  // 使用统一顺序避免死锁
    pthread_join(t1, NULL);
    pthread_join(t2, NULL);
    return 0;
}
```

## 断言验证

### 功能正确性验证

```c
#include <assert.h>
#include <stdlib.h>

// 二分查找 - 带功能验证
int binary_search(int *arr, int n, int target) {
    __CPROVER_assume(n >= 0);
    __CPROVER_assume(__CPROVER_forall {
        int i; int j;
        (0 <= i && i < j && j < n) ==> (arr[i] <= arr[j])
    });  // 假设数组已排序

    int low = 0;
    int high = n - 1;

    while (low <= high) {
        int mid = low + (high - low) / 2;

        __CPROVER_loop_invariant(
            low >= 0 && high < n &&
            (low == 0 || arr[low-1] < target) &&
            (high == n-1 || arr[high+1] > target)
        );

        if (arr[mid] == target) {
            return mid;  // 找到
        } else if (arr[mid] < target) {
            low = mid + 1;
        } else {
            high = mid - 1;
        }
    }

    return -1;  // 未找到
}

// 验证性质
void verify_binary_search() {
    int arr[5] = {1, 3, 5, 7, 9};

    // 验证存在的元素
    int idx1 = binary_search(arr, 5, 5);
    __CPROVER_assert(idx1 == 2, "5 is at index 2");

    // 验证不存在的元素
    int idx2 = binary_search(arr, 5, 4);
    __CPROVER_assert(idx2 == -1, "4 is not in array");
}

int main() {
    verify_binary_search();
    return 0;
}
```

### 算术属性验证

```c
#include <assert.h>
#include <limits.h>

// 安全加法 - 带溢出检查
int safe_add(int a, int b, int *result) {
    // 检查溢出
    if (a > 0 && b > INT_MAX - a) return -1;  // 正溢出
    if (a < 0 && b < INT_MIN - a) return -1;  // 负溢出

    *result = a + b;
    return 0;
}

// 验证版本
void verified_add(int a, int b) {
    __CPROVER_assume(a >= 0 && a <= 1000);
    __CPROVER_assume(b >= 0 && b <= 1000);

    int result;
    int status = safe_add(a, b, &result);

    if (status == 0) {
        __CPROVER_assert(result == a + b, "Addition correct");
        __CPROVER_assert(result >= 0, "Result non-negative");
    }
}

int main() {
    int a, b;
    __CPROVER_havoc_object(&a);
    __CPROVER_havoc_object(&b);

    __CPROVER_assume(a >= 0 && a <= 1000);
    __CPROVER_assume(b >= 0 && b <= 1000);

    verified_add(a, b);
    return 0;
}
```

验证命令：

```bash
cbmc arithmetic.c --signed-overflow-check --function main --unwind 1
```

## 实际应用案例

### 验证嵌入式驱动

```c
#include <stdint.h>
#include <assert.h>

// 模拟硬件寄存器
volatile uint32_t *STATUS_REG = (uint32_t *)0x40000000;
volatile uint32_t *DATA_REG = (uint32_t *)0x40000004;

#define READY_FLAG 0x01

// 带超时的设备读取
int device_read_with_timeout(uint32_t *data, uint32_t timeout_ms) {
    uint32_t elapsed = 0;

    while ((*STATUS_REG & READY_FLAG) == 0) {
        if (elapsed >= timeout_ms) {
            return -1;  // 超时
        }
        elapsed++;
    }

    *data = *DATA_REG;
    return 0;
}

// 验证超时逻辑
void verify_timeout() {
    uint32_t data;
    int result;

    // 场景1：设备就绪
    *STATUS_REG = READY_FLAG;
    result = device_read_with_timeout(&data, 100);
    __CPROVER_assert(result == 0, "Ready device succeeds");

    // 场景2：设备未就绪（需要展开循环）
    *STATUS_REG = 0;
    result = device_read_with_timeout(&data, 5);
    __CPROVER_assert(result == -1 || result == 0,
                     "Result is valid");
}

int main() {
    verify_timeout();
    return 0;
}
```

## 与其他工具对比

| 特性 | CBMC | KLEE | Valgrind | ASan |
|------|------|------|----------|------|
| 方法 | 符号执行 | 符号执行 | 动态分析 | 动态分析 |
| 完备性 | 有界完备 | 路径完备 | 测试依赖 | 测试依赖 |
| 内存检查 | 是 | 是 | 是 | 是 |
| 并发检查 | 是 | 有限 | 否 | 否 |
| 断言验证 | 是 | 是 | 否 | 否 |
| 性能开销 | 高（分析时） | 高（分析时） | 中等 | 低 |
| 误报率 | 低 | 低 | 低 | 低 |
| 漏报率 | 可能（界限内） | 可能 | 可能 | 可能 |

## 最佳实践

1. **渐进式验证**: 从简单属性开始，逐步增加复杂度
2. **合理设置界限**: 展开界限要足够覆盖典型场景
3. **使用断言**: 显式标注程序不变量和期望属性
4. **模块化分析**: 验证单个函数而非整个程序
5. **处理非确定性**: 使用 `__CPROVER_havoc_object` 生成测试输入

## 总结

CBMC 是 C/C++ 程序验证的强大工具，能够自动检测多种类型的错误。通过有界模型检测技术，CBMC 在有限范围内提供完备的验证保证。虽然存在状态爆炸问题，但对于安全关键代码和驱动程序等中等规模程序，CBMC 是非常有效的验证工具。

---

> **状态**: ✅ 已完成
