# C/C++ 互操作深度指南

> **标准**: ISO/IEC 14882:2024 (C++26), ISO/IEC 9899:2024 (C23), Itanium C++ ABI
> **前置知识**: [C接口基础](README.md), [ABI形式化](../../02_Formal_Semantics_and_Physics/06_C_Assembly_Mapping/README.md)
> **后续延伸**: [Zig-C连接](../../08_Zig_C_Connection/README.md), [Rust互操作](../../03_System_Technology_Domains/32_Rust_Interoperability/README.md)
> **对标资源**: Itanium C++ ABI, Microsoft C++ ABI, cppreference.com (extern "C")
> **难度**: 4/5 | **预计学习时间**: 3-4 小时

---

## 目录

- [C/C++ 互操作深度指南](#cc-互操作深度指南)
  - [目录](#目录)
  - [一、为什么需要C/C++互操作](#一为什么需要cc互操作)
    - [1.1 典型场景](#11-典型场景)
    - [1.2 核心挑战](#12-核心挑战)
  - [二、extern "C" 的完整语义](#二extern-c-的完整语义)
    - [2.1 基本用法](#21-基本用法)
    - [2.2 条件编译模板](#22-条件编译模板)
    - [2.3 C调用C++的封装模式](#23-c调用c的封装模式)
  - [三、名称修饰（Name Mangling）](#三名称修饰name-mangling)
    - [3.1 名称修饰对比](#31-名称修饰对比)
    - [3.2 查看名称修饰的工具](#32-查看名称修饰的工具)
  - [四、ABI边界：类型与内存布局](#四abi边界类型与内存布局)
    - [4.1 可安全穿越边界的类型](#41-可安全穿越边界的类型)
    - [4.2 不可穿越边界的类型](#42-不可穿越边界的类型)
    - [4.3 不透明指针模式（Opaque Pointer / PIMPL）](#43-不透明指针模式opaque-pointer--pimpl)
  - [五、异常处理边界](#五异常处理边界)
    - [5.1 核心规则](#51-核心规则)
    - [5.2 异常传播到C++的策略](#52-异常传播到c的策略)
    - [5.3 信号与异常的交互](#53-信号与异常的交互)
  - [六、C++特性穿越ABI](#六c特性穿越abi)
    - [6.1 RAII到C的映射](#61-raii到c的映射)
    - [6.2 虚函数到C的映射](#62-虚函数到c的映射)
  - [七、工程实践：混合项目构建](#七工程实践混合项目构建)
    - [7.1 CMake混合项目](#71-cmake混合项目)
    - [7.2 链接顺序](#72-链接顺序)
    - [7.3 跨平台ABI兼容](#73-跨平台abi兼容)
  - [八、常见陷阱](#八常见陷阱)
    - [8.1 陷阱1：混合new/malloc和delete/free](#81-陷阱1混合newmalloc和deletefree)
    - [8.2 陷阱2：结构体填充差异](#82-陷阱2结构体填充差异)
    - [8.3 陷阱3：布尔类型差异](#83-陷阱3布尔类型差异)
    - [8.4 陷阱4：线程局部存储](#84-陷阱4线程局部存储)
  - [九、参考资源](#九参考资源)
    - [9.1 标准与文档](#91-标准与文档)
    - [9.2 工具](#92-工具)

---

## 一、为什么需要C/C++互操作

### 1.1 典型场景

| 场景 | 说明 | 复杂度 |
|:-----|:-----|:------:|
| **C++调用C库** | 使用C标准库、POSIX、OpenSSL等 | ⭐ |
| **C调用C++库** | 将C++库封装为C接口供C项目使用 | ⭐⭐ |
| **混合编译** | 同一项目同时包含.c和.cpp文件 | ⭐⭐ |
| **动态库边界** | .so/.dll的ABI稳定接口 | ⭐⭐⭐ |
| **C++到C的迁移** | 逐步将C++代码迁移到C | ⭐⭐⭐ |
| **插件系统** | C++应用加载C插件，或反之 | ⭐⭐⭐ |

### 1.2 核心挑战

C和C++虽然共享底层语义，但在**链接层面**是完全不同的语言：

```
C编译器                    C++编译器
  │                          │
  ▼                          ▼
foo.c ──► foo.o            foo.cpp ──► foo.o
  │                          │
  │  符号: _foo              │  符号: _Z3foov (g++) / ?foo@@YAXXZ (MSVC)
  │                          │
  └──► 链接器无法匹配不同名称的符号 ◄──┘
```

**四大核心差异**：

1. **名称修饰**：C++编译器修饰函数名以支持重载、命名空间、模板
2. **异常传播**：C++异常无法穿越C代码边界
3. **RTTI与虚表**：C++对象布局包含vptr，C无法理解
4. **构造/析构**：C++的RAII语义需要显式管理

---

## 二、extern "C" 的完整语义

### 2.1 基本用法

```cpp
// C++头文件：声明C接口
#ifdef __cplusplus
extern "C" {
#endif

// 以下所有声明使用C链接（无名称修饰）
void init_engine(const char* config);
int process_data(const char* input, size_t len, char** output);
void shutdown_engine(void);

typedef struct {
    double x, y, z;
} vec3_t;

#ifdef __cplusplus
}
#endif
```

**规则**：

- `extern "C"` 只影响**链接名称**，不影响**类型检查**或**调用约定**
- 在 `extern "C"` 块内，函数不能重载（C不支持）
- 在 `extern "C"` 块内，不能使用C++特有的参数类型（如引用、类对象值传递）

### 2.2 条件编译模板

```cpp
// mylib.h — 可被C和C++同时包含的头文件
#ifndef MYLIB_H
#define MYLIB_H

// 统一入口宏
#ifdef __cplusplus
#define MYLIB_EXTERN_C_BEGIN extern "C" {
#define MYLIB_EXTERN_C_END   }
#define MYLIB_API extern "C"
#else
#define MYLIB_EXTERN_C_BEGIN
#define MYLIB_EXTERN_C_END
#define MYLIB_API
#endif

MYLIB_EXTERN_C_BEGIN

// ========== C兼容接口 ==========

typedef struct mylib_context mylib_context_t;

MYLIB_API mylib_context_t* mylib_create(const char* config);
MYLIB_API int mylib_process(mylib_context_t* ctx,
                            const void* input, size_t input_len,
                            void** output, size_t* output_len);
MYLIB_API void mylib_destroy(mylib_context_t* ctx);

MYLIB_EXTERN_C_END

// ========== C++扩展接口（仅在C++中可见） ==========
#ifdef __cplusplus

namespace mylib {

class Context {
public:
    explicit Context(const std::string& config);
    std::vector<uint8_t> process(const std::vector<uint8_t>& input);
    ~Context();
private:
    mylib_context_t* c_handle_;
};

} // namespace mylib

#endif // __cplusplus

#endif // MYLIB_H
```

### 2.3 C调用C++的封装模式

```cpp
// mylib.cpp — C++实现，暴露C接口
#include "mylib.h"
#include <string>
#include <vector>
#include <memory>

// C++内部实现
class Engine {
public:
    explicit Engine(const std::string& config) : config_(config) {}
    std::vector<uint8_t> process(const std::vector<uint8_t>& input) {
        // C++复杂逻辑...
        return input;  // 示例
    }
private:
    std::string config_;
};

// C接口实现（extern "C" 在头文件中已声明）
mylib_context_t* mylib_create(const char* config) {
    try {
        auto* engine = new Engine(std::string(config));
        return reinterpret_cast<mylib_context_t*>(engine);
    } catch (...) {
        return nullptr;
    }
}

int mylib_process(mylib_context_t* ctx,
                  const void* input, size_t input_len,
                  void** output, size_t* output_len) {
    if (!ctx || !input || !output || !output_len) return -EINVAL;

    try {
        auto* engine = reinterpret_cast<Engine*>(ctx);
        std::vector<uint8_t> in(static_cast<const uint8_t*>(input),
                                static_cast<const uint8_t*>(input) + input_len);
        auto out = engine->process(in);

        // 使用C内存分配返回结果（调用者负责free）
        *output_len = out.size();
        *output = malloc(out.size());
        if (!*output) return -ENOMEM;
        memcpy(*output, out.data(), out.size());
        return 0;
    } catch (...) {
        return -EFAULT;
    }
}

void mylib_destroy(mylib_context_t* ctx) {
    if (ctx) {
        delete reinterpret_cast<Engine*>(ctx);
    }
}
```

```c
// main.c — C代码调用C++库
#include "mylib.h"
#include <stdio.h>
#include <stdlib.h>

int main(void) {
    mylib_context_t* ctx = mylib_create("{\"threads\": 4}");
    if (!ctx) {
        fprintf(stderr, "Failed to create engine\n");
        return 1;
    }

    const char data[] = "Hello, C++ from C!";
    void* output = NULL;
    size_t output_len = 0;

    int ret = mylib_process(ctx, data, sizeof(data), &output, &output_len);
    if (ret != 0) {
        fprintf(stderr, "Process failed: %d\n", ret);
        mylib_destroy(ctx);
        return 1;
    }

    printf("Output: %.*s\n", (int)output_len, (char*)output);
    free(output);  // C调用者负责释放
    mylib_destroy(ctx);
    return 0;
}
```

**编译**：

```bash
# 编译C++库
g++ -O2 -fPIC -shared -o libmylib.so mylib.cpp

# 编译C程序并链接
gcc -O2 -o main_c main.c -L. -lmylib -lstdc++
```

---

## 三、名称修饰（Name Mangling）

### 3.1 名称修饰对比

```cpp
// 源码
namespace ns {
    void foo(int);
    void foo(double);
    class Bar {
        void baz(int) const;
    };
}
```

| 编译器 | `ns::foo(int)` | `ns::foo(double)` | `ns::Bar::baz(int) const` |
|:-------|:---------------|:------------------|:---------------------------|
| **GCC/Clang** (Itanium ABI) | `_ZN2ns3fooEi` | `_ZN2ns3fooEd` | `_ZN2ns3Bar3bazEi` |
| **MSVC** | `?foo@ns@@YAXH@Z` | `?foo@ns@@YAXN@Z` | `?baz@Bar@ns@@QBEXH@Z` |
| **C** (无修饰) | `foo` | `foo` | `baz` |

> **规则**：`extern "C"` 完全禁用名称修饰，函数名在目标文件中保持原样。

### 3.2 查看名称修饰的工具

```bash
# GCC/Clang: 使用 nm 查看符号表
g++ -c mylib.cpp
nm mylib.o | grep foo
# 输出: _ZN2ns3fooEi T

gcc -c mylib.c
nm mylib.o | grep foo
# 输出: foo T

# 使用 c++filt 解码修饰名
echo _ZN2ns3fooEi | c++filt
# 输出: ns::foo(int)
```

---

## 四、ABI边界：类型与内存布局

### 4.1 可安全穿越边界的类型

| C类型 | C++类型 | 兼容性 | 说明 |
|:------|:--------|:------:|:-----|
| `int` | `int` | ✅ | 完全相同 |
| `double` | `double` | ✅ | 完全相同（IEEE 754） |
| `char*` | `char*` | ✅ | 指针布局相同 |
| `struct T` | `struct T` | ⚠️ | POD/标准布局类型相同；非POD不同 |
| `enum T` | `enum T` | ⚠️ | C++11 enum class不同；C23 enum有类型 |
| `void*` | `void*` | ✅ | 通用指针 |
| `T[]` | `T[]` | ✅ | 数组衰减规则相同 |
| `FILE*` | `std::FILE*` | ✅ | C++标准保证 |

### 4.2 不可穿越边界的类型

| C++类型 | 原因 | 替代方案 |
|:--------|:-----|:---------|
| `std::string` | 含内部指针、SSO缓冲区 | 使用 `const char*` + `size_t` |
| `std::vector<T>` | 含三个指针（begin/end/capacity） | 使用 `T* data + size_t len` |
| `std::unique_ptr<T>` | 含deleter | 使用原始指针 + 显式destroy函数 |
| `T&`（引用） | C无引用 | 使用 `T*` 指针 |
| 类对象值传递 | C++可能调用拷贝构造 | 使用不透明指针（opaque pointer） |
| 有虚函数的类 | 含vptr | 使用不透明指针 + 虚函数表的手动封装 |

### 4.3 不透明指针模式（Opaque Pointer / PIMPL）

```cpp
// C++头文件（C可见部分）
#ifdef __cplusplus
extern "C" {
#endif

typedef struct widget widget_t;

widget_t* widget_create(int width, int height);
void widget_draw(widget_t* w, int x, int y);
void widget_destroy(widget_t* w);

#ifdef __cplusplus
}
#endif
```

```cpp
// C++实现（C不可见）
#include "widget.h"
#include <vector>
#include <string>

// C++内部实现
class WidgetImpl {
public:
    int width, height;
    std::vector<uint8_t> pixels;
    std::string label;
    // ... 复杂C++成员
};

extern "C" {

widget_t* widget_create(int width, int height) {
    try {
        auto* impl = new WidgetImpl{width, height};
        impl->pixels.resize(width * height * 4);
        return reinterpret_cast<widget_t*>(impl);
    } catch (...) {
        return nullptr;
    }
}

void widget_draw(widget_t* w, int x, int y) {
    if (!w) return;
    auto* impl = reinterpret_cast<WidgetImpl*>(w);
    // 使用C++特性绘制...
}

void widget_destroy(widget_t* w) {
    delete reinterpret_cast<WidgetImpl*>(w);
}

} // extern "C"
```

```c
// C代码使用C++库
#include "widget.h"

int main(void) {
    widget_t* w = widget_create(800, 600);
    if (!w) return 1;
    widget_draw(w, 0, 0);
    widget_destroy(w);
    return 0;
}
```

---

## 五、异常处理边界

### 5.1 核心规则

> **C++异常绝不可以传播到C代码中。** 如果C++函数通过 `extern "C"` 被C调用，该函数必须捕获所有异常。

```cpp
// ❌ 危险：异常可能传播到C调用者
extern "C" int unsafe_process(const char* input) {
    std::string s(input);      // 可能抛出 std::bad_alloc
    std::vector<int> v = parse(s);  // 可能抛出解析异常
    return v.size();
}

// ✅ 安全：捕获所有异常
extern "C" int safe_process(const char* input) {
    try {
        std::string s(input);
        std::vector<int> v = parse(s);
        return v.size();
    } catch (const std::bad_alloc&) {
        return -ENOMEM;
    } catch (const std::exception& e) {
        return -EINVAL;
    } catch (...) {
        return -EFAULT;  // 未知异常
    }
}
```

### 5.2 异常传播到C++的策略

```cpp
// C回调函数（由C++调用）
typedef int (*c_callback_t)(int x, void* user_data);

// C++包装器：将C回调包装为可抛出异常的C++函数
class CallbackWrapper {
public:
    CallbackWrapper(c_callback_t cb, void* ud) : cb_(cb), ud_(ud) {}

    int operator()(int x) {
        int result = cb_(x, ud_);
        if (result < 0) {
            // 将C错误码转换为C++异常
            throw std::runtime_error("C callback failed with code " +
                                     std::to_string(result));
        }
        return result;
    }

private:
    c_callback_t cb_;
    void* ud_;
};
```

### 5.3 信号与异常的交互

```cpp
// C信号处理函数中不可使用C++异常
extern "C" void signal_handler(int sig) {
    // ❌ 错误：信号处理函数中抛出异常是UB
    // throw std::runtime_error("Signal!");

    // ✅ 正确：设置原子标志，由主循环处理
    g_signal_received.store(sig, std::memory_order_relaxed);
}
```

---

## 六、C++特性穿越ABI

### 6.1 RAII到C的映射

```cpp
// C++ RAII类
class FileGuard {
public:
    explicit FileGuard(const char* path) : fd_(open(path, O_RDONLY)) {}
    ~FileGuard() { if (fd_ >= 0) close(fd_); }
    int fd() const { return fd_; }
private:
    int fd_;
};

// C等价实现（显式生命周期管理）
typedef struct {
    int fd;
} file_handle_t;

file_handle_t file_open(const char* path) {
    return (file_handle_t){.fd = open(path, O_RDONLY)};
}

void file_close(file_handle_t* h) {
    if (h && h->fd >= 0) {
        close(h->fd);
        h->fd = -1;
    }
}

// C中使用（模拟RAII）
void process_file(const char* path) {
    file_handle_t f = file_open(path);
    if (f.fd < 0) return;

    // ... 使用 f.fd ...

    file_close(&f);  // 必须显式关闭
}
```

### 6.2 虚函数到C的映射

```cpp
// C++虚函数
class Shape {
public:
    virtual ~Shape() = default;
    virtual double area() const = 0;
    virtual void draw() const = 0;
};

class Circle : public Shape {
public:
    explicit Circle(double r) : radius_(r) {}
    double area() const override { return 3.14159 * radius_ * radius_; }
    void draw() const override { /* 绘制圆形 */ }
private:
    double radius_;
};
```

```c
// C等价实现：手动虚函数表

typedef struct shape_vtable {
    double (*area)(const void* self);
    void   (*draw)(const void* self);
    void   (*destroy)(void* self);
} shape_vtable_t;

typedef struct {
    const shape_vtable_t* vtable;
} shape_t;

// Circle实现
typedef struct {
    shape_t base;
    double radius;
} circle_t;

static double circle_area(const void* self) {
    const circle_t* c = (const circle_t*)self;
    return 3.14159 * c->radius * c->radius;
}

static void circle_draw(const void* self) {
    (void)self;  /* 绘制圆形 */
}

static void circle_destroy(void* self) {
    free(self);
}

static const shape_vtable_t circle_vtable = {
    .area = circle_area,
    .draw = circle_draw,
    .destroy = circle_destroy
};

shape_t* circle_create(double radius) {
    circle_t* c = malloc(sizeof(circle_t));
    if (!c) return NULL;
    c->base.vtable = &circle_vtable;
    c->radius = radius;
    return (shape_t*)c;
}

// 多态调用
void use_shape(shape_t* s) {
    printf("Area: %f\n", s->vtable->area(s));
    s->vtable->draw(s);
}

void destroy_shape(shape_t* s) {
    if (s) s->vtable->destroy(s);
}
```

---

## 七、工程实践：混合项目构建

### 7.1 CMake混合项目

```cmake
# CMakeLists.txt
cmake_minimum_required(VERSION 3.20)
project(MyMixedProject LANGUAGES C CXX)

# 设置标准
set(CMAKE_C_STANDARD 23)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# C++库
add_library(mylib_cpp SHARED
    src/engine.cpp
    src/utils.cpp
)
target_include_directories(mylib_cpp PUBLIC include)

# C库（调用C++库）
add_library(mylib_c SHARED
    src/c_api.cpp      # C++实现，暴露C接口
    src/c_helpers.c    # 纯C辅助函数
)
target_link_libraries(mylib_c PUBLIC mylib_cpp)

# C可执行文件
add_executable(client_c src/client.c)
target_link_libraries(client_c PRIVATE mylib_c)

# C++可执行文件
add_executable(client_cpp src/client.cpp)
target_link_libraries(client_cpp PRIVATE mylib_cpp)
```

### 7.2 链接顺序

```bash
# C程序链接C++库时，必须链接C++标准库
gcc -o client client.c -L. -lmylib_c -lmylib_cpp -lstdc++

# 或者使用 g++ 作为链接器（自动包含libstdc++）
g++ -o client client.c -L. -lmylib_c -lmylib_cpp
```

### 7.3 跨平台ABI兼容

| 平台 | C++ ABI | 兼容性策略 |
|:-----|:--------|:-----------|
| Linux x86_64 | Itanium ABI | GCC/Clang/Intel兼容 |
| Linux ARM64 | Itanium ABI | GCC/Clang兼容 |
| Windows x64 | Microsoft ABI | MSVC/Clang-cl兼容；MinGW使用Itanium |
| macOS | Itanium ABI | Apple Clang兼容 |

> **关键**：同一项目中所有C++代码必须使用**相同的编译器和ABI**。跨编译器边界传递C++对象（即使是POD）是**未定义行为**。

---

## 八、常见陷阱

### 8.1 陷阱1：混合new/malloc和delete/free

```cpp
// ❌ 严重错误：C++ new，C free
extern "C" char* create_buffer(size_t size) {
    return new char[size];  // C++ new
}

// C代码
char* buf = create_buffer(100);
free(buf);  // ❌ UB！必须用 delete[]

// ✅ 正确：C接口统一使用malloc/free
extern "C" char* create_buffer(size_t size) {
    return (char*)malloc(size);
}
```

### 8.2 陷阱2：结构体填充差异

```cpp
// ❌ 危险：依赖特定填充
struct Packet {
    uint8_t type;
    uint32_t len;   // 可能填充3字节
};

// ✅ 正确：使用pack pragma或属性
#pragma pack(push, 1)
struct Packet {
    uint8_t type;
    uint32_t len;
};
#pragma pack(pop)

// 或C23方式
struct [[gnu::packed]] Packet {
    uint8_t type;
    uint32_t len;
};
```

### 8.3 陷阱3：布尔类型差异

```c
// C: bool 需要 <stdbool.h>，定义 _Bool
// C++: bool 是内置类型

// C头文件
#include <stdbool.h>
extern "C" bool check_status(void);  // C: _Bool, C++: bool

// 问题：C99 _Bool 和 C++ bool 在ABI上通常兼容（大小都为1），
// 但最好不要在ABI边界使用bool，改用int
extern "C" int check_status(void);  // 更安全
```

### 8.4 陷阱4：线程局部存储

```cpp
// C++ thread_local
thread_local int counter = 0;

// C _Thread_local
_Thread_local int counter = 0;

// 问题：不同编译器的TLS实现可能不兼容
// 解决方案：避免在ABI边界暴露TLS变量
```

---

## 九、参考资源

### 9.1 标准与文档

| 资源 | 说明 |
|:-----|:-----|
| Itanium C++ ABI | Linux/GCC/Clang的C++ ABI标准 |
| Microsoft C++ ABI | MSVC的ABI文档 |
| cppreference.com `extern "C"` | 语言参考 |
| C++ Core Guidelines I.26-30 | C++与C的接口最佳实践 |

### 9.2 工具

| 工具 | 用途 |
|:-----|:-----|
| `nm` | 查看符号表 |
| `c++filt` | 解码C++修饰名 |
| `objdump -t` | 查看目标文件符号 |
| `readelf -s` | ELF符号信息 |

---

> **总结**: C/C++互操作是**系统编程的必备技能**。核心原则：**`extern "C"` 封装接口、不透明指针隐藏C++实现、捕获所有异常、统一内存管理（malloc/free）、避免非POD类型穿越边界**。遵循这些原则，可以安全高效地在同一项目中使用C和C++。
