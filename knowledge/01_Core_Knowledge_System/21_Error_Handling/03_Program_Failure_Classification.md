# 程序失败分类体系（Gustedt 四分类法）

> **标准**: ISO/IEC 9899:2024 (C23) Annex K, Jens Gustedt *Modern C* 3rd Ed. Chapter 1
> **前置知识**: [抽象状态机](../01_Basic_Layer/00_Abstract_State_Machine.md), [错误处理基础](README.md)
> **后续延伸**: [防御性编程](00_Error_Handling_Complete_Guide.md), [MISRA C安全标准](../09_Safety_Standards/MISRA_C_2023/README.md)
> **对标资源**: Modern C (Gustedt, INRIA/ISO WG14), NASA JPL Power of 10, CERT C
> **难度**: 3/5 | **预计学习时间**: 1-2 小时

---

## 目录

- [程序失败分类体系（Gustedt 四分类法）](#程序失败分类体系gustedt-四分类法)
  - [目录](#目录)
  - [一、引言](#一引言)
    - [1.1 C程序失败的现状](#11-c程序失败的现状)
    - [1.2 Gustedt的核心观点](#12-gustedt的核心观点)
  - [二、四分类法](#二四分类法)
    - [2.1 第一类：错误行为 (Wrongdoings)](#21-第一类错误行为-wrongdoings)
    - [2.2 第二类：性能退化 (Degradation)](#22-第二类性能退化-degradation)
    - [2.3 第三类：不幸事件 (Unfortunate Events)](#23-第三类不幸事件-unfortunate-events)
    - [2.4 第四类：编程失误 (Logic Errors / Bugs)](#24-第四类编程失误-logic-errors--bugs)
  - [三、处理策略](#三处理策略)
    - [3.1 四分类决策矩阵](#31-四分类决策矩阵)
    - [3.2 C23 `static_assert` 与 `_Static_assert`](#32-c23-static_assert-与-_static_assert)
    - [3.3 C23 Annex K 边界检查接口](#33-c23-annex-k-边界检查接口)
  - [四、与C语言特性的映射](#四与c语言特性的映射)
    - [4.1 标准库中的失败分类](#41-标准库中的失败分类)
    - [4.2 `errno` 值的分类](#42-errno-值的分类)
  - [五、工业实践对比](#五工业实践对比)
    - [5.1 NASA JPL Power of 10](#51-nasa-jpl-power-of-10)
    - [5.2 MISRA C:2023 映射](#52-misra-c2023-映射)
    - [5.3 Linux内核错误处理](#53-linux内核错误处理)
  - [六、防御性编程决策树](#六防御性编程决策树)
  - [七、参考资源](#七参考资源)
    - [7.1 核心文献](#71-核心文献)
    - [7.2 C23标准引用](#72-c23标准引用)
    - [7.3 工具支持](#73-工具支持)

---

## 一、引言

### 1.1 C程序失败的现状

C程序可能因多种原因失败，但传统的错误处理（返回错误码、设置errno）将**所有失败混为一谈**。这种模糊性导致：

- 开发者无法区分"可恢复错误"和"致命错误"
- 安全关键系统中，错误的恢复策略可能掩盖更严重的问题
- 调试时难以定位根本失败原因

### 1.2 Gustedt的核心观点

> *"Not all failures are equal. Understanding the taxonomy of program failure is the foundation of reliable C programming."*
> — Jens Gustedt, *Modern C* 3rd Ed.

Gustedt在 *Modern C* 中提出了程序失败的**四分类法**，将C程序中所有可能的失败归纳为四个互斥类别。这一分类体系：

- 指导**错误处理策略的选择**
- 决定**日志记录的级别**
- 定义**恢复操作的边界**
- 支撑**安全关键系统的故障分析**

---

## 二、四分类法

```
程序失败 (Program Failure)
├── 1. 错误行为 (Wrongdoings / Programming Errors)
├── 2. 性能退化 (Degradation / Resource Exhaustion)
├── 3. 不幸事件 (Unfortunate Events / External Failures)
└── 4. 编程失误 (Programming Errors / Logic Bugs)
```

> **注意**: 不同文献对第4类的命名略有差异。Gustedt原文使用"Programming Errors"涵盖第1类和第4类的某些方面。本节采用扩展的实用分类，将**前置条件违反**与**逻辑错误**区分开。

---

### 2.1 第一类：错误行为 (Wrongdoings)

**定义**：程序违反了**语言规则**或**平台约束**，导致抽象状态机进入**未定义行为（Undefined Behavior, UB）**状态。

**特征**：

- 违反C标准语义（如空指针解引用、整数溢出、数组越界）
- 违反平台约束（如栈溢出、对齐要求）
- **不可恢复**：一旦发生UB，程序状态已不可信

**典型示例**：

```c
// ❌ Wrongdoing: 空指针解引用 → UB
int* p = NULL;
*p = 42;  // 错误行为：违反C23 6.5.3.2

// ❌ Wrongdoing: 有符号整数溢出 → UB
int x = INT_MAX;
x = x + 1;  // 错误行为：违反C23 6.5/5

// ❌ Wrongdoing: 通过已释放指针写入 → UB
char* buf = malloc(100);
free(buf);
buf[0] = 'A';  // 错误行为：Use-after-free
```

**处理策略**：

| 场景 | 策略 |
|:-----|:-----|
| 开发/调试 | 使用ASan/UBSan捕获，立即终止并输出堆栈 |
| 生产环境 | 记录致命日志，调用 `quick_exit` 或 `abort`，**不尝试恢复** |
| 安全关键系统 | 触发看门狗或安全关机（Fail-Safe） |

> **原则**: Wrongdoing一旦发生，程序状态已不可信。任何恢复尝试都可能导致级联故障。

---

### 2.2 第二类：性能退化 (Degradation)

**定义**：程序仍在**定义行为**范围内运行，但**资源消耗**超出了预期或可用容量。

**特征**：

- 内存分配失败（`malloc` 返回 `NULL`）
- 文件描述符耗尽
- CPU/IO性能低于实时要求
- **可恢复**：资源问题可能通过释放、重试或降级解决

**典型示例**：

```c
// ⚠️ Degradation: 内存分配失败
void* buf = malloc(SIZE_MAX);  // 可能返回 NULL
if (buf == NULL) {
    // 资源耗尽，但程序状态仍可信
    // 策略：释放缓存、降级服务、或优雅退出
}

// ⚠️ Degradation: 文件打开失败（资源限制）
FILE* fp = fopen("/tmp/data", "w");
if (fp == NULL && errno == ENFILE) {
    // 文件描述符耗尽
    // 策略：关闭不用的文件，重试
}
```

**处理策略**：

| 场景 | 策略 |
|:-----|:-----|
| 内存不足 | 释放缓存、使用Arena分配器、优雅拒绝请求 |
| 文件描述符耗尽 | 关闭闲置句柄、限制并发连接数 |
| 实时性不满足 | 任务降级、跳过非关键帧、降低采样率 |
| 网络超时 | 指数退避重试、切换到备用节点 |

> **原则**: Degradation是**可管理的**。关键是建立资源预算（Resource Budgeting）和降级路径（Degradation Path）。

---

### 2.3 第三类：不幸事件 (Unfortunate Events)

**定义**：程序**正确执行**了所有操作，但**外部环境**导致结果不符合预期。

**特征**：

- 用户输入了无效数据
- 配置文件格式正确但内容不合逻辑
- 网络对端发送了协议错误的数据包
- 文件系统权限不足（但程序请求本身是合法的）
- **可恢复**：问题在程序外部，程序可以通过验证和拒绝来处理

**典型示例**：

```c
// ⚠️ Unfortunate Event: 用户输入无效
long val = strtol(user_input, &end, 10);
if (end == user_input || *end != '\0') {
    // 输入不是有效整数
    // 策略：提示用户重新输入
}

// ⚠️ Unfortunate Event: 配置文件存在但值不合理
if (config.max_threads == 0) {
    // 配置值不合逻辑（0线程无法工作）
    // 策略：使用默认值，记录警告
}

// ⚠️ Unfortunate Event: 对端发送了无效协议数据
if (packet.magic != PROTO_MAGIC) {
    // 数据包格式错误
    // 策略：丢弃数据包，记录统计
}
```

**与Degradation的区别**：

| 维度 | Degradation | Unfortunate Event |
|:-----|:------------|:------------------|
| 原因 | 资源不足（内部） | 外部输入/环境不符合预期 |
| 状态机 | 仍在定义状态 | 仍在定义状态 |
| 示例 | malloc返回NULL | 用户输入了负数年龄 |
| 恢复 | 释放资源、重试 | 验证、拒绝、使用默认值 |

**处理策略**：

| 场景 | 策略 |
|:-----|:-----|
| 用户输入无效 | 验证后立即拒绝，返回错误信息 |
| 配置不合理 | 使用安全默认值，记录警告日志 |
| 网络协议错误 | 丢弃数据包，记录统计，可能断开连接 |
| 权限不足 | 返回EPERM给用户，建议检查权限 |

> **原则**: Unfortunate Events是**预期内的**。程序应通过前置验证（Precondition Checking）和优雅拒绝（Graceful Rejection）来处理。

---

### 2.4 第四类：编程失误 (Logic Errors / Bugs)

**定义**：程序语法和语义完全正确，但**逻辑实现**与**意图**不符，导致**错误结果**。

**特征**：

- 算法实现错误（排序算法写错）
- 条件判断错误（`>` 写成了 `<`）
- 状态机转换错误
- **最难发现**：编译器和静态分析器通常无法检测
- **恢复策略**：取决于后果的严重程度

**典型示例**：

```c
// ❌ Logic Error: 条件方向错误
if (temperature > 100.0) {
    // 意图：高温告警
    // 实际：逻辑正确，但阈值可能是90.0
}

// ❌ Logic Error: 算法实现错误（冒泡排序缺少内层循环条件）
for (int i = 0; i < n; i++) {
    for (int j = 0; j < n; j++) {  // 应为 j < n - i - 1
        if (arr[j] > arr[j + 1]) {  // 当j=n-1时越界！
            // 这里同时触发了 Wrongdoing（越界）
        }
    }
}

// ❌ Logic Error: 整数除法截断
int average = sum / count;  // 如果sum < count，结果为0，可能不是预期
```

**与Wrongdoing的关联**：

逻辑错误常常**导致**错误行为。例如，错误的循环边界导致数组越界（UB）。但在分类时：

- **根本原因**：Logic Error（算法/逻辑写错）
- **直接表现**：Wrongdoing（数组越界→UB）

**处理策略**：

| 场景 | 策略 |
|:-----|:-----|
| 单元测试覆盖 | 通过测试用例捕获逻辑错误 |
| 断言（Assertion） | 在关键不变量处插入 `assert`，将Logic Error转化为可检测的失败 |
| 形式化验证 | 对核心算法使用CBMC或Frama-C验证 |
| 代码审查 | 交叉检查算法实现与设计文档的一致性 |

> **原则**: Logic Error是**开发者的问题**，不是程序运行时可恢复的。通过测试和验证在开发阶段消除。

---

## 三、处理策略

### 3.1 四分类决策矩阵

| 类别 | 是否可恢复 | 是否开发者错误 | 处理优先级 | 典型响应 |
|:-----|:----------:|:-------------:|:----------:|:---------|
| Wrongdoing | ❌ 否 | ✅ 是 | 🔴 P0 | 立即终止/安全关机 |
| Degradation | ⚠️ 是（有限） | ❌ 否 | 🟡 P1 | 资源释放/降级/重试 |
| Unfortunate Event | ✅ 是 | ❌ 否 | 🟢 P2 | 验证拒绝/默认值 |
| Logic Error | ❌ 否（根因） | ✅ 是 | 🔴 P0 | 测试/断言/验证 |

### 3.2 C23 `static_assert` 与 `_Static_assert`

```c
// 编译期捕获 Logic Error（配置不一致）
static_assert(sizeof(int) == 4, "假设int为32位");

// 运行期断言：将潜在的 Logic Error 转化为可控失败
#include <assert.h>

void process_buffer(const char* buf, size_t len) {
    assert(buf != NULL);           // Wrongdoing 预防
    assert(len <= MAX_BUF_SIZE);   // Degradation 预防
    // ...
}
```

### 3.3 C23 Annex K 边界检查接口

```c
#define __STDC_WANT_LIB_EXT1__ 1
#include <string.h>

errno_t copy_string(char* dst, rsize_t dstmax, const char* src) {
    if (dst == NULL || src == NULL) {
        // Unfortunate Event: 用户传入了NULL
        return EINVAL;
    }
    if (dstmax > RSIZE_MAX) {
        // Wrongdoing 预防：防止整数溢出攻击
        return ERANGE;
    }
    return strcpy_s(dst, dstmax, src);  // 边界安全复制
}
```

---

## 四、与C语言特性的映射

### 4.1 标准库中的失败分类

| 标准库函数 | 失败模式 | 分类 | 返回值 |
|:-----------|:---------|:-----|:-------|
| `malloc` | 内存不足 | Degradation | `NULL` |
| `fopen` | 文件不存在 | Unfortunate Event | `NULL`, `errno=ENOENT` |
| `fopen` | 权限不足 | Unfortunate Event | `NULL`, `errno=EACCES` |
| `fopen` | 描述符耗尽 | Degradation | `NULL`, `errno=ENFILE` |
| `printf` | 格式字符串错误 | Wrongdoing (UB) | 未定义 |
| `strcpy` | 目标缓冲区太小 | Wrongdoing (UB) | 缓冲区溢出 |
| `strcpy_s` | 目标缓冲区太小 | Degradation/Unfortunate | 返回非零，触发约束处理器 |
| `memcpy` | 源/目标重叠 | Wrongdoing (UB) | 未定义（应使用memmove） |
| `assert` | 条件为假 | Logic Error | 输出诊断信息，调用 `abort` |

### 4.2 `errno` 值的分类

```c
// Degradation 信号
ENOMEM  // 内存不足
ENFILE  // 文件描述符耗尽
EDQUOT  // 磁盘配额耗尽

// Unfortunate Event 信号
ENOENT  // 文件不存在
EACCES  // 权限不足
EINVAL  // 无效参数
EILSEQ  // 非法字节序列

// 可能关联 Wrongdoing
EFAULT  // 非法地址（几乎总是Bug）
EOVERFLOW // 值太大（可能是Degradation或Wrongdoing）
```

---

## 五、工业实践对比

### 5.1 NASA JPL Power of 10

| JPL规则 | 关联分类 | 目的 |
|:--------|:---------|:-----|
| Rule 1: 简单控制流 | Logic Error | 减少逻辑复杂性，降低错误概率 |
| Rule 2: 固定循环上界 | Wrongdoing | 防止无限循环（UB的一种形式） |
| Rule 3: 禁止运行时堆分配 | Degradation | 消除内存耗尽风险 |
| Rule 5: 最小断言密度 | Logic Error | 运行时捕获逻辑错误 |
| Rule 10: 零警告编译 | Wrongdoing | 编译期捕获潜在UB |

### 5.2 MISRA C:2023 映射

| MISRA规则 | 目标分类 | 机制 |
|:----------|:---------|:-----|
| Rule 9.1 | Wrongdoing | 禁止访问未初始化对象 |
| Rule 17.7 | Unfortunate Event | 必须检查所有返回值 |
| Rule 21.3 | Wrongdoing | 禁止使用已弃用函数（如gets） |
| Dir 4.7 | Degradation | 错误信息必须记录和报告 |

### 5.3 Linux内核错误处理

Linux内核使用 **`-errno` 返回码体系** 处理Degradation和Unfortunate Events：

```c
// 文件：fs/open.c
long do_sys_open(int dfd, const char __user *filename, int flags, umode_t mode)
{
    struct open_how how = build_open_how(flags, mode);
    return do_sys_openat2(dfd, filename, &how);
    // 返回 -ENOENT, -ENOMEM, -EACCES 等
    // 调用者分类：-ENOENT → Unfortunate, -ENOMEM → Degradation
}
```

对于Wrongdoing（如空指针解引用），内核使用 `BUG()` 或 `panic()` 终止。

---

## 六、防御性编程决策树

```
函数接收到参数/请求
    │
    ├── 是否为 NULL / 无效地址？
    │   ├── 是 → 检查是否为预期内的可选参数
    │   │   ├── 是 → 跳过操作（Unfortunate Event，可选处理）
    │   │   └── 否 → assert失败 / 立即返回错误（Wrongdoing预防）
    │   └── 否 → 继续
    │
    ├── 资源分配是否成功？
    │   ├── 否 → 资源耗尽（Degradation）
    │   │   ├── 可释放缓存？ → 释放后重试
    │   │   ├── 可降级？ → 降低服务质量
    │   │   └── 否则 → 返回错误码，请求上级处理
    │   └── 是 → 继续
    │
    ├── 输入值是否在合法范围内？
    │   ├── 否 → 验证失败（Unfortunate Event）
    │   │   ├── 有安全默认值？ → 使用默认值，记录警告
    │   │   └── 否则 → 返回 EINVAL
    │   └── 是 → 继续
    │
    ├── 算法执行结果是否符合不变量？
    │   ├── 否 → Logic Error（或触发了Wrongdoing）
    │   │   ├── Debug构建？ → assert失败，终止调试
    │   │   └── Release构建？ → 记录严重日志，进入安全模式
    │   └── 是 → 操作成功
    │
    └── 返回成功
```

---

## 七、参考资源

### 7.1 核心文献

| 资源 | 作者 | 年份 | 相关内容 |
|:-----|:-----|:----:|:---------|
| *Modern C*, 3rd Ed. | Jens Gustedt (INRIA/ISO WG14) | 2024 | 第1章：程序失败分类体系 |
| "The Power of 10" | Gerard Holzmann (NASA JPL) | 2006 | 安全编码规则与失败预防 |
| CERT C Coding Standard | SEI CMU | 2024 | 错误处理规则集 |
| MISRA C:2023 | MISRA WG | 2023 | 嵌入式失败处理规范 |

### 7.2 C23标准引用

- **6.5/5**: 表达式求值与UB
- **7.22.3**: 内存管理函数
- **Annex K**: 边界检查接口（`strcpy_s`, `memcpy_s`）
- **7.2/3**: `assert` 宏语义

### 7.3 工具支持

| 工具 | 捕获类别 | 使用方式 |
|:-----|:---------|:---------|
| AddressSanitizer | Wrongdoing（内存错误） | `gcc -fsanitize=address` |
| UBSan | Wrongdoing（UB行为） | `gcc -fsanitize=undefined` |
| Frama-C | Logic Error / Wrongdoing | 形式化验证 |
| CBMC | Logic Error | 有界模型检查 |
| clang-tidy | Logic Error / Wrongdoing | 静态分析规则 |

---

> **总结**: Gustedt四分类法将C程序失败划分为 **Wrongdoing（UB）**、**Degradation（资源耗尽）**、**Unfortunate Event（外部无效输入）** 和 **Logic Error（逻辑Bug）**。每一类有明确的处理策略：**UB不可恢复，资源问题可降级管理，外部问题通过验证拒绝，逻辑错误通过测试和断言在开发期消除**。这一分类体系是构建可靠C程序的**元框架**。
