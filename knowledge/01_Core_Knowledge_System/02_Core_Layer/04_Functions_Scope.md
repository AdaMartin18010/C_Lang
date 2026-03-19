# 函数与作用域深度解析

> **层级定位**: 01 Core Knowledge System / 02 Core_Layer
> **对应标准**: ISO C §6.2.1, §6.7, §6.9
> **难度级别**: L2-L4
> **预估学习时间**: 6-8小时

---

## 📋 本节概要

| 属性 | 内容 |
|:-----|:-----|
| **核心概念** | 函数定义、参数传递、作用域规则、链接属性、存储期 |
| **前置知识** | 基本语法、数据类型 |
| **后续延伸** | 函数指针、递归、并发编程 |
| **权威来源** | ISO C标准, K&R C |

---

## 📖 1. 函数基础

### 1.1 函数定义与声明

```c
// 完整函数定义
return_type function_name(parameter_list) {
    // 函数体
    return value;
}

// 示例
int max(int a, int b) {
    return (a > b) ? a : b;
}

// 函数声明（原型）
int max(int a, int b);  // 参数名可省略: int max(int, int);

// 旧式函数定义（K&R风格，已废弃）
int old_max(a, b)
    int a;
    int b;
{
    return (a > b) ? a : b;
}
```

### 1.2 参数传递机制

```c
#include <stdio.h>

// 值传递 - 修改不影响原变量
void swap_by_value(int a, int b) {
    int temp = a;
    a = b;
    b = temp;
    printf("Inside swap_by_value: a=%d, b=%d\n", a, b);
}

// 指针传递 - 可修改原变量
void swap_by_pointer(int *a, int *b) {
    int temp = *a;
    *a = *b;
    *b = temp;
    printf("Inside swap_by_pointer: *a=%d, *b=%d\n", *a, *b);
}

// 数组传递 - 退化为指针
void print_array(int arr[], int size) {
    // 这里sizeof(arr) == sizeof(int*), 不是数组大小！
    for (int i = 0; i < size; i++) {
        printf("%d ", arr[i]);
    }
    printf("\n");
}

int main() {
    int x = 10, y = 20;

    swap_by_value(x, y);
    printf("After swap_by_value: x=%d, y=%d\n", x, y);  // 未改变！

    swap_by_pointer(&x, &y);
    printf("After swap_by_pointer: x=%d, y=%d\n", x, y);  // 已交换

    int arr[] = {1, 2, 3, 4, 5};
    print_array(arr, 5);

    return 0;
}
```

---

## 📖 2. 作用域规则

### 2.1 四种作用域

```c
#include <stdio.h>

// 文件作用域（全局）
int global_var = 100;
static int static_global = 200;  // 内部链接

void scope_demo(void) {
    // 块作用域（局部）
    int local_var = 300;

    // 函数原型作用域
    int inner_func(int x);  // x只在原型中可见

    {
        // 嵌套块作用域
        int local_var = 400;  // 遮蔽外层local_var
        printf("Inner local_var: %d\n", local_var);  // 400
    }

    printf("Outer local_var: %d\n", local_var);  // 300
}

// 演示extern
extern int external_var;  // 定义在其他文件

int main() {
    scope_demo();
    return 0;
}
```

### 2.2 标识符遮蔽

```c
#include <stdio.h>

int x = 10;  // 全局x

void shadow_demo(void) {
    int x = 20;  // 遮蔽全局x
    {
        int x = 30;  // 遮蔽外层x
        printf("Innermost x: %d\n", x);  // 30
    }
    printf("Inner x: %d\n", x);  // 20
}

int main() {
    shadow_demo();
    printf("Global x: %d\n", x);  // 10
    return 0;
}
```

---

## 📖 3. 存储期与链接属性

### 3.1 存储期类型

```c
#include <stdio.h>

// 静态存储期 - 程序整个运行期存在
static int static_var = 0;

void storage_demo(void) {
    // 自动存储期 - 函数执行时创建，返回时销毁
    auto int auto_var = 10;  // auto通常省略

    // 静态局部变量 - 函数调用间保持值
    static int static_local = 0;
    static_local++;
    printf("static_local: %d\n", static_local);
}

// 动态存储期 - 由malloc/free管理
void dynamic_demo(void) {
    int *ptr = malloc(sizeof(int) * 10);
    // 使用ptr...
    free(ptr);  // 显式释放
}

int main() {
    storage_demo();  // 输出: 1
    storage_demo();  // 输出: 2
    storage_demo();  // 输出: 3
    return 0;
}
```

