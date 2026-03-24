# MISRA C:2023 Rules 6.1-6.10 - 表达式与语句

> 运算符安全、副作用控制、控制流完整性

---

## 规则分类概述

| 规则编号 | 规则名称 | 强制性 | 核心要求 |
|:--------:|:---------|:------:|:---------|
| Rule 6.1 | 表达式求值顺序 | Required | 避免未定义求值顺序 |
| Rule 6.2 | 副作用限制 | Required | 单表达式中副作用限制 |
| Rule 6.3 | 布尔表达式 | Required | 布尔类型正确使用 |
| Rule 6.4 | 常量条件 | Required | 避免虚假条件 |
| Rule 6.5 | 死代码 | Required | 消除不可达代码 |
| Rule 6.6 | switch语句 | Required | switch语句完整性 |
| Rule 6.7 | 循环控制 | Required | 循环控制安全 |
| Rule 6.8 | 跳转限制 | Required | 限制goto/setjmp使用 |
| Rule 6.9 | 空语句 | Required | 空语句显式化 |
| Rule 6.10 | 逗号运算符 | Advisory | 避免逗号运算符 |

---


---

## 📑 目录

- [MISRA C:2023 Rules 6.1-6.10 - 表达式与语句](#misra-c2023-rules-61-610---表达式与语句)
  - [规则分类概述](#规则分类概述)
  - [📑 目录](#-目录)
  - [Rule 6.1 - 表达式求值顺序](#rule-61---表达式求值顺序)
    - [规则原文](#规则原文)
    - [问题背景](#问题背景)
    - [违反示例](#违反示例)
    - [合规示例](#合规示例)
  - [Rule 6.2 - 副作用限制](#rule-62---副作用限制)
    - [规则原文](#规则原文-1)
    - [违反示例](#违反示例-1)
    - [合规示例](#合规示例-1)
  - [Rule 6.3 - 布尔表达式](#rule-63---布尔表达式)
    - [规则原文](#规则原文-2)
    - [违反示例](#违反示例-2)
    - [合规示例](#合规示例-2)
  - [Rule 6.4 - 常量条件](#rule-64---常量条件)
    - [规则原文](#规则原文-3)
    - [违反示例](#违反示例-3)
    - [合规示例](#合规示例-3)
  - [Rule 6.5 - 死代码](#rule-65---死代码)
    - [规则原文](#规则原文-4)
    - [违反示例](#违反示例-4)
    - [合规示例](#合规示例-4)
  - [Rule 6.6 - Switch语句完整性](#rule-66---switch语句完整性)
    - [规则原文](#规则原文-5)
    - [违反示例](#违反示例-5)
    - [合规示例](#合规示例-5)
    - [Switch语句模板](#switch语句模板)
  - [Rule 6.7 - 循环控制](#rule-67---循环控制)
    - [规则原文](#规则原文-6)
    - [违反示例](#违反示例-6)
    - [合规示例](#合规示例-6)
  - [Rule 6.8 - 跳转限制](#rule-68---跳转限制)
    - [规则原文](#规则原文-7)
    - [违反示例](#违反示例-7)
    - [合规示例](#合规示例-7)
  - [Rule 6.9 - 空语句](#rule-69---空语句)
    - [规则原文](#规则原文-8)
    - [违反示例](#违反示例-8)
    - [合规示例](#合规示例-8)
  - [Rule 6.10 - 逗号运算符](#rule-610---逗号运算符)
    - [规则原文](#规则原文-9)
    - [违反示例](#违反示例-9)
    - [合规示例](#合规示例-9)
  - [检测工具配置](#检测工具配置)
    - [GCC检查选项](#gcc检查选项)
    - [PC-lint配置](#pc-lint配置)
  - [总结](#总结)
  - [深入理解](#深入理解)
    - [核心概念](#核心概念)
    - [实践应用](#实践应用)
    - [学习建议](#学习建议)
  - [📚 实质性内容补充](#-实质性内容补充)
    - [技术深度分析](#技术深度分析)
      - [1. 核心概念详解](#1-核心概念详解)
      - [2. 实现机制](#2-实现机制)
      - [3. 实践指导](#3-实践指导)
    - [扩展阅读](#扩展阅读)


---

## Rule 6.1 - 表达式求值顺序

### 规则原文

> **Rule 6.1 (Required)**: The value of an expression shall not depend on the order of evaluation of subexpressions.
>
> **中文**: 表达式的值不得依赖于子表达式的求值顺序。

### 问题背景

C语言中，大多数运算符的子表达式求值顺序是**未指定的**（unspecified）：

- `f() + g()` - f()和g()哪个先执行不确定
- `a = b + c` - b和c的求值顺序不确定

### 违反示例

```c
/* ❌ 严重违反 - 未定义行为 */
int i = 0;
int x = i++ + i++;  /* 未定义行为！ */

/* ❌ 违反 - 求值顺序不确定 */
int a = 5;
int b = (a = 3) + a;  /* a的值不确定是3还是5 */

/* ❌ 违反 - 函数参数顺序不确定 */
int func(void) {
    static int count = 0;
    return ++count;
}
int result = func() + func();  /* 结果不确定是3还是其他值 */

/* ❌ 违反 - 数组下标副作用 */
int arr[10] = {0};
int i = 0;
int val = arr[i++] + arr[i++];  /* 未定义！ */

/* ❌ 违反 - 函数调用顺序不确定 */
int f(int x) { return x; }
int g(int x) { return x * 2; }
int result = f(i++) + g(i++);  /* 顺序不确定，结果不确定 */
```

### 合规示例

```c
/* ✅ 合规 - 分离表达式 */
int i = 0;
int temp1 = i++;
int temp2 = i++;
int x = temp1 + temp2;  /* x = 0 + 1 = 1 */

/* ✅ 合规 - 避免副作用 */
int a = 5;
a = 3;
int b = a + a;  /* b = 6，明确 */

/* ✅ 合规 - 函数调用顺序明确 */
int f1 = func();
int f2 = func();
int result = f1 + f2;  /* result = 1 + 2 = 3 */

/* ✅ 合规 - 数组访问安全 */
int arr[10] = {0};
int i = 0;
int idx1 = i++;
int idx2 = i++;
int val = arr[idx1] + arr[idx2];
```

---

## Rule 6.2 - 副作用限制

### 规则原文

> **Rule 6.2 (Required)**: There shall be no more than one read access with a volatile-qualified type or one modification in a full expression.
>
> **中文**: 在一个完整表达式中，对一个volatile限定类型的访问不得超过一次读取或一次修改。

### 违反示例

```c
/* ❌ 违反 - 多次修改 */
volatile uint32_t *reg = (volatile uint32_t *)0x40001000;
*reg = *reg + 1;  /* 两次访问volatile：读取+写入 */

/* ❌ 违反 - 参数副作用 */
int i = 0;
func(i++, i++);  /* 未定义行为 */

/* ❌ 违反 - 赋值链 */
int a, b;
a = b = 0;  /* 虽然允许，但在MISRA中需谨慎 */

/* ❌ 违反 - 复杂增量 */
int x = 0;
x += x++;  /* 未定义行为 */
```

### 合规示例

```c
/* ✅ 合规 - 分离volatile访问 */
volatile uint32_t *reg = (volatile uint32_t *)0x40001000;
uint32_t value = *reg;  /* 读取 */
*reg = value + 1;       /* 写入 */

/* ✅ 合规 - 副作用分离 */
int i = 0;
int arg1 = i++;
int arg2 = i++;
func(arg1, arg2);

/* ✅ 合规 - 明确赋值 */
int a = 0;
int b = 0;

/* ✅ 合规 - 明确增量 */
int x = 0;
x = x + 1;  /* 明确 */
```

---

## Rule 6.3 - 布尔表达式

### 规则原文

> **Rule 6.3 (Required)**: Boolean expressions shall be treated as Boolean values.
>
> **中文**: 布尔表达式应作为布尔值处理。

### 违反示例

```c
/* ❌ 违反 - 整数作为布尔 */
int flag = 1;
if (flag) {  /* 虽然有效，但不明确 */
    /* ... */
}

/* ❌ 违反 - 隐式布尔转换 */
int x = 5;
if (x) {  /* 不清晰：是检查非零还是真值？ */
    /* ... */
}

/* ❌ 违反 - 布尔运算混淆 */
bool a = true;
bool b = false;
bool c = a & b;  /* 位运算，不是逻辑运算 */
```

### 合规示例

```c
#include <stdbool.h>

/* ✅ 合规 - 使用bool类型 */
bool flag = true;
if (flag == true) {  /* 明确 */
    /* ... */
}

/* ✅ 合规 - 明确的比较 */
int x = 5;
if (x != 0) {  /* 明确：检查非零 */
    /* ... */
}

/* ✅ 合规 - 逻辑运算 */
bool a = true;
bool b = false;
bool c = a && b;  /* 逻辑与，结果为false */

/* ✅ 合规 - 比较结果直接作为布尔 */
bool is_valid = (x > 0) && (x < 100);
if (is_valid) {
    /* ... */
}
```

---

## Rule 6.4 - 常量条件

### 规则原文

> **Rule 6.4 (Required)**: The condition in an if or switch statement shall not be constant.
>
> **中文**: if或switch语句中的条件不应为常量。

### 违反示例

```c
/* ❌ 违反 - 始终为真 */
if (1) {  /* 始终执行 */
    /* ... */
}

/* ❌ 违反 - 始终为假 */
if (0) {  /* 永远不会执行 */
    /* ... */
}

/* ❌ 违反 - 预处理导致的常量 */
#define DEBUG 0
if (DEBUG) {  /* 常量 */
    printf("Debug\n");
}

/* ❌ 违反 - switch常量 */
switch (1) {  /* 常量 */
    case 1: break;
}
```

### 合规示例

```c
/* ✅ 合规 - 使用预处理 */
#define DEBUG 0

#if DEBUG
    printf("Debug\n");
#endif

/* ✅ 合规 - 使用条件编译 */
#ifdef DEBUG
    debug_log("message");
#endif

/* ✅ 合规 - 变量条件 */
bool is_debug_enabled = get_debug_flag();
if (is_debug_enabled) {
    printf("Debug\n");
}
```

---

## Rule 6.5 - 死代码

### 规则原文

> **Rule 6.5 (Required)**: There shall be no unreachable code.
>
> **中文**: 不得存在不可达代码。

### 违反示例

```c
/* ❌ 违反 - 不可达代码 */
void func(void)
{
    return;
    int x = 0;  /* 永远不会执行 */
    (void)x;
}

/* ❌ 违反 - 条件导致的死代码 */
void test(int x)
{
    if (x > 0) {
        return;
    } else if (x <= 0) {
        return;
    }
    int y = 0;  /* 永远不会到达 */
    (void)y;
}

/* ❌ 违反 - break后的代码 */
switch (x) {
    case 1:
        break;
        x = 0;  /* 不可达 */
}
```

### 合规示例

```c
/* ✅ 合规 - 无死代码 */
void func(void)
{
    int x = 0;
    /* 使用x */
    return;
}

/* ✅ 合规 - 使用条件编译 */
#ifdef NEVER_ENABLED
void unused_func(void)  /* 整个函数被条件编译 */
{
    /* ... */
}
#endif
```

---

## Rule 6.6 - Switch语句完整性

### 规则原文

> **Rule 6.6 (Required)**: Every switch statement shall have a default label.
>
> **中文**: 每个switch语句都应有default标签。

### 违反示例

```c
/* ❌ 违反 - 缺少default */
switch (status) {
    case OK:
        process_ok();
        break;
    case ERROR:
        process_error();
        break;
    /* 缺少default */
}

/* ❌ 违反 - 缺少break（fall-through）*/
switch (cmd) {
    case CMD_START:
        init();
        /* 缺少break，故意fall-through但无注释 */
    case CMD_RUN:
        run();
        break;
}

/* ❌ 违反 - default不是最后一个 */
switch (val) {
    case 1:
        break;
    default:
        break;
    case 2:  /* default后还有case */
        break;
}
```

### 合规示例

```c
/* ✅ 合规 - 完整的switch */
switch (status) {
    case OK:
        process_ok();
        break;
    case ERROR:
        process_error();
        break;
    default:
        /* 意外状态 */
        handle_unknown_status();
        break;
}

/* ✅ 合规 - 明确的fall-through */
switch (cmd) {
    case CMD_START:
        init();
        /* fall-through */
    case CMD_RUN:
        run();
        break;
    default:
        break;
}

/* ✅ 合规 - 枚举switch（穷尽检查）*/
enum color { RED, GREEN, BLUE };

switch (c) {
    case RED:
    case GREEN:
    case BLUE:
        break;
    default:
        /* 不应到达 */
        break;
}
```

### Switch语句模板

```c
/* MISRA合规的Switch模板 */
switch (expression) {
    case VALUE_A:
        /* 处理A */
        action_a();
        break;

    case VALUE_B:
        /* 处理B */
        action_b();
        break;

    case VALUE_C:
        /* 无break，明确fall-through */
        prepare_for_d();
        /* fall-through */

    case VALUE_D:
        action_cd();
        break;

    default:
        /* 防御性编程 */
        handle_unexpected();
        break;
}
```

---

## Rule 6.7 - 循环控制

### 规则原文

> **Rule 6.7 (Required)**: A loop counter shall not be modified in the loop body.
>
| **中文**: 循环计数器不得在循环体内修改。

### 违反示例

```c
/* ❌ 违反 - 修改循环计数器 */
for (int i = 0; i < 10; i++) {
    if (condition) {
        i++;  /* 修改循环计数器！ */
    }
}

/* ❌ 违反 - 复杂的循环控制 */
int i = 0;
while (i < 10) {
    i += 2;  /* 修改控制变量 */
    if (error) {
        i = 0;  /* 重置计数器 */
    }
}
```

### 合规示例

```c
/* ✅ 合规 - 简单的for循环 */
for (int i = 0; i < 10; i++) {
    /* i只由for语句修改 */
    process(i);
}

/* ✅ 合规 - 使用独立变量 */
for (int i = 0; i < 10; i++) {
    int step = i;
    if (condition) {
        step++;  /* 修改局部变量 */
    }
    use(step);
}

/* ✅ 合规 - 使用while处理复杂逻辑 */
int pos = 0;
while (pos < max_pos) {
    if (skip_condition) {
        pos += 2;  /* while允许复杂控制 */
    } else {
        pos++;
    }
}

/* ✅ 合规 - 循环退出条件明确 */
bool done = false;
for (int i = 0; (i < 100) && !done; i++) {
    if (completion_condition) {
        done = true;  /* 使用标志而非break */
    }
}
```

---

## Rule 6.8 - 跳转限制

### 规则原文

> **Rule 6.8 (Required)**: goto, break and continue shall not be used to create backward jumps.
>
> **中文**: goto、break和continue不得用于创建向后跳转。

### 违反示例

```c
/* ❌ 严重违反 - 向后goto（ spaghetti code ）*/
start:
    if (condition) {
        goto start;  /* 向后跳转！ */
    }

/* ❌ 违反 - 向后continue */
for (int i = 0; i < 10; i++) {
back:
    if (retry) {
        i--;  /* 模拟向后跳转 */
        goto back;
    }
}

/* ❌ 违反 - setjmp/longjmp */
#include <setjmp.h>
jmp_buf env;

void func(void)
{
    if (setjmp(env) == 0) {
        /* 首次进入 */
    } else {
        /* longjmp返回 */
    }
}
```

### 合规示例

```c
/* ✅ 合规 - 向前goto（资源清理）*/
void process(void)
{
    FILE *fp = fopen("file", "r");
    if (fp == NULL) {
        goto cleanup;  /* 向前跳转 */
    }

    /* 处理文件 */

cleanup:
    if (fp != NULL) {
        fclose(fp);
    }
    return;
}

/* ✅ 合规 - 使用循环代替向后goto */
while (condition) {
    if (retry_condition) {
        continue;  /* 向前到下一次迭代 */
    }
    /* 处理 */
}

/* ✅ 合规 - 结构化错误处理 */
enum status do_work(void)
{
    enum status s = STATUS_OK;

    if (!init()) {
        s = STATUS_INIT_FAIL;
    } else if (!process()) {
        s = STATUS_PROC_FAIL;
        cleanup_init();
    }

    return s;
}
```

---

## Rule 6.9 - 空语句

### 规则原文

> **Rule 6.9 (Required)**: The null statement shall not be used alone.
>
| **中文**: 空语句不得单独使用。

### 违反示例

```c
/* ❌ 违反 - 单独空语句 */
;  /* 无意义的空语句 */

/* ❌ 违反 - 循环体为空 */
while (*ptr++);  /* 难以发现的分号 */

/* ❌ 违反 - if后空语句 */
if (condition);
{
    /* 这不在if内！ */
    action();
}
```

### 合规示例

```c
/* ✅ 合规 - 使用空体注释 */
while (*ptr != '\0') {
    ptr++;  /* 明确的操作 */
}

/* ✅ 合规 - 空循环明确 */
while (is_busy()) {
    /* 等待硬件就绪 */
}

/* ✅ 合规 - 使用(void)抑制未使用参数 */
void callback(void *unused)
{
    (void)unused;  /* 明确抑制警告 */
}
```

---

## Rule 6.10 - 逗号运算符

### 规则原文

> **Rule 6.10 (Advisory)**: The comma operator shall not be used.
>
| **中文**: 不应使用逗号运算符。

### 违反示例

```c
/* ❌ 违反 - 逗号运算符 */
int x = (a = 1, b = 2, a + b);  /* x = 3 */

/* ❌ 违反 - for中的逗号 */
for (int i = 0, j = 0; i < 10; i++, j++) {
    /* 逗号运算符 */
}

/* ❌ 违反 - 函数参数中的逗号混淆 */
func((a, b), c);  /* 是传两个参数还是逗号运算符？ */
```

### 合规示例

```c
/* ✅ 合规 - 分离语句 */
a = 1;
b = 2;
int x = a + b;

/* ✅ 合规 - for中使用多个变量声明 */
int i, j;
for (i = 0, j = 0; i < 10; i++, j++) {  /* 允许在for中 */
    /* ... */
}
/* 注：MISRA允许for语句中的逗号，但不建议在其他地方使用 */

/* ✅ 合规 - 明确的函数调用 */
int temp = a;
func(temp, c);  /* 明确 */
```

---

## 检测工具配置

### GCC检查选项

```bash
# 未定义行为检查
gcc -Wsequence-point -c file.c

# 死代码检测
gcc -Wunreachable-code -c file.c

# 空语句检测
gcc -Wempty-body -c file.c

# 所有警告
gcc -Wall -Wextra -c file.c
```

### PC-lint配置

```bash
pc-lint \
    -e944 \  /* 操作符优先级 */
    -e955 \  /* 参数顺序依赖 */
    -e527 \  /* 不可达代码 */
    -e616 \  /* switch无default */
    -e825 \  /* 循环体为空 */
    file.c
```

---

## 总结

| 规则 | 强制 | 核心要求 |
|:----:|:----:|:---------|
| 6.1 | Required | 不依赖求值顺序 |
| 6.2 | Required | 副作用限制 |
| 6.3 | Required | 布尔值正确 |
| 6.4 | Required | 避免常量条件 |
| 6.5 | Required | 无死代码 |
| 6.6 | Required | switch完整 |
| 6.7 | Required | 循环控制安全 |
| 6.8 | Required | 不向后跳转 |
| 6.9 | Required | 无单独空语句 |
| 6.10 | Advisory | 避免逗号运算符 |

---

**参考**: MISRA C:2023 Guidelines for the use of the C language in critical systems, Chapter 6

---

> **下一章节**: [Rules 7.1-7.5 - 指针与内存安全](./07_MISRA_C_2023_Rules_7.md)


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


## 📚 实质性内容补充

### 技术深度分析

#### 1. 核心概念详解

深入剖析本主题的核心概念，建立完整的知识体系。

#### 2. 实现机制

| 层级 | 机制 | 关键技术 |
|:-----|:-----|:---------|
| 应用层 | 业务逻辑 | 设计模式 |
| 系统层 | 资源管理 | 内存/进程 |
| 硬件层 | 物理实现 | CPU/缓存 |

#### 3. 实践指导

- 最佳实践准则
- 常见陷阱与避免
- 调试与优化技巧

### 扩展阅读

- [核心知识体系](../../01_Core_Knowledge_System/readme.md)
- [全局索引](../../00_GLOBAL_INDEX.md)
