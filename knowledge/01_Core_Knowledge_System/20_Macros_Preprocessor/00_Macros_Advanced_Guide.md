# C 预处理器与宏高级技巧

> **难度**: L3-L5
> **对应标准**: ISO C §6.10
> **核心目标**: 从简单替换到编译期元编程

---

## 1. 预处理器的工作机制

### 1.1 翻译阶段

```
源代码 → 三字符组替换 → 行拼接 → 标记化 → 预处理器扩展 → 字符串连接 → 编译
                    ↑
              #include / #define / #if 在此阶段执行
```

预处理器**不是编译器**：

- 不理解 C 类型系统
- 不做语法分析
- 纯文本替换

### 1.2 宏展开规则

```c
#define MAX(a, b) ((a) > (b) ? (a) : (b))

int x = MAX(3, 5);
// 展开为: int x = ((3) > (5) ? (3) : (5));

int y = MAX(i++, j++);
// ❌ 危险：展开为 ((i++) > (j++) ? (i++) : (j++))
// i 或 j 可能递增两次！
```

---

## 2. 安全宏技巧

### 2.1 括号保护

```c
// ❌ 错误：没有括号保护
#define SQUARE(x) x * x
SQUARE(3 + 4);      // 3 + 4 * 3 + 4 = 19（不是49）

// ✅ 正确：每个参数和整体都加括号
#define SQUARE(x) ((x) * (x))
SQUARE(3 + 4);      // ((3 + 4) * (3 + 4)) = 49

// ❌ 错误：多语句宏没有 do-while(0)
#define SWAP(a, b) \
    int tmp = (a); \
    (a) = (b); \
    (b) = tmp;

if (cond)
    SWAP(x, y);     // 只有第一行在 if 内！
else
    ...;

// ✅ 正确：do { } while(0) 包装
#define SWAP(a, b) do { \
    typeof(a) tmp = (a); \
    (a) = (b); \
    (b) = tmp; \
} while(0)

if (cond)
    SWAP(x, y);     // 整个宏在 if 内
else
    ...;
```

### 2.2 可变参数宏（C99）

```c
// 调试日志宏
#ifdef DEBUG
    #define LOG(fmt, ...) fprintf(stderr, "[%s:%d] " fmt "\n", \
                                  __FILE__, __LINE__, ##__VA_ARGS__)
#else
    #define LOG(fmt, ...) ((void)0)
#endif

// 使用
LOG("Value: %d", x);           // 展开为 fprintf(...)
LOG("No args");                 // ##__VA_ARGS__ 处理空参数

// 编译期断言（C11 _Static_assert 之前的实现）
#define STATIC_ASSERT(expr, msg) \
    typedef char _static_assert_##__LINE__[(expr) ? 1 : -1]
```

### 2.3 字符串化与连接

```c
#define STR(x) #x           // 字符串化
#define CONCAT(a, b) a##b   // 标记连接

STR(hello);         // "hello"
CONCAT(var, 123);   // var123

// 间接层技巧（解决宏参数不展开问题）
#define STR_HELPER(x) #x
#define STR(x) STR_HELPER(x)

#define VERSION 42
STR(VERSION);       // "42"（不是 "VERSION"）
```

---

## 3. X-Macro 模式

### 3.1 数据驱动代码生成

```c
// 定义错误码表（单一定义源）
#define ERROR_CODES(X) \
    X(OK,             0,  "Success") \
    X(NOT_FOUND,     -1,  "Resource not found") \
    X(PERMISSION,    -2,  "Permission denied") \
    X(TIMEOUT,       -3,  "Operation timed out") \
    X(OOM,           -4,  "Out of memory")

// 生成枚举
enum ErrorCode {
    #define X(name, code, msg) ERR_##name = code,
    ERROR_CODES(X)
    #undef X
};

// 生成错误码到字符串的映射
const char *error_name(enum ErrorCode code) {
    switch (code) {
        #define X(name, code, msg) case ERR_##name: return #name;
        ERROR_CODES(X)
        #undef X
        default: return "UNKNOWN";
    }
}

// 生成错误消息映射
const char *error_message(enum ErrorCode code) {
    switch (code) {
        #define X(name, code, msg) case ERR_##name: return msg;
        ERROR_CODES(X)
        #undef X
        default: return "Unknown error";
    }
}
```

### 3.2 寄存器映射生成

```c
#define REGISTERS(X) \
    X(R0,  0x00,  "General purpose") \
    X(R1,  0x04,  "General purpose") \
    X(SP,  0x10,  "Stack pointer") \
    X(LR,  0x14,  "Link register") \
    X(PC,  0x18,  "Program counter")

// 生成结构体
typedef struct {
    #define X(name, offset, desc) uint32_t name;
    REGISTERS(X)
    #undef X
} RegisterBank;

// 生成偏移量常量
enum RegisterOffset {
    #define X(name, offset, desc) OFFSET_##name = offset,
    REGISTERS(X)
    #undef X
};
```

---

## 4. 条件编译高级技巧

### 4.1 特性检测