### 3.2 链接属性

```c
// file1.c
int external_var = 100;  // 外部链接
default int default_var = 200;  // 同external
static int internal_var = 300;  // 内部链接

// file2.c
extern int external_var;  // 引用file1.c中的变量
// extern int internal_var;  // 错误！internal_var是内部链接
```

---

## 📖 4. 递归与调用栈

### 4.1 递归函数

```c
#include <stdio.h>

// 阶乘递归
unsigned long long factorial(int n) {
    if (n <= 1) return 1;
    return n * factorial(n - 1);
}

// 斐波那契递归（低效）
unsigned long long fib_naive(int n) {
    if (n <= 1) return n;
    return fib_naive(n - 1) + fib_naive(n - 2);
}

// 尾递归优化版本
unsigned long long factorial_tail(int n, unsigned long long acc) {
    if (n <= 1) return acc;
    return factorial_tail(n - 1, n * acc);
}

// 迭代版本（更高效）
unsigned long long factorial_iter(int n) {
    unsigned long long result = 1;
    for (int i = 2; i <= n; i++) {
        result *= i;
    }
    return result;
}

int main() {
    printf("5! = %llu\n", factorial(5));
    printf("10! = %llu\n", factorial_iter(10));
    printf("fib(20) = %llu\n", fib_naive(20));
    return 0;
}
```

### 4.2 调用栈可视化

```
调用 factorial(3):

栈帧3: factorial(3) 等待 3 * factorial(2) 的结果
    ↓ 调用
栈帧2: factorial(2) 等待 2 * factorial(1) 的结果
    ↓ 调用
栈帧1: factorial(1) 返回 1
    ↓ 返回
栈帧2: 计算 2 * 1 = 2, 返回 2
    ↓ 返回
栈帧3: 计算 3 * 2 = 6, 返回 6
```

---

## 📖 5. 可变参数函数

```c
#include <stdio.h>
#include <stdarg.h>

// 计算平均值
double average(int count, ...) {
    va_list args;
    va_start(args, count);

    double sum = 0;
    for (int i = 0; i < count; i++) {
        sum += va_arg(args, double);
    }

    va_end(args);
    return sum / count;
}

// 安全的printf包装
void debug_printf(const char *format, ...) {
    va_list args;
    va_start(args, format);

    printf("[DEBUG] ");
    vprintf(format, args);
    printf("\n");

    va_end(args);
}

int main() {
    printf("Average: %.2f\n", average(4, 10.0, 20.0, 30.0, 40.0));
    debug_printf("Value: %d, String: %s", 42, "hello");
    return 0;
}
```

---

## ⚠️ 常见陷阱

### 陷阱 FUNC01: 返回局部变量指针

```c
// 错误！
int *get_local(void) {
    int local = 42;
    return &local;  // 返回悬空指针！
}

// 正确：使用static或malloc
int *get_static(void) {
    static int static_var = 42;
    return &static_var;  // OK，静态存储期
}

int *get_dynamic(void) {
    int *ptr = malloc(sizeof(int));
    *ptr = 42;
    return ptr;  // OK，但调用者需要free
}
```

### 陷阱 FUNC02: 忽略返回值

```c
// 危险：scanf返回值被忽略
int x;
scanf("%d", &x);  // 如果输入失败，x未初始化！

// 正确做法
if (scanf("%d", &x) != 1) {
    fprintf(stderr, "Input error\n");
    return 1;
}
```

### 陷阱 FUNC03: 递归栈溢出

```c
// 危险：无终止条件的递归
void infinite_recursion(void) {
    infinite_recursion();  // 栈溢出！
}

// 斐波那契的n不能太大
unsigned long long fib(int n) {
    if (n > 90) {  // 限制递归深度
        fprintf(stderr, "n too large\n");
        return 0;
    }
    // ...
}
```

---

## ✅ 质量验收清单

- [x] 函数定义与声明语法
- [x] 参数传递（值/指针/数组）
- [x] 四种作用域详解
- [x] 标识符遮蔽示例
- [x] 三种存储期（自动/静态/动态）
- [x] 链接属性（外部/内部/无）
- [x] 递归与尾递归
- [x] 可变参数函数实现
- [x] 常见陷阱分析

---

> **更新记录**
>
> - 2025-03-09: 创建，补充核心缺失主题


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
