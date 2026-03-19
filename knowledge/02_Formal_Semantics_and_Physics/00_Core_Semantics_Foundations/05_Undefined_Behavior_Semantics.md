# 未定义行为语义边界 (Undefined Behavior Semantics)

> **难度**: L6 | **预估学习时间**: 8-10小时
> **参考**: C标准 §3.4.3, Wang et al. (2012), Cerberus, Memarian et al. (2016)

---

## 1. 未定义行为概述

### 1.1 什么是未定义行为(UB)

根据C23标准 §3.4.3：

> **未定义行为 (Undefined Behavior)**: 使用不可移植或错误的程序构造或数据，标准对此不施加任何要求。

### 1.2 UB的分类

| 类别 | 描述 | 示例 |
|:-----|:-----|:-----|
| **Critical UB** | 可能导致任意后果 | 越界写、空指针解引用 |
| **Timing UB** | 影响执行时间 | 有符号溢出假设 |
| **Informational UB** | 影响结果但不崩溃 | 未初始化值使用 |
| **Library UB** | 库函数误用 | memcpy重叠区域 |

### 1.3 UB在形式语义中的表示

**操作语义**:

```text
⟨e, σ⟩ → error    （显式错误状态）
或
⟨e, σ⟩ → ⊥        （无定义）
或
⟨e, σ⟩ → any σ'   （非确定性：任意结果）
```

**指称语义**:

```text
⟦e⟧ = ⊥           （底部值）
或
⟦e⟧ = λσ. ⊤       （顶部：任意结果）
```

---


---

## 📑 目录

