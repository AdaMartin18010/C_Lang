# MISRA C:2025 规则 1.1-1.5 - 标准C环境

> **规则范围**: 语言标准符合性、环境配置
> **适用标准**: C11/C17/C23
> **状态**: [待MISRA官方文档确认]

---

## 目录

- [MISRA C:2025 规则 1.1-1.5 - 标准C环境](#misra-c2025-规则-11-15---标准c环境)
  - [目录](#目录)
  - [规则分类概述](#规则分类概述)
  - [C:2025变化标记说明](#c2025变化标记说明)
  - [Rule 1.1 - 标准C符合性](#rule-11---标准c符合性)
    - [规则原文 \[待官方确认\]](#规则原文-待官方确认)
    - [C23翻译限制 \[待官方确认\]](#c23翻译限制-待官方确认)
    - [违反示例](#违反示例)
    - [合规示例](#合规示例)
  - [Rule 1.2 - 语言扩展文档化](#rule-12---语言扩展文档化)
    - [规则原文 \[待官方确认\]](#规则原文-待官方确认-1)
    - [C23特性作为标准扩展](#c23特性作为标准扩展)
    - [违反示例](#违反示例-1)
    - [合规示例](#合规示例-1)
  - [Rule 1.3 - 未定义行为](#rule-13---未定义行为)
    - [规则原文 \[待官方确认\]](#规则原文-待官方确认-2)
    - [C23中的UB改进](#c23中的ub改进)
    - [违反示例](#违反示例-2)
    - [合规示例](#合规示例-2)
  - [Rule 1.4 - 运行时错误](#rule-14---运行时错误)
    - [规则原文 \[待官方确认\]](#规则原文-待官方确认-3)
    - [合规示例](#合规示例-3)
  - [Rule 1.5 - 字符集使用](#rule-15---字符集使用)
    - [规则原文 \[待官方确认\]](#规则原文-待官方确认-4)
    - [C23 UTF-8支持 \[待官方确认\]](#c23-utf-8支持-待官方确认)
    - [合规示例](#合规示例-4)
  - [检测工具配置](#检测工具配置)
    - [GCC配置](#gcc配置)
    - [Clang配置](#clang配置)
  - [总结](#总结)

---

## 规则分类概述

| 规则编号 | 规则名称 | 强制性 | C:2025变化 | 核心要求 |
|:--------:|:---------|:------:|:----------:|:---------|
| Rule 1.1 | 标准C符合性 | Required | 🔄 C23支持 | 符合ISO C标准 |
| Rule 1.2 | 语言扩展文档化 | Required | 🔄 更新 | 扩展必须文档化 |
| Rule 1.3 | 未定义行为 | Required | 🔄 C23改进 | 避免未定义行为 |
| Rule 1.4 | 运行时错误 | Mandatory | ➡️ 无变化 | 错误处理不退出 |
| Rule 1.5 | 字符集使用 | Advisory | 🔄 C23 UTF-8 | 标准字符集 |

> **图例**: 🔄 修改 | ➡️ 无变化 | 🆕 新增 | 🗑️ 废弃

---

## C:2025变化标记说明

本文档使用以下标记表示与MISRA C:2023相比的变化：

- **[待官方确认]**: 具体内容需参考MISRA C:2025官方文档
- **[C:2023延续]**: 规则与MISRA C:2023相同
- **[C:2025更新]**: 规则在C:2025中有更新
- **[C23相关]**: 与C23标准特性相关

---

## Rule 1.1 - 标准C符合性

### 规则原文 [待官方确认]

> The program shall contain no violations of the standard C syntax and constraints, and shall not exceed the implementation's translation limits.
>
> **中文**: 程序不得违反标准C的语法和约束，且不得超过实现的翻译限制。

**C:2025变化**:

- 扩展支持C23标准 [待官方确认]
- 翻译限制更新 [待官方确认]

### C23翻译限制 [待官方确认]

| 限制项 | C23预期值 | C11最小值 | 超标后果 |
|:-------|:---------:|:---------:|:---------|
| 嵌套块层数 | [待确认] | 127 | 编译失败 |
| 函数参数 | [待确认] | 127 | 编译失败 |
| 标识符有效长度 | [待确认] | 63 | 名称冲突 |
| 逻辑源行字符数 | [待确认] | 4095 | 截断/编译错误 |
| 宏定义嵌套 | [待确认] | 63 | 预处理错误 |

### 违反示例

```c
/* ❌ 违反 [C:2023延续] - 使用GCC嵌套函数扩展（非标准C） */
void foo(void)
{
    void bar(void) {  /* GCC扩展 */
        printf("nested\n");
    }
    bar();
}

/* ❌ 违反 [C:2023延续] - 超过函数参数限制 */
void func(int a0, ..., int a127, int a128);  /* 128个参数 */

/* ❌ 违反 [C23相关] - 使用编译器不支持C23特性 */
#if __STDC_VERSION__ < 202311L
/* 在不支持C23的编译器上使用C23特性 */
constexpr int x = 5;  /* 编译错误 */
#endif
```

### 合规示例

```c
/* ✅ 合规 [C:2023延续] - 标准C */
void foo(void);

static void bar(void) {  /* 独立的静态函数 */
    printf("standard C\n");
}

void foo(void)
{
    bar();
}

/* ✅ 合规 [C23相关] - 检查C23支持 */
#if __STDC_VERSION__ >= 202311L
    /* C23代码 */
    constexpr int MAX_SIZE = 100;
#else
    /* C11/C17兼容代码 */
    #define MAX_SIZE 100
#endif
```

---

## Rule 1.2 - 语言扩展文档化

### 规则原文 [待官方确认]

> Language extensions shall be documented.
>
> **中文**: 语言扩展必须文档化。

**C:2025变化**:

- C23特性作为标准特性，非扩展 [待官方确认]
- typeof从扩展变为标准（C23） [待官方确认]

### C23特性作为标准扩展

| 特性 | C11/C17状态 | C23状态 | MISRA处理 |
|:-----|:-----------:|:-------:|:---------:|
| `typeof` | 扩展 | 标准 | C23不使用偏离 |
| `nullptr` | 扩展 | 标准 | C23不使用偏离 |
| `constexpr` | 扩展 | 标准 | C23不使用偏离 |
| `_BitInt` | 扩展 | 标准 | C23不使用偏离 |
| `__attribute__` | 扩展 | 扩展 | 需文档化 |
| `asm` | 扩展 | 扩展 | 需文档化 |

### 违反示例

```c
/* ❌ 违反 [C:2023延续] - 未文档化的扩展使用 */
struct __attribute__((packed)) Packet {  /* GCC扩展，未文档化 */
    uint8_t header;
    uint32_t data;
};

/* ❌ 违反 [C:2023延续] - 内联汇编未文档化 */
void delay(void) {
    asm volatile("nop");  /* 未文档化 */
}
```

### 合规示例

```c
/*
 * 合规 [C:2023延续] - 文档化的扩展使用
 * 扩展: GCC packed属性
 * 理由: 硬件寄存器映射需要精确内存布局
 * 范围: drivers/registers.h
 */
struct __attribute__((packed)) Packet {
    uint8_t header;
    uint32_t data;
};

/*
 * 合规 [C23相关] - C23标准特性不需要偏离
 * 这是标准C23，不是扩展
 */
#if __STDC_VERSION__ >= 202311L
    typeof(int) my_int;  /* C23标准特性 */
#else
    __typeof__(int) my_int;  /* GCC扩展，需文档化 */
#endif
```

---

## Rule 1.3 - 未定义行为

### 规则原文 [待官方确认]

> There shall be no occurrence of undefined or critical unspecified behaviour.
>
> **中文**: 不得出现未定义或关键未指定行为。

**C:2025变化**:

- C23修复了一些UB [待官方确认]
- 新增C23特性的UB指导 [待官方确认]

### C23中的UB改进

| UB类型 | C11/C17 | C23改进 | MISRA指导 |
|:-------|:-------:|:-------:|:---------:|
| 有符号溢出 | UB | UB | 使用ckd_add [待确认] |
| 移位超位宽 | UB | UB | 始终检查 |
| 空指针解引用 | UB | UB | 始终检查 |
| 数组越界 | UB | UB | 始终检查 |
| constexpr修改 | N/A | UB [待确认] | 避免修改 |

### 违反示例

```c
/* ❌ 违反 [C:2023延续] - 有符号整数溢出（UB） */
int add(int a, int b) {
    return a + b;  /* 溢出 = 灾难 */
}

/* ❌ 违反 [C:2023延续] - 未检查空指针 */
void process(int *p) {
    *p = 42;  /* p可能为NULL */
}

/* ❌ 违反 [C23相关] - constexpr修改 [待确认] */
#if __STDC_VERSION__ >= 202311L
    constexpr int arr[] = {1, 2, 3};
    /* arr[0] = 5; */  /* 修改constexpr数据是UB */
#endif
```

### 合规示例

```c
/* ✅ 合规 [C23相关] - C23溢出检查 */
#if __STDC_VERSION__ >= 202311L
    #include <stdckdint.h>
#endif

int add_safe(int a, int b, int *result) {
#if __STDC_VERSION__ >= 202311L
    return ckd_add(result, a, b);  /* C23溢出检查 */
#else
    /* C11/C17手动检查 */
    if (b > 0 && a > INT_MAX - b) return 0;  /* 溢出 */
    if (b < 0 && a < INT_MIN - b) return 0;  /* 下溢 */
    *result = a + b;
    return 1;
#endif
}

/* ✅ 合规 [C:2023延续] - 空指针检查 */
void process_safe(int *p) {
    if (p != NULL) {
        *p = 42;
    }
}
```

---

## Rule 1.4 - 运行时错误

### 规则原文 [待官方确认]

> The implementation shall not invoke abort, exit, quick_exit, or thrd_exit.
>
> **中文**: 实现不得调用abort、exit、quick_exit或thrd_exit。

**C:2025变化**: 无变化 [待官方确认]

### 合规示例

```c
/* ✅ 合规 [C:2023延续] - 错误码+恢复机制 */
typedef enum {
    ERR_NONE = 0,
    ERR_HARDWARE,
    ERR_MEMORY
} error_t;

static volatile error_t g_last_error = ERR_NONE;

void handle_error(error_t error)
{
    g_last_error = error;
    log_error(error);
    /* 尝试恢复或进入安全状态 */
    enter_safe_state();
}

/* 不使用exit/abort */

/* ✅ 合规 [C23相关] - 错误处理 */
void process_with_c23_features(void) {
#if __STDC_VERSION__ >= 202311L
    constexpr int MAX_RETRIES = 3;
#else
    const int MAX_RETRIES = 3;
#endif

    for (int i = 0; i < MAX_RETRIES; i++) {
        if (try_operation()) {
            return;  /* 成功 */
        }
    }
    handle_error(ERR_HARDWARE);  /* 错误处理，不退出 */
}
```

---

## Rule 1.5 - 字符集使用

### 规则原文 [待官方确认]

> Characters and strings should only use the C standard source character set.
>
> **中文**: 字符和字符串应仅使用C标准源字符集。

**C:2025变化**: C23 UTF-8支持更新 [待官方确认]

### C23 UTF-8支持 [待官方确认]

| 特性 | C11/C17 | C23 | MISRA指导 |
|:-----|:-------:|:---:|:---------:|
| char字符串 | ASCII | UTF-8兼容 | 建议使用ASCII |
| char8_t | 无 | 新增 | [待确认] |
| u8前缀 | 有 | 有 | [待确认] |
| UTF-8源文件 | 依赖编译器 | 标准支持 | [待确认] |

### 合规示例

```c
/* ✅ 合规 [C:2023延续] - 源代码使用ASCII */
const char *msg = "Temperature: 25C";

/* ✅ 合规 - 本地化通过资源文件 */
const char *localized = get_string(STRING_ID_TEMP);

/* [C23相关] - UTF-8字符 [待官方确认] */
#if __STDC_VERSION__ >= 202311L
    /* C23 char8_t - 使用待定 */
    /* const char8_t *utf8_msg = u8"Hello"; */
#endif

/* ✅ 合规 - 字符串字面量 */
const char *ascii_only = "Safe ASCII string";  /* 推荐 */
```

---

## 检测工具配置

### GCC配置

```bash
# C11/C17标准模式 [C:2023延续]
gcc -std=c11 -pedantic -Werror -c file.c
gcc -std=c17 -pedantic -Werror -c file.c

# C23标准模式 [C23相关]
gcc -std=c23 -pedantic -Werror -c file.c

# 未定义行为检测 [C:2023延续]
gcc -fsanitize=undefined -c file.c

# 严格标准检查 [C:2023延续]
gcc -Wall -Wextra -Wpedantic -Werror -c file.c
```

### Clang配置

```bash
# C11/C17 [C:2023延续]
clang -std=c11 -pedantic -Werror -c file.c

# C23 [C23相关]
clang -std=c23 -pedantic -Werror -c file.c

# 严格检查 [C:2023延续]
clang -Weverything -Werror -c file.c
```

---

## 总结

| 规则 | 强制 | C:2025变化 | 关键行动 |
|:----:|:----:|:----------:|:---------|
| 1.1 | Required | 🔄 C23支持 | 检查编译器C23支持 |
| 1.2 | Required | 🔄 更新 | 注意typeof现在是标准 |
| 1.3 | Required | 🔄 C23改进 | 使用C23溢出检查 |
| 1.4 | Mandatory | ➡️ 无变化 | 保持错误处理策略 |
| 1.5 | Advisory | 🔄 C23 UTF-8 | 评估UTF-8使用策略 |

---

**参考**: MISRA C:2025 Guidelines [待官方确认]
**上一章**: [索引](./00_MISRA_C_2025_Index.md)
**下一章**: 规则 2.x - 代码设计（待创建）

> **免责声明**: 本文档基于MISRA C:2023和公开信息创建，具体规则内容请以MISRA官方发布的C:2025文档为准。
