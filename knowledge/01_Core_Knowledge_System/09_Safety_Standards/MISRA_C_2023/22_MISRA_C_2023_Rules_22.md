# MISRA C:2023 Rules 22.1-22.x - 其他规则

> 杂项规则、最佳实践

---

## 规则分类概述

| 规则编号 | 规则名称 | 强制性 | 核心要求 |
|:--------:|:---------|:------:|:---------|
| Rule 22.1 | 未使用代码 | Required | 移除未使用代码 |
| Rule 22.2 | 死代码 | Required | 消除死代码 |
| Rule 22.3 | 注释 | Advisory | 注释质量 |
| Rule 22.4 | 格式 | Advisory | 代码格式 |
| Rule 22.5 | 复杂度 | Advisory | 复杂度限制 |

---

## Rule 22.1 - 未使用代码

### 规则原文

> **Rule 22.1 (Required)**: All code shall be reachable and used.
>
> **中文**: 所有代码应可达且被使用。

### 违反示例

```c
/* ❌ 违反 - 未使用变量 */
void func(void)
{
    int unused = 10;  /* 从未使用 */
    printf("Hello\n");
}

/* ❌ 违反 - 未使用函数 */
static void helper(void)  /* 只在当前文件 */
{
    /* ... */
}
/* 从未调用 */

/* ❌ 违反 - 未使用宏 */
#define UNUSED_MACRO 100

/* ❌ 违反 - 未使用标签 */
void func(void)
{
unused_label:
    printf("Hello\n");
}
```

### 合规示例

```c
/* ✅ 合规 - 使用或移除 */
void func(void)
{
    int used = 10;
    printf("%d\n", used);
}

/* ✅ 合规 - 条件编译 */
#ifdef DEBUG
static void debug_helper(void)
{
    /* ... */
}
#endif

/* ✅ 合规 - 显式抑制警告 */
void callback(void *unused_param)
{
    (void)unused_param;  /* 明确不使用 */
    /* ... */
}
```

---

## Rule 22.2 - 死代码

### 规则原文

> **Rule 22.2 (Required)**: Dead code shall be removed.
>
> **中文**: 应移除死代码。

### 违反示例

```c
/* ❌ 违反 - return后的代码 */
int func(void)
{
    return 0;
    printf("Never reached\n");  /* 死代码 */
}

/* ❌ 违反 - 无条件跳转后的代码 */
void func(void)
{
    goto end;
    printf("Never reached\n");  /* 死代码 */
end:
    return;
}

/* ❌ 违反 - 总是假的条件 */
void func(void)
{
    if (0) {  /* 编译时常量 */
        printf("Never executed\n");  /* 死代码 */
    }
}
```

### 合规示例

```c
/* ✅ 合规 - 无死代码 */
int func(void)
{
    printf("Doing work\n");
    return 0;
}

/* ✅ 合规 - 条件编译 */
#ifdef NEVER_ENABLED
void unused_func(void)
{
    /* 整个函数条件编译 */
}
#endif
```

---

## Rule 22.3 - 注释

### 规则原文

> **Rule 22.3 (Advisory)**: Comments shall be used to explain the intent of the code.
>
> **中文**: 应使用注释解释代码意图。

### 注释最佳实践

```c
/* ✅ 文件头注释 */
/**
 * @file module.c
 * @brief Brief description of the module
 *
 * Detailed description of what this module does,
 * its responsibilities, and how to use it.
 *
 * @copyright Copyright 2026
 * @license MIT
 */

/* ✅ 函数注释 */
/**
 * @brief Calculate the factorial of n
 *
 * @param n The input value (0 <= n <= 12)
 * @return The factorial of n, or -1 if error
 *
 * @note This function uses iterative algorithm
 *       to avoid stack overflow
 */
int factorial(int n);

/* ✅ 代码块注释 */
/* Validate input parameters */
if (ptr == NULL) {
    return ERROR_NULL_POINTER;
}

/* Process each element in the array */
for (size_t i = 0; i < count; i++) {
    /* Skip disabled items */
    if (!items[i].enabled) {
        continue;
    }

    /* Apply transformation */
    process(&items[i]);
}

/* ❌ 避免 - 无关注释 */
i++;  /* Increment i */

/* ❌ 避免 - 过时注释 */
/* Calculate average of 5 values */  /* 实际处理10个值 */
```

---

## Rule 22.4 - 代码格式

### 规则原文

> **Rule 22.4 (Advisory)**: A consistent coding style shall be used.
>
> **中文**: 应使用一致的编码风格。

### 推荐风格

```c
/* ✅ 缩进 - 4空格 */
void func(void)
{
    if (condition) {
        do_something();
    } else {
        do_other();
    }
}

/* ✅ 大括号 - K&R风格 */
void func(void) {
    while (condition) {
        /* ... */
    }
}

/* ✅ 命名规范 */
#define CONSTANT_VALUE 100    /* 宏：全大写 */

typedef struct {
    int member_var;           /* 成员：小写+下划线 */
} type_name_t;                /* typedef：小写+_t */

void function_name(void);     /* 函数：小写+下划线 */
int global_var;               /* 全局：g_前缀 */
static int static_var;        /* 静态：s_前缀 */

/* ✅ 行长 - 80或100字符 */
void long_function_name(
    int param1,
    int param2,
    int param3);  /* 参数多时分行 */
```

---

## Rule 22.5 - 复杂度

### 规则原文

> **Rule 22.5 (Advisory)**: The cyclomatic complexity of a function shall be limited.
>
> **中文**: 函数的圈复杂度应受限。

### 复杂度计算

圈复杂度 = 决策点数量 + 1

决策点包括：if, while, for, case, &&, ||, ?:

```c
/* 复杂度 = 3 */
void func(int a, int b) {
    if (a && b) {        /* +1 for if, +1 for && */
        do_something();
    } else {
        do_other();
    }
}
```

### 复杂度限制

| 级别 | 最大复杂度 | 说明 |
|:-----|:-----------|:-----|
| 简单 | 1-5 | 推荐 |
| 中等 | 6-10 | 可接受 |
| 复杂 | 11-20 | 建议重构 |
| 极复杂 | >20 | 必须重构 |

### 降低复杂度

```c
/* ❌ 复杂函数（复杂度高）*/
void complex_func(int a, int b, int c)
{
    if (a) {
        if (b) {
            if (c) {
                /* ... */
            } else {
                /* ... */
            }
        } else {
            /* ... */
        }
    } else {
        /* ... */
    }
}

/* ✅ 拆分为小函数 */
static void handle_a_b_c(void) { /* ... */ }
static void handle_a_b_notc(void) { /* ... */ }
static void handle_a_notb(void) { /* ... */ }
static void handle_nota(void) { /* ... */ }

void simple_func(int a, int b, int c)
{
    if (!a) {
        handle_nota();
        return;
    }

    if (!b) {
        handle_a_notb();
        return;
    }

    if (c) {
        handle_a_b_c();
    } else {
        handle_a_b_notc();
    }
}
```

---

## 总结

| 规则 | 强制 | 核心要求 |
|:----:|:----:|:---------|
| 22.1 | Required | 无未使用代码 |
| 22.2 | Required | 无死代码 |
| 22.3 | Advisory | 注释解释意图 |
| 22.4 | Advisory | 一致编码风格 |
| 22.5 | Advisory | 复杂度≤10 |

---

**参考**: MISRA C:2023 Guidelines for the use of the C language in critical systems, Chapter 22

---

> **MISRA C:2023规则文档完成**


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
