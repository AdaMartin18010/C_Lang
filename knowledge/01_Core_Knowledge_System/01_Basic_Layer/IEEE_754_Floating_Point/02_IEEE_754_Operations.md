# IEEE 754-2019 浮点运算

> 浮点数运算、舍入模式、异常处理

---

## 概述

IEEE 754-2019定义了浮点数的算术运算规则，包括基本运算、舍入模式和异常处理机制。

---

## 基本算术运算

### 四则运算规则

```c
#include <math.h>
#include <float.h>
#include <fenv.h>

/* IEEE 754要求基本运算返回正确舍入结果 */
float a = 1.0f;
float b = 3.0f;
float result = a / b;  /* 结果为0.33333334（正确舍入）*/
```

### 运算精度要求

| 运算 | 精度要求 | 说明 |
|:-----|:---------|:-----|
| +, -, ×, ÷ | 正确舍入 | 结果如同精确计算后舍入 |
| √ | 正确舍入 | 平方根 |
| FMA | 单一舍入 | 乘加运算 |
| 转换 | 正确舍入 | 不同格式间转换 |

### 乘加运算 (FMA)

```c
#include <math.h>

/* 标准乘加：两次舍入 */
float standard_fma(float a, float b, float c)
{
    float product = a * b;  /* 第一次舍入 */
    return product + c;      /* 第二次舍入 */
}

/* FMA指令：单次舍入，更高精度 */
float precise_fma(float a, float b, float c)
{
    return fmaf(a, b, c);  /* a*b+c，只舍入一次 */
}

/* FMA应用：提高点积精度 */
float dot_product(const float *a, const float *b, int n)
{
    float sum = 0.0f;
    for (int i = 0; i < n; i++) {
        sum = fmaf(a[i], b[i], sum);  /* 高精度累加 */
    }
    return sum;
}
```

---

## 舍入模式

### 四种舍入模式

```c
#include <fenv.h>

/*
 * IEEE 754舍入模式：
 * FE_TONEAREST   - 向最近偶数舍入（默认）
 * FE_UPWARD      - 向正无穷舍入
 * FE_DOWNWARD    - 向负无穷舍入
 * FE_TOWARDZERO  - 向零舍入（截断）
 */

/* 获取当前舍入模式 */
int current_mode = fegetround();

/* 设置舍入模式 */
fesetround(FE_UPWARD);

/* 使用完毕后恢复 */
#pragma STDC FENV_ACCESS ON
```

### 舍入模式示例

```c
#include <stdio.h>
#include <fenv.h>

void demonstrate_rounding(void)
{
    float values[] = {2.5f, 3.5f, -2.5f, -3.5f};
    const char *modes[] = {
        "To Nearest",
        "Toward +Inf",
        "Toward -Inf",
        "Toward Zero"
    };
    int mode_flags[] = {
        FE_TONEAREST,
        FE_UPWARD,
        FE_DOWNWARD,
        FE_TOWARDZERO
    };

    for (int m = 0; m < 4; m++) {
        fesetround(mode_flags[m]);
        printf("%s:\n", modes[m]);

        for (int i = 0; i < 4; i++) {
            float rounded = rintf(values[i]);  /* 使用当前舍入模式 */
            printf("  %+.1f -> %+.1f\n", values[i], rounded);
        }
    }

    /* 恢复默认 */
    fesetround(FE_TONEAREST);
}

/* 输出：
 * To Nearest:
 *   +2.5 -> +2.0  (偶数)
 *   +3.5 -> +4.0  (偶数)
 *   -2.5 -> -2.0  (偶数)
 *   -3.5 -> -4.0  (偶数)
 * Toward +Inf:
 *   +2.5 -> +3.0
 *   +3.5 -> +4.0
 *   -2.5 -> -2.0
 *   -3.5 -> -3.0
 * ...
 */
```

---

## 特殊值运算

### NaN传播

```c
#include <math.h>

/* NaN在运算中传播 */
float nan_value = NAN;
float result;

result = nan_value + 1.0f;   /* NaN */
result = nan_value * 0.0f;   /* NaN */
result = nan_value / 1.0f;   /* NaN */
result = sqrtf(nan_value);   /* NaN */

/* NaN比较 */
bool test1 = (nan_value == nan_value);  /* false！ */
bool test2 = isnan(nan_value);          /* true */
```

