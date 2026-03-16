# 安全标准与规范

> 安全关键系统C语言开发的国际标准和最佳实践

---

## 标准概览

| 标准 | 应用领域 | 文档状态 |
|:-----|:---------|:--------:|
| [MISRA C:2023](./MISRA_C_2023/) | 汽车、航空、工业 | ✅ 完成(90+规则) |
| [IEEE 754-2019](./IEEE_754_Floating_Point/) | 浮点运算 | ✅ 完成(核心) |
| [ISO 26262](./ISO_26262/) | 汽车功能安全 | ✅ 完成(概述) |
| [DO-178C](./DO_178C/) | 航空软件 | ✅ 完成(概述) |
| [IEC 61508](./IEC_61508/) | 工业功能安全 | ✅ 完成(概述) |
| POSIX.1-2024 | 可移植操作系统 | 📝 待创建 |

---

## MISRA C:2023

安全关键C语言开发的事实标准。

### 文档结构

| 文档 | 内容 | 规则覆盖 |
|:-----|:-----|:--------:|
| [规则索引](./MISRA_C_2023/00_MISRA_C_2023_Index.md) | 完整规则速查 | 全部索引 |
| [概述与Rules 1.x](./MISRA_C_2023/01_MISRA_C_2023_Rules_1-5.md) | 语言扩展 | 1.1-1.5 |
| [Rules 2.x](./MISRA_C_2023/02_MISRA_C_2023_Rules_2.md) | 代码设计 | 2.1-2.7 |
| [Rules 3.x](./MISRA_C_2023/03_MISRA_C_2023_Rules_3.md) | 注释与可读性 | 3.1-3.4 |
| [Rules 4.x](./MISRA_C_2023/04_MISRA_C_2023_Rules_4.md) | 标识符 | 4.1-4.5 |
| [Rules 5.x](./MISRA_C_2023/05_MISRA_C_2023_Rules_5.md) | 类型系统 | 5.1-5.9 |
| [Rules 6.x](./MISRA_C_2023/06_MISRA_C_2023_Rules_6.md) | 表达式与语句 | 6.1-6.10 |
| [Rules 7.x](./MISRA_C_2023/07_MISRA_C_2023_Rules_7.md) | 指针安全 | 7.1-7.5 |
| [Rules 8.x](./MISRA_C_2023/08_MISRA_C_2023_Rules_8.md) | 预处理与标准库 | 8.1-8.15 |
| [Rules 9.x](./MISRA_C_2023/09_MISRA_C_2023_Rules_9.md) | 资源与并发 | 9.1-9.5 |
| [Rules 10.x](./MISRA_C_2023/10_MISRA_C_2023_Rules_10.md) | 声明与定义 | 10.1-10.8 |
| [Rules 11.x](./MISRA_C_2023/11_MISRA_C_2023_Rules_11.md) | 表达式与运算符 | 11.1-11.6 |
| [Rules 12.x](./MISRA_C_2023/12_MISRA_C_2023_Rules_12.md) | 控制流语句 | 12.1-12.6 |
| [Rules 13.x](./MISRA_C_2023/13_MISRA_C_2023_Rules_13.md) | 函数安全 | 13.1-13.6 |

### 规则实施状态

| 章节 | 规则范围 | 状态 | 完成度 |
|:-----|:---------|:----:|:------:|
| 第1章 | Rules 1.1-1.5 | ✅ | 100% |
| 第2章 | Rules 2.1-2.7 | ✅ | 100% |
| 第3章 | Rules 3.1-3.4 | ✅ | 100% |
| 第4章 | Rules 4.1-4.5 | ✅ | 100% |
| 第5章 | Rules 5.1-5.9 | ✅ | 100% |
| 第6章 | Rules 6.1-6.10 | ✅ | 100% |
| 第7章 | Rules 7.1-7.5 | ✅ | 100% |
| 第8章 | Rules 8.1-8.15 | ✅ | 100% |
| 第9章 | Rules 9.1-9.5 | ✅ | 100% |
| 第10章 | Rules 10.1-10.8 | ✅ | 100% |
| 第11章 | Rules 11.1-11.6 | ✅ | 100% |
| 第12章 | Rules 12.1-12.6 | ✅ | 100% |
| 第13章 | Rules 13.1-13.6 | ✅ | 100% |
| 第14-22章 | Rules 14.x-22.x | 📝 | 待创建 |

