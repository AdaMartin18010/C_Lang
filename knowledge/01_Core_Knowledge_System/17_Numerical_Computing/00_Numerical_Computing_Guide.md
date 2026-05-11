# C 语言数值计算指南

> **难度**: L3-L5
> **核心目标**: 理解浮点精度、数值稳定性和高效计算

---

## 1. 浮点数表示

### 1.1 IEEE 754 基础

```c
#include <float.h>

// C 语言浮点类型
float  f;   // IEEE 754 binary32: 1位符号 + 8位指数 + 23位尾数
double d;   // IEEE 754 binary64: 1位符号 + 11位指数 + 52位尾数

// 关键常量
FLT_EPSILON;    // float: 1.1920929e-07 (最小满足 1.0 + ε ≠ 1.0)
DBL_EPSILON;    // double: 2.220446e-16
FLT_MIN;        // 最小正规约数
FLT_MAX;        // 最大有限数
```

### 1.2 浮点陷阱

```c
// ❌ 浮点比较不能用 ==
if (a == b) { }  // 危险！

// ✅ 使用 epsilon 比较
#include <math.h>
#define EPSILON 1e-9
int float_equal(double a, double b) {
    return fabs(a - b) < EPSILON;
}

// 更健壮的相对比较
int float_equal_rel(double a, double b, double rel_tol, double abs_tol) {
    double diff = fabs(a - b);
    double max_val = fmax(fabs(a), fabs(b));
    return diff <= fmax(rel_tol * max_val, abs_tol);
}
```

---

## 2. 数值稳定性

### 2.1 灾难性抵消

```c
// ❌ 不稳定：相近大数相减
float a = 1.0e8f + 1.0f;
float b = 1.0e8f;
float diff = a - b;  // 期望 1.0，实际可能为 0！

// 原因：1.0e8 的精度约为 0.01（ULP），1.0 太小被舍入

// ✅ 使用 double 或 Kahan 求和
double a_d = 1.0e8 + 1.0;
double b_d = 1.0e8;
double diff_d = a_d - b_d;  // 准确得到 1.0
```

### 2.2 Kahan 求和算法

```c
// 减少浮点累加误差
double kahan_sum(const double *arr, size_t n) {
    double sum = 0.0;
    double c = 0.0;  // 补偿值

    for (size_t i = 0; i < n; i++) {
        double y = arr[i] - c;    // 减去之前丢失的低阶位
        double t = sum + y;        // 求和
        c = (t - sum) - y;         // 计算丢失的部分
        sum = t;
    }
    return sum;
}
```

### 2.3 避免溢出和下溢

```c
// ❌ 可能溢出：计算范数
float norm_bad(float x, float y) {
    return sqrt(x*x + y*y);  // x*x 可能溢出
}

// ✅ 缩放法
float norm_good(float x, float y) {
    float max_val = fmax(fabs(x), fabs(y));
    if (max_val == 0) return 0;
    float sx = x / max_val;
    float sy = y / max_val;
    return max_val * sqrt(sx*sx + sy*sy);
}
```

---

## 3. 快速数学

### 3.1 编译器内置函数

```c
// GCC/Clang 提供不检查 errno 的快速版本
#include <math.h>

double r = sqrt(x);        // 标准：可能设 errno
double r = __builtin_sqrt(x);  // 快速：不检查异常，可内联

// 其他快速内置
__builtin_sin(x);
__builtin_cos(x);
__builtin_exp(x);
__builtin_log(x);
```

### 3.2 `-ffast-math` 风险

```bash
# 启用激进优化（可能违反 IEEE 754）
gcc -O3 -ffast-math ...

# 效果：
# - 假设 NaN/Inf 不会出现
# - 允许重排浮点运算（(a+b)+c ≠ a+(b+c)）
# - 允许用 x*x 替换 pow(x, 2)
# - 禁用有符号零的处理
```

> ⚠️ **仅在确定代码不依赖 IEEE 754 精确语义时使用 `-ffast-math`**

---

## 4. 检查清单

- [ ] 浮点比较使用 epsilon，不使用 `==`
- [ ] 大量累加使用 Kahan 求和或 double
- [ ] 相近大数相减前评估精度损失
- [ ] 中间计算使用更高精度类型
- [ ] 注意 `float` 字面量后缀：`3.14f` 而非 `3.14`（后者是 double）

---

> **最后更新**: 2026-05-11
> **参考**: What Every Computer Scientist Should Know About Floating-Point Arithmetic (Goldberg)
