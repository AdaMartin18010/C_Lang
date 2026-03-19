# C预处理器深度解析

> **层级定位**: 01_Core_Knowledge_System > 03_Construction_Layer  
> **难度级别**: L3-L4  
> **前置知识**: C语言基础

---

## 预处理器概述

预处理器在编译之前处理源代码，执行文本替换和条件编译。

### 处理阶段

```
源代码 → 预处理器 → 编译器 → 汇编器 → 链接器
         (宏替换、文件包含、条件编译)
```

---

## 宏定义

### 对象宏

```c
#define PI 3.14159
#define BUFFER_SIZE 1024
#define MAX(a, b) ((a) > (b) ? (a) : (b))
```

### 函数宏

```c
// 带参数的宏
#define SQUARE(x) ((x) * (x))

// 多行宏
#define DEBUG_PRINT(fmt, ...) \
    printf("[%s:%d] " fmt "\n", __FILE__, __LINE__, ##__VA_ARGS__)

// 使用示例
int result = SQUARE(5);  // 展开为 ((5) * (5))
```

### 宏的陷阱

```c
// 陷阱1: 缺少括号
#define BAD_SQUARE(x) x * x
BAD_SQUARE(3 + 2);  // 展开为 3 + 2 * 3 + 2 = 11 (错误!)

// 陷阱2: 多次求值
#define MAX_BAD(a, b) ((a) > (b) ? (a) : (b))
int x = 5;
MAX_BAD(x++, 3);  // x被增加两次!
```

---

## 条件编译

```c
// 头文件保护
#ifndef HEADER_H
#define HEADER_H

// 头文件内容

#endif /* HEADER_H */

// 平台检测
#ifdef _WIN32
    #include <windows.h>
#elif defined(__linux__)
    #include <unistd.h>
#elif defined(__APPLE__)
    #include <mach/mach.h>
#endif

// 调试开关
#ifdef DEBUG
    #define LOG(msg) printf("Debug: %s\n", msg)
#else
    #define LOG(msg)
#endif
```

---

## 预定义宏

| 宏 | 说明 |
|:---|:-----|
| `__FILE__` | 当前文件名 |
| `__LINE__` | 当前行号 |
| `__func__` | 当前函数名 |
| `__DATE__` | 编译日期 |
| `__TIME__` | 编译时间 |
| `__STDC__` | 标准C标志 |
| `__STDC_VERSION__` | C标准版本 |

---

## 实用技巧

### 字符串化

```c
#define STRINGIFY(x) #x
#define TOSTRING(x) STRINGIFY(x)

const char* version = TOSTRING(VERSION);  // 展开为 "1.0"
```

### 连接操作

```c
#define CONCAT(a, b) a ## b
int xy = 10;
int result = CONCAT(x, y);  // 展开为 xy
```

---

> **最后更新**: 2026-03-21
