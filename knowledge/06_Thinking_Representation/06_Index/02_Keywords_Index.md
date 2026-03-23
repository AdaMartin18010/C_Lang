# C关键字索引

> **类型**: 索引文档
> **位置**: `knowledge/06_Thinking_Representation/06_Index/02_Keywords_Index.md`
> **描述**: C语言全部32个关键字按字母排序索引，包含关键字分类和文档位置映射

---

## 索引目录

- [C关键字索引](#c关键字索引)
  - [索引目录](#索引目录)
  - [📑 目录](#-目录)
  - [关键字概览](#关键字概览)
  - [按字母排序索引](#按字母排序索引)
    - [A](#a)
      - [auto](#auto)
    - [B](#b)
      - [break](#break)
    - [C](#c)
      - [case](#case)
      - [char](#char)
      - [const](#const)
      - [continue](#continue)
    - [D](#d)
      - [default](#default)
      - [do](#do)
      - [double](#double)
    - [E](#e)
      - [else](#else)
      - [enum](#enum)
      - [extern](#extern)
    - [F](#f)
      - [float](#float)
      - [for](#for)
    - [G](#g)
      - [goto](#goto)
    - [H](#h)
    - [I](#i)
      - [if](#if)
      - [inline (C99)](#inline-c99)
      - [int](#int)
    - [L](#l)
      - [long](#long)
    - [R](#r)
      - [register](#register)
      - [restrict (C99)](#restrict-c99)
      - [return](#return)
    - [S](#s)
      - [short](#short)
      - [signed](#signed)
      - [sizeof](#sizeof)
      - [static](#static)
      - [struct](#struct)
      - [switch](#switch)
    - [T](#t)
      - [typedef](#typedef)
    - [U](#u)
      - [union](#union)
      - [unsigned](#unsigned)
    - [V](#v)
      - [void](#void)
      - [volatile](#volatile)
    - [W](#w)
      - [while](#while)
  - [按分类索引](#按分类索引)
    - [类型相关 (14个)](#类型相关-14个)
    - [控制流相关 (12个)](#控制流相关-12个)
    - [存储类相关 (5个)](#存储类相关-5个)
    - [类型限定符 (3个)](#类型限定符-3个)
    - [运算符 (1个)](#运算符-1个)
  - [C99新增关键字](#c99新增关键字)
  - [C11新增关键字](#c11新增关键字)
  - [关键字记忆口诀](#关键字记忆口诀)
  - [深入理解](#深入理解)
    - [核心原理](#核心原理)
    - [实践应用](#实践应用)
    - [最佳实践](#最佳实践)

---


---

## 📑 目录

- [C关键字索引](#c关键字索引)
  - [索引目录](#索引目录)
  - [📑 目录](#-目录)
  - [关键字概览](#关键字概览)
  - [按字母排序索引](#按字母排序索引)
    - [A](#a)
      - [auto](#auto)
    - [B](#b)
      - [break](#break)
    - [C](#c)
      - [case](#case)
      - [char](#char)
      - [const](#const)
      - [continue](#continue)
    - [D](#d)
      - [default](#default)
      - [do](#do)
      - [double](#double)
    - [E](#e)
      - [else](#else)
      - [enum](#enum)
      - [extern](#extern)
    - [F](#f)
      - [float](#float)
      - [for](#for)
    - [G](#g)
      - [goto](#goto)
    - [H](#h)
    - [I](#i)
      - [if](#if)
      - [inline (C99)](#inline-c99)
      - [int](#int)
    - [L](#l)
      - [long](#long)
    - [R](#r)
      - [register](#register)
      - [restrict (C99)](#restrict-c99)
      - [return](#return)
    - [S](#s)
      - [short](#short)
      - [signed](#signed)
      - [sizeof](#sizeof)
      - [static](#static)
      - [struct](#struct)
      - [switch](#switch)
    - [T](#t)
      - [typedef](#typedef)
    - [U](#u)
      - [union](#union)
      - [unsigned](#unsigned)
    - [V](#v)
      - [void](#void)
      - [volatile](#volatile)
    - [W](#w)
      - [while](#while)
  - [按分类索引](#按分类索引)
    - [类型相关 (14个)](#类型相关-14个)
    - [控制流相关 (12个)](#控制流相关-12个)
    - [存储类相关 (5个)](#存储类相关-5个)
    - [类型限定符 (3个)](#类型限定符-3个)
    - [运算符 (1个)](#运算符-1个)
  - [C99新增关键字](#c99新增关键字)
  - [C11新增关键字](#c11新增关键字)
  - [关键字记忆口诀](#关键字记忆口诀)
  - [深入理解](#深入理解)
    - [核心原理](#核心原理)
    - [实践应用](#实践应用)
    - [最佳实践](#最佳实践)


---

## 关键字概览

C语言共有 **32个关键字**（C89/C90标准），C99新增 **5个**，C11新增 **7个**（部分为宏形式）。

这些关键字是C语言的保留字，不能用作标识符（变量名、函数名等）。

---

## 按字母排序索引

### A

#### auto

**分类**: 存储类说明符
**说明**: 声明自动存储期变量（局部变量默认，通常省略）
**文档位置**: `knowledge/06_Thinking_Representation/02_Advanced/02_Lifetime.md`
**示例**:

```c
auto int x = 10;  // 等价于 int x = 10;
```

---

### B

#### break

**分类**: 跳转语句
**说明**: 立即终止当前循环或switch语句
**文档位置**: `knowledge/06_Thinking_Representation/03_Expression/03_Control_Flow.md`
**示例**:

```c
for (int i = 0; i < 10; i++) {
    if (i == 5) break;  // 退出循环
}
```

---

### C

#### case

**分类**: 分支语句
**说明**: switch语句中的分支标签
**文档位置**: `knowledge/06_Thinking_Representation/03_Expression/03_Control_Flow.md`
**示例**:

```c
switch (value) {
    case 1: /* ... */ break;
    case 2: /* ... */ break;
}
```

#### char

**分类**: 类型说明符
**说明**: 声明字符型变量（通常为1字节）
**文档位置**: `knowledge/06_Thinking_Representation/01_Basic/01_Data_Types.md`
**示例**:

```c
char c = 'A';
signed char sc = -128;
unsigned char uc = 255;
```

#### const

**分类**: 类型限定符
**说明**: 声明只读变量，值不可修改
**文档位置**: `knowledge/06_Thinking_Representation/02_Advanced/06_Type_Qualifiers.md`
**示例**:

```c
const int MAX = 100;
const char *str = "hello";  // 字符串内容不可修改
```

#### continue

**分类**: 跳转语句
**说明**: 跳过当前迭代，继续下一次循环
**文档位置**: `knowledge/06_Thinking_Representation/03_Expression/03_Control_Flow.md`
**示例**:

```c
for (int i = 0; i < 10; i++) {
    if (i % 2 == 0) continue;  // 跳过偶数
    printf("%d ", i);
}
```

---

### D

#### default

**分类**: 分支语句
**说明**: switch语句中的默认分支
**文档位置**: `knowledge/06_Thinking_Representation/03_Expression/03_Control_Flow.md`
**示例**:

```c
switch (value) {
    case 1: /* ... */ break;
    default: /* 其他情况 */ break;
}
```

#### do

**分类**: 循环语句
**说明**: do-while循环的开头
**文档位置**: `knowledge/06_Thinking_Representation/03_Expression/02_Loops.md`
**示例**:

```c
do {
    // 循环体至少执行一次
} while (condition);
```

#### double

**分类**: 类型说明符
**说明**: 声明双精度浮点型变量
**文档位置**: `knowledge/06_Thinking_Representation/01_Basic/01_Data_Types.md`
**示例**:

```c
double pi = 3.141592653589793;
```

---

### E

#### else

**分类**: 分支语句
**说明**: if语句的否定分支
**文档位置**: `knowledge/06_Thinking_Representation/03_Expression/03_Control_Flow.md`
**示例**:

```c
if (condition) {
    // 条件为真
} else {
    // 条件为假
}
```

#### enum

**分类**: 类型说明符
**说明**: 声明枚举类型
**文档位置**: `knowledge/06_Thinking_Representation/01_Basic/06_Enumeration.md`
**示例**:

```c
enum Color { RED, GREEN, BLUE };
enum Color c = RED;
```

#### extern

**分类**: 存储类说明符
**说明**: 声明外部链接的变量或函数
**文档位置**: `knowledge/06_Thinking_Representation/02_Advanced/03_Linkage.md`
**示例**:

```c
extern int global_var;  // 声明在其他文件中定义
extern void func(void); // 函数声明
```

---

### F

#### float

**分类**: 类型说明符
**说明**: 声明单精度浮点型变量
**文档位置**: `knowledge/06_Thinking_Representation/01_Basic/01_Data_Types.md`
**示例**:

```c
float f = 3.14f;
```

#### for

**分类**: 循环语句
**说明**: for循环控制语句
**文档位置**: `knowledge/06_Thinking_Representation/03_Expression/02_Loops.md`
**示例**:

```c
for (int i = 0; i < 10; i++) {
    // 循环体
}
```

---

### G

#### goto

**分类**: 跳转语句
**说明**: 无条件跳转到标签位置
**文档位置**: `knowledge/06_Thinking_Representation/03_Expression/04_Jumps.md`
**示例**:

```c
if (error) goto cleanup;
// ...
cleanup:
    free(buffer);
```

---

### H

*（无H开头关键字）*

---

### I

#### if

**分类**: 分支语句
**说明**: 条件判断语句
**文档位置**: `knowledge/06_Thinking_Representation/03_Expression/03_Control_Flow.md`
**示例**:

```c
if (condition) {
    // 条件为真时执行
}
```

#### inline (C99)

**分类**: 函数说明符
**说明**: 建议编译器将函数内联展开
**文档位置**: `knowledge/06_Thinking_Representation/04_Function/02_Inline_Functions.md`
**示例**:

```c
inline int max(int a, int b) {
    return (a > b) ? a : b;
}
```

#### int

**分类**: 类型说明符
**说明**: 声明整型变量
**文档位置**: `knowledge/06_Thinking_Representation/01_Basic/01_Data_Types.md`
**示例**:

```c
int count = 0;
unsigned int flags = 0xFF;
```

---

### L

#### long

**分类**: 类型修饰符
**说明**: 声明长整型或双长整型
**文档位置**: `knowledge/06_Thinking_Representation/01_Basic/01_Data_Types.md`
**示例**:

```c
long lg = 1000000L;
long long llg = 10000000000LL;
double d = 1.0L;  // long double
```

---

### R

#### register

**分类**: 存储类说明符
**说明**: 建议编译器将变量存储在寄存器中（现代编译器优化后意义不大）
**文档位置**: `knowledge/06_Thinking_Representation/02_Advanced/02_Lifetime.md`
**示例**:

```c
register int counter;  // 建议存储在寄存器
```

#### restrict (C99)

**分类**: 类型限定符
**说明**: 指示指针是唯一访问对象的方式（用于优化）
**文档位置**: `knowledge/06_Thinking_Representation/02_Advanced/06_Type_Qualifiers.md`
**示例**:

```c
void func(int *restrict p1, int *restrict p2) {
    // p1和p2不会指向同一内存
}
```

#### return

**分类**: 跳转语句
**说明**: 从函数返回，可带返回值
**文档位置**: `knowledge/06_Thinking_Representation/04_Function/01_Function_Basics.md`
**示例**:

```c
int add(int a, int b) {
    return a + b;
}
```

---

### S

#### short

**分类**: 类型修饰符
**说明**: 声明短整型变量
**文档位置**: `knowledge/06_Thinking_Representation/01_Basic/01_Data_Types.md`
**示例**:

```c
short s = 100;
short int si = 200;
```

#### signed

**分类**: 类型修饰符
**说明**: 声明有符号类型
**文档位置**: `knowledge/06_Thinking_Representation/01_Basic/01_Data_Types.md`
**示例**:

```c
signed char sc = -128;
signed int si = -1000;
```

#### sizeof

**分类**: 运算符
**说明**: 获取类型或变量的大小（字节）
**文档位置**: `knowledge/06_Thinking_Representation/03_Expression/05_Operators.md`
**示例**:

```c
size_t sz = sizeof(int);
size_t arr_sz = sizeof(array) / sizeof(array[0]);
```

#### static

**分类**: 存储类说明符
**说明**: 声明静态存储期变量或内部链接函数
**文档位置**: `knowledge/06_Thinking_Representation/02_Advanced/02_Lifetime.md`
**示例**:

```c
static int count = 0;  // 静态局部变量
static void helper(void);  // 内部链接函数
```

#### struct

**分类**: 类型说明符
**说明**: 声明结构体类型
**文档位置**: `knowledge/06_Thinking_Representation/01_Basic/04_Structures.md`
**示例**:

```c
struct Point {
    int x;
    int y;
};
struct Point p = {10, 20};
```

#### switch

**分类**: 分支语句
**说明**: 多分支选择语句
**文档位置**: `knowledge/06_Thinking_Representation/03_Expression/03_Control_Flow.md`
**示例**:

```c
switch (value) {
    case 1: /* ... */ break;
    case 2: /* ... */ break;
    default: /* ... */
}
```

---

### T

#### typedef

**分类**: 存储类说明符（语法上）
**说明**: 为现有类型创建新名称
**文档位置**: `knowledge/06_Thinking_Representation/02_Advanced/07_Typedef.md`
**示例**:

```c
typedef unsigned int uint;
typedef struct Node {
    int data;
    struct Node *next;
} Node;
```

---

### U

#### union

**分类**: 类型说明符
**说明**: 声明联合体类型
**文档位置**: `knowledge/06_Thinking_Representation/01_Basic/04_Structures.md#联合体`
**示例**:

```c
union Data {
    int i;
    float f;
    char c[4];
};
union Data d;
d.i = 10;
```

#### unsigned

**分类**: 类型修饰符
**说明**: 声明无符号类型
**文档位置**: `knowledge/06_Thinking_Representation/01_Basic/01_Data_Types.md`
**示例**:

```c
unsigned int ui = 1000;
unsigned char uc = 255;
```

---

### V

#### void

**分类**: 类型说明符
**说明**: 表示无类型或无返回值
**文档位置**: `knowledge/06_Thinking_Representation/01_Basic/01_Data_Types.md`
**示例**:

```c
void func(void);  // 无返回值，无参数
void *ptr;        // 通用指针
```

#### volatile

**分类**: 类型限定符
**说明**: 指示变量可能被外部因素修改，禁止优化
**文档位置**: `knowledge/06_Thinking_Representation/02_Advanced/06_Type_Qualifiers.md`
**示例**:

```c
volatile int timer;  // 可能被硬件修改
volatile uint32_t *reg = (volatile uint32_t *)0x40000000;
```

---

### W

#### while

**分类**: 循环语句
**说明**: while循环控制语句
**文档位置**: `knowledge/06_Thinking_Representation/03_Expression/02_Loops.md`
**示例**:

```c
while (condition) {
    // 循环体
}
```

---

## 按分类索引

### 类型相关 (14个)

| 关键字 | 说明 | 标准 |
|--------|------|------|
| char | 字符类型 | C89 |
| double | 双精度浮点 | C89 |
| enum | 枚举类型 | C89 |
| float | 单精度浮点 | C89 |
| int | 整型 | C89 |
| long | 长整型修饰符 | C89 |
| short | 短整型修饰符 | C89 |
| signed | 有符号修饰符 | C89 |
| struct | 结构体 | C89 |
| union | 联合体 | C89 |
| unsigned | 无符号修饰符 | C89 |
| void | 无类型 | C89 |
| inline | 内联函数 | C99 |
| _Bool / bool | 布尔类型 | C99 |

### 控制流相关 (12个)

| 关键字 | 说明 | 标准 |
|--------|------|------|
| break | 终止循环/switch | C89 |
| case | switch分支 | C89 |
| continue | 跳过本次迭代 | C89 |
| default | switch默认分支 | C89 |
| do | do-while循环 | C89 |
| else | if否定分支 | C89 |
| for | for循环 | C89 |
| goto | 无条件跳转 | C89 |
| if | 条件判断 | C89 |
| return | 函数返回 | C89 |
| switch | 多分支选择 | C89 |
| while | while循环 | C89 |

### 存储类相关 (5个)

| 关键字 | 说明 | 标准 |
|--------|------|------|
| auto | 自动存储期 | C89 |
| extern | 外部链接 | C89 |
| register | 寄存器建议 | C89 |
| static | 静态存储期 | C89 |
| typedef | 类型定义 | C89 |

### 类型限定符 (3个)

| 关键字 | 说明 | 标准 |
|--------|------|------|
| const | 只读限定 | C89 |
| restrict | 唯一访问指针 | C99 |
| volatile | 易变限定 | C89 |

### 运算符 (1个)

| 关键字 | 说明 | 标准 |
|--------|------|------|
| sizeof | 大小运算符 | C89 |

---

## C99新增关键字

| 关键字 | 说明 | 用途 |
|--------|------|------|
| inline | 内联函数 | 性能优化 |
| restrict | 限制指针 | 编译器优化 |
| _Bool | 布尔类型 | 逻辑值 |
| _Complex | 复数类型 | 数学计算 |
| _Imaginary | 虚数类型 | 数学计算 |

> 注: `bool`, `true`, `false` 通过 `<stdbool.h>` 提供

---

## C11新增关键字

| 关键字 | 说明 | 用途 |
|--------|------|------|
| _Alignas | 对齐指定 | 内存对齐控制 |
| _Alignof | 对齐查询 | 获取对齐要求 |
| _Atomic | 原子操作 | 并发编程 |
| _Generic | 泛型选择 | 类型泛化 |
| _Noreturn | 无返回函数 | 标记不返回函数 |
| _Static_assert | 静态断言 | 编译期检查 |
| _Thread_local | 线程本地存储 | 线程数据 |

---

## 关键字记忆口诀

```
类型定义：char int float double void short long signed unsigned
结构枚举：struct union enum typedef
控制流程：if else switch case default for while do break continue goto return
存储类别：auto static extern register
类型限定：const volatile restrict (C99)
其他运算：sizeof inline (C99)
```

---

> **状态**: ✅ 已完成


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
