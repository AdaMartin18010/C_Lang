# MISRA C:2023 Rules 8.1-8.15 - 预处理与标准库

> 宏安全、头文件管理、标准库使用限制

---

## 规则分类概述

| 规则编号 | 规则名称 | 强制性 | 核心要求 |
|:--------:|:---------|:------:|:---------|
| Rule 8.1 | 宏定义 | Required | 宏参数和体必须括号化 |
| Rule 8.2 | 头文件保护 | Required | 防止重复包含 |
| Rule 8.3 | 未定义宏 | Required | 避免使用未定义宏 |
| Rule 8.4 | 预处理运算符 | Required | #和##使用限制 |
| Rule 8.5 | 预处理器条件 | Required | 条件编译完整 |
| Rule 8.6 | 标准库限制 | Required | 禁用危险函数 |
| Rule 8.7 | 动态内存 | Required | malloc/free限制 |
| Rule 8.8 | 错误处理 | Required | errno和错误检查 |
| Rule 8.9 | 格式化IO | Required | 格式字符串安全 |
| Rule 8.10 | 文件操作 | Required | 文件IO安全 |
| Rule 8.11 | 字符串函数 | Required | 字符串操作安全 |
| Rule 8.12 | 时间函数 | Advisory | 时间函数限制 |
| Rule 8.13 | 随机数 | Advisory | 随机数生成限制 |
| Rule 8.14 | 信号 | Required | 信号处理限制 |
| Rule 8.15 | 多线程 | Required | 线程安全 |

---


---

## 📑 目录

