# MISRA C:2023 Rules 21.1-21.x - 运行时错误

> 算术错误、除以零、溢出检查

---

## 规则分类概述

| 规则编号 | 规则名称 | 强制性 | 核心要求 |
|:--------:|:---------|:------:|:---------|
| Rule 21.1 | 除以零 | Required | 防止除以零 |
| Rule 21.2 | 溢出 | Required | 防止算术溢出 |
| Rule 21.3 | 下溢 | Required | 防止算术下溢 |
| Rule 21.4 | 移位 | Required | 安全移位操作 |

---

## Rule 21.1 - 除以零

### 规则原文

> **Rule 21.1 (Required)**: Division by zero shall be prevented.
>
> **中文**: 应防止除以零。

### 违反示例

```c
/* ❌ 违反 - 可能除以零 */
int divide(int a, int b)
{
    return a / b;  /* b可能为0 */
}

/* ❌ 违反 - 用户输入 */
int calculate(int x, int y)
{
    return 100 / (x - y);  /* x==y时除以零 */
}
```

### 合规示例

```c
/* ✅ 合规 - 检查除数 */
int safe_divide(int a, int b, int *result)
{
    if (b == 0) {
        return ERROR_DIV_BY_ZERO;
    }
    *result = a / b;
    return SUCCESS;
}

/* ✅ 合规 - 浮点除法 */
float safe_float_divide(float a, float b)
{
    if (fabsf(b) < FLT_EPSILON) {
        return INFINITY;  /* 或错误处理 */
    }
    return a / b;
}

/* ✅ 合规 - 模运算检查 */
int safe_modulo(int a, int b)
{
    if (b == 0) {
        return ERROR;
    }
    return a % b;
}
```

---

## Rule 21.2 - 算术溢出

### 规则原文

> **Rule 21.2 (Required)**: Arithmetic overflow shall be prevented.
>
> **中文**: 应防止算术溢出。

### 溢出检查

```c
/* ✅ 加法溢出检查 */
bool safe_add(int a, int b, int *result)
{
    if (b > 0 && a > INT_MAX - b) {
        return false;  /* 正溢出 */
    }
    if (b < 0 && a < INT_MIN - b) {
        return false;  /* 负溢出 */
    }
    *result = a + b;
    return true;
}

/* ✅ 乘法溢出检查 */
bool safe_mul(int a, int b, int *result)
{
    if (a > 0) {
        if (b > 0) {
            if (a > INT_MAX / b) return false;
        } else {
            if (b < INT_MIN / a) return false;
        }
    } else {
        if (b > 0) {
            if (a < INT_MIN / b) return false;
        } else {
            if (a != 0 && b < INT_MAX / a) return false;
        }
    }
    *result = a * b;
    return true;
}

/* ✅ 使用更宽类型 */
int32_t safe_add_32(int32_t a, int32_t b)
{
    int64_t result = (int64_t)a + (int64_t)b;
    if (result > INT32_MAX || result < INT32_MIN) {
        /* 溢出处理 */
    }
    return (int32_t)result;
}
```

---

## Rule 21.3 - 下溢

### 规则原文

> **Rule 21.3 (Required)**: Arithmetic underflow shall be prevented.
>
> **中文**: 应防止算术下溢。

### 下溢检查

```c
/* ✅ 无符号下溢检查 */
bool safe_sub_unsigned(unsigned int a, unsigned int b, unsigned int *result)
{
    if (b > a) {
        return false;  /* 下溢 */
    }
    *result = a - b;
    return true;
}

/* ✅ 有符号下溢检查 */
bool safe_sub_signed(int a, int b, int *result)
{
    if (b > 0 && a < INT_MIN + b) {
        return false;  /* 负溢出 */
    }
    *result = a - b;
    return true;
}

/* ✅ 浮点下溢检查 */
#include <float.h>

bool safe_float_mul(float a, float b, float *result)
{
    *result = a * b;
    if (fabsf(*result) < FLT_MIN && *result != 0.0f) {
        /* 下溢到次正规数 */
        return false;
    }
    return true;
}
```

---

## Rule 21.4 - 移位操作

### 规则原文

> **Rule 21.4 (Required)**: Shift operations shall be used safely.
>
> **中文**: 移位操作应安全使用。

### 违反示例

```c
/* ❌ 违反 - 移位宽度 >= 类型宽度 */
uint32_t x = 1 << 32;  /* 未定义行为 */

/* ❌ 违反 - 负移位 */
int y = 1 << -1;  /* 未定义行为 */

/* ❌ 违反 - 右移负数 */
int z = -1 >> 1;  /* 实现定义的 */

/* ❌ 违反 - 左移有符号负数 */
int w = -1 << 1;  /* 未定义行为 */

/* ❌ 违反 - 移位溢出 */
int overflow = 0x40000000 << 1;  /* 溢出 */
```

### 合规示例

```c
/* ✅ 合规 - 无符号移位 */
uint32_t safe_shift_left(uint32_t value, unsigned int shift)
{
    if (shift >= 32) {
        return 0;  /* 或错误处理 */
    }
    return value << shift;
}

uint32_t safe_shift_right(uint32_t value, unsigned int shift)
{
    if (shift >= 32) {
        return 0;
    }
    return value >> shift;
}

/* ✅ 合规 - 使用宏定义 */
#define SET_BIT(value, bit) ((value) | (1U << (bit)))
#define CLEAR_BIT(value, bit) ((value) & ~(1U << (bit)))
#define TEST_BIT(value, bit) (((value) >> (bit)) & 1U)

/* ✅ 合规 - 循环移位 */
uint32_t rotate_left(uint32_t value, unsigned int shift)
{
    shift &= 31;  /* 限制在0-31 */
    return (value << shift) | (value >> (32 - shift));
}
```

---

**参考**: MISRA C:2023 Guidelines for the use of the C language in critical systems, Chapter 21