### 无穷大运算

```c
#include <math.h>
#include <float.h>

float inf = INFINITY;
float result;

/* 与无穷大的运算 */
result = inf + 1.0f;         /* +inf */
result = inf * 2.0f;         /* +inf */
result = inf / 2.0f;         /* +inf */
result = 1.0f / inf;         /* +0 */
result = inf - inf;          /* NaN (不定式) */
result = inf / inf;          /* NaN (不定式) */
result = 0.0f * inf;         /* NaN (不定式) */
```

### 有符号零

```c
#include <math.h>
#include <stdio.h>

void signed_zero_behavior(void)
{
    float pos_zero = +0.0f;
    float neg_zero = -0.0f;

    /* 比较：+0 == -0 */
    printf("+0 == -0: %d\n", pos_zero == neg_zero);  /* 1 (true) */

    /* 但符号影响某些运算 */
    float div1 = 1.0f / pos_zero;   /* +inf */
    float div2 = 1.0f / neg_zero;   /* -inf */

    printf("1/+0 = %f\n", div1);     /* +inf */
    printf("1/-0 = %f\n", div2);     /* -inf */

    /* atan2考虑零的符号 */
    float atan1 = atan2f(pos_zero, -1.0f);  /* +π */
    float atan2 = atan2f(neg_zero, -1.0f);  /* -π */
}
```

---

## 浮点异常

### 五种浮点异常

```c
#include <fenv.h>

/* IEEE 754异常标志：
 * FE_INVALID       - 无效操作
 * FE_DIVBYZERO     - 除以零
 * FE_OVERFLOW      - 溢出
 * FE_UNDERFLOW     - 下溢
 * FE_INEXACT       - 不精确结果
 */

/* 检查异常 */
void check_exceptions(void)
{
    if (fetestexcept(FE_INVALID)) {
        printf("Invalid operation occurred\n");
    }
    if (fetestexcept(FE_DIVBYZERO)) {
        printf("Division by zero occurred\n");
    }
    if (fetestexcept(FE_OVERFLOW)) {
        printf("Overflow occurred\n");
    }
    if (fetestexcept(FE_UNDERFLOW)) {
        printf("Underflow occurred\n");
    }
    if (fetestexcept(FE_INEXACT)) {
        printf("Inexact result occurred\n");
    }
}

/* 清除异常 */
feclearexcept(FE_ALL_EXCEPT);

/* 启用陷阱（实现定义）*/
feenableexcept(FE_DIVBYZERO | FE_INVALID);
```

### 异常处理示例

```c
#include <fenv.h>
#include <math.h>
#include <stdio.h>

float safe_division(float a, float b)
{
    /* 清除之前的异常 */
    feclearexcept(FE_ALL_EXCEPT);

    float result = a / b;

    /* 检查异常 */
    if (fetestexcept(FE_DIVBYZERO)) {
        fprintf(stderr, "Warning: Division by zero\n");
        return (a > 0) ? INFINITY : -INFINITY;
    }

    if (fetestexcept(FE_INVALID)) {
        fprintf(stderr, "Error: Invalid operation\n");
        return NAN;
    }

    if (fetestexcept(FE_OVERFLOW)) {
        fprintf(stderr, "Warning: Overflow\n");
        return (result > 0) ? FLT_MAX : -FLT_MAX;
    }

    return result;
}
```

---

## 精度控制

### 避免精度损失

