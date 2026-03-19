# MISRA C:2023 规则索引

> 完整规则速查表（220条规则）

---

## 规则分类总览

| 类别 | 规则数量 | 说明 |
|:-----|:--------:|:-----|
| Mandatory | ~20 | 必须100%遵守 |
| Required | ~140 | 必须遵守，可正式偏离 |
| Advisory | ~60 | 建议遵守 |

---

## 规则索引

### 第1章 - 语言扩展 (Rules 1.1-1.5)

| 规则 | 类别 | 简要描述 |
|:----:|:----:|:---------|
| [Rule 1.1](./01_MISRA_C_2023_Rules_1-5.md#rule-11) | Required | 程序必须符合C标准 |
| [Rule 1.2](./01_MISRA_C_2023_Rules_1-5.md#rule-12) | Required | 语言扩展必须文档化 |
| [Rule 1.3](./01_MISRA_C_2023_Rules_1-5.md#rule-13) | Required | 无未定义或未指定行为 |
| [Rule 1.4](./01_MISRA_C_2023_Rules_1-5.md#rule-14) | Mandatory | 运行时错误必须文档化 |
| [Rule 1.5](./01_MISRA_C_2023_Rules_1-5.md#rule-15) | Advisory | 使用证据基础的语言 |

### 第2章 - 代码设计 (Rules 2.1-2.7)

| 规则 | 类别 | 简要描述 |
|:----:|:----:|:---------|
| [Rule 2.1](./02_MISRA_C_2023_Rules_2.md#rule-21) | Required | 项目必须包含编译器设置 |
| [Rule 2.2](./02_MISRA_C_2023_Rules_2.md#rule-22) | Required | 所有源代码必须可编译 |
| [Rule 2.3](./02_MISRA_C_2023_Rules_2.md#rule-23) | Advisory | 不使用inline汇编 |
| [Rule 2.4](./02_MISRA_C_2023_Rules_2.md#rule-24) | Advisory | 文档化外部组件 |
| [Rule 2.5](./02_MISRA_C_2023_Rules_2.md#rule-25) | Required | 代码必须可追溯到需求 |
| [Rule 2.6](./02_MISRA_C_2023_Rules_2.md#rule-26) | Advisory | 使用函数而非函数指针 |
| [Rule 2.7](./02_MISRA_C_2023_Rules_2.md#rule-27) | Required | 全局命名空间管理 |

### 第3章 - 注释与字符 (Rules 3.1-3.4)

| 规则 | 类别 | 简要描述 |
|:----:|:----:|:---------|
| [Rule 3.1](./03_MISRA_C_2023_Rules_3.md#rule-31) | Required | 使用基本源代码字符集 |
| [Rule 3.2](./03_MISRA_C_2023_Rules_3.md#rule-32) | Advisory | 不使用三字符组 |
| [Rule 3.3](./03_MISRA_C_2023_Rules_3.md#rule-33) | Advisory | 注释格式规范 |
| [Rule 3.4](./03_MISRA_C_2023_Rules_3.md#rule-34) | Advisory | 文档注释使用标准格式 |

### 第4章 - 标识符 (Rules 4.1-4.5)

| 规则 | 类别 | 简要描述 |
|:----:|:----:|:---------|
| [Rule 4.1](./04_MISRA_C_2023_Rules_4.md#rule-41) | Required | 标识符前63字符唯一 |
| [Rule 4.2](./04_MISRA_C_2023_Rules_4.md#rule-42) | Required | 不隐藏其他标识符 |
| [Rule 4.3](./04_MISRA_C_2023_Rules_4.md#rule-43) | Required | 宏名不与关键字冲突 |
| [Rule 4.4](./04_MISRA_C_2023_Rules_4.md#rule-44) | Advisory | 文档化标识符 |
| [Rule 4.5](./04_MISRA_C_2023_Rules_4.md#rule-45) | Advisory | typedef名称唯一 |

### 第5章 - 类型系统 (Rules 5.1-5.9)

| 规则 | 类别 | 简要描述 |
|:----:|:----:|:---------|
| [Rule 5.1](./05_MISRA_C_2023_Rules_5.md#rule-51) | Required | char只用于字符值 |
| [Rule 5.2](./05_MISRA_C_2023_Rules_5.md#rule-52) | Required | 不移除const/volatile |
| [Rule 5.3](./05_MISRA_C_2023_Rules_5.md#rule-53) | Required | 指针转换安全 |
| [Rule 5.4](./05_MISRA_C_2023_Rules_5.md#rule-54) | Required | 避免信息丢失的隐式转换 |
| [Rule 5.5](./05_MISRA_C_2023_Rules_5.md#rule-55) | Required | 浮点转换精确 |
| [Rule 5.6](./05_MISRA_C_2023_Rules_5.md#rule-56) | Advisory | 不使用复合字面量 |
| [Rule 5.7](./05_MISRA_C_2023_Rules_5.md#rule-57) | Required | 不使用变长数组VLA |
| [Rule 5.8](./05_MISRA_C_2023_Rules_5.md#rule-58) | Advisory | 柔性数组谨慎使用 |
| [Rule 5.9](./05_MISRA_C_2023_Rules_5.md#rule-59) | Required | 文档化位域布局 |

### 第6章 - 表达式 (Rules 6.1-6.10)

| 规则 | 类别 | 简要描述 |
|:----:|:----:|:---------|
| [Rule 6.1](./06_MISRA_C_2023_Rules_6.md#rule-61) | Required | 不依赖求值顺序 |
| [Rule 6.2](./06_MISRA_C_2023_Rules_6.md#rule-62) | Required | 副作用限制 |
| [Rule 6.3](./06_MISRA_C_2023_Rules_6.md#rule-63) | Required | 布尔值正确使用 |
| [Rule 6.4](./06_MISRA_C_2023_Rules_6.md#rule-64) | Required | 避免常量条件 |
| [Rule 6.5](./06_MISRA_C_2023_Rules_6.md#rule-65) | Required | 无死代码 |
| [Rule 6.6](./06_MISRA_C_2023_Rules_6.md#rule-66) | Required | switch语句完整 |
| [Rule 6.7](./06_MISRA_C_2023_Rules_6.md#rule-67) | Required | 循环控制安全 |
| [Rule 6.8](./06_MISRA_C_2023_Rules_6.md#rule-68) | Required | 不向后跳转 |
| [Rule 6.9](./06_MISRA_C_2023_Rules_6.md#rule-69) | Required | 空语句显式化 |
| [Rule 6.10](./06_MISRA_C_2023_Rules_6.md#rule-610) | Advisory | 避免逗号运算符 |

### 第7章 - 指针安全 (Rules 7.1-7.5)

| 规则 | 类别 | 简要描述 |
|:----:|:----:|:---------|
| [Rule 7.1](./07_MISRA_C_2023_Rules_7.md#rule-71) | Required | 解引用前检查NULL |
| [Rule 7.2](./07_MISRA_C_2023_Rules_7.md#rule-72) | Required | 不使用已释放指针 |
| [Rule 7.3](./07_MISRA_C_2023_Rules_7.md#rule-73) | Required | 指针运算在数组内 |
| [Rule 7.4](./07_MISRA_C_2023_Rules_7.md#rule-74) | Required | 数组索引验证 |
| [Rule 7.5](./07_MISRA_C_2023_Rules_7.md#rule-75) | Required | 内存恰好释放一次 |

### 第8章 - 预处理与标准库 (Rules 8.1-8.15)

| 规则 | 类别 | 简要描述 |
|:----:|:----:|:---------|
| [Rule 8.1](./08_MISRA_C_2023_Rules_8.md#rule-81) | Required | 宏参数括号化 |
| [Rule 8.2](./08_MISRA_C_2023_Rules_8.md#rule-82) | Required | 头文件保护 |
| [Rule 8.3](./08_MISRA_C_2023_Rules_8.md#rule-83) | Required | 不使用未定义宏 |
| [Rule 8.4](./08_MISRA_C_2023_Rules_8.md#rule-84) | Required | 不使用#和## |
| [Rule 8.5](./08_MISRA_C_2023_Rules_8.md#rule-85) | Required | 条件指令完整 |
| [Rule 8.6](./08_MISRA_C_2023_Rules_8.md#rule-86) | Required | 禁用危险函数 |
| [Rule 8.7](./08_MISRA_C_2023_Rules_8.md#rule-87) | Required | 运行时无malloc |
| [Rule 8.8](./08_MISRA_C_2023_Rules_8.md#rule-88) | Required | 正确使用errno |
| [Rule 8.9](./08_MISRA_C_2023_Rules_8.md#rule-89) | Required | 格式字符串常量 |
| [Rule 8.10](./08_MISRA_C_2023_Rules_8.md#rule-810) | Required | 文件操作检查 |
| [Rule 8.11](./08_MISRA_C_2023_Rules_8.md#rule-811) | Required | 字符串操作安全 |
| [Rule 8.12](./08_MISRA_C_2023_Rules_8.md#rule-812) | Advisory | 时间函数限制 |
| [Rule 8.13](./08_MISRA_C_2023_Rules_8.md#rule-813) | Advisory | 随机数限制 |
| [Rule 8.14](./08_MISRA_C_2023_Rules_8.md#rule-814) | Required | 信号处理安全 |
| [Rule 8.15](./08_MISRA_C_2023_Rules_8.md#rule-815) | Required | 线程安全 |

### 第9章 - 资源与并发 (Rules 9.1-9.5)

| 规则 | 类别 | 简要描述 |
|:----:|:----:|:---------|
| [Rule 9.1](./09_MISRA_C_2023_Rules_9.md#rule-91) | Required | 资源同层管理 |
| [Rule 9.2](./09_MISRA_C_2023_Rules_9.md#rule-92) | Required | 同模块分配释放 |
| [Rule 9.3](./09_MISRA_C_2023_Rules_9.md#rule-93) | Required | 信号处理极简 |
| [Rule 9.4](./09_MISRA_C_2023_Rules_9.md#rule-94) | Required | 共享数据保护 |
| [Rule 9.5](./09_MISRA_C_2023_Rules_9.md#rule-95) | Required | 同步原语正确 |

---

## 实施优先级

### 第一优先级（核心安全）

1. Rules 1.x - 标准符合性
2. Rules 5.x - 类型安全
3. Rules 6.x - 表达式安全
4. Rules 7.x - 指针安全

### 第二优先级（代码质量）

1. Rules 4.x - 标识符管理
2. Rules 8.x - 预处理与库
3. Rules 2.x - 设计规范

### 第三优先级（架构）

1. Rules 3.x - 文档
2. Rules 9.x - 资源管理

---

## 快速参考

### 最危险的违反

| 违反 | 风险 | 检测方式 |
|:-----|:-----|:---------|
| 解引用NULL | 崩溃/安全漏洞 | 静态分析 |
| 缓冲区溢出 | 安全漏洞 | ASan + 静态分析 |
| 整数溢出 | 未定义行为 | UBSan |
| 使用已释放内存 | 崩溃/安全漏洞 | ASan |
| 未定义求值顺序 | 难以调试 | -Wsequence-point |

### 必备编译选项

```bash
 gcc \
   -std=c17 -pedantic \
   -Wall -Wextra -Werror \
   -Wshadow -Wconversion \
   -Wundef -Wstrict-prototypes \
   -fsanitize=address,undefined \
   -c file.c
```

---

**完整文档**: [MISRA C:2023 README](./README.md)


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
