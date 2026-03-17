# 现代 C 编程


---

## 📑 目录

- [现代 C 编程](#现代-c-编程)
  - [📑 目录](#-目录)
  - [概述](#概述)
  - [C 语言标准演进](#c-语言标准演进)
    - [标准版本对比](#标准版本对比)
    - [标准特性检测](#标准特性检测)
  - [C11/C17 核心特性](#c11c17-核心特性)
    - [泛型选择 (\_Generic)](#泛型选择-_generic)
    - [原子操作与内存序](#原子操作与内存序)
    - [线程支持](#线程支持)
    - [匿名结构体与联合体](#匿名结构体与联合体)
  - [C23 现代特性](#c23-现代特性)
    - [属性语法 (Attributes)](#属性语法-attributes)
    - [现代初始化与字面量](#现代初始化与字面量)
    - [静态断言与诊断](#静态断言与诊断)
  - [安全编码实践](#安全编码实践)
    - [边界检查接口 (C11 Annex K - 可选)](#边界检查接口-c11-annex-k---可选)
    - [防御性编程模式](#防御性编程模式)
    - [内存安全模式](#内存安全模式)
  - [现代工具链](#现代工具链)
    - [编译器警告与静态分析](#编译器警告与静态分析)
    - [现代构建配置](#现代构建配置)
    - [现代调试技术](#现代调试技术)
  - [示例：现代 C 项目结构](#示例现代-c-项目结构)
  - [延伸阅读](#延伸阅读)


---

## 概述

现代 C 编程代表了 C 语言标准的演进成果，融合了 C99、C11、C17 和 C23 标准的先进特性。现代 C 强调类型安全、并发支持、可移植性和安全编码实践，使 C 语言在系统编程领域保持强大竞争力。

---

## C 语言标准演进

### 标准版本对比

```
┌─────────┬──────────┬─────────────────────────────────────────────┐
│ 标准     │ 年份     │ 核心特性                                     │
├─────────┼──────────┼─────────────────────────────────────────────┤
│ C89/C90 │ 1989/90  │ 基础语言，ANSI 标准化                        │
│ C99     │ 1999     │ 变长数组、内联函数、<stdbool.h>、复数        │
│ C11     │ 2011     │ 泛型选择、原子操作、线程支持、匿名结构体     │
│ C17     │ 2017/18  │ 缺陷修复，C11 稳定版                         │
│ C23     │ 2023     │ 现代属性、改进初始化、constexpr、auto        │
└─────────┴──────────┴─────────────────────────────────────────────┘
```

### 标准特性检测

```c
// 检测编译器支持的 C 标准
#if __STDC_VERSION__ >= 202311L
    #define C_STANDARD 23
    #pragma message("Using C23 standard")
#elif __STDC_VERSION__ >= 201710L
    #define C_STANDARD 17
    #pragma message("Using C17 standard")
#elif __STDC_VERSION__ >= 201112L
    #define C_STANDARD 11
    #pragma message("Using C11 standard")
#elif __STDC_VERSION__ >= 199901L
    #define C_STANDARD 99
    #pragma message("Using C99 standard")
#else
    #define C_STANDARD 90
    #pragma message("Using C89/C90 standard")
#endif

// 特性检测宏
#ifdef __STDC_NO_ATOMICS__
    #error "Atomic operations not supported"
#endif

#ifdef __STDC_NO_THREADS__
    #warning "Threads not supported, using fallback"
#endif

#ifdef __STDC_NO_VLA__
    #warning "Variable length arrays not supported"
#endif
```

---

## C11/C17 核心特性

### 泛型选择 (_Generic)

```c
#include <stdio.h>
#include <math.h>

// 类型泛化数学函数
#define abs(x) _Generic((x), \
    int: abs_int, \
    long: labs, \
    long long: llabs, \
    float: fabsf, \
    double: fabs, \
    long double: fabsl \
)(x)

int abs_int(int x) { return x < 0 ? -x : x; }

// 泛型打印函数
#define print_value(x) _Generic((x), \
    int: print_int, \
    double: print_double, \
    char*: print_string, \
    const char*: print_string, \
    default: print_unknown \
)(x)

void print_int(int x) { printf("int: %d\n", x); }
void print_double(double x) { printf("double: %f\n", x); }
void print_string(char *s) { printf("string: %s\n", s); }
void print_unknown(void *x) { printf("unknown: %p\n", (void*)x); }

// 使用示例
void demonstrate_generic(void)
{
    print_value(42);           // int
    print_value(3.14);         // double
    print_value("hello");      // string
}
```

### 原子操作与内存序

```c
#include <stdatomic.h>
#include <stdbool.h>
#include <stdio.h>

// 原子类型定义
_Atomic int atomic_counter = 0;
_Atomic(bool) flag = false;
atomic_int shared_value = 0;  // 标准原子类型

// 无锁计数器
void atomic_increment(void)
{
    // 原子 fetch-add，返回旧值
    int old = atomic_fetch_add(&atomic_counter, 1);
    printf("Old value: %d\n", old);
}

// CAS (Compare-And-Swap) 实现
bool atomic_compare_and_swap(atomic_int *ptr, int expected, int desired)
{
    return atomic_compare_exchange_strong(ptr, &expected, desired);
}

// 内存序控制
void memory_order_demo(void)
{
    atomic_int x = 0;
    atomic_int y = 0;

    // memory_order_relaxed: 最宽松，仅保证原子性
    atomic_store_explicit(&x, 1, memory_order_relaxed);

    // memory_order_release: 释放语义，之前的写入对其他线程可见
    atomic_store_explicit(&y, 1, memory_order_release);

    // memory_order_acquire: 获取语义，看到其他线程的释放写入
    int val = atomic_load_explicit(&y, memory_order_acquire);

    // memory_order_seq_cst: 顺序一致性（默认）
    atomic_fetch_add(&x, 1);  // 默认使用 seq_cst
}

// 无锁队列节点（简化）
struct Node {
    void *data;
    _Atomic(struct Node *) next;
};
```

### 线程支持

```c
#include <threads.h>
#include <stdio.h>
#include <stdlib.h>

// 线程本地存储
thread_local int thread_id = 0;

// 线程函数
int worker_thread(void *arg)
{
    int id = *(int *)arg;
    thread_id = id;

    printf("Thread %d starting (local id: %d)\n", id, thread_id);

    // 模拟工作
    thrd_sleep(&(struct timespec){.tv_sec = 1}, NULL);

    printf("Thread %d finished\n", id);
    return id * 10;  // 返回值
}

// 线程池简化示例
#define THREAD_COUNT 4

void demonstrate_threads(void)
{
    thrd_t threads[THREAD_COUNT];
    int thread_args[THREAD_COUNT];

    // 创建线程
    for (int i = 0; i < THREAD_COUNT; i++) {
        thread_args[i] = i;
        if (thrd_create(&threads[i], worker_thread, &thread_args[i])
            != thrd_success) {
            fprintf(stderr, "Failed to create thread %d\n", i);
            exit(EXIT_FAILURE);
        }
    }

    // 等待线程完成
    for (int i = 0; i < THREAD_COUNT; i++) {
        int result;
        thrd_join(threads[i], &result);
        printf("Thread %d returned: %d\n", i, result);
    }
}

// 互斥锁与条件变量
mtx_t mutex;
cnd_t condition;
int shared_data = 0;

int producer_thread(void *arg)
{
    (void)arg;

    mtx_lock(&mutex);
    shared_data = 42;
    printf("Producer: data ready\n");
    cnd_signal(&condition);  // 通知消费者
    mtx_unlock(&mutex);

    return 0;
}

int consumer_thread(void *arg)
{
    (void)arg;

    mtx_lock(&mutex);
    while (shared_data == 0) {
        cnd_wait(&condition, &mutex);  // 等待通知
    }
    printf("Consumer: got data %d\n", shared_data);
    mtx_unlock(&mutex);

    return 0;
}
```

### 匿名结构体与联合体

```c
// 匿名结构体成员 (C11)
struct Vector3D {
    union {
        struct { double x, y, z; };  // 匿名结构体
        double coords[3];
    };
};

void vector_demo(void)
{
    struct Vector3D v = { .x = 1.0, .y = 2.0, .z = 3.0 };

    // 两种访问方式
    printf("x = %f\n", v.x);           // 通过命名成员
    printf("x = %f\n", v.coords[0]);   // 通过数组
}

// 标签联合体（类型安全）
enum ShapeType { SHAPE_CIRCLE, SHAPE_RECTANGLE, SHAPE_TRIANGLE };

struct Shape {
    enum ShapeType type;
    union {
        struct { double radius; } circle;
        struct { double width, height; } rectangle;
        struct { double a, b, c; } triangle;
    } data;
};

double shape_area(const struct Shape *s)
{
    switch (s->type) {
        case SHAPE_CIRCLE:
            return 3.14159 * s->data.circle.radius * s->data.circle.radius;
        case SHAPE_RECTANGLE:
            return s->data.rectangle.width * s->data.rectangle.height;
        default:
            return 0.0;
    }
}
```

---

## C23 现代特性

### 属性语法 (Attributes)

```c
// C23 标准属性语法
[[nodiscard]] int important_function(void);
[[deprecated("Use new_function instead")]] void old_function(void);
[[maybe_unused]] int unused_var = 0;
[[noreturn]] void abort_program(void);

// 属性与函数结合
[[nodiscard, gnu::pure]] int calculate(int x);

// 属性在声明中的应用
struct [[packed]] PacketHeader {
    uint16_t magic;
    uint32_t length;
};

// 分支预测提示
[[likely]] if (condition) {
    // 预期经常执行
}

[[unlikely]] if (error_condition) {
    // 预期很少执行
}

// 回退到 GCC/Clang 扩展
#ifndef __STDC_VERSION__
    #define FALLTHROUGH __attribute__((fallthrough))
    #define NODISCARD   __attribute__((warn_unused_result))
#else
    #define FALLTHROUGH [[fallthrough]]
    #define NODISCARD   [[nodiscard]]
#endif
```

### 现代初始化与字面量

```c
// 指定初始化器增强
struct Config {
    int timeout;
    int retries;
    const char *name;
};

struct Config cfg = {
    .timeout = 30,
    .retries = 3,
    .name = "default",
};

// 数组指定初始化 (C23 增强)
int array[10] = {
    [0] = 1,
    [5 ... 8] = 42,  // 范围初始化
    [9] = 99
};

// typeof 运算符 (C23)
typeof(int) x = 10;               // x 是 int
typeof(x) y = 20;                 // y 与 x 同类型
typeof(int[5]) arr;               // arr 是 int[5]

// auto 类型推导 (C23)
// auto i = 42;         // i 推导为 int
// auto s = "hello";    // s 推导为 char*
// auto f = 3.14f;      // f 推导为 float

// constexpr (C23)
// constexpr int SIZE = 100;
// constexpr int factorial(int n) {
//     return n <= 1 ? 1 : n * factorial(n - 1);
// }
```

### 静态断言与诊断

```c
// 编译时断言
static_assert(sizeof(int) >= 4, "int must be at least 32 bits");
static_assert(alignof(double) == 8, "double alignment check");

// C23 静态断言消息可选
static_assert(sizeof(void*) == 8);  // 64 位系统检查

// 预处理诊断
#if __STDC_VERSION__ < 201112L
    #error "C11 or later required"
#endif

// 编译器警告控制 (GCC/Clang)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-variable"
    int unused = 42;
#pragma GCC diagnostic pop
```

---

## 安全编码实践

### 边界检查接口 (C11 Annex K - 可选)

```c
#ifdef __STDC_LIB_EXT1__
#define __STDC_WANT_LIB_EXT1__ 1
#include <string.h>

// 安全字符串操作
errno_t safe_strcpy(char *dest, rsize_t destsz, const char *src)
{
    return strcpy_s(dest, destsz, src);
}

errno_t safe_strcat(char *dest, rsize_t destsz, const char *src)
{
    return strcat_s(dest, destsz, src);
}

errno_t safe_gets(char *str, rsize_t n)
{
    return gets_s(str, n);  // 安全版本的 gets
}
#endif

// 可移植的安全包装
#ifndef __STDC_LIB_EXT1__
    #include <string.h>
    #include <stdlib.h>

    typedef int errno_t;
    typedef size_t rsize_t;

    errno_t memcpy_s(void *dest, rsize_t destsz, const void *src, rsize_t count)
    {
        if (!dest || !src) return -1;
        if (destsz < count) return -1;
        memcpy(dest, src, count);
        return 0;
    }
#endif
```

### 防御性编程模式

```c
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

// 空指针检查宏
#define CHECK_NULL(ptr, ret) do { \
    if ((ptr) == NULL) { \
        fprintf(stderr, "Error: NULL pointer at %s:%d\n", __FILE__, __LINE__); \
        return (ret); \
    } \
} while(0)

// 安全检查包装
#define SAFE_DIVIDE(a, b) ({ \
    typeof(b) _b = (b); \
    (_b == 0) ? 0 : (a) / _b; \
})

// 整数溢出检查
bool safe_add_int(int a, int b, int *result)
{
    if (b > 0 && a > INT_MAX - b) return false;  // 正溢出
    if (b < 0 && a < INT_MIN - b) return false;  // 负溢出
    *result = a + b;
    return true;
}

bool safe_mul_int(int a, int b, int *result)
{
    if (a > 0) {
        if (b > 0) {
            if (a > INT_MAX / b) return false;
        } else {
            if (b < INT_MIN / a) return false;
        }
    } else {
        if (b > 0) {
            if (a < INT_MIN / b) return false;
        } else {
            if (a != 0 && b < INT_MAX / a) return false;
        }
    }
    *result = a * b;
    return true;
}

// 数组边界访问
#define ARRAY_SIZE(arr) (sizeof(arr) / sizeof((arr)[0]))

#define SAFE_ARRAY_ACCESS(arr, idx, default_val) \
    (((idx) < ARRAY_SIZE(arr)) ? (arr)[(idx)] : (default_val))

// 灵活的数组访问函数
int *safe_array_get(int *arr, size_t size, size_t index, bool *success)
{
    if (index >= size) {
        if (success) *success = false;
        return NULL;
    }
    if (success) *success = true;
    return &arr[index];
}
```

### 内存安全模式

```c
#include <stdlib.h>
#include <string.h>

// 安全的内存分配
#define SAFE_MALLOC(type, count) ({ \
    size_t _size = sizeof(type) * (count); \
    type *_ptr = malloc(_size); \
    if (_ptr) memset(_ptr, 0, _size); \
    _ptr; \
})

#define SAFE_CALLOC(nmemb, size) calloc((nmemb), (size))

// 带清零的释放
#define SAFE_FREE(ptr) do { \
    if (ptr) { \
        free(ptr); \
        (ptr) = NULL; \
    } \
} while(0)

// 智能指针模拟（简化）
struct smart_ptr {
    void *ptr;
    size_t size;
    void (*destructor)(void *);
};

void smart_ptr_init(struct smart_ptr *sp, size_t size)
{
    sp->ptr = calloc(1, size);
    sp->size = size;
    sp->destructor = free;
}

void smart_ptr_release(struct smart_ptr *sp)
{
    if (sp && sp->ptr && sp->destructor) {
        sp->destructor(sp->ptr);
        sp->ptr = NULL;
        sp->size = 0;
    }
}
```

---

## 现代工具链

### 编译器警告与静态分析

```bash
# GCC/Clang 推荐警告选项
CFLAGS="-std=c23 -Wall -Wextra -Wpedantic \
        -Werror=implicit-function-declaration \
        -Werror=return-type \
        -Wshadow \
        -Wstrict-prototypes \
        -Wmissing-prototypes \
        -Wcast-align \
        -Wconversion \
        -Wsign-conversion \
        -Wnull-dereference \
        -Wdouble-promotion \
        -Wformat=2 \
        -Wundef \
        -Wredundant-decls \
        -Wwrite-strings \
        -Wfloat-equal \
        -Wswitch-enum \
        -Wvla \
        -fstack-protector-strong \
        -fPIE \
        -D_FORTIFY_SOURCE=2 \
        -fsanitize=undefined,address"

# 静态分析工具
# Clang Static Analyzer
scan-build gcc -c program.c

# Cppcheck
cppcheck --enable=all --std=c23 --inconclusive src/

# Infer (Facebook)
infer run -- gcc -c program.c
```

### 现代构建配置

```cmake
# CMakeLists.txt 示例
cmake_minimum_required(VERSION 3.21)
project(ModernCProject VERSION 1.0.0 LANGUAGES C)

# C23 标准
set(CMAKE_C_STANDARD 23)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

# 编译选项
add_compile_options(
    -Wall -Wextra -Wpedantic
    -Werror=return-type
    -Wshadow
    -fstack-protector-strong
    $<$<CONFIG:Debug>:-fsanitize=address,undefined>
    $<$<CONFIG:Debug>:-O0 -g3>
    $<$<CONFIG:Release>:-O3 -DNDEBUG>
)

# 链接选项
add_link_options(
    $<$<CONFIG:Debug>:-fsanitize=address,undefined>
)

# 目标定义
add_executable(${PROJECT_NAME} src/main.c)
target_include_directories(${PROJECT_NAME} PRIVATE include)

# 启用测试
enable_testing()
add_subdirectory(tests)
```

### 现代调试技术

```c
// 断言与调试宏
#include <assert.h>

#ifdef NDEBUG
    #define DEBUG_PRINT(fmt, ...) ((void)0)
#else
    #define DEBUG_PRINT(fmt, ...) \
        fprintf(stderr, "[DEBUG %s:%d] " fmt "\n", \
                __FILE__, __LINE__, ##__VA_ARGS__)
#endif

// 编译时断言
#define STATIC_ASSERT(cond, msg) _Static_assert(cond, msg)

// 运行时检查宏
#define RUNTIME_ASSERT(cond) do { \
    if (!(cond)) { \
        fprintf(stderr, "Assertion failed: %s at %s:%d\n", \
                #cond, __FILE__, __LINE__); \
        abort(); \
    } \
} while(0)

// 可达性标记 (C23)
// [[unreachable]] 标记不可达代码路径
```

---

## 示例：现代 C 项目结构

```
modern_c_project/
├── include/
│   ├── project/
│   │   ├── core.h          # 核心模块接口
│   │   ├── utils.h         # 工具函数
│   │   └── config.h        # 配置（版本检测）
│   └── project.h           # 主头文件
├── src/
│   ├── core.c              # 核心实现
│   ├── utils.c             # 工具实现
│   └── main.c              # 入口
├── tests/
│   ├── test_core.c
│   └── test_utils.c
├── CMakeLists.txt
└── .clang-format           # 代码格式化配置
```

```c
// include/project/config.h
#ifndef PROJECT_CONFIG_H
#define PROJECT_CONFIG_H

#if __STDC_VERSION__ < 201112L
    #error "C11 or later is required"
#endif

#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>

#if __STDC_VERSION__ >= 202311L
    #define NODISCARD [[nodiscard]]
    #define MAYBE_UNUSED [[maybe_unused]]
#else
    #define NODISCARD __attribute__((warn_unused_result))
    #define MAYBE_UNUSED __attribute__((unused))
#endif

#endif /* PROJECT_CONFIG_H */
```

---

## 延伸阅读

- [C11 标准文档 (ISO/IEC 9899:2011)](https://www.iso.org/standard/57853.html)
- [C23 草案 (n3096)](https://www.open-std.org/jtc1/sc22/wg14/www/docs/n3096.pdf)
- [SEI CERT C 编码标准](https://wiki.sei.cmu.edu/confluence/display/c)
- [C 安全编码规则 (ISO/IEC TS 17961)](https://www.iso.org/standard/64843.html)