```c
#include <math.h>

/* ❌ 问题：大数吃小数 */
float bad_sum(void)
{
    float sum = 100000000.0f;
    sum += 1.0f;  /* 1.0f被忽略！ */
    sum += 1.0f;  /* 1.0f被忽略！ */
    return sum;   /* 仍然是100000000.0f */
}

/* ✅ Kahan求和算法 */
float kahan_sum(const float *arr, int n)
{
    float sum = 0.0f;
    float c = 0.0f;  /* 补偿 */

    for (int i = 0; i < n; i++) {
        float y = arr[i] - c;  /* 减去补偿 */
        float t = sum + y;      /* 临时和 */
        c = (t - sum) - y;      /* 计算新的补偿 */
        sum = t;
    }

    return sum;
}

/* ✅ 按绝对值从小到大求和 */
int compare_abs(const void *a, const void *b)
{
    float fa = fabsf(*(const float *)a);
    float fb = fabsf(*(const float *)b);
    return (fa > fb) - (fa < fb);
}

float precise_sum(float *arr, int n)
{
    /* 先排序 */
    qsort(arr, n, sizeof(float), compare_abs);

    float sum = 0.0f;
    for (int i = 0; i < n; i++) {
        sum += arr[i];  /* 从小到大累加 */
    }
    return sum;
}
```

---

## 浮点比较

### 安全的浮点比较

```c
#include <math.h>
#include <stdbool.h>
#include <float.h>

/* 绝对误差比较 */
bool almost_equal_abs(float a, float b, float epsilon)
{
    return fabsf(a - b) < epsilon;
}

/* 相对误差比较 */
bool almost_equal_rel(float a, float b, float rel_eps)
{
    float diff = fabsf(a - b);
    float max_val = fmaxf(fabsf(a), fabsf(b));
    return diff <= rel_eps * max_val;
}

/* ULP-based比较（推荐）*/
bool almost_equal_ulp(float a, float b, int max_ulp)
{
    /* 处理NaN */
    if (isnan(a) || isnan(b)) {
        return false;
    }

    /* 处理相同值 */
    if (a == b) {
        return true;
    }

    /* 处理零 */
    if ((a == 0.0f && b == 0.0f) ||
        (a == -0.0f && b == -0.0f)) {
        return true;
    }

    /* 计算ULP差异 */
    union { float f; int32_t i; } ua, ub;
    ua.f = a;
    ub.f = b;

    /* 处理符号 */
    if (ua.i < 0) ua.i = 0x80000000 - ua.i;
    if (ub.i < 0) ub.i = 0x80000000 - ub.i;

    int32_t ulp_diff = abs(ua.i - ub.i);
    return ulp_diff <= max_ulp;
}

/* 使用示例 */
bool compare_results(float computed, float expected)
{
    /* 允许2个ULP的差异 */
    return almost_equal_ulp(computed, expected, 2);
}
```

---

## 编译器优化与严格别名

### 浮点优化选项

```bash
# GCC/Clang
 -ffast-math          # 允许激进优化（可能破坏IEEE 754）
 -fno-math-errno      # 不设置数学函数的errno
 -funsafe-math-optimizations  # 允许不安全的数学优化
 -ffinite-math-only   # 假设无NaN/Inf

# 严格IEEE 754合规
 -ffloat-store        # 防止扩展精度问题
 -frounding-math      # 尊重舍入模式
 -fsignaling-nans     # 支持信号NaN
```

### 严格别名规则

```c
/* ❌ 违反严格别名 */
float f = 3.14f;
int32_t bits = *(int32_t *)&f;  /* 未定义行为！ */

/* ✅ 合规 - 使用union */
union float_bits {
    float f;
    int32_t i;
};

int32_t get_float_bits(float f)
{
    union float_bits fb = { .f = f };
    return fb.i;
}

/* ✅ C23 - 使用stdbit.h（如果可用）*/
#include <stdbit.h>  /* C23 */
int32_t bits = stdbitcopyf(f);
```

---

## 参考

- IEEE 754-2019 Standard for Floating-Point Arithmetic
- [What Every Computer Scientist Should Know About Floating-Point Arithmetic](https://docs.oracle.com/cd/E19957-01/806-3568/ncg_goldberg.html)
- [Floating-Point Guide](https://floating-point-gui.de/)

---

> **上一章**: [IEEE 754基础](./01_IEEE_754_Basics.md)


---

## 深入理解

### 技术原理

深入探讨相关技术原理和实现细节。

### 实践指南

- 步骤1：理解基础概念
- 步骤2：掌握核心原理
- 步骤3：应用实践

### 相关资源

- 文档链接
- 代码示例
- 参考文章

---

> **最后更新**: 2026-03-21  
> **维护者**: AI Code Review