**总计**: ~90条规则已文档化（共220条，约41%）

### 快速检查清单

```markdown
## 类型安全
□ 不使用隐式窄化转换
□ char类型只用于字符
□ 不使用变长数组(VLA)
□ 枚举类型完整处理

## 指针安全
□ 解引用前检查NULL
□ free后立即置NULL
□ 指针运算在数组边界内
□ 不使用已释放内存

## 表达式安全
□ 不依赖表达式求值顺序
□ 单表达式中只有一个副作用
□ 不使用逗号运算符
□ 布尔表达式使用bool类型

## 控制流
□ switch有default
□ 不使用向后goto
□ 循环计数器不在体内修改
□ 无死代码
```

---

## 功能安全标准

### ASIL/DAL/SIL对比

| 标准 | 等级系统 | 最高等级 | C语言要求 |
|:-----|:---------|:---------|:----------|
| ISO 26262 | ASIL (A-D) | ASIL D | MISRA C Required+Mandatory |
| DO-178C | DAL (A-E) | Level A | MISRA C Required+Mandatory |
| IEC 61508 | SIL (1-4) | SIL 4 | MISRA C Required+Mandatory |

### 标准关系

```
IEC 61508 (通用基础)
    ├── ISO 26262 (汽车)
    ├── EN 50128 (铁路)
    ├── IEC 62304 (医疗)
    ├── IEC 61511 (过程工业)
    └── IEC 62061 (机械)

DO-178C (航空，独立发展但与MISRA兼容)
```

---

## IEEE 754-2019

浮点运算国际标准。

### 文档结构

| 文档 | 内容 |
|:-----|:-----|
| [基础](./IEEE_754_Floating_Point/01_IEEE_754_Basics.md) | 二进制格式、特殊值、精度 |
| [运算](./IEEE_754_Floating_Point/02_IEEE_754_Operations.md) | 运算、舍入模式、异常 |

### 关键概念

```c
#include <math.h>
#include <float.h>
#include <fenv.h>

/* 机器精度 */
float eps_f = FLT_EPSILON;    /* ~1.19e-07 */
double eps_d = DBL_EPSILON;   /* ~2.22e-16 */

/* 特殊值 */
float inf = INFINITY;
float nan = NAN;

/* 舍入模式 */
fesetround(FE_TONEAREST);     /* 默认 */
fesetround(FE_UPWARD);        /* 向正无穷 */

/* 异常检查 */
feclearexcept(FE_ALL_EXCEPT);
/* 运算... */
if (fetestexcept(FE_OVERFLOW)) {
    /* 处理溢出 */
}
```

---

## 工具链

### 静态分析工具

| 工具 | 厂商 | 说明 | 价格 |
|:-----|:-----|:-----|:----:|
| PC-lint/Flexelint | Gimpel | 行业标杆 | 💰 |
| Coverity | Synopsys | 深度分析 | 💰💰 |
| Polyspace | MathWorks | 形式化验证 | 💰💰 |
| Klocwork | Perforce | 企业级 | 💰💰 |
| Cppcheck | 开源 | 免费方案 | 免费 |

### 编译器检查

```bash
# GCC
 gcc -Wall -Wextra -Wpedantic \
     -Wshadow -Wconversion \
     -Wundef -Wstrict-prototypes \
     -c file.c

# Clang（更严格）
 clang -Weverything \
       -Wno-padded \
       -c file.c
```

---

## 参考资源

- [MISRA C:2023](https://misra.org.uk/) - 官方标准
- [IEEE 754-2019](https://ieeexplore.ieee.org/document/8766229) - 浮点标准
- [ISO 26262](https://www.iso.org/standard/68383.html) - 汽车功能安全
- [DO-178C](https://my.rtca.org/) - 航空软件标准
- [IEC 61508](https://www.iec.ch/functionalsafety) - 工业功能安全

---

**维护者**: C_Lang Knowledge Base Team
**最后更新**: 2026-03
