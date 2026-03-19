# 安全标准与规范

> 安全关键系统C语言开发的国际标准和最佳实践

---

## 标准概览

| 标准 | 应用领域 | 文档状态 |
|:-----|:---------|:--------:|
| [MISRA C:2023](./MISRA_C_2023/README.md) | 汽车、航空、工业 | ✅ 完成(220条规则+工具) |
| [IEEE 754-2019](./IEEE_754_Floating_Point/README.md) | 浮点运算 | ✅ 完成(核心) |
| [ISO 26262](./ISO_26262/README.md) | 汽车功能安全 | ✅ 完成(概述+ASIL分解) |
| [DO-178C](./DO_178C/README.md) | 航空软件 | ✅ 完成(概述+MC/DC) |
| [IEC 61508](./IEC_61508/README.md) | 工业功能安全 | ✅ 完成(概述+SIL实施) |
| [POSIX.1-2024](./POSIX_1_2024/README.md) | 可移植操作系统 | ✅ 完成(概述) |
| [快速参考指南](./00_Quick_Reference_Guide.md) | 全部标准 | ✅ 完成(速查表) |

---

## MISRA C:2023 ✅ 100%完成

安全关键C语言开发的事实标准。

### 文档结构（22章完整覆盖）

| 文档 | 内容 | 规则范围 | 状态 |
|:-----|:-----|:---------|:----:|
| [规则索引](./MISRA_C_2023/00_MISRA_C_2023_Index.md) | 完整规则速查 | 全部220条 | ✅ |
| [01_Rules_1-5](./MISRA_C_2023/01_MISRA_C_2023_Rules_1-5.md) | 语言扩展 | 1.1-1.5 | ✅ |
| [02_Rules_2](./MISRA_C_2023/02_MISRA_C_2023_Rules_2.md) | 代码设计 | 2.1-2.7 | ✅ |
| [03_Rules_3](./MISRA_C_2023/03_MISRA_C_2023_Rules_3.md) | 注释与可读性 | 3.1-3.4 | ✅ |
| [04_Rules_4](./MISRA_C_2023/04_MISRA_C_2023_Rules_4.md) | 标识符 | 4.1-4.5 | ✅ |
| [05_Rules_5](./MISRA_C_2023/05_MISRA_C_2023_Rules_5.md) | 类型系统 | 5.1-5.9 | ✅ |
| [06_Rules_6](./MISRA_C_2023/06_MISRA_C_2023_Rules_6.md) | 表达式与语句 | 6.1-6.10 | ✅ |
| [07_Rules_7](./MISRA_C_2023/07_MISRA_C_2023_Rules_7.md) | 指针安全 | 7.1-7.5 | ✅ |
| [08_Rules_8](./MISRA_C_2023/08_MISRA_C_2023_Rules_8.md) | 预处理与标准库 | 8.1-8.15 | ✅ |
| [09_Rules_9](./MISRA_C_2023/09_MISRA_C_2023_Rules_9.md) | 资源与并发 | 9.1-9.5 | ✅ |
| [10_Rules_10](./MISRA_C_2023/10_MISRA_C_2023_Rules_10.md) | 声明与定义 | 10.1-10.8 | ✅ |
| [11_Rules_11](./MISRA_C_2023/11_MISRA_C_2023_Rules_11.md) | 表达式与运算符 | 11.1-11.6 | ✅ |
| [12_Rules_12](./MISRA_C_2023/12_MISRA_C_2023_Rules_12.md) | 控制流语句 | 12.1-12.6 | ✅ |
| [13_Rules_13](./MISRA_C_2023/13_MISRA_C_2023_Rules_13.md) | 函数安全 | 13.1-13.6 | ✅ |
| [14_Rules_14](./MISRA_C_2023/14_MISRA_C_2023_Rules_14.md) | 初始化 | 14.1-14.4 | ✅ |
| [15_Rules_15](./MISRA_C_2023/15_MISRA_C_2023_Rules_15.md) | 类型转换 | 15.1-15.7 | ✅ |
| [16_Rules_16](./MISRA_C_2023/16_MISRA_C_2023_Rules_16.md) | 函数调用 | 16.1-16.7 | ✅ |
| [17_Rules_17](./MISRA_C_2023/17_MISRA_C_2023_Rules_17.md) | 结构与联合 | 17.1-17.8 | ✅ |
| [18_Rules_18](./MISRA_C_2023/18_MISRA_C_2023_Rules_18.md) | 指针和数组 | 18.1-18.6 | ✅ |
| [19_Rules_19](./MISRA_C_2023/19_MISRA_C_2023_Rules_19.md) | 预处理指令 | 19.1-19.3 | ✅ |
| [20_Rules_20](./MISRA_C_2023/20_MISRA_C_2023_Rules_20.md) | 标准库 | 20.1-20.x | ✅ |
| [21_Rules_21](./MISRA_C_2023/21_MISRA_C_2023_Rules_21.md) | 运行时错误 | 21.1-21.x | ✅ |
| [22_Rules_22](./MISRA_C_2023/22_MISRA_C_2023_Rules_22.md) | 其他规则 | 22.1-22.x | ✅ |

