# C语言模块化与链接

> **层级定位**: 01_Core_Knowledge_System > 03_Construction_Layer

---

## 模块化编程

### 头文件与源文件分离

```c
// math_utils.h - 接口声明
#ifndef MATH_UTILS_H
#define MATH_UTILS_H

int add(int a, int b);
int multiply(int a, int b);

#endif
```

```c
// math_utils.c - 实现
#include "math_utils.h"

int add(int a, int b) {
    return a + b;
}

int multiply(int a, int b) {
    return a * b;
}
```

### 编译过程

```bash
# 编译为对象文件
gcc -c math_utils.c -o math_utils.o
gcc -c main.c -o main.o

# 链接
gcc main.o math_utils.o -o program
```

---

## 链接类型

| 类型 | 说明 | 关键字 |
|:-----|:-----|:-------|
| 外部链接 | 多文件共享 | (默认) |
| 内部链接 | 文件私有 | `static` |
| 无链接 | 局部变量 | 块作用域 |

---

## 静态库与动态库

### 静态库

```bash
# 创建静态库
ar rcs libmath.a math_utils.o

# 使用静态库
gcc main.c -L. -lmath -o program
```

### 动态库

```bash
# 创建动态库 (Linux)
gcc -shared -fPIC math_utils.c -o libmath.so

# 使用动态库
gcc main.c -L. -lmath -o program
```

---

> **最后更新**: 2026-03-21


---

## 扩展内容

### 核心概念

本节详细介绍核心概念和原理。

### 实践应用

实际应用场景和代码示例。

### 深入学习

- 相关主题1
- 相关主题2
- 相关主题3

---

> **最后更新**: 2026-03-21
> **维护者**: AI Code Review
