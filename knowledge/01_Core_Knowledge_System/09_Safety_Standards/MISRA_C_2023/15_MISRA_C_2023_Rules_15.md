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
﻿# MISRA C:2023 Rules 15.1-15.7 - 类型转换

> 显式转换、窄化转换、指针转换安全

---

## 规则分类概述

| 规则编号 | 规则名称 | 强制性 | 核心要求 |
|:--------:|:---------|:------:|:---------|
| Rule 15.1 | 显式转换 | Required | 避免隐式窄化转换 |
| Rule 15.2 | 整数转换 | Required | 有符号/无符号安全转换 |
| Rule 15.3 | 浮点转换 | Required | 浮点精度安全 |
| Rule 15.4 | 指针转换 | Required | 对象指针转换限制 |
| Rule 15.5 | 函数指针 | Required | 函数指针转换限制 |
| Rule 15.6 | 空指针 | Required | 空指针常量使用 |
| Rule 15.7 | 类型双关 | Required | 避免不安全类型双关 |

---

## Rule 15.1 - 显式转换

### 规则原文

> **Rule 15.1 (Required)**: Implicit conversions that may result in information loss shall not be performed.
>
> **中文**: 不得执行可能导致信息丢失的隐式转换。

### 违反示例

```c
/* ❌ 违反 - 宽转窄 */
int large = 100000;
short small = large;  /* 可能溢出 */

/* ❌ 违反 - 有符号转无符号 */
int signed_val = -1;
unsigned int unsigned_val = signed_val;  /* 变成大正数 */

/* ❌ 违反 - 浮点转整数 */
float f = 3.7f;
int i = f;  /* 小数丢失 */

/* ❌ 违反 - 双精度转单精度 */
double d = 1.797693e+308;
float f = d;  /* 变成Inf */

/* ❌ 违反 - 函数参数 */
void func(short s);
func(100000);  /* 隐式窄化 */

/* ❌ 违反 - 返回值 */
int64_t calc(void) { return 1LL << 40; }
int result = calc();  /* 截断 */
```

### 合规示例

```c
/* ✅ 合规 - 显式转换并检查 */
if (large > SHRT_MAX || large < SHRT_MIN) {
    /* 错误处理 */
} else {
    short small = (short)large;  /* 安全 */
}

/* ✅ 合规 - 显式转换 */
unsigned int unsigned_val = (unsigned int)signed_val;  /* 明确意图 */

/* ✅ 合规 - 显式截断 */
int i = (int)f;  /* 明确截断小数 */
int rounded = (int)round(f);  /* 正确舍入 */

/* ✅ 合规 - 检查浮点范围 */
#include <float.h>
if (d > FLT_MAX || d < -FLT_MAX) {
    /* 溢出处理 */
} else {
    float f = (float)d;
}

/* ✅ 合规 - 使用匹配类型 */
void func(short s);
short value = (short)100000;  /* 先转换 */
func(value);

/* ✅ 合规 - 返回类型匹配 */
int64_t result64 = calc();
int32_t result32;
if (result64 > INT32_MAX || result64 < INT32_MIN) {
    /* 溢出处理 */
} else {
    result32 = (int32_t)result64;
}
```

---

## Rule 15.2 - 整数转换

### 规则原文

> **Rule 15.2 (Required)**: Signed and unsigned integers shall not be mixed in expressions.
>
> **中文**: 有符号和无符号整数不得在表达式中混合使用。

### 违反示例

```c
/* ❌ 违反 - 有符号/无符号混合 */
int a = -1;
unsigned int b = 1;
if (a < b) {  /* false！a被转换为无符号大数 */
    /* ... */
}

/* ❌ 违反 - 混合运算 */
int x = 10;
unsigned int y = 20;
unsigned int z = x - y;  /* 无符号下溢 */

/* ❌ 违反 - 循环条件 */
for (int i = 0; i < count; i++) {  /* count如果是unsigned，混合 */
    /* ... */
}
```

### 合规示例

```c
/* ✅ 合规 - 统一使用有符号 */
int a = -1;
int b = 1;
if (a < b) {  /* true */
    /* ... */
}

/* ✅ 合规 - 统一使用无符号 */
unsigned int x = 10;
unsigned int y = 20;
if (x >= y) {  /* 明确比较 */
    /* ... */
}

/* ✅ 合规 - 显式转换 */
int a = -1;
unsigned int b = 1;
if ((unsigned int)a < b) {  /* 显式 */
    /* ... */
}

/* ✅ 合规 - 使用size_t用于大小 */
size_t count = get_count();
for (size_t i = 0; i < count; i++) {
    /* ... */
}
```

---

## Rule 15.3 - 浮点转换

### 规则原文

> **Rule 15.3 (Required)**: Floating-point conversions shall be explicit.
>
> **中文**: 浮点转换必须显式进行。

### 合规示例

```c
/* ✅ 合规 - 显式浮点转换 */
double d = 3.14159;
float f = (float)d;  /* 显式 */

/* ✅ 合规 - 显式转整数 */
float f = 3.7f;
int i = (int)f;  /* 截断 */
int rounded = (int)round(f);  /* 舍入 */

/* ✅ 合规 - 检查精度丢失 */
float original = 1.23456789f;
double extended = (double)original;  /* 安全，精度增加 */
float back = (float)extended;  /* 可能丢失精度 */
if (original != back) {
    /* 精度丢失 */
}
```

