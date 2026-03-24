# C2y if 声明语句深度解析

> **标准**: ISO/IEC 9899:C2y (草案 N3356) | **难度**: ⭐⭐⭐⭐⭐
> **前置知识**: C23 标准、作用域规则、控制流语句

---

## 目录

- [C2y if 声明语句深度解析](#c2y-if-声明语句深度解析)
  - [目录](#目录)
  - [一、if 声明概述](#一if-声明概述)
    - [1.1 什么是 if 声明 (N3356)](#11-什么是-if-声明-n3356)
    - [1.2 设计动机](#12-设计动机)
      - [问题 1: 变量作用域污染](#问题-1-变量作用域污染)
      - [问题 2: 资源泄漏风险](#问题-2-资源泄漏风险)
      - [问题 3: 重复代码](#问题-3-重复代码)
      - [C2y if 声明的解决方案](#c2y-if-声明的解决方案)
    - [1.3 与 C++17 if-init 的关系](#13-与-c17-if-init-的关系)
  - [二、语法语义详解](#二语法语义详解)
    - [2.1 if (declaration; condition) 语法](#21-if-declaration-condition-语法)
      - [语法要素详解](#语法要素详解)
      - [声明的限制](#声明的限制)
    - [2.2 变量作用域规则](#22-变量作用域规则)
      - [作用域示例](#作用域示例)
      - [else 分支中的可见性](#else-分支中的可见性)
    - [2.3 else if 声明](#23-else-if-声明)
      - [最佳实践：避免变量名遮蔽](#最佳实践避免变量名遮蔽)
    - [2.4 与 switch 对比](#24-与-switch-对比)
  - [三、使用场景](#三使用场景)
    - [3.1 临时变量限制作用域](#31-临时变量限制作用域)
    - [3.2 资源获取与检查](#32-资源获取与检查)
    - [3.3 查找操作](#33-查找操作)
    - [3.4 解析/转换操作](#34-解析转换操作)
  - [四、代码示例](#四代码示例)
    - [4.1 基本用法](#41-基本用法)
    - [4.2 文件操作](#42-文件操作)
    - [4.3 查找操作](#43-查找操作)
    - [4.4 链式 if 声明](#44-链式-if-声明)
    - [4.5 与 for 声明对比](#45-与-for-声明对比)
    - [4.6 内存分配检查](#46-内存分配检查)
    - [4.7 字符串解析](#47-字符串解析)
  - [五、与 C++17 if-init 对比](#五与-c17-if-init-对比)
    - [5.1 语法差异](#51-语法差异)
    - [5.2 语义兼容](#52-语义兼容)
      - [表达式语句的转换](#表达式语句的转换)
    - [5.3 互操作性](#53-互操作性)
  - [六、实现原理](#六实现原理)
    - [6.1 编译器如何实现](#61-编译器如何实现)
    - [6.2 语法糖分析](#62-语法糖分析)
    - [6.3 性能开销](#63-性能开销)
    - [6.4 与块作用域的关系](#64-与块作用域的关系)
  - [七、最佳实践](#七最佳实践)
    - [7.1 何时使用 if 声明](#71-何时使用-if-声明)
    - [7.2 代码可读性考虑](#72-代码可读性考虑)
    - [7.3 与 goto 的替代关系](#73-与-goto-的替代关系)
  - [八、编译器支持](#八编译器支持)
    - [8.1 Clang 22+ 支持](#81-clang-22-支持)
    - [8.2 GCC 支持状态](#82-gcc-支持状态)
    - [8.3 使用方法](#83-使用方法)
  - [参考资源](#参考资源)
  - [总结](#总结)
  - [深入理解](#深入理解)
    - [核心概念](#核心概念)
    - [实践应用](#实践应用)
    - [学习建议](#学习建议)

---

## 一、if 声明概述

### 1.1 什么是 if 声明 (N3356)

C2y 标准引入了 **if 声明**（if declaration）这一革命性的控制流特性，允许在 if 语句的条件表达式之前执行一个声明语句。这一特性源自 C++17 的 if-init 语句，但经过针对 C 语言特性的适配和优化。

```text
┌─────────────────────────────────────────────────────────────────────┐
│                        if 声明形式化定义                              │
├─────────────────────────────────────────────────────────────────────┤
│                                                                      │
│  语法形式:                                                           │
│  ─────────────────                                                   │
│  if (declaration; condition) statement                               │
│  if (declaration; condition) statement else statement                │
│                                                                      │
│  语义规则:                                                           │
│  ─────────────────                                                   │
│  1. declaration 执行后，其声明的变量在 if 和 else 分支中都可见         │
│  2. 变量作用域延伸到整个 if-else 结构结束                              │
│  3. condition 中使用 declaration 声明的变量                            │
│  4. 支持所有类型的声明：变量、类型定义、枚举等                          │
│                                                                      │
│  执行流程:                                                           │
│  ─────────────────                                                   │
│  ┌─────────────┐    ┌─────────────┐    ┌─────────────┐              │
│  │ 执行声明语句 │ -> │ 求值条件    │ -> │ 选择分支    │              │
│  │ declaration │    │ condition   │    │ 执行        │              │
│  └─────────────┘    └─────────────┘    └─────────────┘              │
│                                              │                       │
│                    ┌─────────────────────────┘                       │
│                    v                                                 │
│              ┌─────────┐    ┌─────────┐                             │
│              │ 条件为真 │    │ 条件为假 │                             │
│              │ if 分支  │    │ else 分支│                             │
│              └─────────┘    └─────────┘                             │
│                                                                      │
└─────────────────────────────────────────────────────────────────────┘
```

### 1.2 设计动机

if 声明的设计旨在解决 C 语言中长期存在的几个代码质量问题：

#### 问题 1: 变量作用域污染

```c
// 传统 C 代码的问题：变量作用域过大
void process_file(const char* filename) {
    FILE* fp = fopen(filename, "r");  // fp 的作用域延伸到函数结束
    if (fp == NULL) {
        perror("Failed to open file");
        return;
    }
    // ... 使用 fp ...
    fclose(fp);
    // fp 在这里仍然可见，可能被误用
}
```

#### 问题 2: 资源泄漏风险

```c
// 多个资源获取时的复杂管理
void complex_operation(void) {
    Resource* r1 = acquire_resource1();
    if (r1 == NULL) goto cleanup_none;

    Resource* r2 = acquire_resource2();
    if (r2 == NULL) goto cleanup_r1;

    Resource* r3 = acquire_resource3();
    if (r3 == NULL) goto cleanup_r2;

    // ... 使用资源 ...

cleanup_r3:
    release_resource(r3);
cleanup_r2:
    release_resource(r2);
cleanup_r1:
    release_resource(r1);
cleanup_none:
    return;
}
```

#### 问题 3: 重复代码

```c
// 查找操作中的重复模式
Node* find_node(Tree* tree, Key key) {
    Node* result = search_tree(tree, key);  // 声明
    if (result != NULL) {                    // 检查
        return result;                       // 使用
    }
    // result 在后续代码中不再使用，但仍然可见

    // 另一种查找
    Node* alt_result = search_alternate(tree, key);  // 又一个声明
    if (alt_result != NULL) {                         // 又一个检查
        return alt_result;
    }
    // alt_result 也继续可见

    return NULL;
}
```

#### C2y if 声明的解决方案

```c
// C2y: 变量作用域精确控制
void process_file_c2y(const char* filename) {
    if (FILE* fp = fopen(filename, "r"); fp != NULL) {
        // fp 只在这里可见
        // ... 使用 fp ...
        fclose(fp);
    } else {
        perror("Failed to open file");
    }
    // fp 在这里不可见！
}
```

### 1.3 与 C++17 if-init 的关系

| 特性 | C++17 if-init | C2y if 声明 |
|------|---------------|-------------|
| 基本语法 | `if (init; cond)` | `if (decl; cond)` |
| 表达式语句 | 支持 | 支持（表达式语句本质上是声明） |
| 变量声明 | 支持 | 支持 |
| 多变量声明 | 支持 | 支持 |
| 结构化绑定 | 支持 (C++17) | 不支持（C 无此特性） |
| constexpr if | 支持 (C++17) | 不支持（C 无此特性） |
| 与 C 的兼容性 | C++ 专用 | C 标准特性 |

```cpp
// C++17 if-init 示例
if (auto fp = std::fopen("file.txt", "r"); fp != nullptr) {
    // 使用 fp
    std::fclose(fp);
}
// fp 已超出作用域
```

```c
// C2y 等价代码
if (FILE* fp = fopen("file.txt", "r"); fp != NULL) {
    // 使用 fp
    fclose(fp);
}
// fp 已超出作用域
```

---

## 二、语法语义详解

### 2.1 if (declaration; condition) 语法

if 声明的完整语法形式：

```text
if-declaration-statement:
    "if" "(" declaration ";" expression ")" statement
    "if" "(" declaration ";" expression ")" statement "else" statement
```

#### 语法要素详解

```c
// 1. 基本变量声明
if (int x = compute_value(); x > 0) {
    // x 在这里可用
}

// 2. 多变量声明
if (int a = get_a(), b = get_b(); a + b > 0) {
    // a 和 b 都可用
}

// 3. 带初始化的指针声明
if (Node* node = find_node(tree, key); node != NULL) {
    // node 可用
}

// 4. 复合类型声明
if (struct Data data = load_data(); data.valid) {
    // data 可用
}

// 5. 类型定义声明（限制作用域）
if (typedef int MyInt; true) {
    MyInt x = 10;  // MyInt 在此作用域内
}
// MyInt 在这里不可用
```

#### 声明的限制

```c
// 合法声明
if (int x = 10; x > 0) { }           // ✅ 简单声明
if (const int* p = get_ptr(); p) { } // ✅ 带限定符
if (static int s = 0; s++ < 10) { }  // ✅ 静态存储期（但需谨慎）

// 非法声明
if (extern int x = 10; x > 0) { }    // ❌ extern 不能初始化
if (int f(void); true) { }           // ❌ 函数声明（无意义）
if (struct S; true) { }              // ❌ 不完整类型声明
```

### 2.2 变量作用域规则

if 声明引入的变量具有特殊的作用域规则：

```text
┌─────────────────────────────────────────────────────────────────────┐
│                      if 声明作用域规则                                │
├─────────────────────────────────────────────────────────────────────┤
│                                                                      │
│  作用域范围:                                                         │
│  ─────────────────                                                   │
│  声明的变量在以下范围内可见：                                         │
│  1. 条件表达式                                                       │
│  2. if 语句体                                                        │
│  3. else 语句体（如果存在）                                           │
│  4. 嵌套的 else if 结构                                              │
│                                                                      │
│  作用域结束点:                                                       │
│  ─────────────────                                                   │
│  整个 if-else 结构结束时，变量离开作用域                              │
│                                                                      │
│  嵌套规则:                                                           │
│  ─────────────────                                                   │
│  内层 if 声明可以遮蔽外层 if 声明的变量                               │
│  但这不是推荐做法（可读性差）                                          │
│                                                                      │
└─────────────────────────────────────────────────────────────────────┘
```

#### 作用域示例

```c
#include <stdio.h>

void scope_demo(void) {
    int outer = 1;

    if (int x = 10; x > 0) {
        // x = 10 在此可见
        printf("if branch: x = %d\n", x);
        int inner = 2;  // 仅在 if 分支内
        printf("inner = %d\n", inner);
    } else {
        // x = 10 在 else 分支也可见！
        printf("else branch: x = %d\n", x);
        // inner 不可见
    }
    // x 和 inner 都不可见

    // 嵌套 if 声明
    if (int a = 5; a > 0) {
        if (int b = a + 1; b > a) {  // a 在内层可见
            printf("nested: a=%d, b=%d\n", a, b);
        }
        // b 不可见，a 仍可见
        printf("outer if: a=%d\n", a);
    }
}
```

#### else 分支中的可见性

```c
// else 分支中访问声明变量
if (FILE* fp = fopen("data.txt", "r"); fp != NULL) {
    // 成功打开，使用 fp
    process_file(fp);
    fclose(fp);
} else {
    // 失败时，fp 仍然可见（值为 NULL）
    // 可以用于错误报告
    fprintf(stderr, "Failed to open file, fp = %p\n", (void*)fp);
    // 但不能解引用！
}
// fp 超出作用域
```

### 2.3 else if 声明

else if 声明支持链式使用，形成多分支条件判断：

```c
// 链式 else if 声明
void find_resource(int priority) {
    if (Resource* r = try_acquire(HIGH_PRIORITY); r != NULL) {
        printf("Acquired high priority resource\n");
        use_resource(r);
        release(r);
    } else if (Resource* r = try_acquire(MEDIUM_PRIORITY); r != NULL) {
        // 注意：这里声明了新的 r，遮蔽了外层的 r（值为 NULL）
        printf("Acquired medium priority resource\n");
        use_resource(r);
        release(r);
    } else if (Resource* r = try_acquire(LOW_PRIORITY); r != NULL) {
        printf("Acquired low priority resource\n");
        use_resource(r);
        release(r);
    } else {
        printf("Failed to acquire any resource\n");
    }
    // 所有 r 都不可见
}
```

#### 最佳实践：避免变量名遮蔽

```c
// 更好的做法：使用不同的变量名
void find_resource_clean(int priority) {
    if (Resource* high = try_acquire(HIGH_PRIORITY); high != NULL) {
        use_resource(high);
        release(high);
    } else if (Resource* medium = try_acquire(MEDIUM_PRIORITY); medium != NULL) {
        use_resource(medium);
        release(medium);
    } else if (Resource* low = try_acquire(LOW_PRIORITY); low != NULL) {
        use_resource(low);
        release(low);
    } else {
        printf("Failed to acquire any resource\n");
    }
}
```

### 2.4 与 switch 对比

if 声明与 switch 语句在功能上有一些重叠，但各有适用场景：

| 特性 | if 声明 | switch 语句 |
|------|---------|-------------|
| 条件类型 | 任意布尔表达式 | 整数/枚举表达式 |
| 声明能力 | 支持声明变量 | 不支持 |
| 分支数量 | 任意（链式） | 多个 case |
| 贯穿行为 | 需要显式 else | 需要显式 break |
| 适用场景 | 复杂条件、资源获取 | 离散值匹配 |

```c
// if 声明适合的场景
void classify_value(int x) {
    // 复杂条件判断
    if (int abs_x = x >= 0 ? x : -x; abs_x < 10) {
        printf("Small value\n");
    } else if (abs_x < 100) {
        printf("Medium value\n");
    } else {
        printf("Large value\n");
    }
}

// switch 适合的场景
void handle_command(enum Cmd cmd) {
    switch (cmd) {
        case CMD_START:  start();  break;
        case CMD_STOP:   stop();   break;
        case CMD_PAUSE:  pause();  break;
        default:         error();  break;
    }
}
```

---

## 三、使用场景

### 3.1 临时变量限制作用域

if 声明最常见的用途是限制临时变量的作用域：

```c
// 传统方式：临时变量污染整个函数
int process_data_traditional(const char* input) {
    int len = strlen(input);  // len 的作用域过大
    if (len == 0) {
        return ERROR_EMPTY;
    }

    char* buffer = malloc(len + 1);  // buffer 的作用域也过大
    if (buffer == NULL) {
        return ERROR_NOMEM;
    }

    strcpy(buffer, input);
    // ... 处理 buffer ...

    free(buffer);
    return SUCCESS;
    // len 和 buffer 在函数剩余部分仍然可见
}

// C2y 方式：精确控制作用域
int process_data_c2y(const char* input) {
    if (int len = strlen(input); len == 0) {
        return ERROR_EMPTY;
    }
    // len 在这里不可见！

    if (char* buffer = malloc(strlen(input) + 1); buffer == NULL) {
        return ERROR_NOMEM;
    } else {
        strcpy(buffer, input);
        // ... 处理 buffer ...
        free(buffer);
    }
    // buffer 在这里不可见！

    return SUCCESS;
}
```

### 3.2 资源获取与检查

if 声明是处理资源获取和错误检查的理想工具：

```c
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>

// 文件操作
void copy_file(const char* src_path, const char* dst_path) {
    if (int src_fd = open(src_path, O_RDONLY); src_fd < 0) {
        perror("Failed to open source file");
        return;
    } else {
        // src_fd 在此作用域内有效
        if (int dst_fd = open(dst_path, O_WRONLY | O_CREAT | O_TRUNC, 0644);
            dst_fd < 0) {
            perror("Failed to open destination file");
            close(src_fd);
        } else {
            // 两个文件描述符都可用
            char buffer[4096];
            ssize_t n;

            while ((n = read(src_fd, buffer, sizeof(buffer))) > 0) {
                if (write(dst_fd, buffer, n) != n) {
                    perror("Write error");
                    break;
                }
            }

            close(dst_fd);
        }
        close(src_fd);
    }
    // 所有描述符都超出作用域
}

// 动态内存分配
void* safe_malloc(size_t size) {
    if (void* ptr = malloc(size); ptr != NULL) {
        return ptr;
    }
    // ptr 超出作用域

    // 尝试其他分配策略
    if (void* ptr = fallback_allocator(size); ptr != NULL) {
        return ptr;
    }

    return NULL;
}
```

### 3.3 查找操作

if 声明在查找操作中特别有用：

```c
// 数据结构查找
typedef struct Node {
    int key;
    int value;
    struct Node* left;
    struct Node* right;
} Node;

int* find_value(Node* root, int key) {
    // 在树中查找
    if (Node* found = search_bst(root, key); found != NULL) {
        return &found->value;
    }

    // 在备用表中查找
    if (Node* found = search_hash_table(key); found != NULL) {
        return &found->value;
    }

    // 在缓存中查找
    if (Node* found = search_cache(key); found != NULL) {
        return &found->value;
    }

    return NULL;
}

// 字符串查找
const char* find_config(const char* key) {
    // 先查环境变量
    if (const char* env = getenv(key); env != NULL) {
        return env;
    }

    // 再查配置文件
    if (const char* cfg = config_file_get(key); cfg != NULL) {
        return cfg;
    }

    // 最后使用默认值
    if (const char* def = default_values_get(key); def != NULL) {
        return def;
    }

    return NULL;
}
```

### 3.4 解析/转换操作

if 声明可以简化解析和转换操作：

```c
#include <stdlib.h>
#include <ctype.h>

// 字符串转整数
bool parse_int(const char* str, int* result) {
    if (char* endptr; true) {
        long val = strtol(str, &endptr, 10);

        if (*endptr != '\0') {
            return false;  // 有未解析字符
        }

        if (val < INT_MIN || val > INT_MAX) {
            return false;  // 溢出
        }

        *result = (int)val;
        return true;
    }
}

// 解析版本号
bool parse_version(const char* str, int* major, int* minor, int* patch) {
    if (char* p1; true) {
        *major = (int)strtol(str, &p1, 10);
        if (*p1 != '.') return false;

        if (char* p2; true) {
            *minor = (int)strtol(p1 + 1, &p2, 10);
            if (*p2 != '.') return false;

            if (char* p3; true) {
                *patch = (int)strtol(p2 + 1, &p3, 10);
                if (*p3 != '\0') return false;

                return true;
            }
        }
    }
    return false;
}

// 更复杂的解析示例：URL 解析
bool parse_url(const char* url, char* host, size_t host_size,
               int* port, char* path, size_t path_size) {
    // 解析协议
    if (const char* p = strstr(url, "://"); p != NULL) {
        p += 3;  // 跳过 "://"

        // 查找主机和端口的分隔符
        if (const char* colon = strchr(p, ':'); colon != NULL) {
            // 有端口
            size_t host_len = colon - p;
            if (host_len >= host_size) return false;

            memcpy(host, p, host_len);
            host[host_len] = '\0';

            *port = atoi(colon + 1);

            if (const char* slash = strchr(colon, '/'); slash != NULL) {
                strncpy(path, slash, path_size - 1);
                path[path_size - 1] = '\0';
            } else {
                path[0] = '/';
                path[1] = '\0';
            }
        } else {
            // 无端口，使用默认端口
            if (const char* slash = strchr(p, '/'); slash != NULL) {
                size_t host_len = slash - p;
                if (host_len >= host_size) return false;

                memcpy(host, p, host_len);
                host[host_len] = '\0';

                *port = 80;
                strncpy(path, slash, path_size - 1);
                path[path_size - 1] = '\0';
            }
        }
        return true;
    }
    return false;
}
```

---

## 四、代码示例

### 4.1 基本用法

```c
/*
 * 示例 1: if 声明的基本用法
 * 演示变量声明、条件判断和作用域规则
 */

#include <stdio.h>
#include <stdbool.h>

int compute_value(int input) {
    return input * 2 + 1;
}

void basic_usage_demo(void) {
    // 基本用法
    if (int x = compute_value(5); x > 10) {
        printf("x = %d is greater than 10\n", x);
    } else {
        printf("x = %d is not greater than 10\n", x);
    }
    // x 在这里不可见

    // 多变量声明
    if (int a = 10, b = 20; a + b > 25) {
        printf("Sum %d is greater than 25\n", a + b);
    }

    // 使用布尔类型
    if (bool flag = compute_value(3) > 5; flag) {
        printf("Flag is true\n");
    }

    // 复杂条件
    if (int val = compute_value(7); val > 10 && val < 20) {
        printf("Value %d is in range (10, 20)\n", val);
    }
}

int main(void) {
    printf("=== Basic Usage Demo ===\n");
    basic_usage_demo();
    return 0;
}
```

### 4.2 文件操作

```c
/*
 * 示例 2: 文件操作中的 if 声明
 * 演示资源获取、错误处理和作用域控制
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BUFFER_SIZE 1024

// 简单的文件复制
bool copy_file_simple(const char* src, const char* dst) {
    if (FILE* src_fp = fopen(src, "rb"); src_fp == NULL) {
        fprintf(stderr, "Error: Cannot open source file '%s'\n", src);
        return false;
    } else {
        // src_fp 在此作用域内有效
        if (FILE* dst_fp = fopen(dst, "wb"); dst_fp == NULL) {
            fprintf(stderr, "Error: Cannot open destination file '%s'\n", dst);
            fclose(src_fp);
            return false;
        } else {
            // 两个文件指针都可用
            char buffer[BUFFER_SIZE];
            size_t bytes_read;
            size_t total_copied = 0;

            while ((bytes_read = fread(buffer, 1, BUFFER_SIZE, src_fp)) > 0) {
                if (fwrite(buffer, 1, bytes_read, dst_fp) != bytes_read) {
                    fprintf(stderr, "Error: Write failed\n");
                    fclose(dst_fp);
                    fclose(src_fp);
                    return false;
                }
                total_copied += bytes_read;
            }

            printf("Copied %zu bytes\n", total_copied);
            fclose(dst_fp);
        }
        fclose(src_fp);
    }
    return true;
}

// 读取整个文件到内存
char* read_file_contents(const char* filename, size_t* out_size) {
    if (FILE* fp = fopen(filename, "rb"); fp == NULL) {
        perror("Failed to open file");
        return NULL;
    } else {
        // 获取文件大小
        if (fseek(fp, 0, SEEK_END) != 0) {
            perror("Failed to seek");
            fclose(fp);
            return NULL;
        }

        if (long file_size = ftell(fp); file_size < 0) {
            perror("Failed to get file size");
            fclose(fp);
            return NULL;
        } else {
            rewind(fp);

            if (char* contents = malloc(file_size + 1); contents == NULL) {
                perror("Failed to allocate memory");
                fclose(fp);
                return NULL;
            } else {
                if (size_t read_size = fread(contents, 1, file_size, fp);
                    read_size != (size_t)file_size) {
                    fprintf(stderr, "Warning: Read %zu bytes, expected %ld\n",
                            read_size, file_size);
                }

                contents[file_size] = '\0';
                *out_size = file_size;

                fclose(fp);
                return contents;
            }
        }
    }
}

// 安全的日志写入
void log_message(const char* logfile, const char* message) {
    if (FILE* fp = fopen(logfile, "a"); fp == NULL) {
        // 无法打开日志文件，输出到 stderr
        fprintf(stderr, "[LOG ERROR] %s\n", message);
    } else {
        time_t now = time(NULL);
        struct tm* tm_info = localtime(&now);
        char timestamp[26];
        strftime(timestamp, 26, "%Y-%m-%d %H:%M:%S", tm_info);

        fprintf(fp, "[%s] %s\n", timestamp, message);
        fclose(fp);
    }
}

int main(void) {
    printf("=== File Operations Demo ===\n");

    // 创建测试文件
    if (FILE* fp = fopen("test_input.txt", "w"); fp != NULL) {
        fprintf(fp, "Hello, C2y if declaration!\n");
        fprintf(fp, "This is a test file.\n");
        fclose(fp);
    }

    // 复制文件
    if (copy_file_simple("test_input.txt", "test_output.txt")) {
        printf("File copied successfully!\n");
    }

    // 读取文件内容
    size_t size;
    if (char* contents = read_file_contents("test_output.txt", &size);
        contents != NULL) {
        printf("File contents:\n%s\n", contents);
        free(contents);
    }

    // 记录日志
    log_message("test.log", "Test message from C2y if declaration demo");

    // 清理
    remove("test_input.txt");
    remove("test_output.txt");
    remove("test.log");

    return 0;
}
```

### 4.3 查找操作

```c
/*
 * 示例 3: 查找操作中的 if 声明
 * 演示多级查找策略和短路求值
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

// 简单哈希表实现
typedef struct HashEntry {
    char* key;
    int value;
    struct HashEntry* next;
} HashEntry;

typedef struct {
    HashEntry** buckets;
    size_t size;
} HashTable;

#define HASH_SIZE 101

unsigned int hash(const char* key) {
    unsigned int h = 0;
    while (*key) {
        h = h * 31 + (unsigned char)*key++;
    }
    return h % HASH_SIZE;
}

HashTable* hash_table_create(void) {
    HashTable* ht = malloc(sizeof(HashTable));
    ht->size = HASH_SIZE;
    ht->buckets = calloc(HASH_SIZE, sizeof(HashEntry*));
    return ht;
}

void hash_table_put(HashTable* ht, const char* key, int value) {
    unsigned int h = hash(key);
    HashEntry* entry = malloc(sizeof(HashEntry));
    entry->key = strdup(key);
    entry->value = value;
    entry->next = ht->buckets[h];
    ht->buckets[h] = entry;
}

int* hash_table_get(HashTable* ht, const char* key) {
    unsigned int h = hash(key);
    for (HashEntry* e = ht->buckets[h]; e != NULL; e = e->next) {
        if (strcmp(e->key, key) == 0) {
            return &e->value;
        }
    }
    return NULL;
}

// 缓存（LRU 简化版）
typedef struct Cache {
    char key[64];
    int value;
    bool valid;
} Cache;

#define CACHE_SIZE 16
static Cache cache[CACHE_SIZE];

int* cache_get(const char* key) {
    for (int i = 0; i < CACHE_SIZE; i++) {
        if (cache[i].valid && strcmp(cache[i].key, key) == 0) {
            return &cache[i].value;
        }
    }
    return NULL;
}

void cache_put(const char* key, int value) {
    static int next_slot = 0;
    strncpy(cache[next_slot].key, key, 63);
    cache[next_slot].key[63] = '\0';
    cache[next_slot].value = value;
    cache[next_slot].valid = true;
    next_slot = (next_slot + 1) % CACHE_SIZE;
}

// 多级查找：缓存 -> 哈希表 -> 计算
int get_config_value(const char* key, HashTable* config_table) {
    // 1. 先查缓存
    if (int* cached = cache_get(key); cached != NULL) {
        printf("  [Cache hit] %s = %d\n", key, *cached);
        return *cached;
    }

    // 2. 再查哈希表
    if (int* from_table = hash_table_get(config_table, key); from_table != NULL) {
        printf("  [Table hit] %s = %d\n", key, *from_table);
        // 更新缓存
        cache_put(key, *from_table);
        return *from_table;
    }

    // 3. 使用默认值
    int default_value = 0;
    if (strcmp(key, "timeout") == 0) default_value = 30;
    else if (strcmp(key, "retries") == 0) default_value = 3;
    else if (strcmp(key, "buffer_size") == 0) default_value = 4096;

    printf("  [Default] %s = %d\n", key, default_value);
    cache_put(key, default_value);
    return default_value;
}

// 查找用户（多级回退）
typedef struct User {
    int id;
    char name[64];
    char email[128];
} User;

User* find_user_by_id(int id);
User* find_user_by_email(const char* email);
User* find_user_by_phone(const char* phone);

User* find_user(int id, const char* email, const char* phone) {
    // 按优先级查找用户
    if (User* user = find_user_by_id(id); user != NULL) {
        printf("Found user by ID\n");
        return user;
    } else if (User* user = find_user_by_email(email); user != NULL) {
        printf("Found user by email\n");
        return user;
    } else if (User* user = find_user_by_phone(phone); user != NULL) {
        printf("Found user by phone\n");
        return user;
    }

    printf("User not found\n");
    return NULL;
}

// 模拟查找函数
User database[] = {
    {1, "Alice", "alice@example.com"},
    {2, "Bob", "bob@example.com"},
    {3, "Charlie", "charlie@example.com"}
};

User* find_user_by_id(int id) {
    for (size_t i = 0; i < sizeof(database) / sizeof(database[0]); i++) {
        if (database[i].id == id) return &database[i];
    }
    return NULL;
}

User* find_user_by_email(const char* email) {
    for (size_t i = 0; i < sizeof(database) / sizeof(database[0]); i++) {
        if (strcmp(database[i].email, email) == 0) return &database[i];
    }
    return NULL;
}

User* find_user_by_phone(const char* phone) {
    (void)phone;  // 模拟：数据库中没有电话信息
    return NULL;
}

int main(void) {
    printf("=== Lookup Operations Demo ===\n\n");

    // 初始化哈希表
    HashTable* ht = hash_table_create();
    hash_table_put("timeout", 60);
    hash_table_put("retries", 5);

    // 配置查找演示
    printf("Config lookup:\n");
    get_config_value("timeout", ht);
    get_config_value("retries", ht);
    get_config_value("buffer_size", ht);  // 使用默认值
    get_config_value("timeout", ht);      // 从缓存获取

    printf("\nUser lookup:\n");
    User* u1 = find_user(1, NULL, NULL);
    if (u1) printf("  Found: %s\n", u1->name);

    User* u2 = find_user(999, "bob@example.com", NULL);
    if (u2) printf("  Found: %s\n", u2->name);

    User* u3 = find_user(999, "unknown@example.com", "1234567890");
    if (u3) printf("  Found: %s\n", u3->name);

    // 清理
    for (size_t i = 0; i < HASH_SIZE; i++) {
        HashEntry* e = ht->buckets[i];
        while (e) {
            HashEntry* next = e->next;
            free(e->key);
            free(e);
            e = next;
        }
    }
    free(ht->buckets);
    free(ht);

    return 0;
}
```

### 4.4 链式 if 声明

```c
/*
 * 示例 4: 链式 if 声明
 * 演示优先级处理和状态机实现
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

// 资源优先级枚举
typedef enum {
    PRIORITY_CRITICAL,
    PRIORITY_HIGH,
    PRIORITY_NORMAL,
    PRIORITY_LOW,
    PRIORITY_COUNT
} Priority;

const char* priority_name(Priority p) {
    static const char* names[] = {
        "CRITICAL", "HIGH", "NORMAL", "LOW"
    };
    return (p >= 0 && p < PRIORITY_COUNT) ? names[p] : "UNKNOWN";
}

// 模拟资源
typedef struct {
    int id;
    bool available;
    int data;
} Resource;

Resource resources[PRIORITY_COUNT] = {
    {0, false, 100},  // CRITICAL
    {1, true,  80},   // HIGH
    {2, false, 60},   // NORMAL
    {3, true,  40}    // LOW
};

Resource* try_acquire_resource(Priority p) {
    if (p >= 0 && p < PRIORITY_COUNT && resources[p].available) {
        resources[p].available = false;
        return &resources[p];
    }
    return NULL;
}

void release_resource(Resource* r) {
    if (r != NULL) {
        r->available = true;
        printf("Released resource %d (%s priority)\n",
               r->id, priority_name(r->id));
    }
}

// 链式 if 声明：按优先级获取资源
Resource* acquire_with_fallback(void) {
    // 尝试按优先级获取资源
    if (Resource* r = try_acquire_resource(PRIORITY_CRITICAL); r != NULL) {
        printf("Acquired CRITICAL resource (id=%d, data=%d)\n", r->id, r->data);
        return r;
    } else if (Resource* r = try_acquire_resource(PRIORITY_HIGH); r != NULL) {
        printf("Acquired HIGH resource (id=%d, data=%d)\n", r->id, r->data);
        return r;
    } else if (Resource* r = try_acquire_resource(PRIORITY_NORMAL); r != NULL) {
        printf("Acquired NORMAL resource (id=%d, data=%d)\n", r->id, r->data);
        return r;
    } else if (Resource* r = try_acquire_resource(PRIORITY_LOW); r != NULL) {
        printf("Acquired LOW resource (id=%d, data=%d)\n", r->id, r->data);
        return r;
    } else {
        printf("No resources available!\n");
        return NULL;
    }
}

// 状态机实现：解析器状态
typedef enum {
    STATE_IDLE,
    STATE_READING,
    STATE_PROCESSING,
    STATE_WRITING,
    STATE_DONE,
    STATE_ERROR
} ParserState;

typedef struct {
    ParserState state;
    int data;
    char buffer[256];
} Parser;

// 使用链式 if 声明实现状态机
ParserState run_state_machine(Parser* parser) {
    if (parser->state == STATE_IDLE) {
        printf("State: IDLE -> READING\n");
        parser->state = STATE_READING;
    } else if (parser->state == STATE_READING) {
        printf("State: READING -> PROCESSING\n");
        parser->state = STATE_PROCESSING;
    } else if (parser->state == STATE_PROCESSING) {
        if (parser->data < 0) {
            printf("State: PROCESSING -> ERROR (invalid data)\n");
            parser->state = STATE_ERROR;
        } else {
            printf("State: PROCESSING -> WRITING\n");
            parser->state = STATE_WRITING;
        }
    } else if (parser->state == STATE_WRITING) {
        printf("State: WRITING -> DONE\n");
        parser->state = STATE_DONE;
    }
    return parser->state;
}

// 另一种状态机：使用 if 声明管理状态上下文
bool process_request(int request_id) {
    // 获取请求信息
    if (Request* req = get_request(request_id); req == NULL) {
        printf("Request %d not found\n", request_id);
        return false;
    } else {
        // 验证请求
        if (ValidationResult* vr = validate_request(req); !vr->valid) {
            printf("Request %d invalid: %s\n", request_id, vr->error);
            return false;
        } else {
            // 处理请求
            if (ProcessResult* pr = process_request_data(req); pr->error) {
                printf("Request %d processing failed: %s\n",
                       request_id, pr->error_msg);
                return false;
            } else {
                // 完成
                printf("Request %d processed successfully\n", request_id);
                return true;
            }
        }
    }
}

// 模拟结构（简化）
typedef struct Request { int id; bool valid; } Request;
typedef struct ValidationResult { bool valid; const char* error; } ValidationResult;
typedef struct ProcessResult { bool error; const char* error_msg; } ProcessResult;

Request* get_request(int id) {
    static Request r = {1, true};
    return (id == 1) ? &r : NULL;
}

ValidationResult* validate_request(Request* r) {
    static ValidationResult vr = {true, NULL};
    (void)r;
    return &vr;
}

ProcessResult* process_request_data(Request* r) {
    static ProcessResult pr = {false, NULL};
    (void)r;
    return &pr;
}

int main(void) {
    printf("=== Chained if Declaration Demo ===\n\n");

    printf("1. Resource acquisition with fallback:\n");
    Resource* r1 = acquire_with_fallback();
    Resource* r2 = acquire_with_fallback();
    Resource* r3 = acquire_with_fallback();

    printf("\n2. State machine:\n");
    Parser parser = {STATE_IDLE, 100, ""};
    while (run_state_machine(&parser) != STATE_DONE &&
           parser.state != STATE_ERROR) {
        // 继续状态转换
    }

    printf("\n3. Request processing:\n");
    process_request(1);
    process_request(999);

    // 释放资源
    if (r1) release_resource(r1);
    if (r2) release_resource(r2);
    if (r3) release_resource(r3);

    return 0;
}
```

### 4.5 与 for 声明对比

```c
/*
 * 示例 5: if 声明与 for 声明对比
 * 演示两种声明控制结构的区别和适用场景
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// C99 for 声明回顾
void for_declaration_demo(void) {
    printf("=== C99 for declaration ===\n");

    // for 声明：变量只在循环体内可见
    for (int i = 0; i < 5; i++) {
        printf("i = %d\n", i);
    }
    // i 在这里不可见

    // 嵌套 for 声明
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            printf("(%d, %d) ", i, j);
        }
        printf("\n");
    }
}

// C2y if 声明
void if_declaration_demo(void) {
    printf("\n=== C2y if declaration ===\n");

    // if 声明：变量在 if 和 else 分支都可见
    if (int x = 10; x > 5) {
        printf("if branch: x = %d\n", x);
    } else {
        printf("else branch: x = %d\n", x);  // x 在 else 分支也可见
    }
    // x 在这里不可见

    // 链式 if 声明
    if (int a = 1; a == 0) {
        printf("Case 0\n");
    } else if (int b = 2; b == 1) {
        printf("Case 1\n");
    } else if (int c = 3; c == 3) {
        printf("Case 3: a=%d, b=%d, c=%d\n", a, b, c);
    }
}

// 对比：资源获取
void resource_comparison(void) {
    printf("\n=== Resource acquisition comparison ===\n");

    // 传统方式（for 声明风格不适用）
    printf("Traditional way:\n");
    FILE* fp1 = fopen("test1.txt", "r");
    if (fp1 == NULL) {
        perror("fopen");
    } else {
        // 使用 fp1
        fclose(fp1);
    }
    // fp1 仍然可见

    // C2y if 声明方式
    printf("\nC2y if declaration way:\n");
    if (FILE* fp2 = fopen("test2.txt", "r"); fp2 == NULL) {
        perror("fopen");
    } else {
        // 使用 fp2
        fclose(fp2);
    }
    // fp2 不可见

    // 模拟 for 声明风格（不自然）
    printf("\nFor-style (awkward):\n");
    FILE* fp3;
    for (fp3 = fopen("test3.txt", "r"); fp3 != NULL; fclose(fp3), fp3 = NULL) {
        // 使用 fp3 - 但只执行一次
        break;
    }
    if (fp3 == NULL) {
        perror("fopen");
    }
}

// 对比：查找操作
void lookup_comparison(void) {
    printf("\n=== Lookup comparison ===\n");

    int key = 42;

    // 传统方式
    printf("Traditional way:\n");
    int* result1 = lookup_table1(key);
    if (result1 != NULL) {
        printf("Found in table1: %d\n", *result1);
    } else {
        int* result2 = lookup_table2(key);
        if (result2 != NULL) {
            printf("Found in table2: %d\n", *result2);
        } else {
            printf("Not found\n");
        }
    }
    // result1, result2 仍然可见

    // C2y if 声明方式
    printf("\nC2y if declaration way:\n");
    if (int* r1 = lookup_table1(key); r1 != NULL) {
        printf("Found in table1: %d\n", *r1);
    } else if (int* r2 = lookup_table2(key); r2 != NULL) {
        printf("Found in table2: %d\n", *r2);
    } else {
        printf("Not found\n");
    }
    // r1, r2 都不可见

    // for 声明方式（不适用）
    printf("\nFor-style (not applicable for lookups)\n");
}

// 对比：迭代 vs 条件
void iteration_vs_condition(void) {
    printf("\n=== Iteration vs Condition ===\n");

    // for 声明用于迭代
    printf("For declaration for iteration:\n");
    int data[] = {10, 20, 30, 40, 50};
    for (int i = 0; i < 5; i++) {
        printf("data[%d] = %d\n", i, data[i]);
    }

    // if 声明用于条件分支
    printf("\nIf declaration for conditions:\n");
    if (int sum = data[0] + data[1] + data[2]; sum > 50) {
        printf("Sum %d is greater than 50\n", sum);
    } else {
        printf("Sum %d is not greater than 50\n", sum);
    }

    // 组合使用
    printf("\nCombined usage:\n");
    for (int i = 0; i < 5; i++) {
        if (int doubled = data[i] * 2; doubled > 50) {
            printf("data[%d] = %d, doubled = %d (> 50)\n", i, data[i], doubled);
        }
    }
}

// 模拟查找函数
int table1_data = 100;
int table2_data = 200;

int* lookup_table1(int key) {
    (void)key;
    return NULL;  // 模拟未找到
}

int* lookup_table2(int key) {
    (void)key;
    return &table2_data;  // 模拟找到
}

int main(void) {
    for_declaration_demo();
    if_declaration_demo();
    resource_comparison();
    lookup_comparison();
    iteration_vs_condition();
    return 0;
}
```

### 4.6 内存分配检查

```c
/*
 * 示例 6: 内存分配检查
 * 演示使用 if 声明进行安全的内存分配
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

// 安全的 malloc 包装
#define SAFE_MALLOC(ptr, size) \
    if ((ptr) = malloc(size); (ptr) == NULL) { \
        fprintf(stderr, "Memory allocation failed at %s:%d\n", __FILE__, __LINE__); \
    } else

// 动态数组结构
typedef struct {
    int* data;
    size_t size;
    size_t capacity;
} IntVector;

IntVector* int_vector_create(size_t initial_capacity) {
    if (IntVector* vec = malloc(sizeof(IntVector)); vec == NULL) {
        return NULL;
    } else {
        if (int* data = malloc(initial_capacity * sizeof(int)); data == NULL) {
            free(vec);
            return NULL;
        } else {
            vec->data = data;
            vec->size = 0;
            vec->capacity = initial_capacity;
            return vec;
        }
    }
}

bool int_vector_push(IntVector* vec, int value) {
    if (vec->size >= vec->capacity) {
        // 需要扩容
        size_t new_capacity = vec->capacity * 2;
        if (new_capacity == 0) new_capacity = 4;

        if (int* new_data = realloc(vec->data, new_capacity * sizeof(int));
            new_data == NULL) {
            return false;
        } else {
            vec->data = new_data;
            vec->capacity = new_capacity;
        }
    }

    vec->data[vec->size++] = value;
    return true;
}

void int_vector_free(IntVector* vec) {
    if (vec != NULL) {
        free(vec->data);
        free(vec);
    }
}

// 安全的字符串复制
char* safe_strdup(const char* str) {
    if (str == NULL) return NULL;

    size_t len = strlen(str) + 1;
    if (char* copy = malloc(len); copy == NULL) {
        return NULL;
    } else {
        memcpy(copy, str, len);
        return copy;
    }
}

// 安全的字符串连接
char* safe_strcat(const char* s1, const char* s2) {
    if (s1 == NULL || s2 == NULL) return NULL;

    size_t len1 = strlen(s1);
    size_t len2 = strlen(s2);

    if (char* result = malloc(len1 + len2 + 1); result == NULL) {
        return NULL;
    } else {
        memcpy(result, s1, len1);
        memcpy(result + len1, s2, len2 + 1);
        return result;
    }
}

// 多维度数组分配
int** allocate_matrix(size_t rows, size_t cols) {
    if (int** matrix = malloc(rows * sizeof(int*)); matrix == NULL) {
        return NULL;
    } else {
        bool allocation_failed = false;

        for (size_t i = 0; i < rows; i++) {
            if (int* row = malloc(cols * sizeof(int)); row == NULL) {
                // 清理已分配的内存
                for (size_t j = 0; j < i; j++) {
                    free(matrix[j]);
                }
                free(matrix);
                allocation_failed = true;
                break;
            } else {
                matrix[i] = row;
            }
        }

        if (allocation_failed) {
            return NULL;
        }

        return matrix;
    }
}

void free_matrix(int** matrix, size_t rows) {
    if (matrix != NULL) {
        for (size_t i = 0; i < rows; i++) {
            free(matrix[i]);
        }
        free(matrix);
    }
}

// 复杂数据结构分配
typedef struct Node {
    int value;
    struct Node* next;
} Node;

typedef struct {
    Node* head;
    size_t size;
} LinkedList;

LinkedList* linked_list_create(void) {
    if (LinkedList* list = malloc(sizeof(LinkedList)); list == NULL) {
        return NULL;
    } else {
        list->head = NULL;
        list->size = 0;
        return list;
    }
}

bool linked_list_push_front(LinkedList* list, int value) {
    if (Node* node = malloc(sizeof(Node)); node == NULL) {
        return false;
    } else {
        node->value = value;
        node->next = list->head;
        list->head = node;
        list->size++;
        return true;
    }
}

void linked_list_free(LinkedList* list) {
    if (list != NULL) {
        Node* current = list->head;
        while (current != NULL) {
            Node* next = current->next;
            free(current);
            current = next;
        }
        free(list);
    }
}

int main(void) {
    printf("=== Memory Allocation Demo ===\n\n");

    // 动态数组演示
    printf("1. Dynamic array:\n");
    if (IntVector* vec = int_vector_create(4); vec == NULL) {
        printf("Failed to create vector\n");
    } else {
        for (int i = 0; i < 10; i++) {
            if (!int_vector_push(vec, i * 10)) {
                printf("Failed to push %d\n", i);
                break;
            }
        }

        printf("Vector contents: ");
        for (size_t i = 0; i < vec->size; i++) {
            printf("%d ", vec->data[i]);
        }
        printf("\n");

        int_vector_free(vec);
    }

    // 字符串操作演示
    printf("\n2. String operations:\n");
    if (char* copy = safe_strdup("Hello, C2y!"); copy != NULL) {
        printf("Copied: %s\n", copy);

        if (char* combined = safe_strcat(copy, " Welcome!"); combined != NULL) {
            printf("Combined: %s\n", combined);
            free(combined);
        }

        free(copy);
    }

    // 矩阵分配演示
    printf("\n3. Matrix allocation:\n");
    if (int** matrix = allocate_matrix(3, 4); matrix == NULL) {
        printf("Failed to allocate matrix\n");
    } else {
        // 初始化矩阵
        for (size_t i = 0; i < 3; i++) {
            for (size_t j = 0; j < 4; j++) {
                matrix[i][j] = (int)(i * 4 + j);
            }
        }

        // 打印矩阵
        printf("Matrix:\n");
        for (size_t i = 0; i < 3; i++) {
            for (size_t j = 0; j < 4; j++) {
                printf("%3d ", matrix[i][j]);
            }
            printf("\n");
        }

        free_matrix(matrix, 3);
    }

    // 链表演示
    printf("\n4. Linked list:\n");
    if (LinkedList* list = linked_list_create(); list == NULL) {
        printf("Failed to create list\n");
    } else {
        for (int i = 0; i < 5; i++) {
            if (!linked_list_push_front(list, i * 10)) {
                printf("Failed to push %d\n", i);
                break;
            }
        }

        printf("List contents (front to back): ");
        for (Node* n = list->head; n != NULL; n = n->next) {
            printf("%d ", n->value);
        }
        printf("\n");

        linked_list_free(list);
    }

    return 0;
}
```

### 4.7 字符串解析

```c
/*
 * 示例 7: 字符串解析
 * 演示使用 if 声明进行复杂的字符串解析
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>

// 解析整数（带错误检查）
bool parse_int_safe(const char* str, int* result) {
    if (str == NULL || *str == '\0') {
        return false;
    }

    if (char* endptr; true) {
        long val = strtol(str, &endptr, 10);

        // 检查是否有未解析字符
        if (*endptr != '\0') {
            return false;
        }

        // 检查范围
        if (val < INT_MIN || val > INT_MAX) {
            return false;
        }

        *result = (int)val;
        return true;
    }
}

// 解析浮点数
bool parse_double_safe(const char* str, double* result) {
    if (str == NULL || *str == '\0') {
        return false;
    }

    if (char* endptr; true) {
        double val = strtod(str, &endptr);

        if (*endptr != '\0') {
            return false;
        }

        *result = val;
        return true;
    }
}

// 解析键值对（key=value）
bool parse_key_value(const char* str, char* key, size_t key_size,
                     char* value, size_t value_size) {
    if (str == NULL) return false;

    if (const char* eq = strchr(str, '='); eq == NULL) {
        return false;
    } else {
        size_t key_len = eq - str;
        if (key_len >= key_size) {
            return false;
        }

        memcpy(key, str, key_len);
        key[key_len] = '\0';

        strncpy(value, eq + 1, value_size - 1);
        value[value_size - 1] = '\0';

        return true;
    }
}

// 解析 CSV 行
int parse_csv_line(const char* line, char** fields, int max_fields) {
    if (line == NULL || fields == NULL || max_fields <= 0) {
        return 0;
    }

    int count = 0;
    const char* p = line;

    while (*p && count < max_fields) {
        // 跳过前导空白
        while (isspace(*p)) p++;

        if (*p == '\0') break;

        // 处理带引号的字段
        if (*p == '"') {
            p++;
            if (char* field = malloc(strlen(line) + 1); field == NULL) {
                // 清理已分配的字段
                for (int i = 0; i < count; i++) {
                    free(fields[i]);
                }
                return -1;
            } else {
                size_t i = 0;
                while (*p && *p != '"') {
                    field[i++] = *p++;
                }
                field[i] = '\0';

                if (*p == '"') p++;  // 跳过结束引号

                fields[count++] = field;
            }
        } else {
            // 处理普通字段
            if (char* field = malloc(strlen(line) + 1); field == NULL) {
                for (int i = 0; i < count; i++) {
                    free(fields[i]);
                }
                return -1;
            } else {
                size_t i = 0;
                while (*p && *p != ',' && !isspace(*p)) {
                    field[i++] = *p++;
                }
                field[i] = '\0';

                fields[count++] = field;
            }
        }

        // 跳过逗号
        if (*p == ',') p++;
    }

    return count;
}

// 解析 URL 查询参数
bool parse_query_param(const char* query, const char* param_name,
                       char* value, size_t value_size) {
    if (query == NULL || param_name == NULL) return false;

    size_t name_len = strlen(param_name);
    const char* p = query;

    while (*p) {
        // 查找参数名
        if (strncmp(p, param_name, name_len) == 0 && p[name_len] == '=') {
            p += name_len + 1;

            // 复制值
            size_t i = 0;
            while (*p && *p != '&' && i < value_size - 1) {
                value[i++] = *p++;
            }
            value[i] = '\0';

            return true;
        }

        // 跳到下一个参数
        while (*p && *p != '&') p++;
        if (*p == '&') p++;
    }

    return false;
}

// 解析时间字符串（HH:MM:SS）
bool parse_time(const char* str, int* hour, int* minute, int* second) {
    if (str == NULL) return false;

    if (int h = 0, m = 0, s = 0;
        sscanf(str, "%d:%d:%d", &h, &m, &s) == 3) {
        if (h >= 0 && h < 24 && m >= 0 && m < 60 && s >= 0 && s < 60) {
            *hour = h;
            *minute = m;
            *second = s;
            return true;
        }
    }

    return false;
}

// 解析版本号（major.minor.patch）
bool parse_version(const char* str, int* major, int* minor, int* patch) {
    if (str == NULL) return false;

    if (char* p1; true) {
        long maj = strtol(str, &p1, 10);
        if (*p1 != '.') return false;

        if (char* p2; true) {
            long min = strtol(p1 + 1, &p2, 10);
            if (*p2 != '.') return false;

            if (char* p3; true) {
                long pat = strtol(p2 + 1, &p3, 10);
                if (*p3 != '\0') return false;

                *major = (int)maj;
                *minor = (int)min;
                *patch = (int)pat;
                return true;
            }
        }
    }

    return false;
}

int main(void) {
    printf("=== String Parsing Demo ===\n\n");

    // 整数解析
    printf("1. Integer parsing:\n");
    if (int val; parse_int_safe("12345", &val)) {
        printf("  Parsed: %d\n", val);
    }
    if (int val; !parse_int_safe("12.34", &val)) {
        printf("  Correctly rejected: '12.34'\n");
    }

    // 键值对解析
    printf("\n2. Key-value parsing:\n");
    char key[64], value[256];
    if (parse_key_value("name=John Doe", key, sizeof(key), value, sizeof(value))) {
        printf("  Key: '%s', Value: '%s'\n", key, value);
    }

    // CSV 解析
    printf("\n3. CSV parsing:\n");
    char* fields[10];
    if (int count = parse_csv_line("apple,\"banana split\",cherry,date",
                                    fields, 10); count > 0) {
        printf("  Parsed %d fields:\n", count);
        for (int i = 0; i < count; i++) {
            printf("    [%d]: '%s'\n", i, fields[i]);
            free(fields[i]);
        }
    }

    // 查询参数解析
    printf("\n4. Query parameter parsing:\n");
    char param_value[256];
    if (parse_query_param("name=Alice&age=30&city=Beijing", "age",
                          param_value, sizeof(param_value))) {
        printf("  age = %s\n", param_value);
    }

    // 时间解析
    printf("\n5. Time parsing:\n");
    if (int h, m, s; parse_time("14:30:45", &h, &m, &s)) {
        printf("  Time: %02d:%02d:%02d\n", h, m, s);
    }

    // 版本号解析
    printf("\n6. Version parsing:\n");
    if (int maj, min, pat; parse_version("2.15.3", &maj, &min, &pat)) {
        printf("  Version: %d.%d.%d\n", maj, min, pat);
    }

    return 0;
}


### 4.8 复杂资源管理

```c
/*
 * 示例 8: 复杂资源管理
 * 演示使用 if 声明处理多个相关资源的获取和释放
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

// 资源类型定义
typedef struct {
    int id;
    void* data;
    size_t size;
    bool acquired;
} Resource;

typedef struct {
    Resource* resources;
    size_t count;
    size_t capacity;
} ResourcePool;

// 资源池创建
ResourcePool* resource_pool_create(size_t capacity) {
    if (ResourcePool* pool = malloc(sizeof(ResourcePool)); pool == NULL) {
        return NULL;
    } else {
        if (Resource* res = calloc(capacity, sizeof(Resource)); res == NULL) {
            free(pool);
            return NULL;
        } else {
            pool->resources = res;
            pool->count = 0;
            pool->capacity = capacity;
            return pool;
        }
    }
}

void resource_pool_destroy(ResourcePool* pool) {
    if (pool != NULL) {
        for (size_t i = 0; i < pool->count; i++) {
            if (pool->resources[i].acquired) {
                free(pool->resources[i].data);
            }
        }
        free(pool->resources);
        free(pool);
    }
}

// 数据库连接模拟
typedef struct {
    int conn_id;
    bool connected;
    char server[256];
} DBConnection;

DBConnection* db_connect(const char* server) {
    if (DBConnection* conn = malloc(sizeof(DBConnection)); conn == NULL) {
        return NULL;
    } else {
        conn->conn_id = rand();
        conn->connected = true;
        strncpy(conn->server, server, sizeof(conn->server) - 1);
        conn->server[sizeof(conn->server) - 1] = '\0';
        printf("Connected to %s (conn_id=%d)\n", server, conn->conn_id);
        return conn;
    }
}

void db_disconnect(DBConnection* conn) {
    if (conn != NULL) {
        if (conn->connected) {
            printf("Disconnected from %s (conn_id=%d)\n",
                   conn->server, conn->conn_id);
            conn->connected = false;
        }
        free(conn);
    }
}

// 事务模拟
typedef struct {
    int tx_id;
    bool active;
    DBConnection* conn;
} Transaction;

Transaction* transaction_begin(DBConnection* conn) {
    if (Transaction* tx = malloc(sizeof(Transaction)); tx == NULL) {
        return NULL;
    } else {
        tx->tx_id = rand();
        tx->active = true;
        tx->conn = conn;
        printf("Transaction %d started\n", tx->tx_id);
        return tx;
    }
}

void transaction_commit(Transaction* tx) {
    if (tx != NULL && tx->active) {
        printf("Transaction %d committed\n", tx->tx_id);
        tx->active = false;
    }
}

void transaction_rollback(Transaction* tx) {
    if (tx != NULL && tx->active) {
        printf("Transaction %d rolled back\n", tx->tx_id);
        tx->active = false;
    }
}

void transaction_free(Transaction* tx) {
    if (tx != NULL) {
        if (tx->active) {
            transaction_rollback(tx);
        }
        free(tx);
    }
}

// 复杂操作：多资源协调
bool perform_complex_operation(const char* server) {
    printf("\nStarting complex operation...\n");

    // 1. 建立数据库连接
    if (DBConnection* conn = db_connect(server); conn == NULL) {
        fprintf(stderr, "Failed to connect to database\n");
        return false;
    } else {
        // 2. 开始事务
        if (Transaction* tx = transaction_begin(conn); tx == NULL) {
            fprintf(stderr, "Failed to start transaction\n");
            db_disconnect(conn);
            return false;
        } else {
            // 3. 创建资源池
            if (ResourcePool* pool = resource_pool_create(10); pool == NULL) {
                fprintf(stderr, "Failed to create resource pool\n");
                transaction_rollback(tx);
                transaction_free(tx);
                db_disconnect(conn);
                return false;
            } else {
                // 4. 分配缓冲区
                if (char* buffer = malloc(4096); buffer == NULL) {
                    fprintf(stderr, "Failed to allocate buffer\n");
                    resource_pool_destroy(pool);
                    transaction_rollback(tx);
                    transaction_free(tx);
                    db_disconnect(conn);
                    return false;
                } else {
                    // 5. 执行操作
                    bool success = true;

                    // 模拟操作...
                    strcpy(buffer, "Operation data");
                    printf("Buffer content: %s\n", buffer);

                    if (success) {
                        transaction_commit(tx);
                        printf("Operation completed successfully\n");
                    } else {
                        transaction_rollback(tx);
                        printf("Operation failed\n");
                    }

                    // 清理（按相反顺序）
                    free(buffer);
                }
                resource_pool_destroy(pool);
            }
            transaction_free(tx);
        }
        db_disconnect(conn);
    }

    return true;
}

// 使用 goto 清理的对比版本
bool perform_with_goto(const char* server) {
    printf("\nStarting operation with goto...\n");

    DBConnection* conn = db_connect(server);
    if (conn == NULL) goto cleanup_none;

    Transaction* tx = transaction_begin(conn);
    if (tx == NULL) goto cleanup_conn;

    ResourcePool* pool = resource_pool_create(10);
    if (pool == NULL) goto cleanup_tx;

    char* buffer = malloc(4096);
    if (buffer == NULL) goto cleanup_pool;

    // 执行操作
    strcpy(buffer, "Operation data");
    printf("Buffer content: %s\n", buffer);

    transaction_commit(tx);
    printf("Operation completed successfully\n");

    free(buffer);
cleanup_pool:
    resource_pool_destroy(pool);
cleanup_tx:
    transaction_free(tx);
cleanup_conn:
    db_disconnect(conn);
cleanup_none:
    return (conn != NULL);
}

// 改进版：使用辅助函数减少嵌套
bool perform_with_helpers(const char* server) {
    printf("\nStarting operation with helpers...\n");

    if (DBConnection* conn = db_connect(server); conn == NULL) {
        return false;
    }

    bool result = perform_with_connection(conn);
    db_disconnect(conn);
    return result;
}

bool perform_with_connection(DBConnection* conn) {
    if (Transaction* tx = transaction_begin(conn); tx == NULL) {
        return false;
    } else {
        bool result = perform_with_transaction(tx);
        transaction_free(tx);
        return result;
    }
}

bool perform_with_transaction(Transaction* tx) {
    if (ResourcePool* pool = resource_pool_create(10); pool == NULL) {
        transaction_rollback(tx);
        return false;
    } else {
        bool result = perform_with_pool(tx, pool);
        resource_pool_destroy(pool);
        return result;
    }
}

bool perform_with_pool(Transaction* tx, ResourcePool* pool) {
    (void)pool;  // 未使用

    if (char* buffer = malloc(4096); buffer == NULL) {
        transaction_rollback(tx);
        return false;
    } else {
        strcpy(buffer, "Operation data");
        printf("Buffer content: %s\n", buffer);

        transaction_commit(tx);
        printf("Operation completed successfully\n");

        free(buffer);
        return true;
    }
}

// 文件处理链
bool process_files_chain(const char** filenames, size_t count) {
    if (count == 0) return true;

    if (FILE* fp1 = fopen(filenames[0], "r"); fp1 == NULL) {
        perror(filenames[0]);
        return false;
    } else {
        printf("Processing %s...\n", filenames[0]);

        if (count == 1) {
            fclose(fp1);
            return true;
        }

        if (FILE* fp2 = fopen(filenames[1], "r"); fp2 == NULL) {
            perror(filenames[1]);
            fclose(fp1);
            return false;
        } else {
            printf("Processing %s...\n", filenames[1]);

            // 继续链式处理...
            // 在实际应用中，这可能是一个递归或循环

            fclose(fp2);
        }
        fclose(fp1);
    }

    return true;
}

int main(void) {
    printf("=== Complex Resource Management Demo ===\n");

    // 使用 if 声明的方式
    perform_complex_operation("localhost");

    // 使用 goto 的方式
    perform_with_goto("localhost");

    // 使用辅助函数的方式
    perform_with_helpers("localhost");

    // 文件处理链
    printf("\n=== File Chain Processing ===\n");
    const char* files[] = {"file1.txt", "file2.txt"};
    process_files_chain(files, 2);

    return 0;
}
```

---

## 五、与 C++17 if-init 对比

### 5.1 语法差异

| 特性 | C++17 if-init | C2y if 声明 |
|------|---------------|-------------|
| 语法形式 | `if (init; cond)` | `if (decl; cond)` |
| 初始化语句类型 | 表达式语句 | 声明语句 |
| 变量声明 | ✅ 支持 | ✅ 支持 |
| 多变量声明 | ✅ 支持 | ✅ 支持 |
| 结构化绑定 | ✅ C++17 支持 | ❌ C 不支持 |
| 类型推导 | ✅ auto | ✅ typeof |
| 复合赋值 | ✅ 支持 | ✅ 支持 |

```cpp
// C++17 示例
if (auto it = map.find(key); it != map.end()) {
    // 使用 it
}

// C++17 结构化绑定（C 不支持）
if (auto [iter, inserted] = set.insert(value); inserted) {
    // iter 和 inserted 都可用
}
```

```c
// C2y 等效代码
if (Entry* it = hash_table_find(table, key); it != NULL) {
    // 使用 it
}

// C2y 需要手动解构（C 不支持结构化绑定）
if (InsertResult result = hash_table_insert(table, key, value); result.inserted) {
    // result.iter 和 result.inserted 可用
}
```

### 5.2 语义兼容

C2y if 声明与 C++17 if-init 在语义上高度兼容，主要区别在于：

```text
┌─────────────────────────────────────────────────────────────────────┐
│                     语义兼容性分析                                    │
├─────────────────────────────────────────────────────────────────────┤
│                                                                      │
│  共同点:                                                             │
│  ─────────────────                                                   │
│  1. 声明的变量在 if 和 else 分支都可见                                │
│  2. 变量作用域延伸到整个 if-else 结构结束                              │
│  3. 支持 else if 链式结构                                             │
│  4. 支持任意复杂的条件表达式                                           │
│                                                                      │
│  差异点:                                                             │
│  ─────────────────                                                   │
│  1. C++ 支持表达式语句（包括函数调用、赋值等）                          │
│     C2y 仅支持声明语句                                                │
│  2. C++ 支持 auto 类型推导                                             │
│     C2y 使用 typeof（C23）                                            │
│  3. C++ 支持 constexpr if（编译时条件）                                │
│     C2y 无此特性                                                      │
│                                                                      │
└─────────────────────────────────────────────────────────────────────┘
```

#### 表达式语句的转换

```cpp
// C++: 表达式语句
if (count++; count > threshold) {
    // count 已递增
}
```

```c
// C2y: 需要改写为声明语句
if (int new_count = count + 1; new_count > threshold) {
    count = new_count;
    // 使用 new_count
}
```

### 5.3 互操作性

C 和 C++ 的 if-init/if 声明可以在头文件中实现某种程度的互操作：

```c
// common.h - 条件编译实现互操作
#ifdef __cplusplus
    #define IF_DECL(type, name, init, cond) \
        if (type name = (init); (cond))
#else
    // C2y
    #define IF_DECL(type, name, init, cond) \
        if (type name = (init); (cond))
#endif

// 使用示例
void example(void) {
    IF_DECL(FILE*, fp, fopen("file.txt", "r"), fp != NULL) {
        // 处理文件
        fclose(fp);
    } else {
        // 错误处理
    }
}
```

---

## 六、实现原理

### 6.1 编译器如何实现

if 声明的实现相对直接，编译器需要进行以下处理：

```text
┌─────────────────────────────────────────────────────────────────────┐
│                      编译器实现分析                                   │
├─────────────────────────────────────────────────────────────────────┤
│                                                                      │
│  词法分析:                                                           │
│  ─────────────────                                                   │
│  识别新的语法结构：if (declaration; expression)                      │
│  分号在 declaration 和 expression 之间是必需的                       │
│                                                                      │
│  语法分析:                                                           │
│  ─────────────────                                                   │
│  selection_statement:                                                │
│      IF '(' declaration expression ')' statement                     │
│      IF '(' declaration expression ')' statement ELSE statement      │
│                                                                      │
│  语义分析:                                                           │
│  ─────────────────                                                   │
│  1. 创建新的作用域                                                   │
│  2. 处理 declaration                                                 │
│  3. 在 declaration 的作用域中求值 expression                          │
│  4. 根据条件选择分支                                                  │
│  5. 两个分支共享 declaration 的作用域                                  │
│  6. 整个 if-else 结束后销毁作用域                                      │
│                                                                      │
│  代码生成:                                                           │
│  ─────────────────                                                   │
│  if 声明生成的代码与传统方式等价：                                     │
│                                                                      │
│  if (T x = init; cond) { ... } else { ... }                          │
│  等价于：                                                            │
│  {                                                                   │
│      T x = init;                                                     │
│      if (cond) { ... } else { ... }                                  │
│  }                                                                   │
│                                                                      │
└─────────────────────────────────────────────────────────────────────┘
```

### 6.2 语法糖分析

从语义角度看，if 声明可以被视为一种语法糖：

```c
// C2y if 声明
if (FILE* fp = fopen("file.txt", "r"); fp != NULL) {
    process(fp);
    fclose(fp);
} else {
    error_handler();
}
```

```c
// 等效的传统 C 代码
{
    FILE* fp = fopen("file.txt", "r");
    if (fp != NULL) {
        process(fp);
        fclose(fp);
    } else {
        error_handler();
    }
}
// fp 超出作用域
```

虽然语法糖的分析是正确的，但 if 声明提供了更好的：

1. **代码可读性** - 声明和条件检查在同一行
2. **意图表达** - 明确表示变量的用途
3. **错误预防** - 减少忘记添加额外作用域块的风险

### 6.3 性能开销

if 声明是**零开销抽象**：

```text
┌─────────────────────────────────────────────────────────────────────┐
│                        性能分析                                       │
├─────────────────────────────────────────────────────────────────────┤
│                                                                      │
│  运行时开销:                                                         │
│  ─────────────────                                                   │
│  ✅ 零开销 - 生成的代码与手动添加作用域块完全相同                      │
│  ✅ 没有额外的函数调用                                                 │
│  ✅ 没有额外的内存分配                                                 │
│                                                                      │
│  编译时开销:                                                         │
│  ─────────────────                                                   │
│  • 微乎其微 - 仅增加作用域管理的复杂度                                 │
│  • 对编译时间的影响可以忽略                                           │
│                                                                      │
│  代码大小:                                                           │
│  ─────────────────                                                   │
│  ✅ 与等效的传统代码生成相同的机器码                                   │
│  ✅ 不增加二进制文件大小                                               │
│                                                                      │
└─────────────────────────────────────────────────────────────────────┘
```

### 6.4 与块作用域的关系

if 声明与普通块作用域的关系：

```c
void scope_relationship(void) {
    int outer = 1;

    // 普通块作用域
    {
        int inner = 2;
        printf("inner = %d\n", inner);
    }
    // inner 不可见

    // if 声明作用域
    if (int if_var = 3; if_var > 0) {
        printf("if_var = %d\n", if_var);
        int if_inner = 4;
        printf("if_inner = %d\n", if_inner);
    } else {
        printf("else: if_var = %d\n", if_var);  // if_var 仍可见
    }
    // if_var 和 if_inner 都不可见

    // 嵌套块与 if 声明
    {
        int block_var = 5;
        if (int if_var2 = 6; if_var2 > 0) {
            printf("block_var = %d, if_var2 = %d\n", block_var, if_var2);
        }
        // if_var2 不可见，但 block_var 仍可见
        printf("block_var = %d\n", block_var);
    }
}
```

---

## 七、最佳实践

### 7.1 何时使用 if 声明

if 声明特别适合以下场景：

| 场景 | 示例 | 推荐度 |
|------|------|--------|
| 资源获取与检查 | `if (FILE* f = fopen(...); f)` | ⭐⭐⭐⭐⭐ |
| 查找操作 | `if (Node* n = find(...); n)` | ⭐⭐⭐⭐⭐ |
| 内存分配 | `if (void* p = malloc(...); p)` | ⭐⭐⭐⭐⭐ |
| 类型转换检查 | `if (int val = atoi(...); val > 0)` | ⭐⭐⭐⭐ |
| 临时计算 | `if (int sum = a + b; sum > threshold)` | ⭐⭐⭐⭐ |
| 简单布尔检查 | `if (bool flag = check(); flag)` | ⭐⭐⭐ |

```c
// ✅ 推荐：资源获取
if (FILE* fp = fopen("data.txt", "r"); fp != NULL) {
    // 使用 fp
    fclose(fp);
}

// ✅ 推荐：查找操作
if (User* user = find_user_by_id(id); user != NULL) {
    return user->name;
}

// ⚠️ 谨慎：简单情况
if (bool is_valid = validate(input); is_valid) {
    // 可以，但可能过度
}
// 替代：直接调用
if (validate(input)) {
    // 更简洁
}
```

### 7.2 代码可读性考虑

使用 if 声明时需要注意可读性：

```c
// ✅ 好的实践：清晰的意图
if (FILE* config = open_config_file(); config != NULL) {
    load_settings(config);
    close_config(config);
}

// ❌ 不好的实践：过于复杂的声明
if (int a = compute(), b = another_compute(), c = yet_another();
    a + b * c > threshold && some_condition(a)) {
    // 难以阅读
}

// ✅ 改进：分解复杂声明
int a = compute();
int b = another_compute();
int c = yet_another();
if (a + b * c > threshold && some_condition(a)) {
    // 更清晰
}

// ✅ 好的实践：有意义的变量名
if (FILE* input_file = fopen(filename, "r"); input_file != NULL) {
    // ...
}

// ❌ 不好的实践：无意义的缩写
if (FILE* f = fopen(fn, "r"); f) {
    // ...
}
```

### 7.3 与 goto 的替代关系

if 声明可以替代许多 goto 的使用场景：

```c
// 传统 goto 方式（资源清理）
bool process_with_goto(const char* filename) {
    FILE* fp = fopen(filename, "r");
    if (fp == NULL) goto cleanup_none;

    char* buffer = malloc(1024);
    if (buffer == NULL) goto cleanup_fp;

    // 处理...

    free(buffer);
cleanup_fp:
    fclose(fp);
cleanup_none:
    return fp != NULL;
}

// C2y if 声明方式
bool process_with_if_decl(const char* filename) {
    if (FILE* fp = fopen(filename, "r"); fp == NULL) {
        return false;
    } else {
        if (char* buffer = malloc(1024); buffer == NULL) {
            fclose(fp);
            return false;
        } else {
            // 处理...

            free(buffer);
        }
        fclose(fp);
    }
    return true;
}

// 更深层嵌套的情况
typedef struct ResourceA { int id; } ResourceA;
typedef struct ResourceB { int id; } ResourceB;
typedef struct ResourceC { int id; } ResourceC;

ResourceA* acquire_a(void) { return malloc(sizeof(ResourceA)); }
ResourceB* acquire_b(void) { return malloc(sizeof(ResourceB)); }
ResourceC* acquire_c(void) { return malloc(sizeof(ResourceC)); }
void release_a(ResourceA* a) { free(a); }
void release_b(ResourceB* b) { free(b); }
void release_c(ResourceC* c) { free(c); }

// goto 方式
void nested_cleanup_goto(void) {
    ResourceA* a = acquire_a();
    if (a == NULL) goto cleanup_none;

    ResourceB* b = acquire_b();
    if (b == NULL) goto cleanup_a;

    ResourceC* c = acquire_c();
    if (c == NULL) goto cleanup_b;

    // 使用 a, b, c

    release_c(c);
cleanup_b:
    release_b(b);
cleanup_a:
    release_a(a);
cleanup_none:
    ;
}

// if 声明方式（嵌套较深）
void nested_cleanup_if(void) {
    if (ResourceA* a = acquire_a(); a == NULL) {
        return;
    } else {
        if (ResourceB* b = acquire_b(); b == NULL) {
            release_a(a);
        } else {
            if (ResourceC* c = acquire_c(); c == NULL) {
                release_b(b);
                release_a(a);
            } else {
                // 使用 a, b, c

                release_c(c);
                release_b(b);
            }
            release_a(a);
        }
    }
}

// 折中方案：辅助函数
void nested_cleanup_helpers(void) {
    if (ResourceA* a = acquire_a(); a != NULL) {
        process_with_a(a);
        release_a(a);
    }
}

void process_with_a(ResourceA* a) {
    if (ResourceB* b = acquire_b(); b != NULL) {
        process_with_b(a, b);
        release_b(b);
    }
}

void process_with_b(ResourceA* a, ResourceB* b) {
    (void)a;  // 使用 a
    if (ResourceC* c = acquire_c(); c != NULL) {
        // 使用 a, b, c
        release_c(c);
    }
}
```

---

## 八、编译器支持

### 8.1 Clang 22+ 支持

Clang 22 开始实验性支持 C2y 的 if 声明特性：

```bash
# 启用 C2y 模式
---

## 🔗 文档关联

### 核心关联
| 文档 | 关系类型 | 说明 |
|:-----|:---------|:-----|
| [内存管理](../../../01_Core_Knowledge_System/02_Core_Layer/02_Memory_Management.md) | 核心关联 | 内存管理基础 |
| [指针深度](../../../01_Core_Knowledge_System/02_Core_Layer/01_Pointer_Depth.md) | 核心关联 | 指针深度基础 |
| [并发编程](../../../03_System_Technology_Domains/14_Concurrency_Parallelism/readme.md) | 核心关联 | 并发编程基础 |
| [数据类型](../../../01_Core_Knowledge_System/01_Basic_Layer/02_Data_Type_System.md) | 核心关联 | 数据类型基础 |
| [数组与指针](../../../01_Core_Knowledge_System/02_Core_Layer/05_Arrays_Pointers.md) | 核心关联 | 数组与指针基础 |

### 扩展阅读
| 文档 | 关系类型 | 说明 |
|:-----|:---------|:-----|
| [软件工程](../../../01_Core_Knowledge_System/05_Engineering_Layer/readme.md) | 核心关联 | 软件工程基础 |
| [形式语义](../../../02_Formal_Semantics_and_Physics/readme.md) | 核心关联 | 形式语义基础 |
| [系统技术](../../../03_System_Technology_Domains/readme.md) | 核心关联 | 系统技术基础 |
| [工业场景](../../../04_Industrial_Scenarios/readme.md) | 核心关联 | 工业场景基础 |
| [思维表征](../../../06_Thinking_Representation/readme.md) | 核心关联 | 思维表征基础 |
clang -std=c2y -c source.c

# 或明确指定 C2y 版本
clang -std=iso9899:202y -c source.c
```

Clang 支持状态：

| 特性 | 支持状态 | 版本 |
|------|----------|------|
| if 声明 | 实验性 | Clang 22+ |
| 多变量声明 | 支持 | Clang 22+ |
| else if 链 | 支持 | Clang 22+ |

### 8.2 GCC 支持状态

GCC 对 C2y if 声明的支持正在开发中：

| GCC 版本 | 支持状态 | 备注 |
|----------|----------|------|
| GCC 14 | 不支持 | - |
| GCC 15 | 计划中 | 预计实验性支持 |
| GCC 16+ | 待定 | - |

```bash
# GCC 支持后预计的使用方式
gcc -std=c2y -c source.c
```

### 8.3 使用方法

在编译器完全支持之前，可以使用以下方式编写兼容代码：

```c
// feature_test.h - 特性检测
#ifndef C2Y_IF_DECLARATION_SUPPORTED
    #if defined(__clang__) && (__clang_major__ >= 22)
        #if __has_feature(c_if_declaration)
            #define C2Y_IF_DECLARATION_SUPPORTED 1
        #endif
    #elif defined(__GNUC__) && (__GNUC__ >= 15)
        #define C2Y_IF_DECLARATION_SUPPORTED 1
    #endif
#endif

// compat.h - 兼容性宏
#ifdef C2Y_IF_DECLARATION_SUPPORTED
    #define IF_DECL(type, name, init, cond) \
        if (type name = (init); (cond))
#else
    // 回退到传统方式
    #define IF_DECL(type, name, init, cond) \
        { type name = (init); if (cond)
    #define IF_DECL_END } }
#endif

// 使用示例
void example(void) {
    IF_DECL(FILE*, fp, fopen("file.txt", "r"), fp != NULL) {
        // 处理文件
        fclose(fp);
    }
#ifndef C2Y_IF_DECLARATION_SUPPORTED
    IF_DECL_END
#endif
}
```

---

## 参考资源

1. **ISO/IEC 9899:C2y** - C2y 标准草案（预计 2026-2027 发布）
2. **N3356** - if 声明提案文档
3. **C++17 Standard** - if-init 语句参考（ISO/IEC 14882:2017）
4. **Clang C2y Status** - <https://clang.llvm.org/c_status.html>
5. **GCC C2y Roadmap** - <https://gcc.gnu.org/wiki/C2x>
6. **WG14 文档** - <http://www.open-std.org/jtc1/sc22/wg14/>

---

## 总结

C2y if 声明是一项重要的语言特性，它：

1. **提高了代码安全性** - 通过限制临时变量的作用域
2. **改善了代码可读性** - 将声明和条件检查结合
3. **减少了资源泄漏风险** - 变量在正确的作用域内自动超出范围
4. **与 C++ 兼容** - 便于 C/C++ 混合代码的维护

虽然这是一个相对简单的语法糖，但它解决了 C 语言中长期存在的代码质量问题，是现代 C 编程的重要工具。

---

> **文档状态**: 初版完成 | **验证**: 基于 C2y 草案 N3356 | **最后更新**: 2026-03-19


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
