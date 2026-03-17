# MISRA C:2023 Rules 11.1-11.6 - 表达式与运算符

> 运算符优先级、隐式转换、复杂表达式限制

---

## 规则分类概述

| 规则编号 | 规则名称 | 强制性 | 核心要求 |
|:--------:|:---------|:------:|:---------|
| Rule 11.1 | 运算符优先级 | Required | 使用括号明确优先级 |
| Rule 11.2 | 隐式转换 | Required | 避免窄化转换 |
| Rule 11.3 | 复杂表达式 | Required | 限制表达式复杂度 |
| Rule 11.4 | 混合类型 | Required | 混合类型运算限制 |
| Rule 11.5 | 位运算 | Required | 位运算类型安全 |
| Rule 11.6 | 逻辑运算符 | Required | 逻辑运算符正确使用 |

---


---

## 📑 目录

- [MISRA C:2023 Rules 11.1-11.6 - 表达式与运算符](#misra-c2023-rules-111-116---表达式与运算符)
  - [规则分类概述](#规则分类概述)
  - [📑 目录](#-目录)
  - [Rule 11.1 - 运算符优先级](#rule-111---运算符优先级)
    - [规则原文](#规则原文)
    - [优先级表（从高到低）](#优先级表从高到低)
    - [违反示例](#违反示例)
    - [合规示例](#合规示例)
  - [Rule 11.2 - 隐式转换](#rule-112---隐式转换)
    - [规则原文](#规则原文-1)
    - [违反示例](#违反示例-1)
    - [合规示例](#合规示例-1)
  - [Rule 11.3 - 复杂表达式](#rule-113---复杂表达式)
    - [规则原文](#规则原文-2)
    - [复杂度限制准则](#复杂度限制准则)
    - [违反示例](#违反示例-2)
    - [合规示例](#合规示例-2)
  - [Rule 11.4 - 混合类型运算](#rule-114---混合类型运算)
    - [规则原文](#规则原文-3)
    - [违反示例](#违反示例-3)
    - [合规示例](#合规示例-3)
  - [Rule 11.5 - 位运算](#rule-115---位运算)
    - [规则原文](#规则原文-4)
    - [违反示例](#违反示例-4)
    - [合规示例](#合规示例-4)
  - [Rule 11.6 - 逻辑运算符](#rule-116---逻辑运算符)
    - [规则原文](#规则原文-5)
    - [违反示例](#违反示例-5)
    - [合规示例](#合规示例-5)
  - [表达式安全检查清单](#表达式安全检查清单)
  - [检测工具配置](#检测工具配置)
    - [GCC](#gcc)
    - [Clang](#clang)
  - [总结](#总结)


---

## Rule 11.1 - 运算符优先级

### 规则原文

> **Rule 11.1 (Required)**: Parentheses shall be used to clarify the precedence of operators where the precedence is not obvious.
>
> **中文**: 在运算符优先级不明显的地方，应使用括号来明确优先级。

### 优先级表（从高到低）

| 优先级 | 运算符 | 说明 |
|:------:|:-------|:-----|
| 1 | `()` `[]` `->` `.` | 函数调用、下标、成员访问 |
| 2 | `++` `--` `*` `&` `+` `-` `!` `~` `(type)` `sizeof` | 一元运算符 |
| 3 | `*` `/` `%` | 乘除取模 |
| 4 | `+` `-` | 加减 |
| 5 | `<<` `>>` | 移位 |
| 6 | `<` `<=` `>` `>=` | 关系比较 |
| 7 | `==` `!=` | 相等比较 |
| 8 | `&` | 按位与 |
| 9 | `^` | 按位异或 |
| 10 | `\|` | 按位或 |
| 11 | `&&` | 逻辑与 |
| 12 | `\|\|` | 逻辑或 |
| 13 | `?:` | 条件运算符 |
| 14 | `=` `+=` `-=` 等 | 赋值 |
| 15 | `,` | 逗号 |

### 违反示例

```c
/* ❌ 违反 - 不清楚的优先级 */
int x = a + b * c;  /* 知道优先级，但... */
int y = a << b + c;  /* 是 (a<<b)+c 还是 a<<(b+c)？ */
bool r = a & b == c;  /* 是 (a&b)==c 还是 a&(b==c)？ */
int z = a ? b : c + d;  /* 是 (a?b:c)+d 还是 a?b:(c+d)？*/

/* ❌ 违反 - 混合运算符 */
if (a && b || c && d)  /* 虽然正确，但可读性差 */
if (a & b == 0)  /* 错误：先比较b==0 */
if (a + b << c)  /* 移位优先级低于加法 */
```

### 合规示例

```c
/* ✅ 合规 - 明确的括号 */
int x = a + (b * c);  /* 即使知道优先级，也显式 */
int y = a << (b + c);  /* 明确意图 */
bool r = (a & b) == c;  /* 位运算先进行 */
int z = a ? b : (c + d);  /* 明确三元运算范围 */

/* ✅ 合规 - 复杂条件分组 */
if ((a && b) || (c && d))  /* 清晰的逻辑分组 */
if ((a & b) == 0)  /* 位运算结果比较 */
if ((a + b) << c)  /* 加法后移位 */

/* ✅ 合规 - 多层嵌套 */
result = ((a * b) + (c * d)) / ((e - f) + 1);

/* ✅ 合规 - 位运算 */
flags = (flags & ~MASK) | (value << SHIFT);
```

---

## Rule 11.2 - 隐式转换

### 规则原文

> **Rule 11.2 (Required)**: Conversions shall not be performed between pointer to object and any other type.
>
> **中文**: 不得进行对象指针与其他类型之间的转换。

### 违反示例

```c
/* ❌ 违反 - 指针转整数 */
int *p = &value;
uintptr_t addr = (int)p;  /* 错误：应该是(intptr_t)p */

/* ❌ 违反 - 不同对象指针转换 */
int *ip = &ival;
float *fp = (float *)ip;  /* 危险！类型混淆 */

/* ❌ 违反 - 指针转void*后丢失信息 */
void *vp = p;
int *ip2 = vp;  /* 隐式转换，警告 */

/* ❌ 违反 - 结构体指针随意转换 */
struct Header *h = (struct Header *)buffer;  /* 不安全 */
```

### 合规示例

```c
/* ✅ 合规 - 使用uintptr_t */
#include <stdint.h>
int *p = &value;
uintptr_t addr = (uintptr_t)p;  /* 正确 */

/* ✅ 合规 - 使用union（类型双关）*/
union Converter {
    float f;
    int32_t i;
};

union Converter c;
c.f = 3.14f;
int32_t bits = c.i;  /* 安全 */

/* ✅ 合规 - memcpy（严格别名安全）*/
float f = 3.14f;
int32_t i;
memcpy(&i, &f, sizeof(i));  /* 安全，无类型混淆 */

/* ✅ 合规 - 字符指针访问字节 */
void *data = get_data();
unsigned char *bytes = (unsigned char *)data;  /* char*允许访问任何对象 */
/* 可以安全访问bytes[0], bytes[1]等 */
```

---

## Rule 11.3 - 复杂表达式

### 规则原文

> **Rule 11.3 (Required)**: The complexity of expressions shall be limited.
>
> **中文**: 应限制表达式的复杂度。

### 复杂度限制准则

| 类型 | 建议限制 | 说明 |
|:-----|:---------|:-----|
| 嵌套层数 | ≤ 3 | 括号嵌套 |
| 运算符数量 | ≤ 5 | 单个表达式 |
| 函数调用 | ≤ 2 | 单个表达式 |
| 条件嵌套 | ≤ 2 | 三元运算符 |

### 违反示例

```c
/* ❌ 违反 - 过度复杂 */
result = (a + b) * (c - d) / ((e + f) * (g - h));

/* ❌ 违反 - 嵌套太深 */
x = a ? (b ? (c ? d : e) : f) : g;

/* ❌ 违反 - 函数调用太多 */
result = func1(func2(a), func3(func4(b)));

/* ❌ 违反 - 副作用混乱 */
arr[i++] = arr[i++] + func(j++);  /* 未定义行为！ */
```

### 合规示例

```c
/* ✅ 合规 - 分步计算 */
int sum1 = a + b;
int diff1 = c - d;
int product = sum1 * diff1;
int sum2 = e + f;
int diff2 = g - h;
result = product / (sum2 * diff2);

/* ✅ 合规 - 使用临时变量 */
float temp1 = calculate_temp1(a, b);
float temp2 = calculate_temp2(c, d);
result = combine(temp1, temp2);

/* ✅ 合规 - 简化条件 */
if (a) {
    x = b ? c : d;
} else {
    x = e;
}

/* ✅ 合规 - 分离副作用 */
int idx1 = i++;
int idx2 = i++;
int arg = j++;
arr[idx1] = arr[idx2] + func(arg);
```

---

## Rule 11.4 - 混合类型运算

### 规则原文

> **Rule 11.4 (Required)**: The value of an expression shall not depend on the order of evaluation of subexpressions when the subexpressions have side effects.
>
> **中文**: 当子表达式有副作用时，表达式的值不应依赖于子表达式的求值顺序。

### 违反示例

```c
/* ❌ 严重违反 - 未定义行为 */
int x = i++ + i++;  /* 未定义！ */

/* ❌ 违反 - 函数调用顺序不确定 */
int r = func1() + func2();  /* 哪个先调用不确定 */

/* ❌ 违反 - 参数求值顺序不确定 */
result = calculate(a++, b++, a + b);  /* 不确定！ */

/* ❌ 违反 - 自修改 */
arr[i] = i++;  /* 未定义！ */
```

### 合规示例

```c
/* ✅ 合规 - 分离副作用 */
int temp1 = i++;
int temp2 = i++;
int x = temp1 + temp2;

/* ✅ 合规 - 明确调用顺序 */
int f1 = func1();
int f2 = func2();
int r = f1 + f2;

/* ✅ 合规 - 先完成所有副作用 */
int a_val = a++;
int b_val = b++;
int sum = a_val + b_val;
result = calculate(a_val, b_val, sum);

/* ✅ 合规 - 明确索引 */
int idx = i;
arr[idx] = i;
i++;  /* 单独增量 */
```

---

## Rule 11.5 - 位运算

### 规则原文

> **Rule 11.5 (Required)**: Bitwise operators shall only be applied to operands of unsigned underlying type.
>
> **中文**: 位运算符只能应用于无符号底层类型的操作数。

### 违反示例

```c
/* ❌ 违反 - 有符号数位移 */
int flags = 0x80;
int bit = flags >> 31;  /* 实现定义的：算术移位 */

/* ❌ 违反 - 符号扩展问题 */
int8_t byte = 0x80;
int extended = byte << 8;  /* 先符号扩展为0xFFFFFF80，再移位 */

/* ❌ 违反 - 有符号数取反 */
int mask = ~0;  /* 实现定义的结果 */

/* ❌ 违反 - 混合符号 */
int a = -1;
unsigned int b = 1;
unsigned int r = a | b;  /* a先转换为unsigned，变为大正数 */
```

### 合规示例

```c
/* ✅ 合规 - 使用无符号类型 */
uint32_t flags = 0x80;
uint32_t bit = flags >> 31;  /* 逻辑移位，结果确定 */

/* ✅ 合规 - 先扩展为无符号 */
uint8_t byte = 0x80;
uint32_t extended = ((uint32_t)byte) << 8;  /* 0x00008000 */

/* ✅ 合规 - 无符号取反 */
uint32_t mask = ~0U;  /* 0xFFFFFFFF */

/* ✅ 合规 - 掩码操作 */
#define FLAG_A (1U << 0)
#define FLAG_B (1U << 1)
#define FLAG_C (1U << 2)

uint32_t status = 0;
status |= FLAG_A;  /* 设置位 */
status &= ~FLAG_B; /* 清除位 */
bool has_flag = (status & FLAG_C) != 0;  /* 测试位 */

/* ✅ 合规 - 寄存器操作 */
volatile uint32_t *ctrl_reg = (uint32_t *)0x40001000;
*ctrl_reg |= (1U << 3);   /* 设置第3位 */
*ctrl_reg &= ~(1U << 4);  /* 清除第4位 */
```

---

## Rule 11.6 - 逻辑运算符

### 规则原文

> **Rule 11.6 (Required)**: The operands of logical operators shall be effectively Boolean.
>
> **中文**: 逻辑运算符的操作数应有效为布尔类型。

### 违反示例

```c
/* ❌ 违反 - 整数作为布尔 */
if (count) {  /* 不明确：是非零检查？ */
    /* ... */
}

/* ❌ 违反 - 位运算误用为逻辑运算 */
if (flags & MASK) {  /* 位运算结果，不清晰 */
    /* ... */
}

/* ❌ 违反 - 指针隐式转布尔（某些风格不允许）*/
if (ptr) {  /* 虽然有效，但不明确 */
    /* ... */
}

/* ❌ 违反 - 比较结果作为整数 */
int result = (a > b) + (c > d);  /* 比较结果是0或1 */
```

### 合规示例

```c
#include <stdbool.h>

/* ✅ 合规 - 明确的布尔表达式 */
if (count != 0) {  /* 明确 */
    /* ... */
}

/* ✅ 合规 - 明确测试 */
if (count > 0) {  /* 更明确的检查 */
    /* ... */
}

/* ✅ 合规 - 位运算结果转布尔 */
if ((flags & MASK) != 0) {  /* 明确比较 */
    /* ... */
}

/* ✅ 合规 - 指针明确比较 */
if (ptr != NULL) {  /* 明确 */
    /* ... */
}

/* ✅ 合规 - 布尔变量 */
bool is_valid = (a > b) && (c > d);
if (is_valid) {
    /* ... */
}
```

---

## 表达式安全检查清单

```markdown
□ 复杂表达式使用括号分组
□ 限制表达式嵌套层数（≤3）
□ 避免混合类型运算
□ 位运算使用无符号类型
□ 分离有副作用的子表达式
□ 逻辑运算使用明确的布尔表达式
□ 避免依赖求值顺序
□ 使用临时变量简化复杂计算
```

---

## 检测工具配置

### GCC

```bash
gcc \
  -Wparentheses \      /* 建议括号 */
  -Wconversion \       /* 隐式转换警告 */
  -Wsign-conversion \  /* 符号转换 */
  -Wsequence-point \   /* 序列点违规 */
  -c file.c
```

### Clang

```bash
clang \
  -Wparentheses \
  -Winteger-conversion \
  -Wsign-conversion \
  -Wunsequenced \
  -c file.c
```

---

## 总结

| 规则 | 强制 | 核心要求 |
|:----:|:----:|:---------|
| 11.1 | Required | 括号明确优先级 |
| 11.2 | Required | 指针转换安全 |
| 11.3 | Required | 限制复杂度 |
| 11.4 | Required | 不依赖求值顺序 |
| 11.5 | Required | 位运算无符号 |
| 11.6 | Required | 逻辑运算布尔 |

---

**参考**: MISRA C:2023 Guidelines for the use of the C language in critical systems, Chapter 11

---

> **下一章节**: [Rules 12.1-12.x - 控制流语句](./12_MISRA_C_2023_Rules_12.md)
