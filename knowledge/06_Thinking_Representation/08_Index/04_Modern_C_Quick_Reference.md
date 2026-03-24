# 现代C语言特性速查表 (C99/C11/C17/C23)

> **定位**: 快速参考 | **用途**: 现代C特性速查 | **更新**: 2026-03-18

---

## C23 新特性速查

### 1. 关键词与语法

| 特性 | C23新特性 | 示例 | 说明 |
|:-----|:----------|:-----|:-----|
| 空指针 | `nullptr` | `int *p = nullptr;` | 取代 NULL, 类型安全 |
| typeof | `typeof` | `typeof(int) x;` | 类型推导 |
| constexpr | `constexpr` | `constexpr int x = 5;` | 编译期常量 |
| 真/假 | `true/false` | `_Bool flag = true;` | 标准布尔字面量 |
| 静态断言 | `static_assert` | `static_assert(N > 0);` | 无需消息 |

### 2. 属性 (Attributes)

```c
// C23 属性语法
[[nodiscard]] int important_func(void);  // 忽略返回值警告
[[maybe_unused]] int x;                   // 可能未使用
[[noreturn]] void abort(void);            // 不返回
[[deprecated("使用 new_func 替代")]] void old_func(void);
```

### 3. 二进制常量与数字分隔符

```c
int flags = 0b1010'1100'0011'0000;  // 二进制 + 分隔符
int million = 1'000'000;             // 数字分隔符
```

---

## C11 关键特性

### 1. 多线程支持

```c
#include <threads.h>

// 线程创建
thrd_t thread;
thrd_create(&thread, thread_func, arg);

// 互斥锁
mtx_t mutex;
mtx_init(&mutex, mtx_plain);
mtx_lock(&mutex);
mtx_unlock(&mutex);

// 线程局部存储
_Thread_local int thread_local_var;
```

### 2. 原子操作

```c
#include <stdatomic.h>

_Atomic int counter = 0;
atomic_fetch_add(&counter, 1);  // 原子加
atomic_load(&counter);          // 原子读
atomic_store(&counter, 10);     // 原子写

// 内存序
atomic_fetch_add_explicit(&counter, 1, memory_order_relaxed);
```

### 3. 匿名结构与联合体

```c
struct {
    int type;
    union {
        struct { int x, y; };      // 匿名结构体
        struct { float r, g, b; }; // 匿名结构体
    };
} color;

color.x = 10;  // 直接访问
```

### 4. 泛型选择

```c
#define abs(x) _Generic((x), \
    int: abs_int, \
    long: abs_long, \
    float: fabsf, \
    double: fabs \
)(x)
```

---

## C99 重要特性

### 1. 变长数组 (VLA)

```c
void func(int n) {
    int arr[n];  // 变长数组 (C11起可选, C23可能移除)
}
```

### 2. 指定初始化

```c
struct Point p = {
    .x = 10,
    .y = 20  // 顺序无关
};

int arr[10] = { [0] = 1, [5] = 5 };  // 指定索引初始化
```

### 3. 复合字面量

```c
struct Point p = (struct Point){ .x = 10, .y = 20 };

// 函数参数传递
func(&(struct Point){ .x = 1, .y = 2 });
```

### 4. 内联函数

```c
inline int max(int a, int b) {
    return a > b ? a : b;
}
```

### 5. long long 类型

```c
long long ll = 123456789012LL;
unsigned long long ull = 123456789012ULL;
```

### 6. // 单行注释

```c
// C99起支持单行注释
int x = 5;  // 行尾注释
```

---

## 跨版本最佳实践

### 1. 类型安全NULL (C23)

```c
// C23前
#define NULL ((void*)0)
int *p = NULL;  // 可能警告

// C23
int *p = nullptr;  // 类型安全, 无警告
```

### 2. 静态断言

```c
// C11前: 编译期检查技巧
typedef char static_assert_failed[CONDITION ? 1 : -1];

// C11起
static_assert(sizeof(int) >= 4, "int太小");

// C23
static_assert(sizeof(int) >= 4);  // 消息可选
```

### 3. 线程局部存储

```c
// C11前: 编译器扩展
__thread int tls_var;  // GCC
__declspec(thread) int tls_var;  // MSVC

// C11起
_Thread_local int tls_var;

// C23简化 (不推荐)
thread_local int tls_var;  // 需要头文件
```

---

## 特性检测宏

```c
// C标准版本检测
#if __STDC_VERSION__ >= 202311L
    // C23
#elif __STDC_VERSION__ >= 201710L
    // C17
#elif __STDC_VERSION__ >= 201112L
    // C11
#elif __STDC_VERSION__ >= 199901L
    // C99
#else
    // C89/C90
#endif

// 特定特性检测
#ifdef __STDC_NO_ATOMICS__
    // 不支持原子操作
#endif

#ifdef __STDC_NO_THREADS__
    // 不支持线程
#endif

#ifdef __STDC_NO_VLA__
    // 不支持VLA (C11起)
#endif
```

---

## 编译器支持状态 (2024)

| 特性 | GCC | Clang | MSVC | 备注 |
|:-----|:---:|:-----:|:----:|:-----|
| C23 nullptr | 13+ | 15+ | 实验 | 广泛支持 |
| C23 typeof | 13+ | 15+ | 实验 | 广泛支持 |
| C11 _Generic | 4.9+ | 3.0+ | 2019+ | 完全支持 |
| C11 <threads.h> | 4.9+ | 3.3+ | 部分 | Windows用winpthread |
| C11 <stdatomic.h> | 4.9+ | 3.3+ | 2019+ | 完全支持 |
| C99 VLA | 是 | 是 | 否 | MSVC不支持 |

---

**返回**: [索引总览](./readme.md) | [思维表征总览](../readme.md) | [现代C特性](../../01_Core_Knowledge_System/07_Modern_C/03_C23_Core_Features.md)


---

## 深入理解

### 核心原理

深入探讨技术原理和实现细节。

### 实践应用

- 应用场景1
- 应用场景2
- 应用场景3

### 最佳实践

1. 理解基础概念
2. 掌握核心机制
3. 应用到实际项目

---

> **最后更新**: 2026-03-21
> **维护者**: AI Code Review