**总计**: 22章，220条规则，100%文档化

### 规则实施状态

| 章节 | 规则范围 | 完成度 |
|:-----|:---------|:------:|
| 第1-9章 | Rules 1.x-9.x | ✅ 100% |
| 第10-13章 | Rules 10.x-13.x | ✅ 100% |
| 第14-18章 | Rules 14.x-18.x | ✅ 100% |
| 第19-22章 | Rules 19.x-22.x | ✅ 100% |
| **总体** | **220/220条** | **✅ 100%** |

### 快速检查清单

```markdown
## 类型安全
□ 不使用隐式窄化转换 (5.x, 15.x)
□ char类型只用于字符 (5.1)
□ 不使用变长数组VLA (5.7, 18.6)
□ 枚举类型完整处理

## 指针安全
□ 解引用前检查NULL (7.1, 13.3)
□ free后立即置NULL (7.2)
□ 指针运算在数组边界内 (7.3, 18.x)
□ 不使用已释放内存 (7.2)

## 表达式安全
□ 不依赖表达式求值顺序 (6.1, 11.4)
□ 单表达式中只有一个副作用 (6.2)
□ 不使用逗号运算符 (6.10)
□ 布尔表达式使用bool类型 (6.3)
□ 位运算使用无符号类型 (11.5)

## 控制流
□ switch有default且在最后 (6.6, 12.2)
□ 不使用向后goto (6.8, 12.6)
□ 循环计数器不在体内修改 (12.3)
□ 无死代码 (6.5, 22.2)

## 函数安全
□ 参数数量匹配 (16.1)
□ 参数类型兼容 (16.2)
□ 返回值使用或显式忽略 (13.4)
□ 递归有深度限制 (13.5, 16.4)

## 资源管理
□ malloc后检查返回值
□ 资源对称释放 (9.x)
□ 头文件有包含保护 (8.2, 19.x)
□ 宏参数完全括号化 (8.1)

## 运行时安全
□ 防止除以零 (21.1)
□ 防止算术溢出 (21.2)
□ 防止数组越界 (18.1)
□ 字符串操作安全 (18.4)
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
| [基础](../01_Basic_Layer/IEEE_754_Floating_Point/01_IEEE_754_Basics.md) | 二进制格式、特殊值、精度 |
| [运算](../01_Basic_Layer/IEEE_754_Floating_Point/02_IEEE_754_Operations.md) | 运算、舍入模式、异常 |

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
- [POSIX.1-2024](https://pubs.opengroup.org/onlinepubs/9799919799/) - 系统接口标准

---

**维护者**: C_Lang Knowledge Base Team
**最后更新**: 2026-03