---

## Rule 15.4 - 指针转换

### 规则原文

> **Rule 15.4 (Required)**: A pointer to an object shall not be converted to a pointer to a different object type.
>
> **中文**: 对象指针不得转换为指向不同对象类型的指针。

### 违反示例

```c
/* ❌ 违反 - 对象指针混用 */
int arr[10];
float *f = (float *)arr;  /* 危险！ */

/* ❌ 违反 - 结构体指针混用 */
struct Header h;
struct Data *d = (struct Data *)&h;  /* 不安全 */

/* ❌ 违反 - 通过void*间接转换 */
int *ip = &x;
void *vp = ip;
float *fp = vp;  /* 未定义行为 */
```

### 合规示例

```c
/* ✅ 合规 - 使用union（类型双关）*/
union FloatInt {
    float f;
    uint32_t i;
};

uint32_t get_float_bits(float f)
{
    union FloatInt fi = {.f = f};
    return fi.i;
}

/* ✅ 合规 - memcpy */
int src = 0x12345678;
unsigned char bytes[4];
memcpy(bytes, &src, sizeof(bytes));  /* 安全 */

/* ✅ 合规 - char*访问字节 */
int value = 42;
unsigned char *bytes = (unsigned char *)&value;  /* char*允许 */
for (size_t i = 0; i < sizeof(value); i++) {
    printf("%02x ", bytes[i]);
}

/* ✅ 合规 - 不透明指针 */
typedef struct InternalData *Handle;
Handle create(void);
void use(Handle h);
void destroy(Handle h);
```

---

## Rule 15.5 - 函数指针

### 规则原文

> **Rule 15.5 (Required)**: A function pointer shall not be converted to any other type.
>
> **中文**: 函数指针不得转换为任何其他类型。

### 违反示例

```c
/* ❌ 违反 - 函数指针转数据指针 */
int func(int x) { return x; }
void *vp = (void *)func;  /* 未定义行为！ */

/* ❌ 违反 - 函数指针转整数 */
int (*fp)(int) = func;
uintptr_t addr = (uintptr_t)fp;  /* 有问题 */

/* ❌ 违反 - 不同类型函数指针 */
int (*fp_int)(int) = func;
void (*fp_void)(void) = (void (*)(void))fp_int;  /* 危险 */
```

### 合规示例

```c
/* ✅ 合规 - 相同类型函数指针 */
int (*fp)(int) = func;
int result = fp(42);

/* ✅ 合规 - 使用typedef */
typedef int (*Operation)(int, int);

int add(int a, int b) { return a + b; }
int sub(int a, int b) { return a - b; }

Operation ops[] = {add, sub};

/* ✅ 合规 - 回调模式 */
typedef void (*Callback)(void *context, int result);

void register_callback(Callback cb, void *context);
```

---

## Rule 15.6 - 空指针

### 规则原文

> **Rule 15.6 (Required)**: The null pointer constant NULL shall be used to represent a null pointer.
>
> **中文**: 应使用空指针常量NULL表示空指针。

### 合规示例

```c
#include <stddef.h>

/* ✅ 合规 - 使用NULL */
int *p = NULL;

void init(void)
{
    if (p == NULL) {
        p = malloc(sizeof(int));
    }
}

/* ✅ 合规 - 使用0也是允许的，但不推荐 */
int *p2 = 0;  /* 允许但不推荐 */
if (p2 != 0) { }  /* 不推荐 */

/* ✅ 推荐 - 始终使用NULL */
void cleanup(void)
{
    if (p != NULL) {
        free(p);
        p = NULL;
    }
}
```

---

## Rule 15.7 - 类型双关

### 规则原文

> **Rule 15.7 (Required)**: Type punning shall not be used.
>
> **中文**: 不得使用类型双关。

### 违反示例

```c
/* ❌ 违反 - 通过指针类型双关 */
float f = 3.14f;
int32_t bits = *(int32_t *)&f;  /* 严格别名违规 */

/* ❌ 违反 - 复杂类型双关 */
struct Data d;
uint32_t *words = (uint32_t *)&d;  /* 不安全 */
```

### 合规示例

```c
/* ✅ 合规 - 使用union */
union FloatInt {
    float f;
    int32_t i;
};

int32_t safe_type_pun(float f)
{
    union FloatInt fi = {.f = f};
    return fi.i;
}

/* ✅ 合规 - memcpy */
void safe_copy(void *dest, const void *src, size_t n)
{
    memcpy(dest, src, n);  /* 安全 */
}
```

---

## 转换安全检查清单

```markdown
□ 窄化转换使用显式强制转换
□ 检查转换前范围
□ 有符号/无符号不混合
□ 浮点转换检查精度
□ 对象指针不混用类型
□ 函数指针不转换
□ 使用NULL而非0
□ 类型双关使用union
```

---

**参考**: MISRA C:2023 Guidelines for the use of the C language in critical systems, Chapter 15


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
