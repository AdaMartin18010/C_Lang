# MISRA C:2023 完全指南

> 汽车、航空、医疗等安全关键系统的C语言编码规范

---

## 概述

MISRA C (Motor Industry Software Reliability Association C) 是安全关键系统C语言开发的事实标准。MISRA C:2023是该标准的最新版本，包含220条规则，涵盖C11和C17语言特性。

| 属性 | 说明 |
|:-----|:-----|
| **版本** | MISRA C:2023 |
| **语言标准** | C11, C17 |
| **总规则数** | 220条 |
| **强制性** | 分为Mandatory/Required/Advisory |
| **应用领域** | 汽车、航空、铁路、医疗、工业控制 |

---

## 规则分类

### 按章节划分

| 章节 | 内容 | 规则数 | 文档 |
|:-----|:-----|:------:|:-----|
| 第1章 | 语言扩展与标准符合性 | 5 | [Rules 1.1-1.5](./01_MISRA_C_2023_Rules_1-5.md) |
| 第2章 | 代码设计与架构 | 7 | [Rules 2.1-2.7](./02_MISRA_C_2023_Rules_2.md) |
| 第3章 | 注释与可读性 | 4 | [Rules 3.1-3.4](./03_MISRA_C_2023_Rules_3.md) |
| 第4章 | 标识符 | 5 | [Rules 4.1-4.5](./04_MISRA_C_2023_Rules_4.md) |
| 第5章 | 类型系统 | 9 | [Rules 5.1-5.9](./05_MISRA_C_2023_Rules_5.md) |
| 第6章 | 表达式与语句 | 10 | [Rules 6.1-6.10](./06_MISRA_C_2023_Rules_6.md) |
| 第7章 | 指针与内存 | 5 | [Rules 7.1-7.5](./07_MISRA_C_2023_Rules_7.md) |
| 第8章 | 预处理与标准库 | 15 | [Rules 8.1-8.15](./08_MISRA_C_2023_Rules_8.md) |
| 第9章 | 资源与并发 | 5 | [Rules 9.1-9.5](./09_MISRA_C_2023_Rules_9.md) |

### 按强制性划分

| 类别 | 说明 | 合规要求 |
|:-----|:-----|:---------|
| **Mandatory** | 强制性规则 | 必须100%合规，不得偏离 |
| **Required** | 要求规则 | 必须合规，特殊情况可正式偏离 |
| **Advisory** | 建议规则 | 建议遵守，可根据情况灵活处理 |

---

## 核心原则

### 1. 明确定义的行为

```c
/* ✅ 避免未定义行为 */
/* 不明确定表达式求值顺序 */
int a = func1();
int b = func2();
int result = a + b;  /* 明确 */

/* ✅ 避免整数溢出 */
if (a > INT_MAX - b) {
    /* 处理溢出 */
} else {
    int sum = a + b;  /* 安全 */
}
```

### 2. 显式优于隐式

```c
/* ✅ 显式类型转换 */
float f = (float)integer_value;

/* ✅ 显式比较 */
if (ptr != NULL) { }  /* 而非 if (ptr) */
if (count != 0) { }   /* 而非 if (count) */
```

### 3. 防御性编程

```c
/* ✅ 参数验证 */
void process(const char *data, size_t len)
{
    if (data == NULL || len == 0) {
        return;  /* 提前返回 */
    }
    /* 处理 */
}

/* ✅ 默认情况处理 */
switch (status) {
    case OK:    /* ... */ break;
    case ERROR: /* ... */ break;
    default:
        /* 意外情况 */
        log_error("Unexpected status");
        break;
}
```

---

## 快速检查清单

### 代码审查清单

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

## 资源管理
□ malloc后检查返回值
□ 资源对称释放
□ 头文件有包含保护
□ 宏参数完全括号化
```

---

## 工具支持

### 静态分析工具

| 工具 | 厂商 | 说明 |
|:-----|:-----|:-----|
| PC-lint/Flexelint | Gimpel | 行业标杆 |
| Coverity | Synopsys | 深度分析 |
| Polyspace | MathWorks | 形式化验证 |
| Klocwork | Perforce | 企业级 |
| Cppcheck | 开源 | 免费方案 |

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

## 偏离程序

当无法遵守规则时，需要正式的偏离程序：

### 偏离要求

1. **记录偏离原因**
2. **进行风险评估**
3. **获得授权批准**
4. **定期审查偏离**

### 偏离模板

```markdown
## 偏离记录

规则编号: X.Y
规则描述: [规则简要描述]
偏离原因: [技术限制/第三方代码/性能要求等]
风险分析: [不遵守该风险的潜在影响]
缓解措施: [采取的替代安全措施]
偏离范围: [文件/模块/函数]
批准人: [姓名]
批准日期: [日期]
审查日期: [下次审查日期]
```

---

## 相关标准

| 标准 | 关系 | 适用领域 |
|:-----|:-----|:---------|
| ISO 26262 | 参考MISRA C | 汽车功能安全 |
| DO-178C | 可配合使用 | 航空软件 |
| IEC 61508 | 参考MISRA C | 工业安全 |
| IEC 62304 | 可配合使用 | 医疗器械 |
| EN 50128 | 参考MISRA C | 铁路信号 |

---

## 参考资源

### 官方资源

- MISRA C:2023 Guidelines (需购买)
- MISRA Compliance:2020
- MISRA C:2012 Amendment 4

### 延伸阅读

- [ISO/IEC 9899:2018](https://www.iso.org/standard/74528.html) - C17标准
- [SEI CERT C](https://wiki.sei.cmu.edu/confluence/display/c) - CERT安全编码
- [NASA JPL C](https://yurichev.com/mirrors/C/JPL_Coding_Standard_C.pdf) - NASA编码标准

---

## 贡献

如发现错误或建议，请提交issue或PR。

---

**最后更新**: 2026-03

**维护者**: C_Lang Knowledge Base Team
