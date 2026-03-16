# 安全标准与规范

> 安全关键系统C语言开发的国际标准和最佳实践

---

## 标准概览

| 标准 | 应用领域 | 文档状态 |
|:-----|:---------|:--------:|
| [MISRA C:2023](./MISRA_C_2023/) | 汽车、航空、工业 | ✅ 完成(50+规则) |
| [IEEE 754-2019](./IEEE_754_Floating_Point/) | 浮点运算 | ✅ 完成(基础+运算) |
| ISO 26262 | 汽车功能安全 | 📝 待创建 |
| DO-178C | 航空软件 | 📝 待创建 |
| IEC 61508 | 工业安全 | 📝 待创建 |
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
| [Rules 6.x](./MISRA_C_2023/06_MISRA_C_2023_Rules_6.md) | 表达式 | 6.1-6.10 |
| [Rules 7.x](./MISRA_C_2023/07_MISRA_C_2023_Rules_7.md) | 指针安全 | 7.1-7.5 |
| [Rules 8.x](./MISRA_C_2023/08_MISRA_C_2023_Rules_8.md) | 预处理与库 | 8.1-8.15 |
| [Rules 9.x](./MISRA_C_2023/09_MISRA_C_2023_Rules_9.md) | 资源与并发 | 9.1-9.5 |

### 快速检查清单

```markdown
□ 无未定义行为（Rule 1.3）
□ 类型转换安全（Rules 5.x）
□ 指针使用安全（Rules 7.x）
□ 表达式无副作用冲突（Rule 6.1）
□ 内存管理正确（Rules 7.x, 8.7）
□ 资源对称释放（Rules 9.x）
```

---

## IEEE 754-2019

浮点运算国际标准。

### 文档结构

| 文档 | 内容 |
|:-----|:-----|
| [基础](./IEEE_754_Floating_Point/01_IEEE_754_Basics.md) | 二进制格式、特殊值、精度 |
| [运算](./IEEE_754_Floating_Point/02_IEEE_754_Operations.md) | 算术运算、舍入、异常 |

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

## 安全关键编程原则

### 1. 防御性编程

```c
/* 参数验证 */
void process(const char *data, size_t len)
{
    if (data == NULL || len == 0) {
        return;
    }
    /* 处理 */
}

/* 边界检查 */
if (index >= 0 && index < ARRAY_SIZE) {
    array[index] = value;
}
```

### 2. 失败安全

```c
/* 默认安全 */
enum state current = STATE_SAFE;

switch (cmd) {
    case CMD_START:
        current = STATE_RUNNING;
        break;
    default:
        /* 未知命令：保持安全状态 */
        break;
}
```

### 3. 资源管理

```c
/* RAII模式 */
struct resource *r = resource_open();
if (r == NULL) {
    return ERROR;
}

/* 处理 */

resource_close(r);  /* 总是关闭 */
```

---

## 工具链

### 静态分析

| 工具 | 用途 | MISRA支持 |
|:-----|:-----|:---------:|
| PC-lint | 静态分析 | ✅ 完整 |
| Coverity | 深度分析 | ✅ 部分 |
| Cppcheck | 开源检查 | ⚠️ 基础 |
| Clang Static Analyzer | 编译时检查 | ⚠️ 部分 |

### 运行时检查

```bash
# Address Sanitizer
gcc -fsanitize=address -g -o prog prog.c

# Undefined Behavior Sanitizer
gcc -fsanitize=undefined -o prog prog.c

# 浮点异常捕获
gcc -fsignaling-nans -o prog prog.c
```

---

## 参考资源

- [MISRA C:2023](https://misra.org.uk/) - 官方标准
- [IEEE 754-2019](https://ieeexplore.ieee.org/document/8766229) - 浮点标准
- [ISO 26262](https://www.iso.org/standard/68383.html) - 汽车功能安全
- [DO-178C](https://my.rtca.org/) - 航空软件标准

---

**维护者**: C_Lang Knowledge Base Team
**最后更新**: 2026-03
