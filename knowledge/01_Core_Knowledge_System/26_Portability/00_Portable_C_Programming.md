# C 语言可移植性编程指南

> **难度**: L3-L5
> **目标**: 编写能在 Windows、Linux、macOS、嵌入式系统上编译运行的 C 代码
> **核心挑战**: 没有两个C实现完全相同

---

## 1. 可移植性的敌人

### 1.1 平台差异矩阵

| 差异维度 | Linux (GCC/Clang) | Windows (MSVC) | macOS (Clang) | 嵌入式 (ARM GCC) |
|:---------|:------------------|:---------------|:--------------|:-----------------|
| `long` 大小 | 64-bit (LP64) | 32-bit (LLP64) | 64-bit (LP64) | 32-bit (ILP32) |
| `size_t` | `unsigned long` | `unsigned long long` | `unsigned long` | `unsigned int` |
| 字节序 | 小端 | 小端 | 小端 | 通常小端 |
| `inline` 语义 | C99 | C99 + 扩展 | C99 | C99 |
| 对齐支持 | `_Alignas` (C11) | `__declspec(align)` | `_Alignas` | `_Alignas` |
| 线程 | pthread | Win32 Thread | pthread | FreeRTOS/裸机 |

### 1.2 整数类型陷阱

```c
// ❌ 不可移植：假设 long 是 64 位
long big = 1L << 40;  // Windows 上溢出！

// ✅ 使用固定宽度类型
#include <stdint.h>
uint64_t big = 1ULL << 40;   // 总是 64 位
int32_t  exact = 42;          // 总是 32 位

// ✅ 使用 size_t 表示大小
size_t len = strlen(s);       // 平台无关
for (size_t i = 0; i < len; i++) { }  // 正确

// ❌ int 与 size_t 比较警告
for (int i = 0; i < strlen(s); i++) { }  // 有符号/无符号比较
```

---

## 2. 条件编译与平台抽象

### 2.1 平台检测宏

```c
// 操作系统检测
#if defined(_WIN32) || defined(_WIN64)
    #define PLATFORM_WINDOWS
#elif defined(__APPLE__) && defined(__MACH__)
    #define PLATFORM_MACOS
#elif defined(__linux__)
    #define PLATFORM_LINUX
#elif defined(__FreeBSD__)
    #define PLATFORM_FREEBSD
#endif

// 编译器检测
#if defined(__GNUC__)
    #define COMPILER_GCC
    #define COMPILER_VERSION (__GNUC__ * 100 + __GNUC_MINOR__)
#elif defined(__clang__)
    #define COMPILER_CLANG
#elif defined(_MSC_VER)
    #define COMPILER_MSVC
#endif

// 架构检测
#if defined(__x86_64__) || defined(_M_X64)
    #define ARCH_X86_64
#elif defined(__aarch64__) || defined(_M_ARM64)
    #define ARCH_ARM64
#elif defined(__arm__) || defined(_M_ARM)
    #define ARCH_ARM32
#endif
```

### 2.2 抽象层设计

```c
// os_mutex.h — 跨平台互斥锁抽象
#ifndef OS_MUTEX_H
#define OS_MUTEX_H

#ifdef PLATFORM_WINDOWS
    #include <windows.h>
    typedef CRITICAL_SECTION os_mutex_t;
#else
    #include <pthread.h>
    typedef pthread_mutex_t os_mutex_t;
#endif

int  os_mutex_init(os_mutex_t *m);
void os_mutex_lock(os_mutex_t *m);
void os_mutex_unlock(os_mutex_t *m);
void os_mutex_destroy(os_mutex_t *m);

#endif
```

```c
// os_mutex.c
#include "os_mutex.h"

#ifdef PLATFORM_WINDOWS
int os_mutex_init(os_mutex_t *m) {
    InitializeCriticalSection(m);
    return 0;
}
void os_mutex_lock(os_mutex_t *m) { EnterCriticalSection(m); }
void os_mutex_unlock(os_mutex_t *m) { LeaveCriticalSection(m); }
void os_mutex_destroy(os_mutex_t *m) { DeleteCriticalSection(m); }
#else
int os_mutex_init(os_mutex_t *m) {
    return pthread_mutex_init(m, NULL);
}
void os_mutex_lock(os_mutex_t *m) { pthread_mutex_lock(m); }
void os_mutex_unlock(os_mutex_t *m) { pthread_mutex_unlock(m); }
void os_mutex_destroy(os_mutex_t *m) { pthread_mutex_destroy(m); }
#endif
```

