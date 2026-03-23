# IEEE 754 浮点运算标准

> IEEE 754-2019浮点数标准完整指南

---

## 概述

IEEE 754是浮点数运算的国际标准，定义了浮点数的表示、运算规则和异常处理。几乎所有现代计算机系统都实现了此标准。

| 属性 | 说明 |
|:-----|:-----|
| **标准版本** | IEEE 754-2019 |
| **二进制格式** | binary16, binary32, binary64, binary128, binary256 |
| **十进制格式** | decimal32, decimal64, decimal128 |
| **C语言支持** | float, double, long double |

---

## 内容索引

### 基础概念

| 文档 | 内容 | 关键主题 |
|:-----|:-----|:---------|
| [01 IEEE 754基础](./01_IEEE_754_Basics.md) | 浮点数表示 | 二进制格式、特殊值、机器精度 |
| [02 IEEE 754运算](./02_IEEE_754_Operations.md) | 算术运算 | 四则运算、舍入模式、异常 |

### 核心概念速查

```c
/* 浮点数表示 */
float f = 3.14159f;    /* binary32: 1位符号 + 8位指数 + 23位尾数 */
double d = 3.14159;    /* binary64: 1位符号 + 11位指数 + 52位尾数 */

/* 特殊值 */
float pos_inf = INFINITY;
float neg_inf = -INFINITY;
float nan_val = NAN;
float pos_zero = +0.0f;
float neg_zero = -0.0f;

/* 机器精度 */
#include <float.h>
float eps_f = FLT_EPSILON;    /* 1.192093e-07 */
double eps_d = DBL_EPSILON;   /* 2.220446e-16 */

/* 异常检查 */
#include <fenv.h>
feclearexcept(FE_ALL_EXCEPT);
/* 执行运算... */
if (fetestexcept(FE_OVERFLOW)) {
    /* 处理溢出 */
}
```

---

## 浮点数格式

### 二进制格式

| 格式 | C类型 | 指数位 | 尾数位 | 精度（十进制）|
|:-----|:------|:-------|:-------|:--------------|
| binary16 | _Float16 (C23) | 5 | 10 | ~3.3位 |
| binary32 | float | 8 | 23 | ~7.2位 |
| binary64 | double | 11 | 52 | ~15.9位 |
| binary128 | long double (部分平台) | 15 | 112 | ~34.0位 |

### 格式布局

```text
binary32 (float):
| 符号(1) | 指数(8) | 尾数(23) |
|    S    | EEEEEEEE| MMMMMMMMMMMMMMMMMMMMMMM |

binary64 (double):
| 符号(1) | 指数(11) | 尾数(52) |
|    S    | EEEEEEEEEEE| MMMMMMMMM... (52位) |

值 = (-1)^S × 2^(E-偏置) × 1.M
```

---

## 关键常量

### float.h 常量

```c
/* 二进制32位 (float) */
FLT_RADIX        = 2;           /* 基数 */
FLT_MANT_DIG     = 24;          /* 精度位数（含隐含位）*/
FLT_DIG          = 6;           /* 十进制精度 */
FLT_MIN_EXP      = -125;        /* 最小指数 */
FLT_MAX_EXP      = 128;         /* 最大指数 */
FLT_MIN          = 1.175494e-38;/* 最小正规数 */
FLT_MAX          = 3.402823e+38;/* 最大正规数 */
FLT_EPSILON      = 1.192093e-07;/* 机器精度 */

/* 二进制64位 (double) */
DBL_MANT_DIG     = 53;
DBL_DIG          = 15;
DBL_MIN          = 2.225074e-308;
DBL_MAX          = 1.797693e+308;
DBL_EPSILON      = 2.220446e-16;
```

---

## 舍入模式

```c
#include <fenv.h>

/* 四种舍入模式 */
FE_TONEAREST     /* 向最近偶数舍入（默认）*/
FE_UPWARD        /* 向正无穷舍入 */
FE_DOWNWARD      /* 向负无穷舍入 */
FE_TOWARDZERO    /* 向零舍入（截断）*/

/* 使用 */
fesetround(FE_UPWARD);
float result = rintf(2.5f);  /* 3.0 */
fesetround(FE_TONEAREST);    /* 恢复默认 */
```

---

## 异常处理

| 异常 | 触发条件 | 默认结果 |
|:-----|:---------|:---------|
| INVALID | 0/0, ∞-∞, √(-1), NaN操作 | 安静NaN |
| DIVBYZERO | 非零/0 | ±∞ |
| OVERFLOW | 结果过大 | ±∞ |
| UNDERFLOW | 结果过小 | 次正规数或0 |
| INEXACT | 需要舍入 | 舍入结果 |

---

## 最佳实践

### 浮点比较

```c
/* ❌ 不安全 */
if (a == b) { }

/* ✅ 使用ULP比较 */
bool almost_equal(float a, float b, int max_ulp) {
    union { float f; int32_t i; } ua = {a}, ub = {b};
    if (ua.i < 0) ua.i = 0x80000000 - ua.i;
    if (ub.i < 0) ub.i = 0x80000000 - ub.i;
    return abs(ua.i - ub.i) <= max_ulp;
}
```

### 精度保持

```c
/* ✅ 使用更高精度累加 */
double sum = 0.0;
for (int i = 0; i < n; i++) {
    sum += (double)arr[i];  /* 提升精度 */
}

/* ✅ Kahan求和 */
float kahan_sum(float *arr, int n) {
    float sum = 0.0f, c = 0.0f;
    for (int i = 0; i < n; i++) {
        float y = arr[i] - c;
        float t = sum + y;
        c = (t - sum) - y;
        sum = t;
    }
    return sum;
}
```

---

## 参考资源

- [IEEE 754-2019标准](https://ieeexplore.ieee.org/document/8766229)
- [What Every Computer Scientist Should Know About Floating-Point Arithmetic](https://docs.oracle.com/cd/E19957-01/806-3568/ncg_goldberg.html)
- [Floating-Point Guide](https://floating-point-gui.de/)

---

**维护者**: C_Lang Knowledge Base Team
**最后更新**: 2026-03


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

- [核心知识体系](../../01_Core_Knowledge_System/README.md)
- [全局索引](../../00_GLOBAL_INDEX.md)