- [MISRA C:2023 Rules 8.1-8.15 - 预处理与标准库](#misra-c2023-rules-81-815---预处理与标准库)
  - [规则分类概述](#规则分类概述)
  - [📑 目录](#-目录)
  - [Rule 8.1 - 宏定义安全](#rule-81---宏定义安全)
    - [规则原文](#规则原文)
    - [违反示例](#违反示例)
    - [合规示例](#合规示例)
    - [多语句宏模板](#多语句宏模板)
  - [Rule 8.2 - 头文件保护](#rule-82---头文件保护)
    - [规则原文](#规则原文-1)
    - [违反示例](#违反示例-1)
    - [合规示例](#合规示例-1)
  - [Rule 8.3 - 未定义宏](#rule-83---未定义宏)
    - [规则原文](#规则原文-2)
    - [违反示例](#违反示例-2)
    - [合规示例](#合规示例-2)
  - [Rule 8.4 - 预处理运算符限制](#rule-84---预处理运算符限制)
    - [规则原文](#规则原文-3)
    - [违反示例](#违反示例-3)
    - [合规示例](#合规示例-3)
  - [Rule 8.5 - 预处理器条件完整](#rule-85---预处理器条件完整)
    - [规则原文](#规则原文-4)
    - [违反示例](#违反示例-4)
    - [合规示例](#合规示例-4)
  - [Rule 8.6 - 危险函数禁用](#rule-86---危险函数禁用)
    - [规则原文](#规则原文-5)
    - [禁用函数列表](#禁用函数列表)
    - [违反示例](#违反示例-5)
    - [合规示例](#合规示例-5)
  - [Rule 8.7 - 动态内存限制](#rule-87---动态内存限制)
    - [规则原文](#规则原文-6)
    - [嵌入式系统内存策略](#嵌入式系统内存策略)
  - [Rule 8.8 - 错误处理](#rule-88---错误处理)
    - [规则原文](#规则原文-7)
    - [违反示例](#违反示例-6)
    - [合规示例](#合规示例-6)
  - [Rule 8.9 - 格式化IO安全](#rule-89---格式化io安全)
    - [规则原文](#规则原文-8)
    - [违反示例](#违反示例-7)
    - [合规示例](#合规示例-7)
  - [Rule 8.10 - 文件操作安全](#rule-810---文件操作安全)
    - [规则原文](#规则原文-9)
    - [安全文件操作模式](#安全文件操作模式)
  - [检测工具配置](#检测工具配置)
    - [GCC检查](#gcc检查)
    - [PC-lint配置](#pc-lint配置)
  - [总结](#总结)


---

## Rule 8.1 - 宏定义安全

### 规则原文

> **Rule 8.1 (Required)**: Macro parameters shall be enclosed in parentheses.
>
> **中文**: 宏参数必须用括号括起来。

### 违反示例

```c
/* ❌ 违反 - 参数无括号 */
#define SQUARE(x) x * x
SQUARE(a + b);  /* 展开: a + b * a + b，不是(a+b)*(a+b) */

/* ❌ 违反 - 宏体无括号 */
#define DOUBLE(x) (x) + (x)
DOUBLE(x) * 2;  /* 展开: (x)+(x)*2，不是((x)+(x))*2 */

/* ❌ 违反 - 复杂表达式 */
#define MAX(a, b) a > b ? a : b
MAX(x, y) + 1;  /* 优先级问题 */
```

### 合规示例

```c
/* ✅ 合规 - 完全括号化 */
#define SQUARE(x) ((x) * (x))
SQUARE(a + b);  /* 展开: ((a+b)*(a+b)) */

/* ✅ 合规 - 整个表达式括号化 */
#define DOUBLE(x) ((x) + (x))
DOUBLE(x) * 2;  /* 展开: ((x)+(x))*2 */

/* ✅ 合规 - 条件表达式完全括号化 */
#define MAX(a, b) (((a) > (b)) ? (a) : (b))
MAX(x, y) + 1;  /* 展开: (((x)>(y))?(x):(y))+1 */

/* ✅ 合规 - 多语句宏 */
#define SWAP(a, b) do { \
    typeof(a) temp = (a); \
    (a) = (b); \
    (b) = temp; \
} while(0)
```

### 多语句宏模板

```c
/* MISRA合规的多语句宏 */
#define SAFE_FREE(ptr) do { \
    if ((ptr) != NULL) { \
        free(ptr); \
        (ptr) = NULL; \
    } \
} while(0)

/* 带返回值的宏 */
#define CHECK_NULL(ptr, ret) do { \
    if ((ptr) == NULL) { \
        return (ret); \
    } \
} while(0)

/* 使用 */
void *p = malloc(100);
SAFE_FREE(p);  /* 展开为安全的if语句 */
```

---

## Rule 8.2 - 头文件保护

### 规则原文

> **Rule 8.2 (Required)**: All header files shall be protected against multiple inclusion.
>
> **中文**: 所有头文件必须防止重复包含。

### 违反示例

```c
/* ❌ 违反 - 无保护的头文件 */
/* myheader.h */
void func(void);  /* 如果包含两次，会导致重复声明 */

/* ❌ 违反 - 不唯一的保护宏 */
/* file1.h */
#ifndef HEADER_H
#define HEADER_H
/* ... */
#endif

/* file2.h */
#ifndef HEADER_H  /* 与file1.h冲突！ */
#define HEADER_H
/* ... */
#endif
```

### 合规示例

```c
/* ✅ 合规 - 标准保护 */
/* project_module_component.h */
#ifndef PROJECT_MODULE_COMPONENT_H
#define PROJECT_MODULE_COMPONENT_H

/* 内容 */

#endif /* PROJECT_MODULE_COMPONENT_H */

/* ✅ 合规 - 命名空间前缀 */
/* sensor_temperature_h */
#ifndef SENSOR_TEMPERATURE_H
#define SENSOR_TEMPERATURE_H

#include <stdint.h>

/* 类型定义 */
typedef struct {
    int16_t celsius;
    uint8_t precision;
} sensor_temp_t;

/* 函数声明 */
int sensor_temp_init(void);
int sensor_temp_read(sensor_temp_t *out);

#endif /* SENSOR_TEMPERATURE_H */

/* ✅ 合规 - 使用#pragma once（非标准但广泛支持）*/
#pragma once  /* 作为双重保护 */
#ifndef UNIQUE_GUARD
#define UNIQUE_GUARD
/* ... */
#endif
```

---

## Rule 8.3 - 未定义宏

### 规则原文

> **Rule 8.3 (Required)**: A macro shall not be undefined before it has been defined.
>
> **中文**: 宏在定义前不得被取消定义。

### 违反示例

```c
/* ❌ 违反 - 未定义未定义的宏 */
#undef UNDEFINED_MACRO  /* 取消定义未定义的宏 */

/* ❌ 违反 - 条件依赖于未定义宏 */
#if UNDEFINED_MACRO > 0  /* 未定义宏在条件中使用 */
/* ... */
#endif
```

### 合规示例

```c
/* ✅ 合规 - 检查后再#undef */
#ifdef TEMP_MACRO
#undef TEMP_MACRO
#endif

/* ✅ 合规 - 定义默认值 */
#ifndef BUFFER_SIZE
#define BUFFER_SIZE 1024
#endif

/* ✅ 合规 - 使用defined运算符 */
#if defined(DEBUG) && DEBUG > 0
/* 调试代码 */
#endif
```

---

## Rule 8.4 - 预处理运算符限制

### 规则原文

> **Rule 8.4 (Required)**: The # and ## preprocessor operators shall not be used.
>
> **中文**: 不得使用#和##预处理运算符。

### 违反示例

```c
/* ❌ 违反 - 字符串化运算符 # */
#define LOG(msg) printf(#msg "\n")
LOG(Hello);  /* 展开为 printf("Hello" "\n") */

/* ❌ 违反 - 连接运算符 ## */
#define CONCAT(a, b) a ## b
int xy = 10;
CONCAT(x, y);  /* 展开为 xy，即10 */

/* ❌ 违反 - 组合使用 */
#define MAKE_VAR(type, name) type var_##name
MAKE_VAR(int, count);  /* int var_count */
```

### 合规示例

```c
/* ✅ 合规 - 使用函数代替宏 */
void log_message(const char *msg)
{
    printf("%s\n", msg);
}
log_message("Hello");

/* ✅ 合规 - 明确命名 */
int var_count = 0;  /* 不使用宏生成 */

/* ✅ 合规 - 使用类型安全的方式 */
typedef struct {
    int count;
} counter_t;
counter_t var_count;
```

---

## Rule 8.5 - 预处理器条件完整

### 规则原文

> **Rule 8.5 (Required)**: Preprocessor conditional directives shall be complete.
>
> **中文**: 预处理条件指令必须完整。

### 违反示例

```c
/* ❌ 违反 - 不完整的条件 */
#ifdef FEATURE
/* 某些代码 */
/* 缺少#endif */

/* ❌ 违反 - 条件嵌套错误 */
#ifdef A
#ifdef B
#endif  /* 关闭B */
/* A仍然打开，可能导致意外包含 */
```

### 合规示例

```c
/* ✅ 合规 - 完整的条件 */
#ifdef FEATURE_ENABLED
    /* 特性代码 */
#else
    /* 默认代码 */
#endif

/* ✅ 合规 - 清晰的嵌套 */
#ifdef PLATFORM_LINUX
    #ifdef USE_EPOLL
        /* Linux epoll代码 */
    #else
        /* Linux select代码 */
    #endif
#elif defined(PLATFORM_WINDOWS)
    /* Windows代码 */
#else
    #error "Unsupported platform"
#endif

/* ✅ 合规 - 使用静态断言 */
#ifndef BUFFER_SIZE
    #error "BUFFER_SIZE must be defined"
#endif
```

---

## Rule 8.6 - 危险函数禁用

### 规则原文

> **Rule 8.6 (Required)**: The following standard library functions shall not be used.
>
> **中文**: 不得使用以下标准库函数。

### 禁用函数列表

| 函数 | 替代方案 | 原因 |
|:-----|:---------|:-----|
| `gets()` | `fgets()` | 缓冲区溢出 |
| `strcpy()` | `strncpy()` | 缓冲区溢出 |
| `strcat()` | `strncat()` | 缓冲区溢出 |
| `sprintf()` | `snprintf()` | 缓冲区溢出 |
| `vsprintf()` | `vsnprintf()` | 缓冲区溢出 |
| `scanf()` | 验证输入或使用`fgets`+`sscanf` | 输入溢出 |
| `rand()` | 更安全的随机数生成器 | 可预测性 |
| `srand()` | 同上 | 同上 |

### 违反示例

```c
/* ❌ 违反 - 使用gets */
char buf[100];
gets(buf);  /* 危险！ */

/* ❌ 违反 - 使用strcpy */
char dest[10];
strcpy(dest, "long string here");  /* 溢出！ */

/* ❌ 违反 - 使用sprintf */
char buf[10];
int x = 123456;
sprintf(buf, "%d", x);  /* 可能溢出 */
```

### 合规示例

```c
/* ✅ 合规 - 安全字符串输入 */
char buf[100];
if (fgets(buf, sizeof(buf), stdin) != NULL) {
    /* 移除可能的换行符 */
    size_t len = strlen(buf);
    if (len > 0 && buf[len-1] == '\n') {
        buf[len-1] = '\0';
    }
}

/* ✅ 合规 - 安全字符串复制 */
char dest[10];
strncpy(dest, "source", sizeof(dest) - 1);
dest[sizeof(dest) - 1] = '\0';

/* ✅ 合规 - 安全格式化 */
char buf[10];
snprintf(buf, sizeof(buf), "%d", x);  /* 自动截断 */

/* ✅ 合规 - 安全输入解析 */
char line[256];
if (fgets(line, sizeof(line), stdin)) {
    int value;
    if (sscanf(line, "%d", &value) == 1) {
        /* 使用value */
    }
}
```

---

## Rule 8.7 - 动态内存限制

### 规则原文

> **Rule 8.7 (Required)**: Dynamic memory allocation shall not be used after initialization.
>
> **中文**: 初始化后不得使用动态内存分配。

### 嵌入式系统内存策略

```c
/* ✅ 合规 - 启动时分配 */
#define MAX_CONNECTIONS 16

static connection_t *connections[MAX_CONNECTIONS];
static bool initialized = false;

int system_init(void)
{
    /* 初始化阶段分配 */
    for (int i = 0; i < MAX_CONNECTIONS; i++) {
        connections[i] = malloc(sizeof(connection_t));
        if (connections[i] == NULL) {
            /* 清理并返回错误 */
            return -1;
        }
    }
    initialized = true;
    return 0;
}

/* 运行时不分配 */
int connection_get(void)
{
    if (!initialized) {
        return -1;
    }
    /* 从预分配池中获取 */
    for (int i = 0; i < MAX_CONNECTIONS; i++) {
        if (connections[i]->state == STATE_FREE) {
            connections[i]->state = STATE_USED;
            return i;
        }
    }
    return -1;  /* 资源耗尽 */
}

void connection_release(int id)
{
    if (id >= 0 && id < MAX_CONNECTIONS) {
        connections[id]->state = STATE_FREE;
    }
}
```

---

## Rule 8.8 - 错误处理

### 规则原文

> **Rule 8.8 (Required)**: errno shall be used only after a function that sets it has returned an error indicator.
>
> **中文**: 只有函数返回错误指示器后才能使用errno。

### 违反示例

```c
/* ❌ 违反 - 直接使用errno */
FILE *fp = fopen("file", "r");
if (errno == ENOENT) {  /* 错误！fopen可能未设置errno */
    /* ... */
}

/* ❌ 违反 - 不检查返回值 */
strtol(str, NULL, 10);
if (errno == ERANGE) {  /* 可能检查的是之前的错误 */
    /* ... */
}
```

### 合规示例

```c
/* ✅ 合规 - 检查返回值 */
FILE *fp = fopen("file", "r");
if (fp == NULL) {
    if (errno == ENOENT) {
        printf("File not found\n");
    } else {
        printf("Open error: %d\n", errno);
    }
}

/* ✅ 合规 - 使用errno前清零 */
errno = 0;
long val = strtol(str, &endptr, 10);
if (errno == ERANGE) {
    /* 溢出错误 */
}

/* ✅ 合规 - 优先使用返回值 */
int result = some_function();
if (result != 0) {
    /* 使用result判断错误 */
}
```

---

## Rule 8.9 - 格式化IO安全

### 规则原文

> **Rule 8.9 (Required)**: The format string of input/output functions shall be constant.
>
> **中文**: 输入/输出函数的格式字符串必须是常量。

### 违反示例

```c
/* ❌ 严重违反 - 用户控制的格式字符串 */
void log_user_message(const char *user_msg)
{
    printf(user_msg);  /* 格式字符串攻击！ */
}

/* ❌ 违反 - 拼接格式字符串 */
char fmt[100];
sprintf(fmt, "Value: %%%ds", width);  /* 动态格式 */
printf(fmt, value);
```

### 合规示例

```c
/* ✅ 合规 - 使用常量格式 */
void log_user_message(const char *user_msg)
{
    printf("%s", user_msg);  /* 安全的字符串输出 */
}

/* ✅ 合规 - 使用fprintf的安全版本 */
fprintf(file, "%s", user_msg);

/* ✅ 合规 - 预定义格式 */
#define FMT_INT "%d\n"
#define FMT_STR "%s\n"
#define FMT_HEX "0x%x\n"

printf(FMT_INT, value);
printf(FMT_STR, str);
```

---

## Rule 8.10 - 文件操作安全

### 规则原文

> **Rule 8.10 (Required)**: File operations shall be checked for errors.
>
> **中文**: 文件操作必须检查错误。

### 安全文件操作模式

```c
/* ✅ 合规 - 完整文件操作检查 */
int safe_file_copy(const char *src, const char *dst)
{
    FILE *src_fp = NULL;
    FILE *dst_fp = NULL;
    char buffer[4096];
    size_t bytes_read, bytes_written;
    int result = -1;

    /* 打开源文件 */
    src_fp = fopen(src, "rb");
    if (src_fp == NULL) {
        return -1;
    }

    /* 打开目标文件 */
    dst_fp = fopen(dst, "wb");
    if (dst_fp == NULL) {
        goto cleanup;
    }

    /* 复制内容 */
    while ((bytes_read = fread(buffer, 1, sizeof(buffer), src_fp)) > 0) {
        bytes_written = fwrite(buffer, 1, bytes_read, dst_fp);
        if (bytes_written != bytes_read) {
            goto cleanup;
        }
    }

    /* 检查读取错误 */
    if (ferror(src_fp)) {
        goto cleanup;
    }

    result = 0;

cleanup:
    if (src_fp != NULL) {
        fclose(src_fp);
    }
    if (dst_fp != NULL) {
        fclose(dst_fp);
    }
    return result;
}
```

---

## 检测工具配置

### GCC检查

```bash
# 禁用危险函数
gcc -Wdeprecated-declarations -c file.c

# 格式字符串检查
gcc -Wformat=2 -c file.c
gcc -Wformat-security -c file.c

# 缓冲区溢出检查
gcc -Wbuiltin-declaration-mismatch -c file.c
```

### PC-lint配置

```bash
pc-lint \
    -e142 \  /* 宏参数无括号 */
    -e150 \  /* 宏副作用 */
    -e830 \  /* 头文件无保护 */
    -e1065 \ /* 头文件保护不一致 */
    -e1924 \ /* C++风格头文件保护 */
    file.c
```

---

## 总结

| 规则 | 强制 | 核心要求 |
|:----:|:----:|:---------|
| 8.1 | Required | 宏参数括号化 |
| 8.2 | Required | 头文件保护 |
| 8.3 | Required | 不使用未定义宏 |
| 8.4 | Required | 不使用#和## |
| 8.5 | Required | 条件指令完整 |
| 8.6 | Required | 禁用危险函数 |
| 8.7 | Required | 运行时无malloc |
| 8.8 | Required | 正确使用errno |
| 8.9 | Required | 格式字符串常量 |
| 8.10 | Required | 文件操作检查 |

---

**参考**: MISRA C:2023 Guidelines for the use of the C language in critical systems, Chapter 8

---

> **下一章节**: [Rules 9.1-9.5 - 资源管理与并发](./09_MISRA_C_2023_Rules_9.md)