---

## 3. 标准合规性策略

### 3.1 C 标准功能检测

```c
// 检测 C11 支持
#if defined(__STDC_VERSION__) && (__STDC_VERSION__ >= 201112L)
    #define HAS_C11 1
    #include <stdalign.h>
    #include <stdnoreturn.h>
#else
    #define HAS_C11 0
    #define alignas(x) __attribute__((aligned(x)))  // GCC/Clang fallback
#endif

// 检测 C23 支持
#if defined(__STDC_VERSION__) && (__STDC_VERSION__ >= 202311L)
    #define HAS_C23 1
#else
    #define HAS_C23 0
#endif

// 安全使用新特性
#if HAS_C11
    #define STATIC_ASSERT _Static_assert
#else
    #define STATIC_ASSERT(cond, msg) \
        typedef char _static_assert_##__LINE__[(cond) ? 1 : -1]
#endif
```

### 3.2 POSIX vs Windows API

```c
// 路径分隔符
#ifdef PLATFORM_WINDOWS
    #define PATH_SEP '\\'
    #define PATH_SEP_STR "\\"
#else
    #define PATH_SEP '/'
    #define PATH_SEP_STR "/"
#endif

// 换行符
#ifdef PLATFORM_WINDOWS
    #define LINE_END "\r\n"
#else
    #define LINE_END "\n"
#endif

// 动态库加载
#ifdef PLATFORM_WINDOWS
    #include <windows.h>
    #define DLL_HANDLE HMODULE
    #define DLL_LOAD(name) LoadLibraryA(name)
    #define DLL_GETSYM(handle, name) GetProcAddress(handle, name)
    #define DLL_CLOSE(handle) FreeLibrary(handle)
#else
    #include <dlfcn.h>
    #define DLL_HANDLE void*
    #define DLL_LOAD(name) dlopen(name, RTLD_NOW)
    #define DLL_GETSYM(handle, name) dlsym(handle, name)
    #define DLL_CLOSE(handle) dlclose(handle)
#endif
```

---

## 4. 数据对齐与填充

### 4.1 结构体可移植布局

```c
// ❌ 不可移植：依赖编译器填充
typedef struct {
    char type;
    int length;   // 3字节填充（因对齐）
    char data[];
} Packet;

// ✅ 可移植：显式打包（网络协议、文件格式）
typedef struct __attribute__((packed)) {
    uint8_t  type;
    uint32_t length;   // 小端，4字节
} PacketHeader;

// 或 C11 标准方式
#include <stdalign.h>
struct alignas(64) CacheLine {
    int data[16];  // 64字节对齐，避免伪共享
};
```

### 4.2 编译器属性抽象

```c
// 编译器扩展抽象层
#ifdef COMPILER_GCC
    #define ATTR_PACKED __attribute__((packed))
    #define ATTR_UNUSED __attribute__((unused))
    #define ATTR_NONNULL(...) __attribute__((nonnull(__VA_ARGS__)))
    #define LIKELY(x) __builtin_expect(!!(x), 1)
    #define UNLIKELY(x) __builtin_expect(!!(x), 0)
#elif defined(COMPILER_MSVC)
    #define ATTR_PACKED
    #define ATTR_UNUSED
    #define ATTR_NONNULL(...)
    #define LIKELY(x) (x)
    #define UNLIKELY(x) (x)
#else
    #define ATTR_PACKED
    #define ATTR_UNUSED
    #define ATTR_NONNULL(...)
    #define LIKELY(x) (x)
    #define UNLIKELY(x) (x)
#endif
```

---

## 5. 检查清单

- [ ] 使用 `<stdint.h>` 固定宽度类型，不用 `short`/`int`/`long`
- [ ] 使用 `size_t`/`ptrdiff_t` 表示大小和指针差
- [ ] 所有平台相关代码放在抽象层后面
- [ ] 用条件编译隔离平台差异
- [ ] 结构体用于网络/文件时显式打包
- [ ] 在目标平台上测试（CI 多平台矩阵）
- [ ] 避免未定义行为（即使是"能工作"的）

---

> **最后更新**: 2026-05-11
> **参考**: GNU Autoconf, CMake cross-compilation, SQLite portability layer
