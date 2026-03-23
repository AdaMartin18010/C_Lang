# C语言十进制浮点数完全指南

## 目录

- [C语言十进制浮点数完全指南](#c语言十进制浮点数完全指南)
  - [目录](#目录)
  - [1. 十进制浮点数概述](#1-十进制浮点数概述)
    - [1.1 为什么需要十进制浮点数](#11-为什么需要十进制浮点数)
    - [1.2 十进制浮点数的应用场景](#12-十进制浮点数的应用场景)
    - [1.3 C23 标准中的十进制浮点支持](#13-c23-标准中的十进制浮点支持)
  - [2. 与二进制浮点的区别](#2-与二进制浮点的区别)
    - [2.1 根本差异：基数不同](#21-根本差异基数不同)
    - [2.2 精度问题详细分析](#22-精度问题详细分析)
    - [2.3 舍入模式差异](#23-舍入模式差异)
  - [3. IEEE 754-2008 十进制浮点标准](#3-ieee-754-2008-十进制浮点标准)
    - [3.1 标准概述](#31-标准概述)
    - [3.2 编码格式详解](#32-编码格式详解)
    - [3.3 特殊值处理](#33-特殊值处理)
  - [4. \_Decimal32 详解](#4-_decimal32-详解)
    - [4.1 基本格式](#41-基本格式)
    - [4.2 \_Decimal32 运算示例](#42-_decimal32-运算示例)
  - [5. \_Decimal64 详解](#5-_decimal64-详解)
    - [5.1 基本格式](#51-基本格式)
    - [5.2 \_Decimal64 高级运算](#52-_decimal64-高级运算)
  - [6. \_Decimal128 详解](#6-_decimal128-详解)
    - [6.1 基本格式](#61-基本格式)
    - [6.2 \_Decimal128 高精度计算](#62-_decimal128-高精度计算)
  - [7. 字面量表示和后缀](#7-字面量表示和后缀)
    - [7.1 字面量后缀详解](#71-字面量后缀详解)
    - [7.2 字面量陷阱和注意事项](#72-字面量陷阱和注意事项)
  - [8. 运算和转换](#8-运算和转换)
    - [8.1 基本运算](#81-基本运算)
    - [8.2 类型转换](#82-类型转换)
  - [9. 财务计算最佳实践](#9-财务计算最佳实践)
    - [9.1 货币表示规范](#91-货币表示规范)
    - [9.2 错误处理和验证](#92-错误处理和验证)
  - [10. 实际项目: 货币计算库完整实现](#10-实际项目-货币计算库完整实现)
    - [10.1 完整货币库头文件](#101-完整货币库头文件)
    - [10.2 完整货币库实现](#102-完整货币库实现)
    - [10.3 使用示例](#103-使用示例)
  - [11. 与 Cobol/Java BigDecimal 的对比](#11-与-coboljava-bigdecimal-的对比)
    - [11.1 技术对比](#111-技术对比)
    - [11.2 迁移指南](#112-迁移指南)
  - [总结](#总结)
    - [关键要点回顾](#关键要点回顾)
    - [编译命令参考](#编译命令参考)
    - [进一步学习资源](#进一步学习资源)
  - [深入理解](#深入理解)
    - [技术原理](#技术原理)
    - [实践指南](#实践指南)
    - [相关资源](#相关资源)

---

## 1. 十进制浮点数概述

### 1.1 为什么需要十进制浮点数

在金融、商业和数据库应用中，十进制浮点数至关重要。让我们通过具体例子说明为什么二进制浮点数不适合财务计算：

```c
/*
 * 编译: gcc -std=c23 binary_precision_problem.c -o binary_precision_problem
 *
 * 演示二进制浮点数的精度问题
 */

#include <stdio.h>
#include <stdint.h>

int main(void) {
    /*
     * 问题 1: 0.1 无法精确表示
     * 在二进制浮点中，0.1 = 0.0001100110011... (无限循环)
     */
    double price = 0.1;
    double total = 0.0;

    /* 累加 0.1 十次 */
    for (int i = 0; i < 10; i++) {
        total += price;
    }

    printf("=== 二进制浮点数精度问题演示 ===\n\n");

    printf("期望结果: 0.1 * 10 = 1.0\n");
    printf("实际结果: 0.1 * 10 = %.17f\n\n", total);

    /* 比较结果 */
    if (total == 1.0) {
        printf("比较: total == 1.0 ? 是\n");
    } else {
        printf("比较: total == 1.0 ? 否 (精度丢失!)\n");
        printf("误差: %.17f\n\n", total - 1.0);
    }

    /*
     * 问题 2: 财务计算中的舍入误差累积
     */
    double amount1 = 0.05;  /* 5分钱 */
    double amount2 = 0.06;  /* 6分钱 */
    double sum = amount1 + amount2;

    printf("财务计算示例:\n");
    printf("0.05 + 0.06 = %.17f\n", sum);
    printf("期望结果: 0.11\n");
    printf("实际结果: %.17f\n\n", sum);

    /*
     * 问题 3: 除法运算中的精度问题
     */
    double a = 10.0;
    double b = 3.0;
    double result = (a / b) * 3;

    printf("除法运算:\n");
    printf("(10.0 / 3.0) * 3 = %.17f\n", result);
    printf("期望结果: 10.0\n");
    printf("实际结果: %.17f\n\n", result);

    /*
     * 问题 4: 显示二进制表示
     */
    union {
        double d;
        uint64_t u;
    } converter;

    converter.d = 0.1;
    printf("0.1 的 IEEE 754 二进制表示:\n");
    printf("十六进制: 0x%016llX\n", (unsigned long long)converter.u);
    printf("这不是精确的 0.1，而是最接近的近似值\n");

    return 0;
}
```

### 1.2 十进制浮点数的应用场景

| 应用场景 | 为什么需要十进制浮点 | 风险等级 |
|---------|-------------------|---------|
| 货币计算 | 必须精确到分 | 极高 |
| 税务计算 | 法律要求精确 | 极高 |
| 银行利息 | 监管合规要求 | 极高 |
| 库存管理 | 避免累积误差 | 高 |
| 科学计算 | 十进制输入输出 | 中 |
| 数据库系统 | 与SQL DECIMAL兼容 | 高 |

### 1.3 C23 标准中的十进制浮点支持

```c
/*
 * 编译: gcc -std=c23 decimal_types_demo.c -o decimal_types_demo
 * 注意: 需要 GCC 12+ 或 Clang 15+ 支持 _Decimal 类型
 */

#include <stdio.h>

/* C23 标准引入了三种十进制浮点类型 */

int main(void) {
    printf("=== C23 十进制浮点类型 ===\n\n");

    /* _Decimal32: 7位有效数字, 精度约 1E-7 */
    _Decimal32 d32 = 0.1DF;
    printf("_Decimal32  大小: %zu 字节\n", sizeof(_Decimal32));

    /* _Decimal64: 16位有效数字, 精度约 1E-16 */
    _Decimal64 d64 = 0.1DD;
    printf("_Decimal64  大小: %zu 字节\n", sizeof(_Decimal64));

    /* _Decimal128: 34位有效数字, 精度约 1E-34 */
    _Decimal128 d128 = 0.1DL;
    printf("_Decimal128 大小: %zu 字节\n", sizeof(_Decimal128));

    printf("\n=== 精度比较 ===\n");
    printf("所有类型都能精确表示 0.1!\n");

    return 0;
}
```

---

## 2. 与二进制浮点的区别

### 2.1 根本差异：基数不同

```c
/*
 * 编译: gcc -std=c23 radix_comparison.c -o radix_comparison
 *
 * 比较二进制浮点和十进制浮点的基数差异
 */

#include <stdio.h>
#include <float.h>

int main(void) {
    printf("=== 浮点数基数(Radix)比较 ===\n\n");

    /* 二进制浮点数使用基数 2 */
    printf("二进制浮点数 (float/double):\n");
    printf("  基数 (FLT_RADIX): %d\n", FLT_RADIX);
    printf("  表示方式: 数字 × 2^指数\n");
    printf("  可精确表示的分数: 1/2, 1/4, 1/8, 3/4, ...\n");
    printf("  不能精确表示的分数: 1/10, 1/5, 1/100, ...\n\n");

    /* 十进制浮点数使用基数 10 */
    printf("十进制浮点数 (_Decimal32/64/128):\n");
    printf("  基数: 10\n");
    printf("  表示方式: 数字 × 10^指数\n");
    printf("  可精确表示的分数: 1/10, 1/100, 1/1000, ...\n");
    printf("  这正是财务计算需要的!\n\n");

    /* 具体例子 */
    printf("=== 具体例子 ===\n");

    double binary_val = 0.1;
    _Decimal64 decimal_val = 0.1DD;

    printf("数值: 0.1\n");
    printf("  二进制 double:  不精确 (近似值)\n");
    printf("  十进制 _Decimal64: 精确表示\n\n");

    /* 数学原理 */
    printf("=== 数学原理 ===\n");
    printf("1/10 = 1/(2×5) = 0.1 (十进制)\n");
    printf("1/10 在二进制中 = 0.00011001100110011... (无限循环)\n");
    printf("因此二进制浮点无法精确存储 0.1\n");

    return 0;
}
```

### 2.2 精度问题详细分析

```c
/*
 * 编译: gcc -std=c23 precision_analysis.c -o precision_analysis -lm
 *
 * 深入分析精度问题
 */

#include <stdio.h>
#include <math.h>

/* 自定义函数：打印二进制浮点误差 */
void analyze_binary_error(double val, const char* description) {
    double rounded = round(val * 100.0) / 100.0;
    double error = fabs(val - rounded);

    printf("  %s:\n", description);
    printf("    存储值: %.17f\n", val);
    printf("    期望的两位小数: %.2f\n", rounded);
    printf("    绝对误差: %.2e\n", error);
}

int main(void) {
    printf("=== 精度问题详细分析 ===\n\n");

    /* 常见的财务数值 */
    printf("1. 常见财务数值的二进制表示误差:\n");
    analyze_binary_error(0.01, "1分钱 (0.01)");
    analyze_binary_error(0.05, "5分钱 (0.05)");
    analyze_binary_error(0.10, "1角钱 (0.10)");
    analyze_binary_error(0.25, "2角5分 (0.25) - 可以精确表示!");
    printf("\n");

    /* 为什么 0.25 可以精确表示 */
    printf("2. 为什么 0.25 可以精确表示?\n");
    printf("   0.25 = 1/4 = 1/2^2\n");
    printf("   二进制表示: 0.01 (精确的 2位)\n\n");

    /* 累积误差演示 */
    printf("3. 累积误差问题:\n");
    double binary_sum = 0.0;
    _Decimal64 decimal_sum = 0.0DD;

    /* 累加 1000 次 0.001 */
    for (int i = 0; i < 1000; i++) {
        binary_sum += 0.001;
        decimal_sum += 0.001DD;
    }

    printf("   累加 1000 次 0.001:\n");
    printf("   期望结果: 1.0\n");
    printf("   二进制结果: %.15f\n", binary_sum);
    printf("   十进制结果: %.15f (转换为double显示)\n\n", (double)decimal_sum);

    /* 比较误差 */
    printf("4. 误差比较:\n");
    double binary_error = fabs(binary_sum - 1.0);
    _Decimal64 decimal_error = decimal_sum - 1.0DD;

    printf("   二进制误差: %.2e\n", binary_error);
    printf("   十进制误差: 0 (精确计算)\n\n");

    /* 实际影响 */
    printf("5. 实际财务影响:\n");
    printf("   假设每天处理 100 万笔交易\n");
    printf("   每笔交易误差 ~1e-16\n");
    printf("   日累积误差: ~1e-10 (看似很小)\n");
    printf("   但法律要求: 必须精确到分!\n");
    printf("   因此二进制浮点不适合财务计算\n");

    return 0;
}
```

### 2.3 舍入模式差异

```c
/*
 * 编译: gcc -std=c23 rounding_modes.c -o rounding_modes
 *
 * 演示不同的舍入模式
 */

#include <stdio.h>
#include <fenv.h>

int main(void) {
    printf("=== IEEE 754 舍入模式 ===\n\n");

    /* 获取当前舍入模式 */
    int current_mode = fegetround();

    printf("当前舍入模式: ");
    switch (current_mode) {
        case FE_TONEAREST:  printf("向最近舍入(默认)\n"); break;
        case FE_UPWARD:     printf("向正无穷舍入\n"); break;
        case FE_DOWNWARD:   printf("向负无穷舍入\n"); break;
        case FE_TOWARDZERO: printf("向零舍入\n"); break;
        default:            printf("未知\n");
    }

    printf("\n十进制浮点数支持以下舍入模式:\n");
    printf("  1. roundTiesToEven    - 银行家舍入(默认)\n");
    printf("  2. roundTiesToAway    - 远离零舍入\n");
    printf("  3. roundTowardPositive - 向上舍入\n");
    printf("  4. roundTowardNegative - 向下舍入\n");
    printf("  5. roundTowardZero    - 向零截断\n\n");

    /* 银行家舍入示例 */
    printf("银行家舍入(roundTiesToEven)示例:\n");
    printf("  2.5 -> 2 (2是偶数)\n");
    printf("  3.5 -> 4 (4是偶数)\n");
    printf("  这种舍入方式减少了累积误差\n\n");

    /* 财务舍入示例 */
    printf("财务舍入(roundTiesToAway)示例:\n");
    printf("  2.5 -> 3 (远离零)\n");
    printf("  3.5 -> 4 (远离零)\n");
    printf("  这种舍入方式符合直觉，常用于财务\n");

    return 0;
}
```

---

## 3. IEEE 754-2008 十进制浮点标准

### 3.1 标准概述

```c
/*
 * 编译: gcc -std=c23 ieee754_decimal_standard.c -o ieee754_decimal_standard
 *
 * IEEE 754-2008 十进制浮点标准介绍
 */

#include <stdio.h>
#include <stdint.h>

/* IEEE 754-2008 定义的三种十进制格式 */

typedef struct {
    uint32_t fields;
    /* 实际存储使用 DPD (Densely Packed Decimal) 编码 */
} IEEE754_Decimal32;

typedef struct {
    uint64_t fields[2];
} IEEE754_Decimal64;

typedef struct {
    uint64_t fields[4];
} IEEE754_Decimal128;

int main(void) {
    printf("=== IEEE 754-2008 十进制浮点标准 ===\n\n");

    printf("标准发布: 2008年8月\n");
    printf("主要更新: 增加了十进制浮点算术\n\n");

    printf("=== 三种十进制格式 ===\n\n");

    printf("1. decimal32 (_Decimal32 in C)\n");
    printf("   存储大小: 32位 (4字节)\n");
    printf("   精度: 7位十进制数字\n");
    printf("   指数范围: -95 到 +96\n");
    printf("   数值范围: ~1.0E-95 到 ~9.99E+96\n\n");

    printf("2. decimal64 (_Decimal64 in C)\n");
    printf("   存储大小: 64位 (8字节)\n");
    printf("   精度: 16位十进制数字\n");
    printf("   指数范围: -383 到 +384\n");
    printf("   数值范围: ~1.0E-383 到 ~9.99E+384\n\n");

    printf("3. decimal128 (_Decimal128 in C)\n");
    printf("   存储大小: 128位 (16字节)\n");
    printf("   精度: 34位十进制数字\n");
    printf("   指数范围: -6143 到 +6144\n");
    printf("   数值范围: ~1.0E-6143 到 ~9.99E+6144\n\n");

    printf("=== 编码方式 ===\n\n");

    printf("IEEE 754-2008 定义了两种编码方式:\n");
    printf("  1. DPD (Densely Packed Decimal) - 密集打包十进制\n");
    printf("     - 使用 10位编码 3个十进制数字\n");
    printf("     - 编码效率: 1000/1024 ≈ 97.7%\n\n");

    printf("  2. BID (Binary Integer Decimal) - 二进制整数十进制\n");
    printf("     - 使用二进制整数存储尾数\n");
    printf("     - 更适合硬件实现\n\n");

    printf("C语言实现通常使用 BID 编码 (Intel/AMD 处理器)\n");

    return 0;
}
```

### 3.2 编码格式详解

```c
/*
 * 编译: gcc -std=c23 encoding_format.c -o encoding_format
 *
 * 十进制浮点数编码格式详解
 */

#include <stdio.h>
#include <stdint.h>

/*
 * Decimal64 BID 编码布局 (Intel/AMD 使用)
 *
 * 位布局 (64位):
 *  63: 符号位 (S)
 *  62-51: 组合字段 (G)
 *  50-0: 有效数字 (T) - 使用二进制整数编码
 */

void print_decimal64_layout(void) {
    printf("=== Decimal64 BID 编码布局 ===\n\n");

    printf("位分布 (从高位到低位):\n");
    printf("  63    : 符号位 (S) - 0为正, 1为负\n");
    printf("  62-51  : 组合字段 (G) - 包含指数和高位数字\n");
    printf("  50-0   : 有效数字 (T) - 二进制整数编码的尾数\n\n");

    printf("组合字段编码:\n");
    printf("  G4G3G2 是 111 时: 指数为 G0G1T 的形式 (特殊值)\n");
    printf("  其他情况: 指数直接编码\n\n");

    printf("特殊值编码:\n");
    printf("  +Infinity: 0x7800000000000000\n");
    printf("  -Infinity: 0xF800000000000000\n");
    printf("  NaN      : G4G3 = 11, G2 = 1\n");
    printf("  sNaN     : G4G3 = 11, G2 = 0\n");
}

int main(void) {
    print_decimal64_layout();

    printf("\n=== 编码示例 ===\n\n");

    /* 数值 123.456 的编码概念 */
    printf("数值: 123.456\n");
    printf("科学计数法: 1.23456 × 10^2\n");
    printf("或: 123456 × 10^-3\n\n");

    printf("在 BID 编码中:\n");
    printf("  符号位: 0 (正数)\n");
    printf("  指数: -3 (编码后)\n");
    printf("  系数: 123456 (二进制存储)\n\n");

    /* DPD 编码概念 */
    printf("=== DPD 编码 (IBM 使用) ===\n\n");
    printf("DPD 每 10位 编码 3个十进制数字 (0-999)\n");
    printf("编码表:\n");
    printf("  a,b,c,d,e,f,g,h,i 是 3个BCD数字的位\n");
    printf("  p,q,r 是编码位\n\n");

    return 0;
}
```

### 3.3 特殊值处理

```c
/*
 * 编译: gcc -std=c23 special_values.c -o special_values
 *
 * 十进制浮点特殊值处理
 */

#include <stdio.h>
#include <math.h>
#include <string.h>

/* 检查十进制浮点数的宏 (概念性) */

int main(void) {
    printf("=== 十进制浮点特殊值 ===\n\n");

    /* 无穷大 */
    printf("1. 无穷大 (Infinity)\n");
    printf("   正无穷: 1.0DD / 0.0DD\n");
    printf("   负无穷: -1.0DD / 0.0DD\n");
    printf("   比较: Infinity > 任何有限数\n\n");

    /* NaN */
    printf("2. NaN (Not a Number)\n");
    printf("   静默 NaN (qNaN): 0.0DD / 0.0DD 的结果\n");
    printf("   信号 NaN (sNaN): 未初始化的十进制变量\n");
    printf("   比较: NaN != NaN (总是成立)\n\n");

    /* 零的符号 */
    printf("3. 有符号零\n");
    printf("   +0.0DD: 正零\n");
    printf("   -0.0DD: 负零\n");
    printf("   比较: +0.0DD == -0.0DD (相等)\n");
    printf("   1.0DD / +0.0DD = +Infinity\n");
    printf("   1.0DD / -0.0DD = -Infinity\n\n");

    /* 次正规数 */
    printf("4. 次正规数 (Subnormal/_denormal)\n");
    printf("   比最小正规数还小的数\n");
    printf("   精度逐渐降低\n");
    printf("   用于平滑下溢\n\n");

    /* 未初始化值 */
    printf("5. 陷阱表示\n");
    printf("   某些位模式被保留为陷阱\n");
    printf("   访问时可能产生异常\n");

    return 0;
}
```



---

## 4. _Decimal32 详解

### 4.1 基本格式

```c
/*
 * 编译: gcc -std=c23 decimal32_details.c -o decimal32_details
 *
 * _Decimal32 详细说明
 */

#include <stdio.h>
#include <float.h>
#include <stdint.h>

int main(void) {
    printf("=== _Decimal32 详解 ===\n\n");

    /* 基本信息 */
    printf("基本信息:\n");
    printf("  存储大小: %zu 字节 (%zu 位)\n", sizeof(_Decimal32), sizeof(_Decimal32) * 8);
    printf("  C23 关键字: _Decimal32\n");
    printf("  后缀: DF (Decimal Float)\n\n");

    /* 精度 */
    printf("精度规范:\n");
    printf("  有效数字: 7位十进制数字\n");
    printf("  相对精度: 约 1 × 10^-7\n");
    printf("  足够精度: 存储到 9999999 (千万级别)\n\n");

    /* 指数范围 */
    printf("指数范围:\n");
    printf("  最小指数: -95 (emax = 96, emin = 95)\n");
    printf("  最大指数: +96\n");
    printf("  偏置值 (bias): 101\n\n");

    /* 数值范围 */
    printf("数值范围:\n");
    printf("  最小正规数: 1 × 10^-95\n");
    printf("  最大正规数: 9.999999 × 10^96\n");
    printf("  最小次正规数: 1 × 10^-101\n\n");

    /* 财务应用评估 */
    printf("财务应用评估:\n");
    printf("  ✓ 适合: 小额交易 (< 1000万)\n");
    printf("  ✗ 不适合: 大型财务报表\n");
    printf("  ✗ 不适合: 需要高精度计算\n");
    printf("  建议: 仅用于存储, 计算使用 _Decimal64\n\n");

    /* 实际例子 */
    printf("使用示例:\n");
    _Decimal32 price = 99.99DF;
    _Decimal32 quantity = 1000.0DF;
    _Decimal32 total = price * quantity;

    printf("  单价: %.2f (显示为 double)\n", (double)price);
    printf("  数量: %.0f\n", (double)quantity);
    printf("  总价: %.2f\n\n", (double)total);

    /* 编码示例 */
    printf("编码细节:\n");
    printf("  位布局 (32位):\n");
    printf("    31: 符号位\n");
    printf("    30-24: 组合字段 (7位)\n");
    printf("    23-20: 系数位 0-3 (4位)\n");
    printf("    19-10: 系数位 4-9 (10位 DPD)\n");
    printf("    9-0: 系数位 10-19 (10位 DPD)\n");

    return 0;
}
```

### 4.2 _Decimal32 运算示例

```c
/*
 * 编译: gcc -std=c23 decimal32_operations.c -o decimal32_operations
 *
 * _Decimal32 基本运算
 */

#include <stdio.h>

/* 打印 _Decimal32 数组 */
void print_prices(const char* title, _Decimal32* prices, int count) {
    printf("%s:\n", title);
    for (int i = 0; i < count; i++) {
        printf("  [%d]: $%.2f\n", i, (double)prices[i]);
    }
    printf("\n");
}

int main(void) {
    printf("=== _Decimal32 运算示例 ===\n\n");

    /* 基本算术运算 */
    printf("1. 基本算术运算:\n");
    _Decimal32 a = 123.456DF;
    _Decimal32 b = 78.9DF;

    _Decimal32 sum = a + b;
    _Decimal32 diff = a - b;
    _Decimal32 prod = a * b;
    _Decimal32 quot = a / b;

    printf("  a = %.3f\n", (double)a);
    printf("  b = %.3f\n", (double)b);
    printf("  a + b = %.3f\n", (double)sum);
    printf("  a - b = %.3f\n", (double)diff);
    printf("  a * b = %.3f\n", (double)prod);
    printf("  a / b = %.6f\n\n", (double)quot);

    /* 价格计算示例 */
    printf("2. 零售价格计算:\n");
    _Decimal32 base_price = 29.99DF;
    _Decimal32 discount_rate = 0.15DF;  /* 15% 折扣 */
    _Decimal32 tax_rate = 0.08DF;       /* 8% 税费 */

    _Decimal32 discount_amount = base_price * discount_rate;
    _Decimal32 discounted_price = base_price - discount_amount;
    _Decimal32 tax_amount = discounted_price * tax_rate;
    _Decimal32 final_price = discounted_price + tax_amount;

    printf("  原价: $%.2f\n", (double)base_price);
    printf("  折扣 (15%%): -$%.2f\n", (double)discount_amount);
    printf("  折后价: $%.2f\n", (double)discounted_price);
    printf("  税费 (8%%): +$%.2f\n", (double)tax_amount);
    printf("  最终价格: $%.2f\n\n", (double)final_price);

    /* 批量处理 */
    printf("3. 批量价格处理:\n");
    _Decimal32 prices[] = {9.99DF, 19.99DF, 29.99DF, 49.99DF, 99.99DF};
    int n = sizeof(prices) / sizeof(prices[0]);

    _Decimal32 subtotal = 0.0DF;
    for (int i = 0; i < n; i++) {
        subtotal += prices[i];
    }

    print_prices("商品价格", prices, n);
    printf("  小计: $%.2f\n", (double)subtotal);

    _Decimal32 avg_price = subtotal / n;
    printf("  平均价格: $%.2f\n\n", (double)avg_price);

    /* 精度限制演示 */
    printf("4. 精度限制 (7位有效数字):\n");
    _Decimal32 precise = 1234567.0DF;  /* 正好7位 */
    _Decimal32 overflow = 12345678.0DF; /* 8位 - 可能丢失精度 */

    printf("  7位数字 1234567: %.1f (精确)\n", (double)precise);
    printf("  8位数字 12345678: %.1f (注意精度)\n\n", (double)overflow);

    /* 货币转换 */
    printf("5. 货币转换:\n");
    _Decimal32 usd_amount = 100.00DF;
    _Decimal32 exchange_rate = 7.2456DF; /* USD to CNY */

    _Decimal32 cny_amount = usd_amount * exchange_rate;
    printf("  USD %.2f = CNY %.4f\n", (double)usd_amount, (double)cny_amount);

    return 0;
}
```

---

## 5. _Decimal64 详解

### 5.1 基本格式

```c
/*
 * 编译: gcc -std=c23 decimal64_details.c -o decimal64_details
 *
 * _Decimal64 详细说明 - 最实用的十进制浮点类型
 */

#include <stdio.h>
#include <float.h>

int main(void) {
    printf("=== _Decimal64 详解 ===\n\n");

    /* 基本信息 */
    printf("基本信息:\n");
    printf("  存储大小: %zu 字节 (%zu 位)\n", sizeof(_Decimal64), sizeof(_Decimal64) * 8);
    printf("  C23 关键字: _Decimal64\n");
    printf("  后缀: DD (Decimal Double)\n\n");

    /* 精度 */
    printf("精度规范:\n");
    printf("  有效数字: 16位十进制数字\n");
    printf("  相对精度: 约 1 × 10^-16\n");
    printf("  足够精度: 存储到 99,999,999,999,999.99 (百万亿级别)\n\n");

    /* 指数范围 */
    printf("指数范围:\n");
    printf("  最小指数: -383\n");
    printf("  最大指数: +384\n");
    printf("  偏置值 (bias): 398\n\n");

    /* 数值范围 */
    printf("数值范围:\n");
    printf("  最小正规数: 1 × 10^-383\n");
    printf("  最大正规数: 9.999999999999999 × 10^384\n");
    printf("  最小次正规数: 1 × 10^-398\n\n");

    /* 财务应用评估 */
    printf("财务应用评估:\n");
    printf("  ✓ 适合: 几乎所有财务计算\n");
    printf("  ✓ 适合: 企业级财务报表\n");
    printf("  ✓ 适合: 银行利息计算\n");
    printf("  ✓ 适合: 税务计算\n");
    printf("  推荐: 作为默认十进制浮点类型\n\n");

    /* 与 double 比较 */
    printf("与 double 比较:\n");
    printf("  存储大小: 相同 (8字节)\n");
    printf("  精度: 16位十进制 vs 约15-17位十进制等效\n");
    printf("  关键优势: 精确表示十进制分数\n");
    printf("  关键劣势: 运算速度较慢\n\n");

    /* 编码细节 */
    printf("编码细节:\n");
    printf("  位布局 (64位 BID 编码):\n");
    printf("    63: 符号位\n");
    printf("    62-51: 组合字段 (12位)\n");
    printf("    50-0: 有效数字系数 (51位)\n");
    printf("  最大系数: 2^53 - 1 = 9,007,199,254,740,991\n");
    printf("  可精确表示16位十进制整数\n");

    return 0;
}
```

### 5.2 _Decimal64 高级运算

```c
/*
 * 编译: gcc -std=c23 decimal64_advanced.c -o decimal64_advanced
 *
 * _Decimal64 高级运算示例
 */

#include <stdio.h>
#include <math.h>

/* 复利的精确计算 */
_Decimal64 compound_interest(_Decimal64 principal, _Decimal64 rate,
                              int times_per_year, int years) {
    /*
     * 复利公式: A = P × (1 + r/n)^(n×t)
     * P = 本金
     * r = 年利率
     * n = 每年复利次数
     * t = 年数
     */

    _Decimal64 n = (_Decimal64)times_per_year;
    _Decimal64 t = (_Decimal64)years;

    /* 计算 (1 + r/n) */
    _Decimal64 base = 1.0DD + (rate / n);

    /* 计算指数 n×t */
    int exponent = times_per_year * years;

    /* 手动计算幂 (因为 pow 可能不直接支持 _Decimal64) */
    _Decimal64 result = 1.0DD;
    for (int i = 0; i < exponent; i++) {
        result *= base;
    }

    return principal * result;
}

/* 贷款月供计算 */
_Decimal64 loan_payment(_Decimal64 principal, _Decimal64 monthly_rate, int months) {
    /*
     * 月供公式: M = P × [r(1+r)^n] / [(1+r)^n - 1]
     * P = 贷款本金
     * r = 月利率
     * n = 还款月数
     */

    if (monthly_rate == 0.0DD) {
        return principal / (_Decimal64)months;
    }

    /* 计算 (1+r)^n */
    _Decimal64 one_plus_r = 1.0DD + monthly_rate;
    _Decimal64 pow_n = 1.0DD;
    for (int i = 0; i < months; i++) {
        pow_n *= one_plus_r;
    }

    _Decimal64 numerator = monthly_rate * pow_n;
    _Decimal64 denominator = pow_n - 1.0DD;

    return principal * (numerator / denominator);
}

int main(void) {
    printf("=== _Decimal64 高级财务计算 ===\n\n");

    /* 1. 复利计算 */
    printf("1. 复利计算:\n");
    _Decimal64 principal = 10000.00DD;  /* 本金 10,000 */
    _Decimal64 annual_rate = 0.05DD;     /* 年利率 5% */
    int years = 10;

    printf("  本金: $%.2f\n", (double)principal);
    printf("  年利率: %.2f%%\n", (double)(annual_rate * 100.0DD));
    printf("  投资年限: %d 年\n", years);

    _Decimal64 different_frequencies[5];
    const char* freq_names[] = {"年复利", "半年复利", "季复利", "月复利", "日复利"};
    int frequencies[] = {1, 2, 4, 12, 365};

    for (int i = 0; i < 5; i++) {
        different_frequencies[i] = compound_interest(principal, annual_rate,
                                                      frequencies[i], years);
        printf("  %s: $%.2f\n", freq_names[i], (double)different_frequencies[i]);
    }
    printf("\n");

    /* 2. 贷款计算 */
    printf("2. 房屋贷款计算:\n");
    _Decimal64 loan_amount = 500000.00DD;  /* 贷款 50万 */
    _Decimal64 annual_loan_rate = 0.045DD;  /* 年利率 4.5% */
    int loan_years = 30;
    int total_months = loan_years * 12;
    _Decimal64 monthly_rate = annual_loan_rate / 12.0DD;

    _Decimal64 monthly_payment = loan_payment(loan_amount, monthly_rate, total_months);
    _Decimal64 total_payment = monthly_payment * (_Decimal64)total_months;
    _Decimal64 total_interest = total_payment - loan_amount;

    printf("  贷款金额: $%.2f\n", (double)loan_amount);
    printf("  年利率: %.2f%%\n", (double)(annual_loan_rate * 100.0DD));
    printf("  贷款期限: %d 年\n", loan_years);
    printf("  月供: $%.2f\n", (double)monthly_payment);
    printf("  总还款: $%.2f\n", (double)total_payment);
    printf("  总利息: $%.2f\n\n", (double)total_interest);

    /* 3. 税务计算 */
    printf("3. 阶梯税率计算:\n");
    _Decimal64 income = 150000.00DD;  /* 年收入 15万 */

    /* 简化阶梯税率 */
    _Decimal64 tax = 0.0DD;
    _Decimal64 remaining = income;

    /* 0-50,000: 10% */
    _Decimal64 bracket1 = 50000.00DD;
    _Decimal64 rate1 = 0.10DD;
    _Decimal64 taxable1 = remaining < bracket1 ? remaining : bracket1;
    tax += taxable1 * rate1;
    remaining -= taxable1;

    /* 50,001-100,000: 20% */
    if (remaining > 0.0DD) {
        _Decimal64 bracket2 = 50000.00DD;
        _Decimal64 rate2 = 0.20DD;
        _Decimal64 taxable2 = remaining < bracket2 ? remaining : bracket2;
        tax += taxable2 * rate2;
        remaining -= taxable2;
    }

    /* 100,001+: 30% */
    if (remaining > 0.0DD) {
        _Decimal64 rate3 = 0.30DD;
        tax += remaining * rate3;
    }

    _Decimal64 effective_rate = tax / income;

    printf("  年收入: $%.2f\n", (double)income);
    printf("  应缴税款: $%.2f\n", (double)tax);
    printf("  税后收入: $%.2f\n", (double)(income - tax));
    printf("  实际税率: %.2f%%\n\n", (double)(effective_rate * 100.0DD));

    /* 4. 批量交易处理 */
    printf("4. 批量交易处理:\n");
    _Decimal64 transactions[] = {
        1234.56DD, 789.12DD, 3456.78DD, 901.23DD,
        5678.90DD, 234.56DD, 8901.23DD, 456.78DD,
        6789.01DD, 123.45DD
    };
    int num_transactions = sizeof(transactions) / sizeof(transactions[0]);

    _Decimal64 total_amount = 0.0DD;
    _Decimal64 max_transaction = transactions[0];
    _Decimal64 min_transaction = transactions[0];

    for (int i = 0; i < num_transactions; i++) {
        total_amount += transactions[i];
        if (transactions[i] > max_transaction) max_transaction = transactions[i];
        if (transactions[i] < min_transaction) min_transaction = transactions[i];
    }

    _Decimal64 avg_transaction = total_amount / (_Decimal64)num_transactions;

    printf("  交易数量: %d\n", num_transactions);
    printf("  总金额: $%.2f\n", (double)total_amount);
    printf("  平均金额: $%.2f\n", (double)avg_transaction);
    printf("  最大金额: $%.2f\n", (double)max_transaction);
    printf("  最小金额: $%.2f\n", (double)min_transaction);

    return 0;
}
```

---

## 6. _Decimal128 详解

### 6.1 基本格式

```c
/*
 * 编译: gcc -std=c23 decimal128_details.c -o decimal128_details
 *
 * _Decimal128 详细说明 - 超高精度十进制浮点
 */

#include <stdio.h>
#include <float.h>

int main(void) {
    printf("=== _Decimal128 详解 ===\n\n");

    /* 基本信息 */
    printf("基本信息:\n");
    printf("  存储大小: %zu 字节 (%zu 位)\n", sizeof(_Decimal128), sizeof(_Decimal128) * 8);
    printf("  C23 关键字: _Decimal128\n");
    printf("  后缀: DL (Decimal Long)\n\n");

    /* 精度 */
    printf("精度规范:\n");
    printf("  有效数字: 34位十进制数字\n");
    printf("  相对精度: 约 1 × 10^-34\n");
    printf("  足够精度: 存储天文数字级别的精确值\n\n");

    /* 指数范围 */
    printf("指数范围:\n");
    printf("  最小指数: -6143\n");
    printf("  最大指数: +6144\n");
    printf("  偏置值 (bias): 6176\n\n");

    /* 数值范围 */
    printf("数值范围:\n");
    printf("  最小正规数: 1 × 10^-6143\n");
    printf("  最大正规数: 约 9.99 × 10^6144\n");
    printf("  最小次正规数: 1 × 10^-6176\n");
    printf("  注意: 最大数值远超宇宙原子数量 (~10^80)\n\n");

    /* 应用场景 */
    printf("应用场景:\n");
    printf("  ✓ 超高精度科学计算\n");
    printf("  ✓ 国家级别财务核算\n");
    printf("  ✓ 高精度物理模拟\n");
    printf("  ✓ 中间计算结果保持精度\n");
    printf("  ✗ 一般商业应用 (性能开销大)\n\n");

    /* 与 _Decimal64 比较 */
    printf("与 _Decimal64 比较:\n");
    printf("  存储: 2倍 (16 vs 8 字节)\n");
    printf("  精度: 2倍以上 (34 vs 16 位)\n");
    printf("  范围: 指数范围大16倍\n");
    printf("  速度: 显著较慢\n\n");

    /* 编码细节 */
    printf("编码细节:\n");
    printf("  位布局 (128位 BID 编码):\n");
    printf("    127: 符号位\n");
    printf("    126-113: 组合字段 (14位)\n");
    printf("    112-0: 有效数字系数 (113位)\n");
    printf("  最大系数: 2^113 - 1\n");
    printf("  可精确表示34位十进制整数\n");

    return 0;
}
```

### 6.2 _Decimal128 高精度计算

```c
/*
 * 编译: gcc -std=c23 decimal128_precision.c -o decimal128_precision
 *
 * _Decimal128 超高精度计算示例
 */

#include <stdio.h>

/* 高精度 Pi 计算 (使用 Machin-like 公式) */
_Decimal128 compute_pi_decimal128(void) {
    /*
     * Machin 公式: π/4 = 4×arctan(1/5) - arctan(1/239)
     * 使用泰勒级数: arctan(x) = x - x^3/3 + x^5/5 - x^7/7 + ...
     */

    _Decimal128 pi = 0.0DL;
    _Decimal128 one = 1.0DL;
    _Decimal128 four = 4.0DL;
    _Decimal128 five = 5.0DL;
    _Decimal128 two39 = 239.0DL;

    /* 计算 4×arctan(1/5) */
    _Decimal128 arctan_1_5 = 0.0DL;
    _Decimal128 x = one / five;
    _Decimal128 x2 = x * x;
    _Decimal128 term = x;
    _Decimal128 n = one;

    for (int i = 0; i < 50; i++) {
        arctan_1_5 += term / n;
        term = -term * x2;
        n += 2.0DL;
    }

    /* 计算 arctan(1/239) */
    _Decimal128 arctan_1_239 = 0.0DL;
    x = one / two39;
    x2 = x * x;
    term = x;
    n = one;

    for (int i = 0; i < 20; i++) {
        arctan_1_239 += term / n;
        term = -term * x2;
        n += 2.0DL;
    }

    pi = four * (four * arctan_1_5 - arctan_1_239);
    return pi;
}

/* 高精度金融模型计算 */
_Decimal128 high_precision_npv(_Decimal128* cashflows, int n, _Decimal128 rate) {
    /*
     * 净现值 (NPV) 计算
     * NPV = Σ(CF_t / (1+r)^t) for t = 0 to n
     */

    _Decimal128 npv = 0.0DL;
    _Decimal128 discount_factor = 1.0DL;  /* (1+r)^0 = 1 */
    _Decimal128 one_plus_r = 1.0DL + rate;

    for (int t = 0; t < n; t++) {
        npv += cashflows[t] / discount_factor;
        discount_factor *= one_plus_r;
    }

    return npv;
}

int main(void) {
    printf("=== _Decimal128 超高精度计算 ===\n\n");

    /* 1. 高精度 Pi 计算 */
    printf("1. 高精度 Pi 计算:\n");
    _Decimal128 pi = compute_pi_decimal128();

    /* 格式化输出 Pi 的前34位 */
    printf("  Pi (前34位): ");
    _Decimal128 pi_temp = pi;
    _Decimal128 ten = 10.0DL;

    /* 整数部分 */
    printf("3.");
    pi_temp = pi_temp - 3.0DL;

    /* 小数部分 */
    for (int i = 0; i < 34; i++) {
        pi_temp = pi_temp * ten;
        int digit = (int)pi_temp;
        printf("%d", digit);
        pi_temp = pi_temp - (_Decimal128)digit;
    }
    printf("\n  参考值:      3.14159265358979323846264338327950288...\n\n");

    /* 2. 超高精度货币计算 */
    printf("2. 超高精度货币计算:\n");
    _Decimal128 national_debt = 31415926535897932384626433832.7950288DL;
    _Decimal128 population = 331000000.0DL;  /* 3.31亿 */

    _Decimal128 debt_per_person = national_debt / population;

    printf("  国家债务: %.4f\n", (double)national_debt);
    printf("  人口: %.0f\n", (double)population);
    printf("  人均债务: %.4f\n\n", (double)debt_per_person);

    /* 3. 高精度 NPV 计算 */
    printf("3. 高精度净现值 (NPV) 计算:\n");
    _Decimal128 cashflows[] = {
        -1000000.0DL,   /* 初始投资 */
        200000.0DL,     /* 第1年 */
        250000.0DL,     /* 第2年 */
        300000.0DL,     /* 第3年 */
        350000.0DL,     /* 第4年 */
        400000.0DL,     /* 第5年 */
        450000.0DL,     /* 第6年 */
        500000.0DL      /* 第7年 */
    };
    int num_years = sizeof(cashflows) / sizeof(cashflows[0]);
    _Decimal128 discount_rate = 0.08DL;  /* 8% 折现率 */

    _Decimal128 npv_result = high_precision_npv(cashflows, num_years, discount_rate);

    printf("  项目现金流 (10年):\n");
    for (int i = 0; i < num_years; i++) {
        printf("    第%d年: $%.2f\n", i, (double)cashflows[i]);
    }
    printf("  折现率: %.2f%%\n", (double)(discount_rate * 100.0DL));
    printf("  NPV: $%.4f\n\n", (double)npv_result);

    if (npv_result > 0.0DL) {
        printf("  结论: NPV > 0, 项目值得投资\n");
    } else {
        printf("  结论: NPV < 0, 项目不值得投资\n");
    }

    return 0;
}
```



---

## 7. 字面量表示和后缀

### 7.1 字面量后缀详解

```c
/*
 * 编译: gcc -std=c23 literal_suffixes.c -o literal_suffixes
 *
 * 十进制浮点数字面量后缀详解
 */

#include <stdio.h>

int main(void) {
    printf("=== 十进制浮点数字面量后缀 ===\n\n");

    /* 后缀说明 */
    printf("C23 标准定义的后缀:\n\n");

    printf("1. DF 后缀 - Decimal Float (_Decimal32)\n");
    printf("   示例: 3.14159DF, 0.1DF, 1e-7DF\n");
    printf("   大小: 4字节\n");
    printf("   精度: 7位有效数字\n\n");

    printf("2. DD 后缀 - Decimal Double (_Decimal64)\n");
    printf("   示例: 3.141592653589793DD, 0.1DD, 1e-16DD\n");
    printf("   大小: 8字节\n");
    printf("   精度: 16位有效数字\n\n");

    printf("3. DL 后缀 - Decimal Long (_Decimal128)\n");
    printf("   示例: 3.14159265358979323846264338327950288DL\n");
    printf("   大小: 16字节\n");
    printf("   精度: 34位有效数字\n\n");

    /* 字面量示例 */
    printf("=== 字面量表示示例 ===\n\n");

    /* 标准十进制表示 */
    printf("标准十进制表示:\n");
    _Decimal32 d32_1 = 123.456DF;
    _Decimal64 d64_1 = 123.4567890123456DD;
    _Decimal128 d128_1 = 123.4567890123456789012345678901234DL;

    printf("  123.456DF = %.6f (显示为double)\n", (double)d32_1);
    printf("  123.4567890123456DD = %.13f\n", (double)d64_1);
    printf("  123.4567890123456789012345678901234DL (精度超过double显示范围)\n\n");

    /* 科学计数法 */
    printf("科学计数法:\n");
    _Decimal32 d32_sci = 1.5e6DF;       /* 1.5 × 10^6 = 1500000 */
    _Decimal64 d64_sci = 1.5e-6DD;      /* 1.5 × 10^-6 = 0.0000015 */
    _Decimal128 d128_sci = 1.5e30DL;    /* 1.5 × 10^30 */

    printf("  1.5e6DF = %.0f\n", (double)d32_sci);
    printf("  1.5e-6DD = %.9f\n", (double)d64_sci);
    printf("  1.5e30DL = %.6e (科学计数法显示)\n\n", (double)d128_sci);

    /* 整数表示 */
    printf("整数表示 (自动转换为十进制浮点):\n");
    _Decimal32 d32_int = 42DF;
    _Decimal64 d64_int = 1000000DD;

    printf("  42DF = %.0f\n", (double)d32_int);
    printf("  1000000DD = %.0f\n\n", (double)d64_int);

    /* 负数 */
    printf("负数:\n");
    _Decimal32 d32_neg = -123.45DF;
    _Decimal64 d64_neg = -9.99e10DD;

    printf("  -123.45DF = %.2f\n", (double)d32_neg);
    printf("  -9.99e10DD = %.2f\n\n", (double)d64_neg);

    /* 前导零和小数点 */
    printf("前导零和小数点:\n");
    _Decimal32 d32_lead = 0.001DF;
    _Decimal64 d64_lead = .5DD;         /* 等价于 0.5 */
    _Decimal128 d128_lead = 5.DL;       /* 等价于 5.0 */

    printf("  0.001DF = %.3f\n", (double)d32_lead);
    printf("  .5DD = %.1f\n", (double)d64_lead);
    printf("  5.DL = %.1f\n\n", (double)d128_lead);

    /* 十六进制表示 (注意: 十进制浮点不支持十六进制!) */
    printf("重要说明:\n");
    printf("  十进制浮点字面量不支持十六进制表示!\n");
    printf("  0x1.fffffep+2 是二进制浮点字面量\n");
    printf("  十进制浮点必须使用十进制表示法\n");

    return 0;
}
```

### 7.2 字面量陷阱和注意事项

```c
/*
 * 编译: gcc -std=c23 literal_traps.c -o literal_traps
 *
 * 字面量使用中的陷阱
 */

#include <stdio.h>

int main(void) {
    printf("=== 字面量使用陷阱 ===\n\n");

    /* 陷阱 1: 忘记后缀导致精度丢失 */
    printf("陷阱 1: 忘记后缀\n");

    _Decimal64 wrong = 0.1;    /* 警告: 0.1 是 double，不是 _Decimal64 */
    _Decimal64 correct = 0.1DD; /* 正确: 明确指定十进制浮点 */

    printf("  wrong = 0.1 (double 转换): %.17f\n", (double)wrong);
    printf("  correct = 0.1DD: %.17f\n", (double)correct);
    printf("  注意: 某些编译器可能警告隐式转换\n\n");

    /* 陷阱 2: 指数过大或过小 */
    printf("陷阱 2: 指数范围检查\n");

    /* 这些在编译时会检查 */
    _Decimal32 d32_max = 9.999999e96DF;   /* 接近最大指数 */
    _Decimal32 d32_min = 1e-95DF;          /* 接近最小正规数 */

    printf("  Decimal32 最大正规数附近: %.6e\n", (double)d32_max);
    printf("  Decimal32 最小正规数: %.6e\n\n", (double)d32_min);

    /* 陷阱 3: 精度截断 */
    printf("陷阱 3: 精度截断\n");

    _Decimal32 d32_precise = 1.23456789DF;  /* 8位数字，但只有7位精度 */
    _Decimal64 d64_precise = 1.234567890123456789DD; /* 18位数字，但只有16位精度 */

    printf("  1.23456789DF (8位) 存储为: %.7f\n", (double)d32_precise);
    printf("  1.234567890123456789DD (18位) 存储为: %.16f\n\n", (double)d64_precise);

    /* 陷阱 4: 混合运算 */
    printf("陷阱 4: 混合类型运算\n");

    _Decimal32 d32_val = 1.5DF;
    _Decimal64 d64_val = 2.5DD;

    /* 混合运算会转换为更宽的类型 */
    _Decimal64 result = d32_val + d64_val;  /* d32_val 提升为 _Decimal64 */

    printf("  1.5DF + 2.5DD = %.1f\n", (double)result);
    printf("  注意: 窄类型会自动提升为宽类型\n\n");

    /* 最佳实践 */
    printf("=== 最佳实践 ===\n\n");

    printf("1. 始终使用后缀指定字面量类型:\n");
    printf("   _Decimal64 price = 99.99DD;  // 正确\n");
    printf("   _Decimal64 wrong = 99.99;     // 可能有精度问题\n\n");

    printf("2. 使用宏定义常量:\n");
    printf("   #define TAX_RATE 0.08DD\n");
    printf("   #define PI 3.14159265358979323846DL\n\n");

    printf("3. 注意运算结果的类型:\n");
    printf("   _Decimal32 a = 1.0DF;\n");
    printf("   _Decimal64 b = 2.0DD;\n");
    printf("   auto c = a + b;  // c 是 _Decimal64\n");

    return 0;
}
```

---

## 8. 运算和转换

### 8.1 基本运算

```c
/*
 * 编译: gcc -std=c23 decimal_operations.c -o decimal_operations
 *
 * 十进制浮点数基本运算
 */

#include <stdio.h>

/* 算术运算演示 */
void demonstrate_arithmetic(void) {
    printf("=== 算术运算 ===\n\n");

    _Decimal64 a = 100.00DD;
    _Decimal64 b = 30.00DD;

    /* 加法 */
    _Decimal64 sum = a + b;
    printf("加法: %.2f + %.2f = %.2f\n", (double)a, (double)b, (double)sum);

    /* 减法 */
    _Decimal64 diff = a - b;
    printf("减法: %.2f - %.2f = %.2f\n", (double)a, (double)b, (double)diff);

    /* 乘法 */
    _Decimal64 prod = a * b;
    printf("乘法: %.2f * %.2f = %.2f\n", (double)a, (double)b, (double)prod);

    /* 除法 */
    _Decimal64 quot = a / b;
    printf("除法: %.2f / %.2f = %.4f\n", (double)a, (double)b, (double)quot);

    /* 取余 - 十进制浮点不支持 % 运算符 */
    /* 使用 fmod 或手动计算 */
    _Decimal64 mod_result = a - (int)(a / b) * b;
    printf("取余: fmod(%.2f, %.2f) ≈ %.2f\n\n", (double)a, (double)b, (double)mod_result);
}

/* 比较运算演示 */
void demonstrate_comparison(void) {
    printf("=== 比较运算 ===\n\n");

    _Decimal64 x = 1.0DD;
    _Decimal64 y = 1.00DD;  /* 与 x 相等 */
    _Decimal64 z = 1.01DD;

    printf("x = 1.0DD, y = 1.00DD, z = 1.01DD\n\n");

    /* 相等比较 */
    printf("x == y: %s (注意: 1.0 和 1.00 相等)\n", x == y ? "true" : "false");
    printf("x == z: %s\n", x == z ? "true" : "false");

    /* 不等比较 */
    printf("x != z: %s\n", x != z ? "true" : "false");

    /* 大小比较 */
    printf("x < z: %s\n", x < z ? "true" : "false");
    printf("z > x: %s\n", z > x ? "true" : "false");
    printf("x <= y: %s\n", x <= y ? "true" : "false");
    printf("z >= y: %s\n\n", z >= y ? "true" : "false");

    /* NaN 比较 */
    printf("NaN 比较:\n");
    _Decimal64 nan_val = 0.0DD / 0.0DD;  /* 产生 NaN */
    printf("NaN == NaN: %s (NaN 不等于任何值，包括自身)\n",
           nan_val == nan_val ? "true" : "false");
}

/* 一元运算 */
void demonstrate_unary(void) {
    printf("=== 一元运算 ===\n\n");

    _Decimal64 val = 42.5DD;

    /* 正负号 */
    _Decimal64 neg = -val;
    _Decimal64 pos = +val;

    printf("原值: %.2f\n", (double)val);
    printf("负号: %.2f\n", (double)neg);
    printf("正号: %.2f\n\n", (double)pos);

    /* 自增自减 */
    _Decimal64 inc = val;
    _Decimal64 dec = val;

    inc++;  /* 后缀递增 */
    dec--;  /* 后缀递减 */

    printf("后缀 ++: %.2f (原值加1)\n", (double)inc);
    printf("后缀 --: %.2f (原值减1)\n\n", (double)dec);
}

/* 复合赋值运算 */
void demonstrate_compound(void) {
    printf("=== 复合赋值运算 ===\n\n");

    _Decimal64 total = 1000.00DD;

    printf("初始总额: %.2f\n", (double)total);

    total += 250.50DD;  /* 加 */
    printf("+= 250.50: %.2f\n", (double)total);

    total -= 100.00DD;  /* 减 */
    printf("-= 100.00: %.2f\n", (double)total);

    total *= 1.08DD;    /* 乘 (加 8% 税) */
    printf("*= 1.08:   %.2f\n", (double)total);

    total /= 2.0DD;     /* 除 */
    printf("/= 2.0:    %.2f\n", (double)total);
}

int main(void) {
    printf("=== 十进制浮点数基本运算 ===\n\n");

    demonstrate_arithmetic();
    demonstrate_comparison();
    demonstrate_unary();
    demonstrate_compound();

    return 0;
}
```

### 8.2 类型转换

```c
/*
 * 编译: gcc -std=c23 decimal_conversions.c -o decimal_conversions
 *
 * 十进制浮点数类型转换
 */

#include <stdio.h>
#include <stdint.h>

/* 不同类型间的转换 */
void demonstrate_type_conversions(void) {
    printf("=== 十进制浮点类型间转换 ===\n\n");

    /* 窄类型到宽类型 (安全) */
    _Decimal32 d32 = 123.456DF;
    _Decimal64 d64_from32 = d32;  /* 自动提升 */
    _Decimal128 d128_from32 = d32;

    printf("窄到宽转换 (安全):\n");
    printf("  _Decimal32  %.3f\n", (double)d32);
    printf("  -> _Decimal64  %.6f\n", (double)d64_from32);
    printf("  -> _Decimal128 (保持精度)\n\n");

    /* 宽类型到窄类型 (可能丢失精度) */
    _Decimal64 d64_precise = 123456789.123456789DD;
    _Decimal32 d32_from64 = (_Decimal32)d64_precise;  /* 显式转换 */

    printf("宽到窄转换 (可能丢失精度):\n");
    printf("  _Decimal64  %.9f\n", (double)d64_precise);
    printf("  -> _Decimal32  %.6f (精度降低)\n\n", (double)d32_from64);

    /* 混合运算自动提升 */
    _Decimal32 a = 1.5DF;
    _Decimal64 b = 2.5DD;
    auto result = a + b;  /* 结果为 _Decimal64 */

    printf("混合运算自动提升:\n");
    printf("  _Decimal32 + _Decimal64 = _Decimal64\n");
    printf("  结果: %.2f\n\n", (double)result);
}

/* 与整数类型的转换 */
void demonstrate_integer_conversions(void) {
    printf("=== 与整数类型转换 ===\n\n");

    /* 整数到十进制浮点 */
    int int_val = 12345;
    long long_val = 999999999999LL;

    _Decimal64 d_from_int = (_Decimal64)int_val;
    _Decimal64 d_from_long = (_Decimal64)long_val;

    printf("整数到十进制浮点:\n");
    printf("  int %d -> %.0f\n", int_val, (double)d_from_int);
    printf("  long long %lld -> %.0f\n\n", long_val, (double)d_from_long);

    /* 十进制浮点到整数 (截断) */
    _Decimal64 d_val = 12345.6789DD;

    int i_from_d = (int)d_val;           /* 截断小数部分 */
    long long ll_from_d = (long long)d_val;

    printf("十进制浮点到整数 (截断):\n");
    printf("  _Decimal64 %.4f -> int %d\n", (double)d_val, i_from_d);
    printf("  _Decimal64 %.4f -> long long %lld\n\n", (double)d_val, ll_from_d);

    /* 溢出检查 */
    _Decimal64 large_d = 1e20DD;
    int overflow = (int)large_d;

    printf("溢出处理:\n");
    printf("  %.0e -> int %d (溢出，结果未定义或截断)\n\n", (double)large_d, overflow);
}

/* 与二进制浮点转换 */
void demonstrate_float_conversions(void) {
    printf("=== 与二进制浮点转换 ===\n\n");

    printf("警告: 二进制浮点到十进制浮点转换可能引入误差!\n\n");

    /* float/double 到 _Decimal64 */
    double binary_val = 0.1;  /* 不精确的二进制表示 */
    _Decimal64 decimal_from_binary = (_Decimal64)binary_val;

    printf("double 到 _Decimal64:\n");
    printf("  double 0.1 (实际值): %.17f\n", binary_val);
    printf("  -> _Decimal64: %.17f\n", (double)decimal_from_binary);
    printf("  注意: 不精确的二进制值被保留!\n\n");

    /* 正确做法: 使用十进制字面量 */
    _Decimal64 correct_decimal = 0.1DD;
    printf("正确的十进制字面量:\n");
    printf("  _Decimal64 0.1DD: %.17f (精确)\n\n", (double)correct_decimal);

    /* _Decimal64 到 double */
    _Decimal64 precise_decimal = 0.1DD;
    double binary_from_decimal = (double)precise_decimal;

    printf("_Decimal64 到 double:\n");
    printf("  _Decimal64 0.1DD: 精确\n");
    printf("  -> double: %.17f\n", binary_from_decimal);
    printf("  注意: 转换为二进制后可能不精确\n");
}

/* 字符串转换 */
void demonstrate_string_conversions(void) {
    printf("\n=== 字符串转换 ===\n\n");

    printf("字符串到十进制浮点 (使用 sscanf):\n");
    printf("  代码: _Decimal64 val; sscanf(\"123.456\", \"%%f\", &val);\n");
    printf("  注意: 需要转换说明符支持\n\n");

    printf("十进制浮点到字符串 (使用 snprintf):\n");
    printf("  代码: snprintf(buf, sizeof(buf), \"%%.2f\", (double)val);\n");
    printf("  注意: 通常先转换为 double 再格式化\n\n");

    /* 实际示例 */
    _Decimal64 price = 1234.56DD;
    char buffer[64];

    /* 转换为 double 后格式化 */
    snprintf(buffer, sizeof(buffer), "%.2f", (double)price);
    printf("示例: _Decimal64 1234.56DD -> 字符串 \"%s\"\n", buffer);
}

int main(void) {
    printf("=== 十进制浮点数类型转换 ===\n\n");

    demonstrate_type_conversions();
    demonstrate_integer_conversions();
    demonstrate_float_conversions();
    demonstrate_string_conversions();

    return 0;
}
```

---

## 9. 财务计算最佳实践

### 9.1 货币表示规范

```c
/*
 * 编译: gcc -std=c23 financial_best_practices.c -o financial_best_practices
 *
 * 财务计算最佳实践
 */

#include <stdio.h>
#include <stdbool.h>

/* ==================== 类型定义 ==================== */

/* 货币类型 - 使用 _Decimal64 作为默认 */
typedef _Decimal64 Money;

/* 利率类型 */
typedef _Decimal64 Rate;

/* 数量类型 */
typedef _Decimal64 Quantity;

/* ==================== 常量定义 ==================== */

/* 货币后缀 */
#define MONEY_SUFFIX DD
#define RATE_SUFFIX DD

/* 常用常量 */
#define ZERO_MONEY 0.00DD
#define ONE_HUNDRED_PERCENT 1.00DD

/* ==================== 舍入函数 ==================== */

/* 舍入到指定小数位 */
Money round_to_cents(Money amount) {
    /* 使用银行家舍入 (四舍六入五成双) */
    Money multiplier = 100.0DD;
    Money scaled = amount * multiplier;

    /* 手动实现银行家舍入 */
    Money int_part = (Money)(long long)scaled;
    Money frac_part = scaled - int_part;

    if (frac_part > 0.5DD) {
        int_part += 1.0DD;
    } else if (frac_part == 0.5DD) {
        /* 五成双：整数部分为偶数则舍去，为奇数则进位 */
        long long int_val = (long long)int_part;
        if (int_val % 2 != 0) {
            int_part += 1.0DD;
        }
    } else if (frac_part < -0.5DD) {
        int_part -= 1.0DD;
    } else if (frac_part == -0.5DD) {
        long long int_val = (long long)int_part;
        if (int_val % 2 != 0) {
            int_part -= 1.0DD;
        }
    }

    return int_part / multiplier;
}

/* 向上舍入 (财务常用) */
Money round_up_to_cents(Money amount) {
    Money multiplier = 100.0DD;
    Money scaled = amount * multiplier;
    Money int_part = (Money)(long long)scaled;

    if (scaled > int_part) {
        int_part += 1.0DD;
    } else if (scaled < int_part) {
        /* 负数处理 */
        /* 对于负数，向上舍入意味着向零靠近 */
    }

    return int_part / multiplier;
}

/* 向下舍入 */
Money round_down_to_cents(Money amount) {
    Money multiplier = 100.0DD;
    Money scaled = amount * multiplier;
    Money int_part = (Money)(long long)scaled;

    return int_part / multiplier;
}

/* ==================== 货币运算函数 ==================== */

/* 安全的加法 */
Money money_add(Money a, Money b) {
    return a + b;
}

/* 安全的减法 */
Money money_subtract(Money a, Money b) {
    return a - b;
}

/* 计算折扣 */
Money calculate_discount(Money original_price, Rate discount_rate) {
    return round_to_cents(original_price * discount_rate);
}

/* 计算税费 */
Money calculate_tax(Money amount, Rate tax_rate) {
    return round_to_cents(amount * tax_rate);
}

/* 计算最终价格 */
Money calculate_final_price(Money original_price, Rate discount_rate, Rate tax_rate) {
    Money discounted_price = original_price - calculate_discount(original_price, discount_rate);
    Money tax = calculate_tax(discounted_price, tax_rate);
    return discounted_price + tax;
}

/* 计算百分比 */
Rate calculate_percentage(Money part, Money total) {
    if (total == ZERO_MONEY) {
        return ZERO_MONEY;
    }
    return part / total;
}

/* ==================== 验证函数 ==================== */

/* 检查金额是否为非负 */
bool is_non_negative(Money amount) {
    return amount >= ZERO_MONEY;
}

/* 检查金额是否在合理范围内 */
bool is_reasonable_amount(Money amount) {
    Money max_reasonable = 999999999999.99DD;  /* 万亿级别 */
    Money min_reasonable = -999999999999.99DD;
    return amount >= min_reasonable && amount <= max_reasonable;
}

/* ==================== 演示 ==================== */

int main(void) {
    printf("=== 财务计算最佳实践 ===\n\n");

    /* 1. 舍入演示 */
    printf("1. 舍入函数演示:\n");
    Money amounts[] = {10.005DD, 10.015DD, 10.025DD, 10.035DD, -10.005DD};
    int n = sizeof(amounts) / sizeof(amounts[0]);

    printf("   银行家舍入 (四舍六入五成双):\n");
    for (int i = 0; i < n; i++) {
        printf("   %.3f -> %.2f\n", (double)amounts[i],
               (double)round_to_cents(amounts[i]));
    }
    printf("\n");

    /* 2. 完整交易计算 */
    printf("2. 完整交易计算:\n");
    Money price = 199.99DD;
    Rate discount = 0.15DD;  /* 15% 折扣 */
    Rate tax = 0.0875DD;     /* 8.75% 税 */

    Money final = calculate_final_price(price, discount, tax);
    Money discount_amount = calculate_discount(price, discount);
    Money tax_amount = calculate_tax(price - discount_amount, tax);

    printf("   原价: $%.2f\n", (double)price);
    printf("   折扣 (15%%): -$%.2f\n", (double)discount_amount);
    printf("   折后价: $%.2f\n", (double)(price - discount_amount));
    printf("   税费 (8.75%%): +$%.2f\n", (double)tax_amount);
    printf("   最终价格: $%.2f\n\n", (double)final);

    /* 3. 批量处理 */
    printf("3. 批量订单处理:\n");
    struct {
        char* name;
        Money unit_price;
        Quantity quantity;
        Rate item_discount;
    } items[] = {
        {"Widget A", 49.99DD, 10.0DD, 0.10DD},
        {"Widget B", 99.99DD, 5.0DD, 0.15DD},
        {"Widget C", 29.99DD, 20.0DD, 0.05DD}
    };

    int num_items = sizeof(items) / sizeof(items[0]);
    Money subtotal = ZERO_MONEY;

    for (int i = 0; i < num_items; i++) {
        Money line_total = items[i].unit_price * items[i].quantity;
        Money line_discount = calculate_discount(line_total, items[i].item_discount);
        Money line_net = line_total - line_discount;

        printf("   %s:\n", items[i].name);
        printf("     单价: $%.2f x %.0f\n",
               (double)items[i].unit_price, (double)items[i].quantity);
        printf("     小计: $%.2f\n", (double)line_total);
        printf("     折扣: -$%.2f\n", (double)line_discount);
        printf("     净额: $%.2f\n", (double)line_net);

        subtotal += line_net;
    }

    Money order_tax = calculate_tax(subtotal, 0.0875DD);
    Money order_total = subtotal + order_tax;

    printf("   -------------------------\n");
    printf("   订单小计: $%.2f\n", (double)subtotal);
    printf("   税费: $%.2f\n", (double)order_tax);
    printf("   订单总计: $%.2f\n\n", (double)order_total);

    /* 4. 验证函数 */
    printf("4. 金额验证:\n");
    Money valid_amount = 1000.00DD;
    Money invalid_amount = -500.00DD;
    Money huge_amount = 9999999999999.99DD;

    printf("   $%.2f 是非负? %s\n", (double)valid_amount,
           is_non_negative(valid_amount) ? "是" : "否");
    printf("   $%.2f 是非负? %s\n", (double)invalid_amount,
           is_non_negative(invalid_amount) ? "是" : "否");
    printf("   $%.2f 是合理金额? %s\n", (double)huge_amount,
           is_reasonable_amount(huge_amount) ? "是" : "否");

    return 0;
}
```

### 9.2 错误处理和验证

```c
/*
 * 编译: gcc -std=c23 financial_validation.c -o financial_validation
 *
 * 财务数据验证和错误处理
 */

#include <stdio.h>
#include <stdbool.h>
#include <string.h>

/* 错误代码 */
typedef enum {
    ERR_NONE = 0,
    ERR_NEGATIVE_AMOUNT,
    ERR_OVERFLOW,
    ERR_INVALID_RATE,
    ERR_DIVISION_BY_ZERO,
    ERR_PRECISION_LOSS,
    ERR_INVALID_INPUT
} FinancialError;

/* 结果结构体 */
typedef struct {
    _Decimal64 value;
    FinancialError error;
    const char* error_message;
} FinancialResult;

/* 错误消息 */
const char* get_error_message(FinancialError err) {
    switch (err) {
        case ERR_NONE: return "成功";
        case ERR_NEGATIVE_AMOUNT: return "金额不能为负";
        case ERR_OVERFLOW: return "数值溢出";
        case ERR_INVALID_RATE: return "无效的费率";
        case ERR_DIVISION_BY_ZERO: return "除零错误";
        case ERR_PRECISION_LOSS: return "精度丢失警告";
        case ERR_INVALID_INPUT: return "无效输入";
        default: return "未知错误";
    }
}

/* 安全的除法 */
FinancialResult safe_divide(_Decimal64 numerator, _Decimal64 denominator) {
    FinancialResult result = {0};

    if (denominator == 0.0DD) {
        result.error = ERR_DIVISION_BY_ZERO;
        result.error_message = get_error_message(ERR_DIVISION_BY_ZERO);
        return result;
    }

    result.value = numerator / denominator;
    result.error = ERR_NONE;
    result.error_message = get_error_message(ERR_NONE);
    return result;
}

/* 安全的费率计算 */
FinancialResult safe_rate_calculation(_Decimal64 amount, _Decimal64 rate) {
    FinancialResult result = {0};

    /* 验证费率范围 */
    if (rate < 0.0DD || rate > 100.0DD) {
        result.error = ERR_INVALID_RATE;
        result.error_message = get_error_message(ERR_INVALID_RATE);
        return result;
    }

    /* 验证金额 */
    if (amount < 0.0DD) {
        result.error = ERR_NEGATIVE_AMOUNT;
        result.error_message = get_error_message(ERR_NEGATIVE_AMOUNT);
        return result;
    }

    /* 计算并检查溢出 */
    _Decimal64 temp = amount * rate;
    if (temp > 999999999999.99DD) {
        result.error = ERR_OVERFLOW;
        result.error_message = get_error_message(ERR_OVERFLOW);
        return result;
    }

    result.value = temp;
    result.error = ERR_NONE;
    result.error_message = get_error_message(ERR_NONE);
    return result;
}

/* 批量计算验证 */
bool validate_transaction_batch(_Decimal64* amounts, int count,
                                 _Decimal64* expected_total) {
    _Decimal64 calculated_total = 0.0DD;

    for (int i = 0; i < count; i++) {
        /* 检查每个金额 */
        if (amounts[i] < 0.0DD) {
            printf("错误: 第 %d 项金额为负: %.2f\n", i, (double)amounts[i]);
            return false;
        }

        /* 检查累加溢出 */
        _Decimal64 new_total = calculated_total + amounts[i];
        if (new_total < calculated_total) {
            printf("错误: 累加溢出\n");
            return false;
        }

        calculated_total = new_total;
    }

    /* 验证总和 */
    if (expected_total != NULL) {
        if (calculated_total != *expected_total) {
            printf("警告: 计算总和 (%.2f) 与预期 (%.2f) 不符\n",
                   (double)calculated_total, (double)*expected_total);
            return false;
        }
    }

    printf("验证通过: 总和 = %.2f\n", (double)calculated_total);
    return true;
}

/* 精度检查 */
bool check_precision(_Decimal64 value, int expected_decimal_places) {
    _Decimal64 multiplier = 1.0DD;
    for (int i = 0; i < expected_decimal_places; i++) {
        multiplier *= 10.0DD;
    }

    _Decimal64 scaled = value * multiplier;
    _Decimal64 rounded = (_Decimal64)(long long)scaled;

    return (scaled == rounded);
}

int main(void) {
    printf("=== 财务数据验证和错误处理 ===\n\n");

    /* 1. 除法错误处理 */
    printf("1. 安全除法:\n");
    FinancialResult res1 = safe_divide(100.0DD, 0.0DD);
    printf("   100.0 / 0.0: %s\n", res1.error == ERR_NONE ? "成功" : res1.error_message);

    FinancialResult res2 = safe_divide(100.0DD, 4.0DD);
    printf("   100.0 / 4.0: %s, 结果 = %.2f\n\n",
           res2.error_message, (double)res2.value);

    /* 2. 费率计算验证 */
    printf("2. 费率计算验证:\n");
    FinancialResult res3 = safe_rate_calculation(1000.0DD, 0.08DD);
    printf("   计算 8%% 税费: %s, 结果 = %.2f\n",
           res3.error_message, (double)res3.value);

    FinancialResult res4 = safe_rate_calculation(-100.0DD, 0.08DD);
    printf("   负数金额: %s\n", res4.error_message);

    FinancialResult res5 = safe_rate_calculation(100.0DD, 150.0DD);
    printf("   无效费率: %s\n\n", res5.error_message);

    /* 3. 批量交易验证 */
    printf("3. 批量交易验证:\n");
    _Decimal64 transactions[] = {100.00DD, 200.50DD, 50.25DD, 300.75DD};
    int num_trans = sizeof(transactions) / sizeof(transactions[0]);
    _Decimal64 expected = 651.50DD;

    printf("   交易列表:\n");
    for (int i = 0; i < num_trans; i++) {
        printf("     交易 %d: $%.2f\n", i + 1, (double)transactions[i]);
    }
    printf("   预期总和: $%.2f\n", (double)expected);

    bool valid = validate_transaction_batch(transactions, num_trans, &expected);
    printf("   验证结果: %s\n\n", valid ? "通过" : "失败");

    /* 4. 精度检查 */
    printf("4. 精度检查:\n");
    _Decimal64 price1 = 99.99DD;
    _Decimal64 price2 = 99.999DD;

    printf("   %.2f 有2位小数精度? %s\n",
           (double)price1, check_precision(price1, 2) ? "是" : "否");
    printf("   %.3f 有2位小数精度? %s\n",
           (double)price2, check_precision(price2, 2) ? "是" : "否");

    return 0;
}
```


---

## 10. 实际项目: 货币计算库完整实现

### 10.1 完整货币库头文件

```c
/*
 * moneylib.h - 货币计算库头文件
 *
 * 编译: gcc -std=c23 moneylib.c example.c -o money_example
 */

#ifndef MONEYLIB_H
#define MONEYLIB_H

#include <stdbool.h>
#include <stdint.h>

/* ==================== 类型定义 ==================== */

/* 货币类型 - 使用 _Decimal64 确保精确计算 */
typedef _Decimal64 money_t;

/* 利率类型 */
typedef _Decimal64 rate_t;

/* 数量类型 */
typedef _Decimal64 quantity_t;

/* 错误代码 */
typedef enum {
    MONEY_OK = 0,
    MONEY_ERR_NULL_PTR,
    MONEY_ERR_NEGATIVE_AMOUNT,
    MONEY_ERR_OVERFLOW,
    MONEY_ERR_INVALID_RATE,
    MONEY_ERR_DIVISION_BY_ZERO,
    MONEY_ERR_PRECISION_LOSS,
    MONEY_ERR_CURRENCY_MISMATCH
} MoneyError;

/* 舍入模式 */
typedef enum {
    ROUND_HALF_UP,      /* 四舍五入 */
    ROUND_HALF_DOWN,    /* 五舍六入 */
    ROUND_HALF_EVEN,    /* 银行家舍入 (默认) */
    ROUND_UP,           /* 向上舍入 */
    ROUND_DOWN,         /* 向下舍入 (截断) */
    ROUND_CEILING,      /* 向正无穷舍入 */
    ROUND_FLOOR         /* 向负无穷舍入 */
} RoundingMode;

/* 货币结构体 */
typedef struct {
    money_t amount;          /* 金额 */
    char currency_code[4];   /* ISO 4217 货币代码 (如 "USD", "CNY") */
    RoundingMode rounding;   /* 舍入模式 */
} Money;

/* 货币汇率 */
typedef struct {
    char from_currency[4];
    char to_currency[4];
    rate_t rate;
} ExchangeRate;

/* ==================== 常量 ==================== */

/* 货币代码 */
#define CURRENCY_USD "USD"
#define CURRENCY_CNY "CNY"
#define CURRENCY_EUR "EUR"
#define CURRENCY_JPY "JPY"
#define CURRENCY_GBP "GBP"

/* 常用常量 */
#define MONEY_ZERO ((money_t)0.00DD)
#define MONEY_ONE ((money_t)1.00DD)
#define MONEY_HUNDRED ((money_t)100.00DD)

/* 默认小数位 */
#define DEFAULT_DECIMAL_PLACES 2

/* ==================== 函数声明 ==================== */

/* 初始化 */
MoneyError money_init(void);
void money_cleanup(void);

/* 创建和销毁 */
Money* money_create(const char* currency_code, money_t amount);
Money* money_create_zero(const char* currency_code);
void money_free(Money* m);

/* 基本操作 */
MoneyError money_set_amount(Money* m, money_t amount);
money_t money_get_amount(const Money* m);
const char* money_get_currency(const Money* m);
MoneyError money_set_rounding(Money* m, RoundingMode mode);

/* 算术运算 */
Money* money_add(const Money* a, const Money* b, MoneyError* err);
Money* money_subtract(const Money* a, const Money* b, MoneyError* err);
Money* money_multiply(const Money* m, quantity_t multiplier, MoneyError* err);
Money* money_divide(const Money* m, quantity_t divisor, MoneyError* err);
Money* money_percentage(const Money* m, rate_t percent, MoneyError* err);

/* 舍入 */
money_t money_round(money_t amount, int decimal_places, RoundingMode mode);
money_t money_round_to_cents(money_t amount);

/* 比较 */
int money_compare(const Money* a, const Money* b);
bool money_equals(const Money* a, const Money* b);
bool money_less_than(const Money* a, const Money* b);
bool money_greater_than(const Money* a, const Money* b);
bool money_is_zero(const Money* m);
bool money_is_positive(const Money* m);
bool money_is_negative(const Money* m);

/* 汇率转换 */
Money* money_convert(const Money* m, const char* target_currency,
                     rate_t exchange_rate, MoneyError* err);

/* 财务计算 */
money_t money_calculate_discount(money_t original, rate_t discount_rate);
money_t money_calculate_tax(money_t amount, rate_t tax_rate);
money_t money_calculate_interest(money_t principal, rate_t rate, int periods);
money_t money_calculate_compound_interest(money_t principal, rate_t rate,
                                           int times_per_year, int years);

/* 批量处理 */
money_t money_sum(const Money** monies, int count, MoneyError* err);
money_t money_average(const Money** monies, int count, MoneyError* err);
money_t money_min(const Money** monies, int count);
money_t money_max(const Money** monies, int count);

/* 格式化 */
int money_format(const Money* m, char* buffer, size_t buffer_size);
int money_format_with_symbol(const Money* m, char* buffer, size_t buffer_size);

/* 验证 */
bool money_is_valid_amount(money_t amount);
bool money_is_valid_currency_code(const char* code);

/* 错误处理 */
const char* money_error_string(MoneyError err);
void money_print_error(MoneyError err);

#endif /* MONEYLIB_H */
```

### 10.2 完整货币库实现

```c
/*
 * moneylib.c - 货币计算库完整实现
 *
 * 编译: gcc -std=c23 -c moneylib.c -o moneylib.o
 */

#include "moneylib.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

/* ==================== 内部辅助函数 ==================== */

/* 复制货币代码 */
static void copy_currency_code(char* dest, const char* src) {
    if (src == NULL || strlen(src) != 3) {
        strcpy(dest, "XXX");  /* 未知货币 */
    } else {
        strncpy(dest, src, 3);
        dest[3] = '\0';
    }
}

/* 检查货币代码是否匹配 */
static bool currencies_match(const char* a, const char* b) {
    return strcmp(a, b) == 0;
}

/* 执行舍入 */
static money_t do_round(money_t value, int decimal_places, RoundingMode mode) {
    money_t multiplier = MONEY_ONE;
    for (int i = 0; i < decimal_places; i++) {
        multiplier *= (money_t)10.0DD;
    }

    money_t scaled = value * multiplier;
    money_t int_part = (money_t)(long long)scaled;
    money_t frac_part = scaled - int_part;
    money_t abs_frac = frac_part < MONEY_ZERO ? -frac_part : frac_part;

    switch (mode) {
        case ROUND_HALF_UP:
            if (abs_frac >= (money_t)0.5DD) {
                int_part += (frac_part >= MONEY_ZERO) ? MONEY_ONE : -MONEY_ONE;
            }
            break;

        case ROUND_HALF_DOWN:
            if (abs_frac > (money_t)0.5DD) {
                int_part += (frac_part >= MONEY_ZERO) ? MONEY_ONE : -MONEY_ONE;
            }
            break;

        case ROUND_HALF_EVEN:
            if (abs_frac > (money_t)0.5DD) {
                int_part += (frac_part >= MONEY_ZERO) ? MONEY_ONE : -MONEY_ONE;
            } else if (abs_frac == (money_t)0.5DD) {
                long long int_val = (long long)int_part;
                if (int_val % 2 != 0) {
                    int_part += (frac_part >= MONEY_ZERO) ? MONEY_ONE : -MONEY_ONE;
                }
            }
            break;

        case ROUND_UP:
            if (abs_frac > MONEY_ZERO) {
                int_part += (frac_part >= MONEY_ZERO) ? MONEY_ONE : -MONEY_ONE;
            }
            break;

        case ROUND_DOWN:
            /* 截断，什么都不做 */
            break;

        case ROUND_CEILING:
            if (frac_part > MONEY_ZERO) {
                int_part += MONEY_ONE;
            }
            break;

        case ROUND_FLOOR:
            if (frac_part < MONEY_ZERO) {
                int_part -= MONEY_ONE;
            }
            break;
    }

    return int_part / multiplier;
}

/* ==================== 初始化 ==================== */

MoneyError money_init(void) {
    /* 可以在这里设置全局配置 */
    return MONEY_OK;
}

void money_cleanup(void) {
    /* 清理资源 */
}

/* ==================== 创建和销毁 ==================== */

Money* money_create(const char* currency_code, money_t amount) {
    if (!money_is_valid_amount(amount)) {
        return NULL;
    }

    Money* m = (Money*)malloc(sizeof(Money));
    if (m == NULL) {
        return NULL;
    }

    m->amount = amount;
    copy_currency_code(m->currency_code, currency_code);
    m->rounding = ROUND_HALF_EVEN;

    return m;
}

Money* money_create_zero(const char* currency_code) {
    return money_create(currency_code, MONEY_ZERO);
}

void money_free(Money* m) {
    free(m);
}

/* ==================== 基本操作 ==================== */

MoneyError money_set_amount(Money* m, money_t amount) {
    if (m == NULL) {
        return MONEY_ERR_NULL_PTR;
    }
    if (!money_is_valid_amount(amount)) {
        return MONEY_ERR_NEGATIVE_AMOUNT;
    }

    m->amount = amount;
    return MONEY_OK;
}

money_t money_get_amount(const Money* m) {
    return (m == NULL) ? MONEY_ZERO : m->amount;
}

const char* money_get_currency(const Money* m) {
    return (m == NULL) ? "XXX" : m->currency_code;
}

MoneyError money_set_rounding(Money* m, RoundingMode mode) {
    if (m == NULL) {
        return MONEY_ERR_NULL_PTR;
    }
    m->rounding = mode;
    return MONEY_OK;
}

/* ==================== 算术运算 ==================== */

Money* money_add(const Money* a, const Money* b, MoneyError* err) {
    if (err != NULL) *err = MONEY_OK;

    if (a == NULL || b == NULL) {
        if (err != NULL) *err = MONEY_ERR_NULL_PTR;
        return NULL;
    }

    if (!currencies_match(a->currency_code, b->currency_code)) {
        if (err != NULL) *err = MONEY_ERR_CURRENCY_MISMATCH;
        return NULL;
    }

    money_t result = a->amount + b->amount;
    return money_create(a->currency_code, result);
}

Money* money_subtract(const Money* a, const Money* b, MoneyError* err) {
    if (err != NULL) *err = MONEY_OK;

    if (a == NULL || b == NULL) {
        if (err != NULL) *err = MONEY_ERR_NULL_PTR;
        return NULL;
    }

    if (!currencies_match(a->currency_code, b->currency_code)) {
        if (err != NULL) *err = MONEY_ERR_CURRENCY_MISMATCH;
        return NULL;
    }

    money_t result = a->amount - b->amount;
    return money_create(a->currency_code, result);
}

Money* money_multiply(const Money* m, quantity_t multiplier, MoneyError* err) {
    if (err != NULL) *err = MONEY_OK;

    if (m == NULL) {
        if (err != NULL) *err = MONEY_ERR_NULL_PTR;
        return NULL;
    }

    money_t result = m->amount * multiplier;
    result = do_round(result, DEFAULT_DECIMAL_PLACES, m->rounding);

    return money_create(m->currency_code, result);
}

Money* money_divide(const Money* m, quantity_t divisor, MoneyError* err) {
    if (err != NULL) *err = MONEY_OK;

    if (m == NULL) {
        if (err != NULL) *err = MONEY_ERR_NULL_PTR;
        return NULL;
    }

    if (divisor == (quantity_t)0.0DD) {
        if (err != NULL) *err = MONEY_ERR_DIVISION_BY_ZERO;
        return NULL;
    }

    money_t result = m->amount / divisor;
    result = do_round(result, DEFAULT_DECIMAL_PLACES, m->rounding);

    return money_create(m->currency_code, result);
}

Money* money_percentage(const Money* m, rate_t percent, MoneyError* err) {
    if (err != NULL) *err = MONEY_OK;

    if (m == NULL) {
        if (err != NULL) *err = MONEY_ERR_NULL_PTR;
        return NULL;
    }

    money_t result = m->amount * percent;
    result = do_round(result, DEFAULT_DECIMAL_PLACES, m->rounding);

    return money_create(m->currency_code, result);
}

/* ==================== 舍入 ==================== */

money_t money_round(money_t amount, int decimal_places, RoundingMode mode) {
    return do_round(amount, decimal_places, mode);
}

money_t money_round_to_cents(money_t amount) {
    return do_round(amount, 2, ROUND_HALF_EVEN);
}

/* ==================== 比较 ==================== */

int money_compare(const Money* a, const Money* b) {
    if (a == NULL || b == NULL) {
        return (a == b) ? 0 : (a == NULL ? -1 : 1);
    }

    if (!currencies_match(a->currency_code, b->currency_code)) {
        /* 不同货币，按货币代码比较 */
        return strcmp(a->currency_code, b->currency_code);
    }

    if (a->amount < b->amount) return -1;
    if (a->amount > b->amount) return 1;
    return 0;
}

bool money_equals(const Money* a, const Money* b) {
    return money_compare(a, b) == 0;
}

bool money_less_than(const Money* a, const Money* b) {
    return money_compare(a, b) < 0;
}

bool money_greater_than(const Money* a, const Money* b) {
    return money_compare(a, b) > 0;
}

bool money_is_zero(const Money* m) {
    return (m == NULL) ? true : (m->amount == MONEY_ZERO);
}

bool money_is_positive(const Money* m) {
    return (m == NULL) ? false : (m->amount > MONEY_ZERO);
}

bool money_is_negative(const Money* m) {
    return (m == NULL) ? false : (m->amount < MONEY_ZERO);
}

/* ==================== 汇率转换 ==================== */

Money* money_convert(const Money* m, const char* target_currency,
                     rate_t exchange_rate, MoneyError* err) {
    if (err != NULL) *err = MONEY_OK;

    if (m == NULL || target_currency == NULL) {
        if (err != NULL) *err = MONEY_ERR_NULL_PTR;
        return NULL;
    }

    if (exchange_rate <= (rate_t)0.0DD) {
        if (err != NULL) *err = MONEY_ERR_INVALID_RATE;
        return NULL;
    }

    money_t converted = m->amount * exchange_rate;
    converted = do_round(converted, DEFAULT_DECIMAL_PLACES, m->rounding);

    return money_create(target_currency, converted);
}

/* ==================== 财务计算 ==================== */

money_t money_calculate_discount(money_t original, rate_t discount_rate) {
    money_t discount = original * discount_rate;
    return do_round(discount, DEFAULT_DECIMAL_PLACES, ROUND_HALF_UP);
}

money_t money_calculate_tax(money_t amount, rate_t tax_rate) {
    money_t tax = amount * tax_rate;
    return do_round(tax, DEFAULT_DECIMAL_PLACES, ROUND_HALF_UP);
}

money_t money_calculate_interest(money_t principal, rate_t rate, int periods) {
    money_t interest = principal * rate * (money_t)periods;
    return do_round(interest, DEFAULT_DECIMAL_PLACES, ROUND_HALF_UP);
}

money_t money_calculate_compound_interest(money_t principal, rate_t rate,
                                           int times_per_year, int years) {
    quantity_t n = (quantity_t)times_per_year;
    quantity_t base = (quantity_t)1.0DD + rate / n;

    int exponent = times_per_year * years;
    quantity_t factor = (quantity_t)1.0DD;

    for (int i = 0; i < exponent; i++) {
        factor *= base;
    }

    money_t result = principal * (money_t)factor;
    return do_round(result, DEFAULT_DECIMAL_PLACES, ROUND_HALF_UP);
}

/* ==================== 批量处理 ==================== */

money_t money_sum(const Money** monies, int count, MoneyError* err) {
    if (err != NULL) *err = MONEY_OK;

    if (monies == NULL && count > 0) {
        if (err != NULL) *err = MONEY_ERR_NULL_PTR;
        return MONEY_ZERO;
    }

    if (count == 0) {
        return MONEY_ZERO;
    }

    const char* currency = monies[0]->currency_code;
    money_t total = MONEY_ZERO;

    for (int i = 0; i < count; i++) {
        if (monies[i] == NULL) {
            if (err != NULL) *err = MONEY_ERR_NULL_PTR;
            return MONEY_ZERO;
        }
        if (!currencies_match(monies[i]->currency_code, currency)) {
            if (err != NULL) *err = MONEY_ERR_CURRENCY_MISMATCH;
            return MONEY_ZERO;
        }
        total += monies[i]->amount;
    }

    return total;
}

money_t money_average(const Money** monies, int count, MoneyError* err) {
    money_t sum = money_sum(monies, count, err);
    if (err != NULL && *err != MONEY_OK) {
        return MONEY_ZERO;
    }

    if (count == 0) {
        if (err != NULL) *err = MONEY_ERR_DIVISION_BY_ZERO;
        return MONEY_ZERO;
    }

    return sum / (money_t)count;
}

money_t money_min(const Money** monies, int count) {
    if (monies == NULL || count == 0) {
        return MONEY_ZERO;
    }

    money_t min = monies[0]->amount;
    for (int i = 1; i < count; i++) {
        if (monies[i] != NULL && monies[i]->amount < min) {
            min = monies[i]->amount;
        }
    }

    return min;
}

money_t money_max(const Money** monies, int count) {
    if (monies == NULL || count == 0) {
        return MONEY_ZERO;
    }

    money_t max = monies[0]->amount;
    for (int i = 1; i < count; i++) {
        if (monies[i] != NULL && monies[i]->amount > max) {
            max = monies[i]->amount;
        }
    }

    return max;
}

/* ==================== 格式化 ==================== */

int money_format(const Money* m, char* buffer, size_t buffer_size) {
    if (m == NULL || buffer == NULL || buffer_size == 0) {
        return -1;
    }

    return snprintf(buffer, buffer_size, "%s %.2f",
                    m->currency_code, (double)m->amount);
}

int money_format_with_symbol(const Money* m, char* buffer, size_t buffer_size) {
    if (m == NULL || buffer == NULL || buffer_size == 0) {
        return -1;
    }

    const char* symbol = "$";  /* 简化处理，实际应根据货币代码选择符号 */
    if (strcmp(m->currency_code, CURRENCY_EUR) == 0) symbol = "€";
    else if (strcmp(m->currency_code, CURRENCY_GBP) == 0) symbol = "£";
    else if (strcmp(m->currency_code, CURRENCY_JPY) == 0) symbol = "¥";
    else if (strcmp(m->currency_code, CURRENCY_CNY) == 0) symbol = "¥";

    return snprintf(buffer, buffer_size, "%s%.2f %s",
                    symbol, (double)m->amount, m->currency_code);
}

/* ==================== 验证 ==================== */

bool money_is_valid_amount(money_t amount) {
    /* 检查是否为有限数 */
    /* 注意: C23 十进制浮点应始终有限，除非显式创建无穷或 NaN */

    /* 检查是否在合理范围内 */
    money_t max = (money_t)999999999999.99DD;  /* 万亿级别 */
    money_t min = (money_t)-999999999999.99DD;

    return (amount >= min && amount <= max);
}

bool money_is_valid_currency_code(const char* code) {
    if (code == NULL || strlen(code) != 3) {
        return false;
    }

    /* 检查是否都是大写字母 */
    for (int i = 0; i < 3; i++) {
        if (code[i] < 'A' || code[i] > 'Z') {
            return false;
        }
    }

    return true;
}

/* ==================== 错误处理 ==================== */

const char* money_error_string(MoneyError err) {
    switch (err) {
        case MONEY_OK: return "成功";
        case MONEY_ERR_NULL_PTR: return "空指针错误";
        case MONEY_ERR_NEGATIVE_AMOUNT: return "金额不能为负";
        case MONEY_ERR_OVERFLOW: return "数值溢出";
        case MONEY_ERR_INVALID_RATE: return "无效的汇率/费率";
        case MONEY_ERR_DIVISION_BY_ZERO: return "除零错误";
        case MONEY_ERR_PRECISION_LOSS: return "精度丢失警告";
        case MONEY_ERR_CURRENCY_MISMATCH: return "货币类型不匹配";
        default: return "未知错误";
    }
}

void money_print_error(MoneyError err) {
    fprintf(stderr, "货币计算错误: %s\n", money_error_string(err));
}
```

### 10.3 使用示例

```c
/*
 * money_example.c - 货币库使用示例
 *
 * 编译: gcc -std=c23 moneylib.c money_example.c -o money_example
 */

#include "moneylib.h"
#include <stdio.h>

int main(void) {
    printf("=== 货币计算库使用示例 ===\n\n");

    /* 初始化 */
    money_init();

    /* 1. 创建货币对象 */
    printf("1. 创建货币对象:\n");
    Money* price = money_create(CURRENCY_USD, 199.99DD);
    Money* tax = money_create(CURRENCY_USD, 0.0875DD);

    char buffer[64];
    money_format(price, buffer, sizeof(buffer));
    printf("   商品价格: %s\n", buffer);

    /* 2. 计算税费 */
    printf("\n2. 计算税费:\n");
    MoneyError err;
    Money* tax_amount = money_percentage(price, 0.0875DD, &err);

    if (err == MONEY_OK) {
        money_format(tax_amount, buffer, sizeof(buffer));
        printf("   税费 (8.75%%): %s\n", buffer);
    } else {
        money_print_error(err);
    }

    /* 3. 计算总价 */
    printf("\n3. 计算总价:\n");
    Money* total = money_add(price, tax_amount, &err);

    if (err == MONEY_OK) {
        money_format(total, buffer, sizeof(buffer));
        printf("   含税总价: %s\n", buffer);
    }

    /* 4. 批量订单 */
    printf("\n4. 批量订单处理:\n");
    Money* items[] = {
        money_create(CURRENCY_USD, 49.99DD),
        money_create(CURRENCY_USD, 99.99DD),
        money_create(CURRENCY_USD, 29.99DD),
        money_create(CURRENCY_USD, 15.00DD)
    };

    int num_items = sizeof(items) / sizeof(items[0]);
    money_t subtotal = money_sum((const Money**)items, num_items, &err);

    printf("   订单包含 %d 项:\n", num_items);
    for (int i = 0; i < num_items; i++) {
        money_format(items[i], buffer, sizeof(buffer));
        printf("     项目 %d: %s\n", i + 1, buffer);
    }
    printf("   小计: $%.2f\n", (double)subtotal);

    money_t avg = money_average((const Money**)items, num_items, &err);
    printf("   平均价格: $%.2f\n", (double)avg);

    money_t min = money_min((const Money**)items, num_items);
    money_t max = money_max((const Money**)items, num_items);
    printf("   最低价格: $%.2f\n", (double)min);
    printf("   最高价格: $%.2f\n", (double)max);

    /* 5. 汇率转换 */
    printf("\n5. 汇率转换:\n");
    Money* usd_amount = money_create(CURRENCY_USD, 1000.00DD);
    Money* cny_amount = money_convert(usd_amount, CURRENCY_CNY, 7.2456DD, &err);

    if (err == MONEY_OK) {
        money_format(usd_amount, buffer, sizeof(buffer));
        printf("   原金额: %s\n", buffer);
        money_format(cny_amount, buffer, sizeof(buffer));
        printf("   转换后: %s (汇率 7.2456)\n", buffer);
    }

    /* 6. 复利计算 */
    printf("\n6. 复利计算:\n");
    money_t principal = 10000.00DD;
    rate_t annual_rate = 0.05DD;
    int years = 10;

    money_t final = money_calculate_compound_interest(principal, annual_rate, 12, years);

    printf("   本金: $%.2f\n", (double)principal);
    printf("   年利率: %.2f%%\n", (double)(annual_rate * 100.0DD));
    printf("   投资年限: %d 年 (月复利)\n", years);
    printf("   最终金额: $%.2f\n", (double)final);
    printf("   利息收入: $%.2f\n", (double)(final - principal));

    /* 清理 */
    money_free(price);
    money_free(tax);
    money_free(tax_amount);
    money_free(total);
    for (int i = 0; i < num_items; i++) {
        money_free(items[i]);
    }
    money_free(usd_amount);
    money_free(cny_amount);

    money_cleanup();

    printf("\n=== 示例完成 ===\n");

    return 0;
}
```

---

## 11. 与 Cobol/Java BigDecimal 的对比

### 11.1 技术对比

```c
/*
 * 编译: gcc -std=c23 comparison_demo.c -o comparison_demo
 *
 * 与其他语言/系统的十进制计算对比
 */

#include <stdio.h>

/* 技术规格对比表 */
void print_technical_comparison(void) {
    printf("=== 十进制计算技术对比 ===\n\n");

    printf("| 特性 | C _Decimal64 | Java BigDecimal | COBOL COMP-3 |
");
    printf("|------|--------------|-----------------|--------------|
");
    printf("| 存储大小 | 8字节 | 可变 (最小 ~32字节) | 可变 |
");
    printf("| 精度 | 16位十进制 | 用户指定 (无限) | 用户指定 |
");
    printf("| 范围 | 10^(-383)~10^384 | 无限 | 取决于PIC |
");
    printf("| 速度 | 硬件加速 | 软件实现 (慢) | 软件 |
");
    printf("| 内存占用 | 低 | 高 | 中 |
");
    printf("| 精度控制 | 固定 | 完全可控 | 声明式 |
");
    printf("| 舍入模式 | 5种 | 8种 | 有限支持 |
");
    printf("| 硬件支持 | Intel/AMD | 无 | 无 |
");
    printf("| 标准 | IEEE 754-2008 | Java 标准 | COBOL 标准 |
");
    printf("\n");
}

/* COBOL 风格计算演示 */
void cobol_style_example(void) {
    printf("=== COBOL 风格计算 (C实现) ===\n\n");

    printf("COBOL 代码示例:\n");
    printf("  01  PRICE      PIC 9(6)V99 COMP-3.    /* 6位整数 + 2位小数 */\n");
    printf("  01  QUANTITY   PIC 9(4) COMP-3.\n");
    printf("  01  TOTAL      PIC 9(8)V99 COMP-3.\n");
    printf("  COMPUTE TOTAL = PRICE * QUANTITY.\n\n");

    printf("C _Decimal64 等价实现:\n");
    _Decimal64 price = 1234.56DD;    /* 对应 PIC 9(4)V99 */
    _Decimal64 quantity = 100.0DD;    /* 对应 PIC 9(3) */
    _Decimal64 total = price * quantity;

    /* COBOL 风格的舍入 */
    total = total + 0.005DD;  /* 四舍五入到分 */
    total = (_Decimal64)((long long)(total * 100.0DD)) / 100.0DD;

    printf("  _Decimal64 price = 1234.56DD;\n");
    printf("  _Decimal64 quantity = 100.0DD;\n");
    printf("  _Decimal64 total = price * quantity;\n");
    printf("  结果: %.2f\n\n", (double)total);
}

/* Java BigDecimal 风格计算演示 */
void java_style_example(void) {
    printf("=== Java BigDecimal 风格计算 (C实现) ===\n\n");

    printf("Java 代码示例:\n");
    printf("  BigDecimal price = new BigDecimal(\"19.99\");\n");
    printf("  BigDecimal taxRate = new BigDecimal(\"0.0875\");\n");
    printf("  BigDecimal tax = price.multiply(taxRate);\n");
    printf("  tax = tax.setScale(2, RoundingMode.HALF_UP);\n");
    printf("  BigDecimal total = price.add(tax);\n\n");

    printf("C _Decimal64 等价实现:\n");
    _Decimal64 price = 19.99DD;
    _Decimal64 tax_rate = 0.0875DD;

    /* 计算税费 */
    _Decimal64 tax = price * tax_rate;

    /* 手动实现 HALF_UP 舍入到2位小数 */
    tax = tax * 100.0DD;
    tax = tax + (tax >= 0 ? 0.5DD : -0.5DD);
    tax = (_Decimal64)((long long)tax);
    tax = tax / 100.0DD;

    _Decimal64 total = price + tax;

    printf("  _Decimal64 price = 19.99DD;\n");
    printf("  _Decimal64 tax_rate = 0.0875DD;\n");
    printf("  _Decimal64 tax = price * tax_rate;\n");
    printf("  /* 舍入到2位小数 */\n");
    printf("  _Decimal64 total = price + tax;\n");
    printf("  价格: $%.2f\n", (double)price);
    printf("  税费: $%.2f\n", (double)tax);
    printf("  总计: $%.2f\n\n", (double)total);
}

/* 性能对比概念 */
void performance_comparison(void) {
    printf("=== 性能对比 (概念性) ===\n\n");

    printf("相对执行时间 (以 C _Decimal64 = 1 为基准):\n\n");

    printf("操作类型 | C _Decimal64 | Java BigDecimal | COBOL\n");
    printf("---------|--------------|-----------------|-------\n");
    printf("加法     | 1x           | 50-100x         | 10-20x\n");
    printf("乘法     | 1x           | 100-200x        | 20-40x\n");
    printf("除法     | 1x           | 200-500x        | 50-100x\n");
    printf("内存分配 | 无           | 每次操作        | 预分配\n\n");

    printf("说明:\n");
    printf("  - C _Decimal64 使用硬件加速 (Intel/AMD 处理器)\n");
    printf("  - Java BigDecimal 是软件实现，涉及对象创建\n");
    printf("  - COBOL 编译器通常有优化，但仍是软件实现\n\n");
}

/* 互操作性考虑 */
void interoperability_notes(void) {
    printf("=== 互操作性考虑 ===\n\n");

    printf("与 Java BigDecimal 互操作:\n");
    printf("  1. 使用字符串作为交换格式\n");
    printf("  2. 确保精度信息传递\n");
    printf("  3. 注意舍入模式一致性\n");
    printf("  4. JSON/XML 中作为字符串传输\n\n");

    printf("与 COBOL 互操作:\n");
    printf("  1. COBOL COMP-3 是 packed decimal 格式\n");
    printf("  2. 需要字节级转换\n");
    printf("  3. 注意字节序和符号位\n");
    printf("  4. 精度声明要匹配\n\n");

    printf("与数据库 DECIMAL 类型:\n");
    printf("  1. MySQL/PostgreSQL DECIMAL 映射到 _Decimal64\n");
    printf("  2. Oracle NUMBER 类型需要精度检查\n");
    printf("  3. SQL Server decimal 直接兼容\n");
    printf("  4. 使用参数化查询避免字符串转换\n");
}

int main(void) {
    print_technical_comparison();
    cobol_style_example();
    java_style_example();
    performance_comparison();
    interoperability_notes();

    printf("\n=== 选择建议 ===\n\n");

    printf("使用 C _Decimal64 的场景:\n");
    printf("  ✓ 高性能财务计算\n");
    printf("  ✓ 嵌入式/实时系统\n");
    printf("  ✓ 需要与硬件紧密集成\n");
    printf("  ✓ 内存受限环境\n");
    printf("  ✓ 替代 COBOL 的现代方案\n\n");

    printf("使用 Java BigDecimal 的场景:\n");
    printf("  ✓ 需要超过16位精度\n");
    printf("  ✓ Java 生态系统\n");
    printf("  ✓ 需要精确的精度控制\n\n");

    printf("使用 COBOL 的场景:\n");
    printf("  ✓ 遗留系统维护\n");
    printf("  ✓ 严格的监管合规要求\n");
    printf("  ✓ 大型机环境\n");

    return 0;
}
```

### 11.2 迁移指南

```c
/*
 * 编译: gcc -std=c23 migration_guide.c -o migration_guide
 *
 * 从其他系统迁移到 C 十进制浮点
 */

#include <stdio.h>

void cobol_migration(void) {
    printf("=== COBOL 到 C _Decimal 迁移指南 ===\n\n");

    printf("1. 数据类型映射:\n\n");

    printf("   COBOL                    C\n");
    printf("   ------------------------ -----------------------------\n");
    printf("   PIC 9(4)                 int / short\n");
    printf("   PIC 9(9)                 int / long\n");
    printf("   PIC 9(18)                long long\n");
    printf("   PIC 9(6)V99 COMP-3       _Decimal64 (限制精度)\n");
    printf("   PIC 9(16)V99 COMP-3      _Decimal64\n");
    printf("   PIC 9(33)V99 COMP-3      _Decimal128\n\n");

    printf("2. 计算语句转换:\n\n");

    printf("   COBOL:                    C:\n");
    printf("   COMPUTE A = B + C.   =>   _Decimal64 a = b + c;\n");
    printf("   COMPUTE A = B - C.   =>   _Decimal64 a = b - c;\n");
    printf("   COMPUTE A = B * C.   =>   _Decimal64 a = b * c;\n");
    printf("   COMPUTE A = B / C.   =>   _Decimal64 a = b / c;\n");
    printf("   ROUNDED                =>   自定义舍入函数\n\n");

    printf("3. 舍入处理:\n\n");

    printf("   /* COBOL: COMPUTE TOTAL ROUNDED = ... */\n");
    printf("   _Decimal64 calculate_rounded(_Decimal64 value, int decimals) {\n");
    printf("       _Decimal64 factor = 1.0DD;\n");
    printf("       for (int i = 0; i < decimals; i++) factor *= 10.0DD;\n");
    printf("       return round(value * factor) / factor;\n");
    printf("   }\n");
}

void java_migration(void) {
    printf("\n=== Java BigDecimal 到 C _Decimal 迁移指南 ===\n\n");

    printf("1. 构造方法转换:\n\n");

    printf("   Java:                                     C:\n");
    printf("   new BigDecimal(\"123.45\")          =>    123.45DD\n");
    printf("   new BigDecimal(123.45)             =>    避免! 使用字面量\n");
    printf("   BigDecimal.valueOf(123.45)         =>    (_Decimal64)123.45DD\n\n");

    printf("2. 算术方法转换:\n\n");

    printf("   Java:                          C:\n");
    printf("   a.add(b)                 =>    a + b\n");
    printf("   a.subtract(b)            =>    a - b\n");
    printf("   a.multiply(b)            =>    a * b\n");
    printf("   a.divide(b, scale, mode) =>    需要自定义除法函数\n");
    printf("   a.compareTo(b)           =>    (a > b) - (a < b)\n\n");

    printf("3. 精度控制:\n\n");

    printf("   Java:\n");
    printf("   result = value.setScale(2, RoundingMode.HALF_UP);\n\n");

    printf("   C:\n");
    printf("   _Decimal64 set_scale(_Decimal64 value, int scale) {\n");
    printf("       _Decimal64 factor = pow(10, scale);\n");
    printf("       value = value * factor + 0.5DD;\n");
    printf("       value = (_Decimal64)(long long)value;\n");
    printf("       return value / factor;\n");
    printf("   }\n");
}

void best_practices_summary(void) {
    printf("\n=== 迁移最佳实践 ===\n\n");

    printf("1. 逐步迁移:\n");
    printf("   - 先在非关键模块使用 _Decimal64\n");
    printf("   - 建立完整的测试用例\n");
    printf("   - 对比旧系统和新系统的输出\n\n");

    printf("2. 舍入一致性:\n");
    printf("   - 明确文档化舍入策略\n");
    printf("   - 确保与旧系统使用相同的舍入模式\n");
    printf("   - 考虑监管要求的舍入规则\n\n");

    printf("3. 精度检查:\n");
    printf("   - 验证 _Decimal64 (16位) 足够业务需求\n");
    printf("   - 如需更高精度，使用 _Decimal128\n");
    printf("   - 注意中间计算可能产生的精度丢失\n\n");

    printf("4. 性能优化:\n");
    printf("   - 利用硬件加速 (现代 Intel/AMD CPU)\n");
    printf("   - 避免频繁的十进制/二进制转换\n");
    printf("   - 批量处理以减少函数调用开销\n\n");

    printf("5. 测试策略:\n");
    printf("   - 包含边界值测试\n");
    printf("   - 验证舍入边界 (如 x.5)\n");
    printf("   - 大规模数据对比测试\n");
    printf("   - 长期累积误差测试\n");
}

int main(void) {
    cobol_migration();
    java_migration();
    best_practices_summary();

    return 0;
}
```

---

## 总结

### 关键要点回顾

1. **为什么使用十进制浮点**
   - 二进制浮点无法精确表示 0.1、0.01 等常用财务数值
   - 累积误差会导致严重的财务偏差
   - 法律和监管要求精确计算

2. **三种十进制类型选择**
   - `_Decimal32`: 仅用于存储，7位精度
   - `_Decimal64`: 推荐使用，16位精度，适合绝大多数财务计算
   - `_Decimal128`: 超高精度，34位，用于科学计算或中间结果

3. **字面量后缀**
   - `DF`: _Decimal32
   - `DD`: _Decimal64
   - `DL`: _Decimal128

4. **最佳实践**
   - 始终使用后缀指定字面量类型
   - 使用银行家舍入 (ROUND_HALF_EVEN) 减少累积误差
   - 避免与二进制浮点混合运算
   - 建立完整的舍入和验证策略

5. **与其他系统对比**
   - 比 Java BigDecimal 快 50-500 倍
   - 内存占用远低于 BigDecimal
   - 是现代替代 COBOL 的优秀选择

### 编译命令参考

```bash
# 基本编译 (GCC 12+)
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
gcc -std=c23 program.c -o program

# 启用优化
gcc -std=c23 -O3 program.c -o program

# 启用所有警告
gcc -std=c23 -Wall -Wextra -Wpedantic program.c -o program

# 调试模式
gcc -std=c23 -g -O0 program.c -o program

# 链接数学库 (如需要)
gcc -std=c23 program.c -o program -lm
```

### 进一步学习资源

- IEEE 754-2008 标准文档
- C23 标准 (ISO/IEC 9899:2023)
- Intel 64 and IA-32 Architectures Software Developer's Manual
- AMD64 Architecture Programmer's Manual

---

*文档版本: 1.0*
*最后更新: 2026-03-17*
*兼容标准: C23, IEEE 754-2008*


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
