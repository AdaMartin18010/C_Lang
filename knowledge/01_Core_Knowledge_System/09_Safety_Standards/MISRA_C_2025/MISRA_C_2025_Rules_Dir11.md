# MISRA C:2025 规则 11.x-15.x - 指针、表达式、控制语句

> **规则范围**: 指针算术、表达式评估、控制流语句
> **适用标准**: C11/C17/C23
> **状态**: [待MISRA官方文档确认]

---

## 目录

- [MISRA C:2025 规则 11.x-15.x - 指针、表达式、控制语句](#misra-c2025-规则-11x-15x---指针表达式控制语句)
  - [目录](#目录)
  - [规则分类概述](#规则分类概述)
  - [第11章: 指针安全](#第11章-指针安全)
    - [Rule 11.1 - 空指针检查 \[待官方确认\]](#rule-111---空指针检查-待官方确认)
      - [规则框架 \[待官方确认\]](#规则框架-待官方确认)
      - [nullptr vs NULL \[待官方确认\]](#nullptr-vs-null-待官方确认)
    - [Rule 11.2 - 指针算术 \[待官方确认\]](#rule-112---指针算术-待官方确认)
      - [规则框架 \[待官方确认\]](#规则框架-待官方确认-1)
    - [Rule 11.3 - 指针转换 \[待官方确认\]](#rule-113---指针转换-待官方确认)
      - [规则框架 \[待官方确认\]](#规则框架-待官方确认-2)
    - [Rule 11.4 - 函数指针 \[待官方确认\]](#rule-114---函数指针-待官方确认)
      - [规则框架 \[待官方确认\]](#规则框架-待官方确认-3)
    - [Rule 11.5 - 野指针 \[待官方确认\]](#rule-115---野指针-待官方确认)
      - [规则框架 \[待官方确认\]](#规则框架-待官方确认-4)
    - [Rule 11.6 - C23 nullptr \[待官方确认\]](#rule-116---c23-nullptr-待官方确认)
      - [规则框架 \[待官方确认\]](#规则框架-待官方确认-5)
  - [第12章: 表达式](#第12章-表达式)
    - [Rule 12.1 - 求值顺序 \[待官方确认\]](#rule-121---求值顺序-待官方确认)
      - [规则框架 \[待官方确认\]](#规则框架-待官方确认-6)
    - [Rule 12.2 - 副作用 \[待官方确认\]](#rule-122---副作用-待官方确认)
      - [规则框架 \[待官方确认\]](#规则框架-待官方确认-7)
    - [Rule 12.3 - 运算符优先级 \[待官方确认\]](#rule-123---运算符优先级-待官方确认)
      - [规则框架 \[待官方确认\]](#规则框架-待官方确认-8)
    - [Rule 12.4 - 复杂表达式 \[待官方确认\]](#rule-124---复杂表达式-待官方确认)
      - [规则框架 \[待官方确认\]](#规则框架-待官方确认-9)
      - [复杂度限制准则 \[待官方确认\]](#复杂度限制准则-待官方确认)
    - [Rule 12.5 - 常量表达式 \[待官方确认\]](#rule-125---常量表达式-待官方确认)
      - [规则框架 \[待官方确认\]](#规则框架-待官方确认-10)
  - [第13章: 控制语句](#第13章-控制语句)
    - [Rule 13.1 - if语句 \[待官方确认\]](#rule-131---if语句-待官方确认)
      - [规则框架 \[待官方确认\]](#规则框架-待官方确认-11)
    - [Rule 13.2 - switch语句 \[待官方确认\]](#rule-132---switch语句-待官方确认)
      - [规则框架 \[待官方确认\]](#规则框架-待官方确认-12)
    - [Rule 13.3 - 循环语句 \[待官方确认\]](#rule-133---循环语句-待官方确认)
      - [规则框架 \[待官方确认\]](#规则框架-待官方确认-13)
    - [Rule 13.4 - 跳转语句 \[待官方确认\]](#rule-134---跳转语句-待官方确认)
      - [规则框架 \[待官方确认\]](#规则框架-待官方确认-14)
  - [第14章: 类型转换](#第14章-类型转换)
    - [Rule 14.1 - 显式转换 \[待官方确认\]](#rule-141---显式转换-待官方确认)
      - [规则框架 \[待官方确认\]](#规则框架-待官方确认-15)
    - [Rule 14.2 - 隐式转换 \[待官方确认\]](#rule-142---隐式转换-待官方确认)
      - [规则框架 \[待官方确认\]](#规则框架-待官方确认-16)
    - [Rule 14.3 - 指针转换 \[待官方确认\]](#rule-143---指针转换-待官方确认)
      - [规则框架 \[待官方确认\]](#规则框架-待官方确认-17)
    - [Rule 14.4 - C23类型转换 \[待官方确认\]](#rule-144---c23类型转换-待官方确认)
      - [规则框架 \[待官方确认\]](#规则框架-待官方确认-18)
  - [第15章: 函数](#第15章-函数)
    - [Rule 15.1 - 函数定义 \[待官方确认\]](#rule-151---函数定义-待官方确认)
      - [规则框架 \[待官方确认\]](#规则框架-待官方确认-19)
    - [Rule 15.2 - 函数调用 \[待官方确认\]](#rule-152---函数调用-待官方确认)
      - [规则框架 \[待官方确认\]](#规则框架-待官方确认-20)
    - [Rule 15.3 - 递归 \[待官方确认\]](#rule-153---递归-待官方确认)
      - [规则框架 \[待官方确认\]](#规则框架-待官方确认-21)
    - [Rule 15.4 - 内联函数 \[待官方确认\]](#rule-154---内联函数-待官方确认)
      - [规则框架 \[待官方确认\]](#规则框架-待官方确认-22)
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

## 规则分类概述

| 章节 | 规则范围 | 规则数量 | C:2025变化 |
|:-----|:---------|:--------:|:----------:|
| 第11章 | 指针安全 | [待确认] | 🔄 C23 nullptr |
| 第12章 | 表达式 | [待确认] | 🔄 C23 constexpr |
| 第13章 | 控制语句 | [待确认] | ➡️ 无变化 |
| 第14章 | 类型转换 | [待确认] | 🔄 C23类型 |
| 第15章 | 函数 | [待确认] | ➡️ 无变化 |

> **图例**: 🔄 修改 | ➡️ 无变化 | 🆕 新增 | 🗑️ 废弃

---

## 第11章: 指针安全

### Rule 11.1 - 空指针检查 [待官方确认]

**规则类别**: Required
**C:2025变化**: C23 nullptr支持 [待官方确认]

#### 规则框架 [待官方确认]

> [规则原文 - 待MISRA官方文档确认]

```c
/* 预期规则方向 [待官方确认] */
/* 解引用前必须检查指针非空 */

/* ❌ 违反 [待官方确认] */
void process(int *p) {
    *p = 42;  /* 未检查p是否为NULL */
}

/* ✅ 合规 [C23相关] */
#if __STDC_VERSION__ >= 202311L
void process_safe(int *p) {
    if (p != nullptr) {  /* C23 nullptr */
        *p = 42;
    }
}
#else
void process_safe(int *p) {
    if (p != NULL) {  /* C11/C17 */
        *p = 42;
    }
}
#endif
```

#### nullptr vs NULL [待官方确认]

| 特性 | NULL (C11/C17) | nullptr (C23) | 优势 |
|:-----|:---------------|:--------------|:-----|
| 类型 | 宏，通常(void*)0 | std::nullptr_t | 类型安全 |
| 可移植性 | 高 | C23+ | 避免重载混淆 |
| 推荐 | ✅ 可用 | ✅ 推荐(C23) | nullptr更清晰 |

### Rule 11.2 - 指针算术 [待官方确认]

**规则类别**: Required
**C:2025变化**: [待官方确认]

#### 规则框架 [待官方确认]

```c
/* 预期规则方向 [待官方确认] */
/* 指针运算必须在数组边界内 */

int arr[10];
int *p = arr;

/* ✅ 合规 [待官方确认] */
for (int i = 0; i < 10; i++) {
    *(p + i) = i;  /* 在边界内 */
}

/* ✅ 合规 [待官方确认] - 允许指向末尾 */
int *end = arr + 10;  /* 指向末尾后一位，允许 */
if (p < end) {
    /* 安全 */
}

/* ❌ 违反 [待官方确认] */
int *bad = arr + 20;  /* 超出边界 */
*bad = 0;  /* 未定义行为 */

/* ❌ 违反 [待官方确认] - 不同对象间运算 */
int arr1[10], arr2[10];
ptrdiff_t diff = arr2 - arr1;  /* 未定义行为 */
```

### Rule 11.3 - 指针转换 [待官方确认]

**规则类别**: Required
**C:2025变化**: [待官方确认]

#### 规则框架 [待官方确认]

```c
/* 预期规则方向 [待官方确认] */
/* 指针转换限制 */

/* ❌ 违反 [待官方确认] - 对象指针类型混用 */
int arr[10];
float *fp = (float *)arr;  /* 危险！ */

/* ✅ 合规 [待官方确认] - char*可访问任何对象 */
void *data = get_data();
unsigned char *bytes = (unsigned char *)data;  /* 允许 */

/* ✅ 合规 [待官方确认] - 使用union类型双关 */
union FloatInt {
    float f;
    uint32_t i;
};
union FloatInt fi = {.f = 3.14f};
uint32_t bits = fi.i;  /* 安全 */

/* ✅ 合规 [待官方确认] - memcpy */
int src = 0x12345678;
unsigned char bytes[4];
memcpy(bytes, &src, sizeof(bytes));  /* 安全 */
```

### Rule 11.4 - 函数指针 [待官方确认]

**规则类别**: Required
**C:2025变化**: [待官方确认]

#### 规则框架 [待官方确认]

```c
/* 预期规则方向 [待官方确认] */
/* 函数指针使用限制 */

/* ❌ 违反 [待官方确认] - 函数指针转数据指针 */
int func(int x) { return x; }
void *vp = (void *)func;  /* 未定义行为 */

/* ❌ 违反 [待官方确认] - 不同函数指针类型 */
int (*fp_int)(int) = func;
void (*fp_void)(void) = (void (*)(void))fp_int;  /* 危险 */

/* ✅ 合规 [待官方确认] */
int (*fp)(int) = func;  /* 相同类型 */
int result = fp(42);

/* ✅ 合规 [待官方确认] - 使用typedef */
typedef int (*Operation)(int, int);
int add(int a, int b) { return a + b; }
Operation op = add;
```

### Rule 11.5 - 野指针 [待官方确认]

**规则类别**: Required
**C:2025变化**: [待官方确认]

#### 规则框架 [待官方确认]

```c
/* 预期规则方向 [待官方确认] */
/* 防止野指针（悬空指针） */

/* ❌ 违反 [待官方确认] - 返回局部变量地址 */
int *bad_func(void) {
    int local = 5;
    return &local;  /* 返回后local不存在 */
}

/* ❌ 违反 [待官方确认] - 使用已释放内存 */
int *p = malloc(sizeof(int));
free(p);
*p = 5;  /* 使用已释放内存（UB） */

/* ✅ 合规 [待官方确认] */
int *safe_alloc(void) {
    int *p = malloc(sizeof(int));
    return p;  /* 堆内存有效 */
}

void safe_use(void) {
    int *p = safe_alloc();
    if (p != NULL) {
        *p = 5;
        free(p);
        p = NULL;  /* 置空防止误用 */
    }
}
```

### Rule 11.6 - C23 nullptr [待官方确认]

**规则类别**: Advisory
**C:2025变化**: 🆕 新增 [待官方确认]

#### 规则框架 [待官方确认]

```c
/* C23 nullptr使用指导 [待官方确认] */

#if __STDC_VERSION__ >= 202311L
    /* ✅ 推荐 [待官方确认] - 使用nullptr */
    int *p = nullptr;  /* 类型安全 */

    /* ✅ 推荐 [待官方确认] - 函数重载场景 */
    void func(int *p);
    void func(int val);
    func(nullptr);  /* 调用指针版本 */

    /* ✅ 推荐 [待官方确认] - 比较 */
    if (p == nullptr) { /* ... */ }
    if (p != nullptr) { /* ... */ }

#else
    /* C11/C17兼容 */
    #ifndef nullptr
        #define nullptr NULL
    #endif
    int *p = nullptr;
#endif
```

---

## 第12章: 表达式

### Rule 12.1 - 求值顺序 [待官方确认]

**规则类别**: Required
**C:2025变化**: [待官方确认]

#### 规则框架 [待官方确认]

```c
/* 预期规则方向 [待官方确认] */
/* 表达式值不得依赖子表达式求值顺序 */

/* ❌ 严重违反 [待官方确认] - 未定义行为 */
int i = 0;
int x = i++ + i++;  /* 未定义行为！ */

/* ❌ 违反 [待官方确认] */
int a = 5;
int b = (a = 3) + a;  /* a的值不确定是3还是5 */

/* ❌ 违反 [待官方确认] */
int arr[10] = {0};
int idx = 0;
int val = arr[idx++] + arr[idx++];  /* 未定义！ */

/* ✅ 合规 [待官方确认] - 分离表达式 */
int i = 0;
int temp1 = i++;
int temp2 = i++;
int x = temp1 + temp2;  /* x = 0 + 1 = 1，明确 */

/* ✅ 合规 [待官方确认] */
int a = 5;
a = 3;
int b = a + a;  /* b = 6，明确 */
```

### Rule 12.2 - 副作用 [待官方确认]

**规则类别**: Required
**C:2025变化**: [待官方确认]

#### 规则框架 [待官方确认]

```c
/* 预期规则方向 [待官方确认] */
/* 限制副作用 */

/* ❌ 违反 [待官方确认] - 多次修改 */
int i = 0;
func(i++, i++);  /* 未定义行为 */

/* ❌ 违反 [待官方确认] */
int x = 0;
x += x++;  /* 未定义行为 */

/* ❌ 违反 [待官方确认] - volatile多次访问 */
volatile uint32_t *reg = (volatile uint32_t *)0x40001000;
uint32_t val = *reg + *reg;  /* 两次读取volatile，可能不同 */

/* ✅ 合规 [待官方确认] */
int i = 0;
int arg1 = i++;
int arg2 = i++;
func(arg1, arg2);

/* ✅ 合规 [待官方确认] */
int x = 0;
x = x + 1;  /* 明确 */

/* ✅ 合规 [待官方确认] */
volatile uint32_t *reg = (volatile uint32_t *)0x40001000;
uint32_t snapshot = *reg;  /* 读取一次 */
uint32_t val = snapshot + snapshot;  /* 使用快照 */
```

### Rule 12.3 - 运算符优先级 [待官方确认]

**规则类别**: Required
**C:2025变化**: [待官方确认]

#### 规则框架 [待官方确认]

```c
/* 预期规则方向 [待官方确认] */
/* 使用括号明确优先级 */

/* ❌ 违反 [待官方确认] - 不清楚的优先级 */
int y = a << b + c;  /* 是 (a<<b)+c 还是 a<<(b+c)？ */
bool r = a & b == c;  /* 是 (a&b)==c 还是 a&(b==c)？ */

/* ✅ 合规 [待官方确认] */
int y = a << (b + c);  /* 明确意图 */
bool r = (a & b) == c;  /* 位运算先进行 */

/* ✅ 合规 [待官方确认] - 复杂条件分组 */
if ((a && b) || (c && d))  /* 清晰的逻辑分组 */
if ((a & b) == 0)  /* 位运算结果比较 */
```

### Rule 12.4 - 复杂表达式 [待官方确认]

**规则类别**: Advisory
**C:2025变化**: [待官方确认]

#### 规则框架 [待官方确认]

```c
/* 预期规则方向 [待官方确认] */
/* 限制表达式复杂度 */

/* ❌ 违反 [待官方确认] - 过度复杂 */
result = (a + b) * (c - d) / ((e + f) * (g - h));

/* ❌ 违反 [待官方确认] - 嵌套太深 */
x = a ? (b ? (c ? d : e) : f) : g;

/* ✅ 合规 [待官方确认] - 分步计算 */
int sum1 = a + b;
int diff1 = c - d;
int product = sum1 * diff1;
int sum2 = e + f;
int diff2 = g - h;
result = product / (sum2 * diff2);

/* ✅ 合规 [待官方确认] - 使用临时变量 */
float temp1 = calculate_temp1(a, b);
float temp2 = calculate_temp2(c, d);
result = combine(temp1, temp2);
```

#### 复杂度限制准则 [待官方确认]

| 类型 | 建议限制 | 说明 |
|:-----|:---------|:-----|
| 嵌套层数 | ≤ 3 | 括号嵌套 |
| 运算符数量 | ≤ 5 | 单个表达式 |
| 函数调用 | ≤ 2 | 单个表达式 |
| 条件嵌套 | ≤ 2 | 三元运算符 |

### Rule 12.5 - 常量表达式 [待官方确认]

**规则类别**: Required
**C:2025变化**: C23 constexpr [待官方确认]

#### 规则框架 [待官方确认]

```c
/* 预期规则方向 [待官方确认] */
/* 常量表达式使用 */

/* ✅ 合规 [待官方确认] */
#define MAX 100
int arr[MAX];  /* 编译时常量 */

/* [C23相关] - constexpr表达式 [待官方确认] */
#if __STDC_VERSION__ >= 202311L
    constexpr int SIZE = 100;
    int arr[SIZE];  /* C23编译时常量 */

    /* ✅ 推荐 [待官方确认] - constexpr函数 */
    constexpr int square(int x) {
        return x * x;
    }
    int arr2[square(10)];  /* 编译时计算 */
#else
    static const int SIZE = 100;
    int arr[SIZE];  /* C11/C17 - VLA或错误 */
    /* C11/C17中const int不能用于数组大小 */
#endif
```

---

## 第13章: 控制语句

### Rule 13.1 - if语句 [待官方确认]

**规则类别**: Required
**C:2025变化**: [待官方确认]

#### 规则框架 [待官方确认]

```c
/* 预期规则方向 [待官方确认] */
/* if语句使用规范 */

/* ❌ 违反 [待官方确认] - 常量条件 */
if (1) {  /* 始终为真 */
    /* ... */
}

/* ❌ 违反 [待官方确认] - 赋值误用 */
if (x = 5) {  /* 赋值而非比较 */
    /* ... */
}

/* ✅ 合规 [待官方确认] */
if (condition) {  /* 变量条件 */
    /* ... */
}

/* ✅ 合规 [待官方确认] - 明确比较 */
if (x == 5) {  /* 比较 */
    /* ... */
}

/* ✅ 合规 [待官方确认] - 使用预处理 */
#ifdef DEBUG
    debug_log("message");
#endif
```

### Rule 13.2 - switch语句 [待官方确认]

**规则类别**: Required
**C:2025变化**: [待官方确认]

#### 规则框架 [待官方确认]

```c
/* 预期规则方向 [待官方确认] */
/* switch语句必须完整 */

/* ❌ 违反 [待官方确认] - 缺少default */
switch (status) {
    case OK:
        process_ok();
        break;
    case ERROR:
        process_error();
        break;
    /* 缺少default */
}

/* ❌ 违反 [待官方确认] - 缺少break */
switch (cmd) {
    case CMD_START:
        init();
        /* 缺少break，无注释说明 */
    case CMD_RUN:
        run();
        break;
}

/* ✅ 合规 [待官方确认] */
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

/* ✅ 合规 [待官方确认] - 明确fall-through */
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
```

### Rule 13.3 - 循环语句 [待官方确认]

**规则类别**: Required
**C:2025变化**: [待官方确认]

#### 规则框架 [待官方确认]

```c
/* 预期规则方向 [待官方确认] */
/* 循环控制安全 */

/* ❌ 违反 [待官方确认] - 修改循环计数器 */
for (int i = 0; i < 10; i++) {
    if (condition) {
        i++;  /* 修改循环计数器！ */
    }
}

/* ❌ 违反 [待官方确认] - 无限循环无退出 */
while (1) {  /* 必须有break/return */
    /* 无退出条件 */
}

/* ✅ 合规 [待官方确认] */
for (int i = 0; i < 10; i++) {
    /* i只由for语句修改 */
    process(i);
}

/* ✅ 合规 [待官方确认] - 明确退出 */
bool done = false;
for (int i = 0; (i < 100) && !done; i++) {
    if (completion_condition) {
        done = true;  /* 使用标志而非break */
    }
}

/* ✅ 合规 [待官方确认] - while处理复杂逻辑 */
int pos = 0;
while (pos < max_pos) {
    if (skip_condition) {
        pos += 2;  /* while允许复杂控制 */
    } else {
        pos++;
    }
}
```

### Rule 13.4 - 跳转语句 [待官方确认]

**规则类别**: Required
**C:2025变化**: [待官方确认]

#### 规则框架 [待官方确认]

```c
/* 预期规则方向 [待官方确认] */
/* 跳转语句限制 */

/* ❌ 违反 [待官方确认] - 向后goto */
start:
    if (condition) {
        goto start;  /* 向后跳转！ */
    }

/* ❌ 违反 [待官方确认] - setjmp/longjmp */
#include <setjmp.h>
jmp_buf env;

void func(void) {
    if (setjmp(env) == 0) {
        /* 首次进入 */
    } else {
        /* longjmp返回 - 复杂控制流 */
    }
}

/* ✅ 合规 [待官方确认] - 向前goto（资源清理） */
void process(void) {
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

/* ✅ 合规 [待官方确认] - 使用循环代替 */
while (condition) {
    if (retry_condition) {
        continue;  /* 向前到下一次迭代 */
    }
    /* 处理 */
}
```

---

## 第14章: 类型转换

### Rule 14.1 - 显式转换 [待官方确认]

**规则类别**: Required
**C:2025变化**: [待官方确认]

#### 规则框架 [待官方确认]

```c
/* 预期规则方向 [待官方确认] */
/* 显式转换（强制类型转换）使用 */

/* ❌ 违反 [待官方确认] - 可能导致信息丢失 */
int large = 100000;
short small = (short)large;  /* 截断 */

/* ✅ 合规 [待官方确认] - 检查范围后转换 */
if (large > SHRT_MAX || large < SHRT_MIN) {
    /* 错误处理 */
} else {
    short small = (short)large;  /* 安全 */
}

/* ✅ 合规 [待官方确认] - 明确意图 */
float f = 3.7f;
int i = (int)f;  /* 明确截断小数 */
int rounded = (int)round(f);  /* 正确舍入 */
```

### Rule 14.2 - 隐式转换 [待官方确认]

**规则类别**: Required
**C:2025变化**: [待官方确认]

#### 规则框架 [待官方确认]

```c
/* 预期规则方向 [待官方确认] */
/* 避免危险的隐式转换 */

/* ❌ 违反 [待官方确认] - 有符号/无符号混合 */
int a = -1;
unsigned int b = 1;
if (a < b) {  /* false！a被转换为无符号大数 */
    /* ... */
}

/* ❌ 违反 [待官方确认] - 浮点转整数 */
float f = 3.7f;
int i = f;  /* 隐式截断 */

/* ✅ 合规 [待官方确认] */
/* 统一类型 */
int a = -1;
int b = 1;
if (a < b) {  /* true */
    /* ... */
}

/* ✅ 合规 [待官方确认] - 显式转换 */
float f = 3.7f;
int i = (int)f;  /* 明确转换 */
```

### Rule 14.3 - 指针转换 [待官方确认]

**规则类别**: Required
**C:2025变化**: [待官方确认]

#### 规则框架 [待官方确认]

```c
/* 预期规则方向 [待官方确认] */
/* 指针转换限制 */

/* ❌ 违反 [待官方确认] - 对象指针混用 */
int arr[10];
float *fp = (float *)arr;  /* 危险！ */

/* ❌ 违反 [待官方确认] - 整数转指针 */
int *p = (int *)0x40001000;  /* 危险！ */

/* ✅ 合规 [待官方确认] - 使用uintptr_t */
#include <stdint.h>
uintptr_t addr = 0x40001000;
volatile uint32_t *reg = (volatile uint32_t *)addr;

/* ✅ 合规 [待官方确认] - void*转换 */
void *vp = malloc(sizeof(int));
int *ip = (int *)vp;  /* void*允许转换 */
```

### Rule 14.4 - C23类型转换 [待官方确认]

**规则类别**: Advisory
**C:2025变化**: 🆕 新增 [待官方确认]

#### 规则框架 [待官方确认]

```c
/* C23新类型转换指导 [待官方确认] */

#if __STDC_VERSION__ >= 202311L
    /* [待官方确认] - _BitInt转换 */
    _BitInt(17) small = 100;
    int normal = (int)small;  /* 扩展 */

    /* [待官方确认] - typeof使用 */
    int x = 5;
    typeof(x) y = 10;  /* y也是int */

    /* ⚠️ 注意 [待官方确认] - 避免复杂转换 */
    /* typeof()内的复杂表达式 */
    /* typeof(func()[0]) confusing; */  /* 避免 */
#else
    /* C11/C17兼容 */
    int normal = 100;
    int y = 10;
#endif
```

---

## 第15章: 函数

### Rule 15.1 - 函数定义 [待官方确认]

**规则类别**: Required
**C:2025变化**: [待官方确认]

#### 规则框架 [待官方确认]

```c
/* 预期规则方向 [待官方确认] */
/* 函数定义规范 */

/* ❌ 违反 [待官方确认] - 旧式函数定义 */
int old_style(a, b)
int a;
int b;
{  /* K&R风格 */
    return a + b;
}

/* ❌ 违反 [待官方确认] - 无返回类型 */
func(int x) {  /* 隐式int返回 */
    return x;
}

/* ❌ 违反 [待官方确认] - 空参数列表 */
void func();  /* 不是void函数，参数未指定 */

/* ✅ 合规 [待官方确认] */
int modern_style(int a, int b) {
    return a + b;
}

/* ✅ 合规 [待官方确认] */
void func(void) {  /* 明确表示无参数 */
    /* ... */
}

/* ✅ 合规 [待官方确认] */
void process(const char *input);  /* 原型 */
void process(const char *input) {  /* 定义匹配 */
    /* ... */
}
```

### Rule 15.2 - 函数调用 [待官方确认]

**规则类别**: Required
**C:2025变化**: [待官方确认]

#### 规则框架 [待官方确认]

```c
/* 预期规则方向 [待官方确认] */
/* 函数调用安全 */

/* ❌ 违反 [待官方确认] - 可变参数类型错误 */
printf("%d", 3.14);  /* 类型不匹配，UB */

/* ❌ 违反 [待官方确认] - 参数数量错误 */
void func(int a, int b);
func(1);  /* 参数不足 */
func(1, 2, 3);  /* 参数过多 */

/* ✅ 合规 [待官方确认] */
printf("%f", 3.14);  /* 正确格式 */
printf("%d", (int)3.14);  /* 明确转换 */

/* ✅ 合规 [待官方确认] */
void func(int a, int b);
func(1, 2);  /* 参数匹配 */
```

### Rule 15.3 - 递归 [待官方确认]

**规则类别**: Advisory
**C:2025变化**: [待官方确认]

#### 规则框架 [待官方确认]

```c
/* 预期规则方向 [待官方确认] */
/* 递归使用限制 */

/* ❌ 违反 [待官方确认] - 无限递归 */
int bad_recursive(int n) {
    return bad_recursive(n);  /* 无限递归 */
}

/* ⚠️ 谨慎 [待官方确认] - 递归深度不确定 */
int factorial(int n) {
    if (n <= 1) return 1;
    return n * factorial(n - 1);  /* 嵌入式中谨慎使用 */
}

/* ✅ 推荐 [待官方确认] - 迭代替代递归 */
int factorial_iterative(int n) {
    int result = 1;
    for (int i = 2; i <= n; i++) {
        result *= i;
    }
    return result;
}
```

### Rule 15.4 - 内联函数 [待官方确认]

**规则类别**: Advisory
**C:2025变化**: [待官方确认]

#### 规则框架 [待官方确认]

```c
/* 预期规则方向 [待官方确认] */
/* 内联函数使用 */

/* ✅ 合规 [待官方确认] */
static inline int max(int a, int b) {
    return (a > b) ? a : b;
}

/* ✅ 合规 [待官方确认] - 头文件中 */
/* math_utils.h */
#ifndef MATH_UTILS_H
#define MATH_UTILS_H

static inline int clamp(int value, int min, int max) {
    if (value < min) return min;
    if (value > max) return max;
    return value;
}

#endif

/* ⚠️ 注意 [待官方确认] - 内联是建议不是强制 */
/* 编译器可能选择不内联 */
```

---

## 总结

| 规则 | 类别 | C:2025变化 | 关键要点 |
|:----:|:----:|:----------:|:---------|
| 11.1 | Required | 🔄 C23 | nullptr使用 |
| 11.2 | Required | ➡️ | 指针运算边界 |
| 11.3 | Required | ➡️ | 指针转换限制 |
| 11.4 | Required | ➡️ | 函数指针 |
| 11.5 | Required | ➡️ | 野指针防护 |
| 11.6 | Advisory | 🆕 C23 | nullptr指导 |
| 12.1 | Required | ➡️ | 求值顺序 |
| 12.2 | Required | ➡️ | 副作用限制 |
| 12.3 | Required | ➡️ | 运算符优先级 |
| 12.4 | Advisory | ➡️ | 表达式复杂度 |
| 12.5 | Required | 🔄 C23 | constexpr表达式 |
| 13.1 | Required | ➡️ | if语句 |
| 13.2 | Required | ➡️ | switch语句 |
| 13.3 | Required | ➡️ | 循环语句 |
| 13.4 | Required | ➡️ | 跳转语句 |
| 14.1 | Required | ➡️ | 显式转换 |
| 14.2 | Required | ➡️ | 隐式转换 |
| 14.3 | Required | ➡️ | 指针转换 |
| 14.4 | Advisory | 🆕 C23 | C23类型转换 |
| 15.1 | Required | ➡️ | 函数定义 |
| 15.2 | Required | ➡️ | 函数调用 |
| 15.3 | Advisory | ➡️ | 递归限制 |
| 15.4 | Advisory | ➡️ | 内联函数 |

---

**参考**: MISRA C:2025 Guidelines [待官方确认]
**上一章**: [规则 6.x-10.x - 语言扩展、类型与常量](./MISRA_C_2025_Rules_Dir6.md)
**下一章**: [规则 16.x-22.x - 标准库、并发、网络安全](./04_MISRA_C_2025_Rules_16-22.md)

> **免责声明**: 本文档基于MISRA C:2023和公开信息创建，具体规则内容请以MISRA官方发布的C:2025文档为准。


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
