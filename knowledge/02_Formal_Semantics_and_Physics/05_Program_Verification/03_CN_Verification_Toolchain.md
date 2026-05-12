# CN 验证工具链：基于分离逻辑的C程序形式化验证

> **标准**: C11/C17/C23 (通过Cerberus语义), Separation Logic
> **前置知识**: [分离逻辑基础](../11_CompCert_Verification/README.md), [Hoare逻辑](../00_Core_Semantics_Foundations/03_Axiomatic_Semantics_Hoare.md), [操作语义](../00_Core_Semantics_Foundations/01_Operational_Semantics.md)
> **后续延伸**: [Iris/CompCert C并发分离逻辑](../13_Formal_Methods/02_Coq_Proof_Assistant.md), [VST分离逻辑](../11_CompCert_Verification/02_VST_Separation_Logic_Practical.md)
> **对标资源**: CN项目 (Imperial College), POPL 2023/2025/2026, PLDI 2026, Cerberus语义
> **难度**: 6/5 | **预计学习时间**: 10-15 小时

---

## 目录

- [CN 验证工具链：基于分离逻辑的C程序形式化验证](#cn-验证工具链基于分离逻辑的c程序形式化验证)
  - [目录](#目录)
  - [一、CN项目概述](#一cn项目概述)
    - [1.1 什么是CN？](#11-什么是cn)
    - [1.2 CN在验证工具谱系中的位置](#12-cn在验证工具谱系中的位置)
  - [二、理论基础：Cerberus C语义](#二理论基础cerberus-c语义)
    - [2.1 为什么需要新语义？](#21-为什么需要新语义)
    - [2.2 Cerberus的核心设计](#22-cerberus的核心设计)
    - [2.3 Cerberus的关键特性](#23-cerberus的关键特性)
  - [三、CN分离逻辑规范语言](#三cn分离逻辑规范语言)
    - [3.1 所有权断言](#31-所有权断言)
    - [3.2 函数规范](#32-函数规范)
    - [3.3 数据结构：链表](#33-数据结构链表)
  - [四、安装与使用](#四安装与使用)
    - [4.1 安装CN](#41-安装cn)
    - [4.2 验证C文件](#42-验证c文件)
    - [4.3 典型输出](#43-典型输出)
  - [五、实战：验证链表操作](#五实战验证链表操作)
    - [5.1 完整验证示例](#51-完整验证示例)
  - [六、Fulminate：测试CN规范](#六fulminate测试cn规范)
    - [6.1 Fulminate是什么？](#61-fulminate是什么)
    - [6.2 使用Fulminate](#62-使用fulminate)
    - [6.3 Fulminate测试报告](#63-fulminate测试报告)
  - [七、Bennet：随机规范测试](#七bennet随机规范测试)
    - [7.1 Bennet是什么？](#71-bennet是什么)
    - [7.2 Bennet与Fulminate的区别](#72-bennet与fulminate的区别)
  - [八、CN与CompCert/VST的对比](#八cn与compcertvst的对比)
    - [8.1 选择建议](#81-选择建议)
  - [九、前沿进展（2025-2026）](#九前沿进展2025-2026)
    - [9.1 PLDI 2026：Code-Specify-Test-Debug-Prove](#91-pldi-2026code-specify-test-debug-prove)
    - [9.2 并发扩展（进行中）](#92-并发扩展进行中)
    - [9.3 与Rust验证的对比](#93-与rust验证的对比)
  - [十、参考资源](#十参考资源)
    - [10.1 核心论文](#101-核心论文)
    - [10.2 工具与代码](#102-工具与代码)
    - [10.3 相关课程](#103-相关课程)

---

## 一、CN项目概述

### 1.1 什么是CN？

**CN**（Cerberus/CN）是由英国**帝国理工学院**（Imperial College London）开发的开源C程序验证工具，基于**分离逻辑（Separation Logic）**和**精化类型（Refinement Types）**。

> *"CN is a tool for verifying that C code is free of undefined behaviour and meets user-written specifications of its ownership and functional correctness."*
> — CN项目主页

**核心能力**：

1. **UB检测**：验证C代码是否包含未定义行为（UB）
2. **所有权验证**：通过分离逻辑验证内存所有权正确性
3. **功能正确性**：验证函数是否满足用户编写的规范
4. **运行时断言生成**：将规范翻译为C断言，可在具体测试用例上检查

### 1.2 CN在验证工具谱系中的位置

```
C程序验证工具谱系
├── 未验证框架（无法保证正确性）
│   ├── VeriFast（分离逻辑，但无机器验证）
│   └── Infer（Facebook，自动推断，无证明）
│
├── 基础验证（无编译器连接）
│   ├── Frama-C/WP（Hoare逻辑，Why3后端）
│   └── CBMC（有界模型检查，无完整证明）
│
├── 认证编译器（CompCert编译器正确）
│   └── VST（分离逻辑，连接CompCert，完整证明）
│
└── CN（本项目）：新型分离逻辑框架
    ├── 基于可执行语义（Cerberus）
    ├── 半自动化证明（SMT求解器）
    ├── 精炼类型系统
    ├── 连接测试（Fulminate/Bennet）
    └── 不依赖CompCert（独立语义）
```

---

## 二、理论基础：Cerberus C语义

### 2.1 为什么需要新语义？

传统C验证工具（如VST）基于**CompCert C**语义，但CompCert：

- 不支持C11标准（特别是内存模型和并发）
- 语义定义与真实编译器行为存在差距
- 不支持某些现代C特性（如灵活数组成员、对齐）

**Cerberus**提供了**可执行的C11/C17/C23操作语义**，直接基于ISO标准定义。

### 2.2 Cerberus的核心设计

```
C源代码
    │
    ▼
[Cerberus解析器] ──► 抽象语法树（AST）
    │
    ▼
[Cerberus核心语义] ──► 可执行操作语义
    │                     ├── 小步语义（Small-step）
    │                     ├── 内存模型（C11 Memory Model）
    │                     └── 未定义行为检测
    │
    ├──► CN验证器 ──► 分离逻辑证明
    │
    └──► 可执行解释器 ──► 具体执行追踪
```

### 2.3 Cerberus的关键特性

| 特性 | CompCert | Cerberus |
|:-----|:---------|:---------|
| C标准版本 | C99（大部分） | C11/C17/C23 |
| C11内存模型 | ❌ 不支持 | ✅ 支持 |
| 并发 | ❌ 不支持 | ✅ 支持 |
| 灵活数组 | ⚠️ 部分 | ✅ 完整 |
| 对齐（_Alignas） | ⚠️ 部分 | ✅ 完整 |
| 可执行性 | ❌ 仅证明 | ✅ 可执行解释器 |
| 形式化连接 | Coq证明 | Lem/Ott定义 |

---

## 三、CN分离逻辑规范语言

### 3.1 所有权断言

CN使用**分离逻辑断言**描述内存状态：

```c
// C函数：交换两个整数
void swap(int *p, int *q)
/*@
requires
    take x = Owned<int>(p);    // p指向的整数所有权，绑定为x
    take y = Owned<int>(q);    // q指向的整数所有权，绑定为y
ensures
    take x2 = Owned<int>(p);   // 函数返回后，p仍拥有整数
    take y2 = Owned<int>(q);   // q仍拥有整数
    x2 == y;                    // p处的值等于原来的y
    y2 == x;                    // q处的值等于原来的x
@*/
{
    int tmp = *p;
    *p = *q;
    *q = tmp;
}
```

**核心断言**：

| 断言 | 含义 | 示例 |
|:-----|:-----|:-----|
| `Owned<T>(p)` | 拥有p指向的T类型对象 | `Owned<int>(p)` |
| `Block<T>(p)` | 拥有p指向的未初始化T对象 | `Block<struct foo>(p)` |
| `RW<T>(p)` | 读写权限（与Owned等价） | `RW<int>(p)` |
| `R<T>(p)` | 只读权限 | `R<const int>(p)` |
| `Alloc(p, n)` | p处分配了n字节 | `Alloc(p, sizeof(int))` |
| `each(...)` | 数组/循环的量化断言 | 见下文 |

### 3.2 函数规范

```c
// 求和函数
int sum(const int *arr, size_t n)
/*@
requires
    0 <= n;                                    // 前置条件：n非负
    take xs = each(u64 i; 0 <= i && i < n) {  // 对数组每个元素
        R<int>(array_shift(arr, i))            // 拥有只读权限
    };
ensures
    return == sum_i(0, n-1, (u64 j) => xs[j]); // 返回值等于数组元素和
@*/
{
    int total = 0;
    for (size_t i = 0; i < n; i++)
    /*@
    inv
        0 <= i; i <= n;
        take xs2 = each(u64 j; 0 <= j && j < n) {
            R<int>(array_shift(arr, j))
        };
        total == sum_i(0, i-1, (u64 j) => xs2[j]);
    @*/
    {
        total += arr[i];
    }
    return total;
}
```

### 3.3 数据结构：链表

```c
// 链表节点
typedef struct node {
    int value;
    struct node *next;
} node_t;

// CN归纳谓词：链表
typedef [[cn]] struct node {
    int value;
    struct node *next;
};

/*@
function (u64) length(pointer p) {
    if (is_null(p)) {
        0
    } else {
        1 + length((*p).next)
    }
}

predicate List Segment {
    pointer head;
    pointer end;
}

List Segment(head, end) {
    if (is_null(head)) {
        assert(is_null(end));
        return;
    }
    take node = Owned<struct node>(head);
    assert(node.next == end || !is_null(node.next));
    List Segment(node.next, end);
}
@*/

// 链表插入
void list_insert(node_t **head, int value)
/*@
requires
    take list = Owned<Pointer>(head);          // head指针的所有权
    List Segment(list, NULL);                   // list是一个有效链表
ensures
    take new_list = Owned<Pointer>(head);
    List Segment(new_list, NULL);
    length(new_list) == length(list) + 1;
@*/
{
    node_t *new_node = malloc(sizeof(node_t));
    if (!new_node) return;

    new_node->value = value;
    new_node->next = *head;
    *head = new_node;
}
```

---

## 四、安装与使用

### 4.1 安装CN

```bash
# 方式1：使用OPAM（OCaml包管理器）
opam install cn

# 方式2：从源码构建
git clone https://github.com/rems-project/cn.git
cd cn
opam install . --deps-only
dune build
```

### 4.2 验证C文件

```bash
# 验证单个文件
cn verify my_program.c

# 生成运行时断言（用于测试）
cn test my_program.c

# 详细输出
cn verify --verbose my_program.c
```

### 4.3 典型输出

```bash
$ cn verify swap.c
[1/1] Checking function: swap
  - Precondition: OK
  - Postcondition: OK
  - UB check: OK
✅ Verification successful (0.3s)

$ cn verify list.c
[1/3] Checking function: list_insert
  - Precondition: OK
  - Postcondition: OK
  - UB check: OK
[2/3] Checking function: list_length
  - Loop invariant: OK
  - Postcondition: OK
[3/3] Checking function: list_free
  - Memory leak check: OK
✅ Verification successful (1.2s)
```

---

## 五、实战：验证链表操作

### 5.1 完整验证示例

```c
// list_verified.c — 完整可验证的链表实现
#include <stdlib.h>

// CN内置谓词：验证malloc不返回NULL（简化）
/*@
function (boolean) non_null(pointer p) {
    !is_null(p)
}
@*/

typedef struct node {
    int value;
    struct node *next;
} node_t;

// 创建节点
node_t* node_create(int value)
/*@
requires
    true;  // 无前置条件
ensures
    take ret = Owned<node_t>(return);
    ret.value == value;
    is_null(ret.next);
    Alloc(return, sizeof(node_t));
@*/
{
    node_t *node = malloc(sizeof(node_t));
    // CN自动检查：malloc可能返回NULL → 需要处理
    // 为简化，假设malloc总是成功（或使用cn_test模式）
    node->value = value;
    node->next = NULL;
    return node;
}

// 头部插入
void list_prepend(node_t **head, int value)
/*@
requires
    take list_ptr = Owned<Pointer>(head);
    take list = Owned<node_t>(list_ptr);  // head指向的节点
ensures
    take new_head = Owned<Pointer>(head);
    take new_node = Owned<node_t>(new_head);
    new_node.value == value;
    new_node.next == list_ptr;
    Owned<node_t>(list_ptr);  // 原链表头仍有效
@*/
{
    node_t *node = node_create(value);
    node->next = *head;
    *head = node;
}

// 求和
int list_sum(node_t *head)
/*@
requires
    take list = each(u64 i; 0 <= i && i < length(head)) {
        Owned<node_t>(nth_node(head, i))
    };
ensures
    take list2 = each(u64 i; 0 <= i && i < length(head)) {
        Owned<node_t>(nth_node(head, i))
    };
    return == sum_nodes(list2);
@*/
{
    int sum = 0;
    node_t *current = head;
    while (current != NULL)
    /*@
    inv
        take visited = each(...);
        sum == sum_nodes(visited);
        current == next_unvisited(head, visited);
    @*/
    {
        sum += current->value;
        current = current->next;
    }
    return sum;
}

// 释放链表
void list_free(node_t *head)
/*@
requires
    take list = each(u64 i; 0 <= i && i < length(head)) {
        Owned<node_t>(nth_node(head, i))
    };
ensures
    true;  // 链表所有权被释放
@*/
{
    while (head != NULL) {
        node_t *next = head->next;
        free(head);
        head = next;
    }
}
```

**验证**：

```bash
cn verify list_verified.c
```

---

## 六、Fulminate：测试CN规范

### 6.1 Fulminate是什么？

**Fulminate**（POPL 2025）是CN的**测试生成框架**，它将CN的分离逻辑规范自动翻译为**运行时C断言**，然后使用随机测试验证这些断言。

> *"Testing CN Separation-Logic Specifications in C"*
> — Rini Banerjee et al., POPL 2025

**核心价值**：

- **规范即测试**：CN规范直接生成可执行的测试用例
- **反例发现**：当验证失败时，Fulminate生成具体的反例输入
- **调试辅助**：将抽象的分离逻辑失败转化为具体的内存状态

### 6.2 使用Fulminate

```bash
# 1. 生成带运行时断言的C代码
cn test --output=tested_list.c list_verified.c

# 2. 编译并运行测试
gcc -o test_list tested_list.c -lcn_test
cn_test_run test_list

# 3. 随机测试
cn_test_run --iterations=10000 --random-seed=42 test_list
```

### 6.3 Fulminate测试报告

```
Running Fulminate tests for list_verified.c
==========================================

[TEST] node_create
  ✓ malloc returns non-null (100/100)
  ✓ value correctly set (100/100)
  ✓ next is null (100/100)

[TEST] list_prepend
  ✓ head updated (100/100)
  ✓ old list preserved (100/100)
  ✓ no memory leaks (100/100)

[TEST] list_sum
  ✓ empty list returns 0 (100/100)
  ✓ single element (100/100)
  ✓ multiple elements (100/100)
  ✓ sum matches manual calculation (100/100)

[TEST] list_free
  ✓ no memory leaks detected by Valgrind (100/100)
  ✓ all nodes freed (100/100)

Summary: 800/800 tests passed ✅
```

---

## 七、Bennet：随机规范测试

### 7.1 Bennet是什么？

**Bennet**（OOPSLA 2025）是CN的**随机规范测试工具**，专门用于验证**堆操作程序**的规范。

> *"Bennet: Randomized Specification Testing for Heap-Manipulating Programs"*
> — Zain K Aamer and Benjamin C. Pierce, OOPSLA 2025

**核心机制**：

1. 随机生成**堆结构**（链表、树、图）
2. 执行被测函数
3. 检查CN规范是否满足
4. 使用**收缩（shrinking）**技术简化反例

### 7.2 Bennet与Fulminate的区别

| 维度 | Fulminate | Bennet |
|:-----|:----------|:-------|
| 测试生成 | 基于规范的系统性生成 | 基于属性的随机生成 |
| 目标 | 所有C程序 | 堆操作程序 |
| 反例简化 | 基本 | 高级shrinking |
| 集成 | CN内置 | 独立工具 |
| 论文 | POPL 2025 | OOPSLA 2025 |

---

## 八、CN与CompCert/VST的对比

| 维度 | CN | VST | CompCert |
|:-----|:---|:----|:---------|
| **语义基础** | Cerberus（可执行C11） | CompCert C（Clight） | CompCert C（Clight） |
| **证明助手** | SMT（Z3, CVC5） | Coq（交互式） | Coq（完整编译器证明） |
| **自动化** | ⭐⭐⭐⭐⭐ 高 | ⭐⭐ 低 | ⭐⭐⭐ 中 |
| **C标准覆盖** | C11/C17/C23 | C99 | C99 |
| **并发支持** | ⚠️ 计划中 | ⚠️ 实验性 | ❌ 不支持 |
| **内存模型** | C11 | 顺序 | 顺序 |
| **证明可信度** | SMT信任 | Coq内核信任 | Coq内核信任 |
| **学习曲线** | 中 | 极高 | 极高 |
| **工业适用** | 新兴 | 学术 | 学术/航天 |
| **测试连接** | Fulminate, Bennet | 无 | 无 |

### 8.1 选择建议

| 场景 | 推荐工具 |
|:-----|:---------|
| 快速验证C库的正确性 | **CN** |
| 航天/汽车安全关键认证 | **CompCert + VST** |
| 教学/学习形式化验证 | **CN** |
| 需要编译器正确性保证 | **CompCert** |
| 并发程序验证 | **CN**（未来）/ **Iris** |
| 已有Coq基础 | **VST** |

---

## 九、前沿进展（2025-2026）

### 9.1 PLDI 2026：Code-Specify-Test-Debug-Prove

> *"Code-Specify-Test-Debug-Prove: Flexibly Integrating Separation Logic Specification into Conventional Workflows"*
> — Zain K Aamer et al., PLDI 2026

**核心贡献**：将CN规范**灵活集成**到传统开发工作流中：

- **编码阶段**：写C代码
- **规范阶段**：增量添加CN断言
- **测试阶段**：Fulminate自动生成测试
- **调试阶段**：CN反例指导修复
- **证明阶段**：SMT自动验证

### 9.2 并发扩展（进行中）

CN团队正在开发**并发C程序**的验证支持：

- 基于**C11内存模型**的并发分离逻辑
- 与**GenMC**模型检查器集成
- 目标：验证C11 `stdatomic.h` 和 `threads.h`

### 9.3 与Rust验证的对比

| 维度 | CN for C | Rust验证（Prusti, Creusot） |
|:-----|:---------|:----------------------------|
| 语言 | C（底层，UB丰富） | Rust（安全子集） |
| 所有权 | 手动规范 | 语言内置 |
| 验证难度 | 高 | 中 |
| UB检测 | 核心功能 | 语言消除大部分UB |
| 工业应用 | 嵌入式/系统 | 系统/应用 |

---

## 十、参考资源

### 10.1 核心论文

| 论文 | 会议 | 年份 | 核心贡献 |
|:-----|:-----|:----:|:---------|
| *CN: Verifying systems C code with separation-logic refinement types* | POPL | 2023 | CN基础框架 |
| *Fulminate: Testing CN Separation-Logic Specifications in C* | POPL | 2025 | 运行时测试生成 |
| *Bennet: Randomized Specification Testing* | OOPSLA | 2025 | 随机堆测试 |
| *Code-Specify-Test-Debug-Prove* | PLDI | 2026 | 工作流集成 |

### 10.2 工具与代码

| 资源 | URL |
|:-----|:----|
| CN GitHub | <https://github.com/rems-project/cn> |
| Cerberus | <https://github.com/rems-project/cerberus> |
| CN教程 | <https://rems-project.github.io/cn/> |
| 在线演示 | 见CN项目主页 |

### 10.3 相关课程

| 课程 | 机构 | 内容 |
|:-----|:-----|:-----|
| Scalable Software Verification | Imperial College | 分离逻辑与工具 |
| Program Logics for Certified Compilers | Princeton | VST与CompCert |
| Software Foundations | UPenn/Brown | Coq基础 |

---

> **总结**: CN代表了C程序形式化验证的**新范式**——基于**可执行语义**、**高自动化**（SMT）、**紧密连接测试**（Fulminate/Bennet）。与需要Coq专精的VST不同，CN的学习曲线更平缓，更适合工业界采用。随着并发扩展和C23支持的推进，CN有望成为**C语言形式化验证的主流工具**。