```c
// 检测 C 标准版本
#if defined(__STDC_VERSION__)
    #if (__STDC_VERSION__ >= 202311L)
        #define C23 1
        #define C11 1
    #elif (__STDC_VERSION__ >= 201112L)
        #define C11 1
    #elif (__STDC_VERSION__ >= 199901L)
        #define C99 1
    #endif
#endif

// 检测编译器特性
#if defined(__has_attribute)
    #define HAS_ATTRIBUTE(x) __has_attribute(x)
#else
    #define HAS_ATTRIBUTE(x) 0
#endif

#if HAS_ATTRIBUTE(malloc)
    #define ATTR_MALLOC __attribute__((malloc))
#else
    #define ATTR_MALLOC
#endif

// 检测字节序（编译期）
#if defined(__BYTE_ORDER__) && (__BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__)
    #define IS_LITTLE_ENDIAN 1
#elif defined(__BYTE_ORDER__) && (__BYTE_ORDER__ == __ORDER_BIG_ENDIAN__)
    #define IS_LITTLE_ENDIAN 0
#endif
```

### 4.2 头文件守卫

```c
// ✅ 标准方式
#ifndef MY_HEADER_H
#define MY_HEADER_H
// ... 内容
#endif

// ✅ C23 #pragma once（非标准但被广泛支持）
#pragma once

// ✅ 基于路径的唯一宏（避免命名冲突）
// 编译命令: gcc -DPROJECT_UNIQUE_H=my_project_header_h
#ifndef PROJECT_UNIQUE_H
#define PROJECT_UNIQUE_H
#endif
```

---

## 5. `_Generic` 泛型选择（C11）

```c
#include <stdio.h>

// 类型安全的打印宏
#define print(x) _Generic((x), \
    int: print_int, \
    double: print_double, \
    char*: print_string, \
    const char*: print_string \
)(x)

void print_int(int x) { printf("int: %d\n", x); }
void print_double(double x) { printf("double: %f\n", x); }
void print_string(const char *x) { printf("string: %s\n", x); }

// 使用
print(42);        // int: 42
print(3.14);      // double: 3.140000
print("hello");   // string: hello
```

```c
// 泛型最大值（类型安全）
#define max(a, b) _Generic((a) + (b), \
    int: max_int, \
    long: max_long, \
    float: max_float, \
    double: max_double \
)((a), (b))

static inline int max_int(int a, int b) { return a > b ? a : b; }
static inline long max_long(long a, long b) { return a > b ? a : b; }
static inline float max_float(float a, float b) { return a > b ? a : b; }
static inline double max_double(double a, double b) { return a > b ? a : b; }
```

---

## 6. 编译期计算

### 6.1 `sizeof` 技巧

```c
// 编译期数组大小
#define ARRAY_SIZE(arr) (sizeof(arr) / sizeof((arr)[0]))

int nums[] = {1, 2, 3, 4, 5};
for (size_t i = 0; i < ARRAY_SIZE(nums); i++) { }

// 编译期断言数组大小
#define STATIC_ASSERT_ARRAY_SIZE(arr, expected) \
    typedef char _assert_##arr[(ARRAY_SIZE(arr) == (expected)) ? 1 : -1]
```

### 6.2 编译期位掩码生成

```c
#define BIT(n) (1U << (n))
#define MASK(n) ((1U << (n)) - 1)

// 编译期计算
uint32_t flags = BIT(3) | BIT(7);     // 0x88
uint32_t low_mask = MASK(4);           // 0x0F
```

---

## 7. 常见陷阱

| 陷阱 | 示例 | 解决 |
|:-----|:-----|:-----|
| 多次求值 | `MAX(i++, j++)` | 使用 `typeof` + 临时变量，或内联函数 |
| 递归展开 | `#define A B` `#define B A` | 避免循环定义 |
| 优先级问题 | `SQUARE(a+b)` | 始终加括号 |
| 分号问题 | 多语句宏在 `if` 中 | 使用 `do { } while(0)` |
| 空参数 | `LOG("msg")` | 使用 `##__VA_ARGS__` |

---

## 8. 配套代码示例

本节配套可编译代码位于 `examples/macros/` 目录：

| 示例文件 | 演示内容 | 编译命令 |
|---------|---------|---------|
| `x_macros.c` | X-Macro 元编程，从单一数据定义生成枚举/数组/switch | `gcc -O2 -std=c11` |
| `generic_selection.c` | C11 `_Generic` 实现类型安全的泛型 `max` 和 `print` | `gcc -O2 -std=c11` |
| `stringify_concat.c` | `#` 字符串化、`##` 标记粘贴、编译期断言 | `gcc -O2 -std=c11` |

### `_Generic` 与函数重载对比

| 特性 | C `_Generic` | C++ 重载 | 内联函数 |
|------|-------------|---------|---------|
| 编译期选择 | ✅ | ✅ | ✅ |
| 运行时开销 | 无 | 无 | 无 |
| 代码膨胀 | 无（仅选择） | 无 | 可能有 |
| 可移植性 | C11+ | C++ only | 全部 |
| 复杂逻辑 | 有限 | 完整 | 完整 |

---

> **最后更新**: 2026-05-11
> **参考**: K&R Ch.4, Modern C Ch.10, CERT C PRE00-PRE31
