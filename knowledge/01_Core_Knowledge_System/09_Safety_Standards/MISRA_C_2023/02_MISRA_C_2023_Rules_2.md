# MISRA C:2023 Rules 2.1-2.7 - 未使用代码

> 消除死代码、提升代码质量、优化可维护性

---

## 规则分类概述

| 规则编号 | 规则名称 | 强制性 | 核心要求 |
|:--------:|:---------|:------:|:---------|
| Rule 2.1 | 不可达代码 | Required | 不得包含不可执行代码 |
| Rule 2.2 | 无效代码 | Required | 不得包含无副作用的表达式 |
| Rule 2.3 | 未使用类型声明 | Advisory | 删除未使用的typedef/struct/union/enum |
| Rule 2.4 | 未使用标签 | Advisory | 删除未使用的标签 |
| Rule 2.5 | 未使用宏 | Advisory | 删除未使用的宏 |
| Rule 2.6 | 未使用宏参数 | Advisory | 宏参数必须在宏体内使用 |
| Rule 2.7 | 未使用函数参数 | Advisory | 函数参数必须使用或显式标注 |

---


---

## 📑 目录

- [MISRA C:2023 Rules 2.1-2.7 - 未使用代码](#misra-c2023-rules-21-27---未使用代码)
  - [规则分类概述](#规则分类概述)
  - [📑 目录](#-目录)
  - [Rule 2.1 - 不可达代码（Unreachable Code）](#rule-21---不可达代码unreachable-code)
    - [规则原文](#规则原文)
    - [详细解释](#详细解释)
    - [常见不可达代码模式](#常见不可达代码模式)
      - [模式1: return后的代码](#模式1-return后的代码)
      - [模式2: 无条件跳转后的代码](#模式2-无条件跳转后的代码)
      - [模式3: 常量条件导致的不可达分支](#模式3-常量条件导致的不可达分支)
      - [模式4: 死循环后的代码](#模式4-死循环后的代码)
    - [合规示例](#合规示例)
    - [检测工具配置](#检测工具配置)
    - [自动化检测脚本](#自动化检测脚本)
  - [Rule 2.2 - 无效代码（Dead Code）](#rule-22---无效代码dead-code)
    - [规则原文](#规则原文-1)
    - [无效代码 vs 不可达代码](#无效代码-vs-不可达代码)
    - [常见无效代码模式](#常见无效代码模式)
      - [模式1: 未使用的表达式结果](#模式1-未使用的表达式结果)
      - [模式2: 自增/自减但未使用](#模式2-自增自减但未使用)
      - [模式3: 无效的条件判断](#模式3-无效的条件判断)
      - [模式4: 读取但不使用](#模式4-读取但不使用)
    - [合规示例](#合规示例-1)
    - [抑制无效代码警告](#抑制无效代码警告)
  - [Rule 2.3 - 未使用类型声明](#rule-23---未使用类型声明)
    - [规则原文](#规则原文-2)
    - [违反示例](#违反示例)
    - [合规示例](#合规示例-2)
  - [Rule 2.4 - 未使用标签](#rule-24---未使用标签)
    - [规则原文](#规则原文-3)
    - [违反示例](#违反示例-1)
    - [合规示例](#合规示例-3)
  - [Rule 2.5 - 未使用宏](#rule-25---未使用宏)
    - [规则原文](#规则原文-4)
    - [违反示例](#违反示例-2)
    - [合规示例](#合规示例-4)
  - [Rule 2.6 - 未使用宏参数](#rule-26---未使用宏参数)
    - [规则原文](#规则原文-5)
    - [违反示例](#违反示例-3)
    - [合规示例](#合规示例-5)
  - [Rule 2.7 - 未使用函数参数](#rule-27---未使用函数参数)
    - [规则原文](#规则原文-6)
    - [违反示例](#违反示例-4)
    - [合规示例](#合规示例-6)
  - [综合检测与修复](#综合检测与修复)
    - [统一检测脚本](#统一检测脚本)
    - [项目级清理策略](#项目级清理策略)
  - [总结](#总结)
    - [最佳实践](#最佳实践)


---

## Rule 2.1 - 不可达代码（Unreachable Code）

### 规则原文

> **Rule 2.1 (Required)**: A project shall not contain unreachable code.
>
> **中文**: 项目不得包含不可达代码。

### 详细解释

不可达代码是指永远不会被执行的代码。它会增加维护负担、代码大小，并可能导致混淆。

### 常见不可达代码模式

#### 模式1: return后的代码

```c
/* ❌ 违反 Rule 2.1 - return后的代码不可达 */
int calculate(int x)
{
    if (x < 0) {
        return -1;
    }
    return x * 2;
    printf("Result calculated\n");  /* 不可达！ */
}
```

#### 模式2: 无条件跳转后的代码

```c
/* ❌ 违反 Rule 2.1 */
void process(void)
{
    goto cleanup;
    init_resources();  /* 不可达！ */
cleanup:
    release_resources();
}
```

#### 模式3: 常量条件导致的不可达分支

```c
/* ❌ 违反 Rule 2.1 */
void configure(void)
{
    const int mode = 1;  /* 常量 */

    if (mode == 1) {
        setup_mode_a();
    } else {
        setup_mode_b();  /* 永远不可达 */
    }
}
```

#### 模式4: 死循环后的代码

```c
/* ❌ 违反 Rule 2.1 */
void run_forever(void)
{
    while (1) {
        process_data();
    }
    cleanup();  /* 不可达！ */
}
```

### 合规示例

```c
/* ✅ 合规 - 删除不可达代码 */
int calculate(int x)
{
    if (x < 0) {
        return -1;
    }
    return x * 2;
}

/* ✅ 合规 - 使用条件编译 */
#define MODE_A 1

void configure(void)
{
    #if MODE_A
        setup_mode_a();
    #else
        setup_mode_b();
    #endif
}

/* ✅ 合规 - 死循环中的清理 */
_Noreturn void run_forever(void)
{
    while (1) {
        process_data();
        /* 如果需要清理，在循环内处理 */
    }
}
```

### 检测工具配置

```bash
# GCC -Wunreachable-code（需要启用优化）
gcc -O2 -Wunreachable-code -c file.c

# Clang 静态分析器
scan-build gcc -c file.c

# PC-lint
pc-lint -e522 -e527 file.c  /* 522: highest stack, 527: unreachable */

# Cppcheck
cppcheck --enable=all --suppress=missingInclude file.c
```

### 自动化检测脚本

```bash
#!/bin/bash
# check_unreachable.sh

echo "Checking for unreachable code..."

# 使用Clang静态分析
scan-build \
    -enable-checker alpha.core.CallAndMessageUnInitRefArg \
    -enable-checker alpha.deadcode.UnreachableCode \
    gcc -c "$1" 2>&1 | grep -A5 "unreachable"

# 使用GCC优化报告
gcc -O2 -fopt-info-vec-optimized-missed -c "$1" 2>&1 | grep -i "unreachable"
```

---

## Rule 2.2 - 无效代码（Dead Code）

### 规则原文

> **Rule 2.2 (Required)**: There shall be no dead code.
>
> **中文**: 不得包含无效代码。

### 无效代码 vs 不可达代码

| 类型 | 定义 | 示例 |
|:-----|:-----|:-----|
| 不可达代码 | 永远不会执行 | return后的代码 |
| 无效代码 | 执行但无副作用 | `x + 1;`（结果未使用）|

### 常见无效代码模式

#### 模式1: 未使用的表达式结果

```c
/* ❌ 违反 Rule 2.2 */
void process(void)
{
    int x = 10;
    x + 5;  /* 计算结果未使用，无效！ */
    x * 2;  /* 无效！ */
}
```

#### 模式2: 自增/自减但未使用

```c
/* ❌ 违反 Rule 2.2 */
void count(void)
{
    int i = 0;
    i++;  /* i++的结果未使用，但i本身被修改 */
    /* 如果意图是修改i，应该使用i += 1 */
}
```

#### 模式3: 无效的条件判断

```c
/* ❌ 违反 Rule 2.2 */
void check(int x)
{
    x > 10;  /* 比较结果未使用！ */

    if (x > 10) {  /* 正确的写法 */
        process();
    }
}
```

#### 模式4: 读取但不使用

```c
/* ❌ 违反 Rule 2.2 */
void read_sensor(void)
{
    int value = adc_read();  /* 读取但不使用 */
    /* 应该使用value */
}
```

### 合规示例

```c
/* ✅ 合规 - 使用表达式结果 */
void process(int x)
{
    int result = x + 5;
    use_result(result);
}

/* ✅ 合规 - 明确的意图 */
void count(void)
{
    static int counter = 0;
    counter++;  /* 明确用于计数 */
}

/* ✅ 合规 - 使用读取的数据 */
void read_sensor(void)
{
    int value = adc_read();
    if (value > THRESHOLD) {
        trigger_alarm();
    }
}

/* ✅ 合规 - 显式void转换表示故意忽略 */
void discard_result(void)
{
    (void)printf("Message\n");  /* 显式忽略返回值 */
}
```

### 抑制无效代码警告

```c
/* 当故意忽略返回值时 */
#define IGNORE_RESULT(x) ((void)(x))

void example(void)
{
    IGNORE_RESULT(memcpy(dst, src, len));  /* 明确忽略 */
}
```

---

## Rule 2.3 - 未使用类型声明

### 规则原文

> **Rule 2.3 (Advisory)**: A project should not contain unused type declarations.
>
> **中文**: 项目不应包含未使用的类型声明。

### 违反示例

```c
/* ❌ 违反 Rule 2.3 - 未使用的类型 */
typedef int MyInt;  /* 未使用 */

struct UnusedStruct {  /* 未使用 */
    int x;
    int y;
};

union UnusedUnion {  /* 未使用 */
    int i;
    float f;
};

enum UnusedEnum {  /* 未使用 */
    STATE_A,
    STATE_B
};

void func(void)
{
    /* 没有使用上述任何类型 */
}
```

### 合规示例

```c
/* ✅ 合规 - 只声明使用的类型 */
typedef int32_t sensor_value_t;

struct SensorData {
    sensor_value_t value;
    uint32_t timestamp;
};

void process_sensor(struct SensorData *data)
{
    /* 使用声明的类型 */
}
```

---

## Rule 2.4 - 未使用标签

### 规则原文

> **Rule 2.4 (Advisory)**: A project should not contain unused tag declarations.
>
> **中文**: 项目不应包含未使用的标签声明。

### 违反示例

```c
/* ❌ 违反 Rule 2.4 */
void process(void)
{
    if (init() != OK) {
        goto error_handler;
    }

    do_work();
    return;

error_handler:  /* 如果所有错误都在if内处理，这个标签可能未使用 */
    cleanup();
    return;
}

/* ❌ 违反 Rule 2.4 - switch中的未使用case */
enum State { STATE_INIT, STATE_RUN, STATE_STOP };

void handle_state(enum State s)
{
    switch (s) {
    case STATE_INIT:
        init();
        break;
    case STATE_RUN:
        run();
        break;
    /* STATE_STOP 未处理 */
    }
}
```

### 合规示例

```c
/* ✅ 合规 - 所有标签都使用 */
void process(void)
{
    if (init() != OK) {
        goto error_handler;
    }

    if (setup() != OK) {
        goto error_handler;
    }

    do_work();
    return;

error_handler:
    cleanup();
    return;
}

/* ✅ 合规 - 完整的switch处理 */
void handle_state(enum State s)
{
    switch (s) {
    case STATE_INIT:
        init();
        break;
    case STATE_RUN:
        run();
        break;
    case STATE_STOP:
        stop();
        break;
    default:
        /* 处理意外状态 */
        break;
    }
}
```

---

## Rule 2.5 - 未使用宏

### 规则原文

> **Rule 2.5 (Advisory)**: A project should not contain unused macro declarations.
>
> **中文**: 项目不应包含未使用的宏声明。

### 违反示例

```c
/* ❌ 违反 Rule 2.5 */
#define MAX_BUFFER_SIZE 1024  /* 未使用 */
#define DEBUG_MODE 1          /* 未使用 */
#define VERSION "1.0"         /* 未使用 */

void func(void)
{
    /* 没有使用上述宏 */
}
```

### 合规示例

```c
/* ✅ 合规 - 条件编译中的未使用是允许的 */
#ifdef DEBUG
    #define DEBUG_PRINT(x) printf(x)
#else
    #define DEBUG_PRINT(x)  /* 在release中未使用，但允许 */
#endif

/* ✅ 合规 - 头文件保护 */
#ifndef HEADER_H
#define HEADER_H
/* ... */
#endif  /* HEADER_H 只在#ifndef中引用，但这是标准做法 */
```

---

## Rule 2.6 - 未使用宏参数

### 规则原文

> **Rule 2.6 (Advisory)**: A function should not contain unused label declarations.
>
> **中文**: 函数不应包含未使用的标签声明。

### 违反示例

```c
/* ❌ 违反 Rule 2.6 - 宏参数未使用 */
#define MAX(a, b) ((a) > (b) ? (a) : (b))  /* 如果只用a */

/* 错误的使用 */
#define CHECK(x, y) ((x) > 0)  /* y未使用 */

/* ❌ 违反 - 多参数宏中的未使用 */
#define LOG(level, msg, ...) \
    printf("[%d] %s\n", level, msg)  /* __VA_ARGS__未使用 */
```

### 合规示例

```c
/* ✅ 合规 - 所有参数都使用 */
#define MAX(a, b) ((a) > (b) ? (a) : (b))

/* ✅ 合规 - 使用所有参数 */
#define CHECK(x, y) ((x) > (y))

/* ✅ 合规 - 使用可变参数 */
#define LOG(level, msg, ...) \
    printf("[%d] " msg "\n", level, ##__VA_ARGS__)
```

---

## Rule 2.7 - 未使用函数参数

### 规则原文

> **Rule 2.7 (Advisory)**: There should be no unused parameters in functions.
>
> **中文**: 函数中不应存在未使用的参数。

### 违反示例

```c
/* ❌ 违反 Rule 2.7 */
void callback(int event, void *data)
{
    /* 只使用event，未使用data */
    process_event(event);
}

/* ❌ 违反 - 接口要求但必须实现的空函数 */
void unused_handler(int arg1, int arg2)
{
    /* 空实现，参数未使用 */
}
```

### 合规示例

```c
/* ✅ 合规 - 显式忽略未使用的参数 */
void callback(int event, void *data)
{
    (void)data;  /* 显式表示故意忽略 */
    process_event(event);
}

/* ✅ 合规 - 使用GCC属性（如果允许扩展） */
#ifdef __GNUC__
    #define UNUSED __attribute__((unused))
#else
    #define UNUSED
#endif

void callback(int event, void *data UNUSED)
{
    process_event(event);
}

/* ✅ 合规 - 删除不需要的参数 */
void simplified_callback(int event)
{
    process_event(event);
}
```

---

## 综合检测与修复

### 统一检测脚本

```bash
#!/bin/bash
# check_unused.sh - 检测所有未使用代码

echo "=== Checking for unused code ==="

# 1. 未使用函数参数
# GCC
 gcc -Wunused-parameter -c "$1" 2>&1 | grep "warning: unused parameter"

# 2. 未使用宏
 cppcheck --enable=all "$1" 2>&1 | grep "unusedMacro"

# 3. 未使用类型
 clang -Wunused-macros -c "$1" 2>&1 | grep "warning:"

# 4. 不可达代码
 scan-build -enable-checker alpha.deadcode.UnreachableCode gcc -c "$1"
```

### 项目级清理策略

```makefile
# Makefile - 添加清理检查目标

check-unused:
 @echo "Checking for unused code..."
 @# 使用PC-lint
 @pc-lint -w2 -e522 -e529 -e550 -e578 $(SOURCES) 2>&1 | \
  grep -E "(unused|unreachable|dead)" || echo "No unused code found"

 @# 使用Cppcheck
 @cppcheck --enable=all --suppress=missingInclude $(SOURCES) 2>&1 | \
  grep -E "(unusedFunction|unreadVariable)" || true

clean-unused: check-unused
 @echo "Review the output above and manually remove unused code"
 @echo "Automated removal is not recommended for safety-critical code"
```

---

## 总结

| 规则 | 强制 | 核心要求 | 检测重点 |
|:----:|:----:|:---------|:---------|
| 2.1 | Required | 删除不可达代码 | return后的代码、死循环后 |
| 2.2 | Required | 删除无效代码 | 未使用结果的表达式 |
| 2.3 | Advisory | 删除未使用类型 | typedef/struct/union/enum |
| 2.4 | Advisory | 删除未使用标签 | goto标签、case标签 |
| 2.5 | Advisory | 删除未使用宏 | #define定义的宏 |
| 2.6 | Advisory | 使用所有宏参数 | 宏定义中的参数 |
| 2.7 | Advisory | 使用所有函数参数 | 或显式(void)转换 |

### 最佳实践

1. **启用所有编译器警告**：`-Wall -Wextra -Wunused`
2. **定期运行静态分析工具**：PC-lint、Cppcheck、Clang Static Analyzer
3. **代码审查时检查**：特别关注函数末尾、条件分支
4. **使用IDE支持**：现代IDE可以实时标记未使用代码
5. **建立CI检查**：每次提交自动检测未使用代码

---

**参考**: MISRA C:2023 Guidelines for the use of the C language in critical systems, Chapter 2

---

> **下一章节**: [Rules 3.1-3.4 - 注释规范](./03_MISRA_C_2023_Rules_3.md)


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