- [未定义行为语义边界 (Undefined Behavior Semantics)](#未定义行为语义边界-undefined-behavior-semantics)
  - [1. 未定义行为概述](#1-未定义行为概述)
    - [1.1 什么是未定义行为(UB)](#11-什么是未定义行为ub)
    - [1.2 UB的分类](#12-ub的分类)
    - [1.3 UB在形式语义中的表示](#13-ub在形式语义中的表示)
  - [📑 目录](#-目录)
  - [2. C语言UB清单与形式化](#2-c语言ub清单与形式化)
    - [2.1 整数运算UB](#21-整数运算ub)
      - [有符号整数溢出](#有符号整数溢出)
      - [除以零](#除以零)
      - [移位操作违规](#移位操作违规)
    - [2.2 内存访问UB](#22-内存访问ub)
      - [空指针解引用](#空指针解引用)
      - [越界访问](#越界访问)
      - [使用已释放内存](#使用已释放内存)
    - [2.3 严格别名违规](#23-严格别名违规)
      - [规则形式化](#规则形式化)
    - [2.4 未初始化值](#24-未初始化值)
  - [3. 实现定义行为(IDB)与未指定行为](#3-实现定义行为idb与未指定行为)
    - [3.1 三类行为对比](#31-三类行为对比)
    - [3.2 实现定义行为列表](#32-实现定义行为列表)
    - [3.3 未指定行为示例](#33-未指定行为示例)
  - [4. UB的语义模型](#4-ub的语义模型)
    - [4.1 三种语义处理方式](#41-三种语义处理方式)
      - [1. 显式错误 (Explicit Error)](#1-显式错误-explicit-error)
      - [2. 非确定性 (Nondeterminism)](#2-非确定性-nondeterminism)
      - [3. 编译器优化视角](#3-编译器优化视角)
    - [4.2 Cerberus语义模型](#42-cerberus语义模型)
  - [5. UB的检测与防护](#5-ub的检测与防护)
    - [5.1 编译器检测](#51-编译器检测)
    - [5.2 形式化验证](#52-形式化验证)
    - [5.3 MISRA C与UB规避](#53-misra-c与ub规避)
  - [6. C23对UB的改进](#6-c23对ub的改进)
    - [6.1 标准化行为](#61-标准化行为)
    - [6.2 分析性扩展](#62-分析性扩展)
    - [6.3 安全函数](#63-安全函数)
  - [7. UB的形式化推理](#7-ub的形式化推理)
    - [7.1 分离逻辑中的UB](#71-分离逻辑中的ub)
    - [7.2 程序等价性](#72-程序等价性)
    - [7.3 编译器优化与UB](#73-编译器优化与ub)
  - [8. 实际UB案例分析](#8-实际ub案例分析)
    - [8.1 有符号溢出与时间旅行优化](#81-有符号溢出与时间旅行优化)
    - [8.2 别名违规与向量化](#82-别名违规与向量化)
    - [8.3 未初始化值与分支](#83-未初始化值与分支)
  - [9. 安全C方言](#9-安全c方言)
    - [9.1 Checked C (Microsoft)](#91-checked-c-microsoft)
    - [9.2 CHERI能力架构](#92-cheri能力架构)
  - [10. 总结与最佳实践](#10-总结与最佳实践)
    - [10.1 UB规避清单](#101-ub规避清单)
    - [10.2 形式化验证策略](#102-形式化验证策略)
  - [11. 参考文献](#11-参考文献)


---

## 2. C语言UB清单与形式化

### 2.1 整数运算UB

#### 有符号整数溢出

```c
int x = INT_MAX;
x = x + 1;  // UB！
```

**形式化**:

```text
⊢ e₁ : signed_int(n₁)
⊢ e₂ : signed_int(n₂)
n₁ + n₂ ∉ [-2³¹, 2³¹-1]
─────────────────────────────────
⟦e₁ + e₂⟧ = UB
```

#### 除以零

```text
⊢ e₂ : int(0)
─────────────────────────────────
⟦e₁ / e₂⟧ = UB
```

#### 移位操作违规

```text
移位位数 < 0 或 ≥ 类型宽度  → UB
右移负数                   → 实现定义或UB
```

### 2.2 内存访问UB

#### 空指针解引用

```c
int *p = NULL;
*p = 42;  // UB！
```

**形式化**:

```text
⟦p⟧ = null
─────────────────────────────────
⟦*p⟧ = UB
```

#### 越界访问

```c
int a[10];
a[10] = 0;  // UB！（数组索引0-9）
```

**形式化**:

```text
p : ptr(array(τ, n))
i ≥ n ∨ i < 0
─────────────────────────────────
⟦p[i]⟧ = UB
```

#### 使用已释放内存

```c
int *p = malloc(sizeof(int));
free(p);
*p = 42;  // UB！（use-after-free）
```

**形式化**（使用分离逻辑）:

```text
{ p ↦ - } free(p) { emp }
{ emp } *p = 42 { UB }  // p不在有效堆中
```

### 2.3 严格别名违规

#### 规则形式化

C标准 §6.5/7 (有效类型规则):

```text
对象具有有效类型 T
g : ptr(S)
S ≠ T 且 S 不是 char/unsigned char
─────────────────────────────────
⟦*g⟧ = UB  （违反严格别名规则）
```

**违规示例**:

```c
float f = 3.14f;
int* pi = (int*)&f;
int i = *pi;         // UB: float* 通过 int* 访问
```

**合法方式**:

```c
union { float f; int i; } u;
u.f = 3.14f;
int i = u.i;         // OK: 通过union访问
```

### 2.4 未初始化值

```c
int x;
printf("%d\n", x);  // UB！
```

**形式化**:

```text
σ(x) = ⊥  （未初始化标记）
─────────────────────────────────
⟦x⟧ = UB  （如果使用值）
```

**注意**: C23引入了 `__STDC_ANALYZABLE__` 可分析性扩展。

---

## 3. 实现定义行为(IDB)与未指定行为

### 3.1 三类行为对比

| 类型 | 标准保证 | 文档要求 | 示例 |
|:-----|:---------|:---------|:-----|
| **未定义(UB)** | 无保证 | 无 | 有符号溢出 |
| **未指定(Unspecified)** | 结果有限集 | 无 | 求值顺序 |
| **实现定义(IDB)** | 结果有限集 | 必须文档化 | int大小 |

### 3.2 实现定义行为列表

```text
- 基本类型的大小和对齐 (C23 §5.2.4.2.1)
- 有符号整数表示（补码/反码/原码）- C23只要求补码
- 枚举的整数类型选择
- 位域的存储布局
```

### 3.3 未指定行为示例

```c
f() + g();  // f()和g()的调用顺序未指定
```

**形式化**:

```text
⟦f() + g()⟧ ∈ { ⟦f()⟧ + ⟦g()⟧, ⟦g()⟧ + ⟦f()⟧ }
```

---

## 4. UB的语义模型

### 4.1 三种语义处理方式

#### 1. 显式错误 (Explicit Error)

```text
配置包含错误状态：
⟨e, σ⟩ → ⟨error, σ⟩ 或 stuck
```

**优点**: 清晰、可调试
**缺点**: 不反映实际C（允许编译器假设无UB）

#### 2. 非确定性 (Nondeterminism)

```text
⟦UB⟧ = { v | v ∈ Value }  （任意值）
```

**优点**: 捕捉C标准的"任意行为"描述
**缺点**: 难以推理

#### 3. 编译器优化视角

**关键洞察**: 编译器可假设UB永不发生。

```text
如果程序P包含可达的UB，则编译器可：
- 删除P中该路径后的所有代码
- 重新排列内存访问
- 假设条件永不为真/假
```

**示例**:

```c
void f(int* p) {
    int x = *p;      // 如果p为null，这是UB
    if (p == NULL)   // 编译器可假设永不为真
        return;      // 可删除此分支
    // ...
}
```

### 4.2 Cerberus语义模型

Cerberus项目提供了C的**可执行语义**，明确定义UB：

```text
Core是C的简化中间语言

C程序 → Core → 执行 → { 值, UB, 未指定 }
```

**UB分类 (Cerberus)**:

- `Undefined_error`: 标准UB
- `Implementation_defined`: 实现定义
- `Unspecified`: 未指定行为

---

## 5. UB的检测与防护

### 5.1 编译器检测

| 工具 | 方法 | 覆盖范围 |
|:-----|:-----|:---------|
| **UBSan** | 运行时检查 | 算术UB、对齐、对象边界 |
| **MSAN** | 内存检查 | 未初始化值 |
| **ASan** | 内存检查 | use-after-free、越界 |
| **TSan** | 线程检查 | 数据竞争 |

### 5.2 形式化验证

**CompCert方法**:

```coq
(* 源语言语义明确排除UB *)
Inductive outcome : Type :=
  | Out_normal : outcome
  | Out_exit : outcome
  | Out_return : option val -> outcome
  | Out_ub : string -> outcome.  (* UB标记 *)

(* 证明编译器不引入新UB *)
Theorem preservation_no_ub:
  forall s t,
  compile s = t ->
  no_ub s -> no_ub t.
```

### 5.3 MISRA C与UB规避

MISRA C规则直接针对UB：

```text
Rule 12.2: 移位操作必须保证在有效范围
Rule 13.3: 除法前必须检查除数非零
Rule 9.1: 变量使用前必须初始化
```

---

## 6. C23对UB的改进

### 6.1 标准化行为

C23将部分历史UB改为**实现定义**或**指定**：

| 旧行为 | C23状态 | 原因 |
|:-------|:--------|:-----|
| 有符号整数表示 | 仅允许补码 | 所有现代系统已使用补码 |
| `_Bool`转换 | 明确定义 | 简化语义 |

### 6.2 分析性扩展

C23引入 `__STDC_ANALYZABLE__`:

```c
#ifdef __STDC_ANALYZABLE__
// 编译器支持额外的可分析性保证
// 如：程序流分析友好的语义
#endif
```

### 6.3 安全函数

边界检查接口 (Annex K，可选):

```c
errno_t strcpy_s(char* dest, rsize_t destsz, const char* src);
// 运行时检查 destsz，溢出时调用约束处理程序
```

---

## 7. UB的形式化推理

### 7.1 分离逻辑中的UB

```text
错误状态 = 假 (False)
{ false } S { Q }  对任何S和Q都有效（爆炸原理）
```

**Hoare逻辑**:

```text
如果前置条件允许UB发生，
则任何后置条件都可证明。

这对应编译器"UB永不发生"假设。
```

### 7.2 程序等价性

**UB敏感等价**:

```text
P₁ ~ P₂ 如果对于所有输入：
- 都无UB且结果相同，或
- 都UB，或
- 一个有UB一个无（不等价）
```

**示例**:

```c
// 程序A
int f(int x) {
    return x + 1 > x;  // 可能溢出UB
}

// 程序B
int f(int x) {
    return 1;          // 总是返回1
}

// 不等价：当x = INT_MAX时，A有UB而B返回1
```

### 7.3 编译器优化与UB

**优化规则示例**:

```text
(* 代数简化 *)
⟦x - x⟧ → ⟦0⟧
条件: x无UB（如不是trap表示）

(* 空指针检查删除 *)
if (p == NULL) return;  // 可删除
条件: *p被解引用，编译器假设p≠NULL
```

---

## 8. 实际UB案例分析

### 8.1 有符号溢出与时间旅行优化

```c
int f(int x) {
    if (x < x + 1) {
        // 编译器可假设此分支总是执行
        // 因为有符号溢出是UB
        return 1;
    }
    return 0;
}
// GCC/O2: 总是返回1
```

### 8.2 别名违规与向量化

```c
void add(int* a, int* b, int n) {
    for (int i = 0; i < n; i++)
        a[i] += b[i];
}
```

如果 `a` 和 `b` 重叠，严格别名规则被违反。

**Clang/LLVM**: 使用 `noalias` 元数据假设无重叠，进行激进向量化。

### 8.3 未初始化值与分支

```c
int x;
if (some_condition)
    x = 5;
// 未检查some_condition
return x;  // UB如果some_condition为假
```

---

## 9. 安全C方言

### 9.1 Checked C (Microsoft)

扩展C提供边界检查指针：

```c
int *p : count(n);  // p指向n个int的数组
```

编译时插入运行时检查，UB转为可捕获错误。

### 9.2 CHERI能力架构

硬件级内存安全：

```text
指针 = 地址 + 界限 + 权限
```

越界访问触发硬件异常，UB转为确定性陷阱。

---

## 10. 总结与最佳实践

### 10.1 UB规避清单

```text
□ 使用无符号类型进行位运算
□ 检查除数非零
□ 检查数组边界
□ 初始化所有变量
□ 避免指针类型双关
□ 使用static_assert验证编译时常量
□ 启用编译器警告 (-Wall -Wextra -Werror)
□ 使用UBSan/ASan测试
```

### 10.2 形式化验证策略

```text
1. 用CompCert编译关键代码
2. 用Frama-C/WP验证Hoare三元组
3. 用VST验证指针操作
4. 用CBMC模型检查边界情况
```

---

## 11. 参考文献

1. **ISO/IEC 9899:2023**, §3.4.3 Undefined Behavior
2. **Wang, X.** et al. (2012). "Undefined Behavior: What Happened to My Code?" *APSYS*.
3. **Memarian, K.** et al. (2016). "Into the Depths of C: Elaborating the De Facto Standards". *PLDI*.
4. **Cerberus Documentation**: <https://www.cl.cam.ac.uk/~pes20/cerberus/>
5. **LLVM Language Reference**: Undefined Behavior section
6. **Seacord, R.** (2013). *Secure Coding in C and C++*. Addison-Wesley.
7. **MISRA C:2012 Guidelines**

---

> **关联文档**: [C类型理论](./04_C_Type_Theory.md) | [C11内存模型](../01_Game_Semantics/02_C11_Memory_Model.md) | [CompCert验证](../11_CompCert_Verification/01_Compcert_Overview.md)


---

## 深入理解

### 核心概念

本主题的核心概念包括：基础理论、实现机制、实际应用。

### 实践应用

- 应用场景1
- 应用场景2
- 应用场景3

### 学习建议

1. 先理解基础概念
2. 再进行实践练习
3. 最后深入源码

---

> **最后更新**: 2026-03-21  
> **维护者**: AI Code Review
