# WebAssembly 与 C 语言入门指南


> **版本**: v1.0.0
> **最后更新**: 2026-03-25
> **作者**: C_Lang Team
> **难度**: L3 进阶

---

## 目录

- [WebAssembly 与 C 语言入门指南](#webassembly-与-c-语言入门指南)
  - [目录](#目录)
  - [1. WebAssembly 概述](#1-webassembly-概述)
    - [1.1 什么是 WebAssembly](#11-什么是-webassembly)
    - [1.2 WebAssembly 的历史与发展](#12-webassembly-的历史与发展)
    - [1.3 WebAssembly 的技术定位](#13-webassembly-的技术定位)
    - [1.4 WebAssembly 的应用场景](#14-webassembly-的应用场景)
  - [2. WebAssembly 架构深度解析](#2-webassembly-架构深度解析)
    - [2.1 WebAssembly 模块结构](#21-webassembly-模块结构)
    - [2.2 线性内存模型](#22-线性内存模型)
    - [2.3 栈式虚拟机执行模型](#23-栈式虚拟机执行模型)
    - [2.4 类型系统](#24-类型系统)
    - [2.5 指令集概述](#25-指令集概述)
  - [3. 为什么选择 C/C++ 开发 WASM](#3-为什么选择-cc-开发-wasm)
    - [3.1 C/C++ 作为源语言的优势](#31-cc-作为源语言的优势)
    - [3.2 与其他语言的对比](#32-与其他语言的对比)
    - [3.3 C/C++ 到 WebAssembly 的编译流程](#33-cc-到-webassembly-的编译流程)
    - [3.4 C/C++ 特定优化](#34-cc-特定优化)
  - [4. Emscripten 工具链详解](#4-emscripten-工具链详解)
    - [4.1 Emscripten 架构](#41-emscripten-架构)
    - [4.2 安装 Emscripten SDK](#42-安装-emscripten-sdk)
      - [4.2.1 使用 Git 安装（推荐）](#421-使用-git-安装推荐)
      - [4.2.2 验证安装](#422-验证安装)
    - [4.3 Emscripten 目录结构](#43-emscripten-目录结构)
    - [4.4 核心工具介绍](#44-核心工具介绍)
  - [5. 从 C/C++ 编译到 WASM 的完整流程](#5-从-cc-编译到-wasm-的完整流程)
    - [5.1 基础编译命令](#51-基础编译命令)
    - [5.2 编译选项详解](#52-编译选项详解)
    - [5.3 编译流程图](#53-编译流程图)
  - [6. 第一个 "Hello, WASM" 程序](#6-第一个-hello-wasm-程序)
    - [6.1 最简单的 C 程序](#61-最简单的-c-程序)
    - [6.2 运行程序](#62-运行程序)
    - [6.3 程序结构解析](#63-程序结构解析)
    - [6.4 导出自定义函数](#64-导出自定义函数)
    - [6.5 在 HTML 中使用导出的函数](#65-在-html-中使用导出的函数)
  - [7. 浏览器中运行 WASM 的方法](#7-浏览器中运行-wasm-的方法)
    - [7.1 使用 JavaScript API 加载 WASM](#71-使用-javascript-api-加载-wasm)
      - [7.1.1 基本加载方法](#711-基本加载方法)
      - [7.1.2 完整的 WASM 加载器类](#712-完整的-wasm-加载器类)
    - [7.2 在不同环境中的运行](#72-在不同环境中的运行)
      - [7.2.1 在浏览器中运行](#721-在浏览器中运行)
      - [7.2.2 在 Web Worker 中运行](#722-在-web-worker-中运行)
      - [7.2.3 在 Node.js 中运行](#723-在-nodejs-中运行)
    - [7.3 与 JavaScript 的交互模式](#73-与-javascript-的交互模式)
  - [8. 实战项目](#8-实战项目)
    - [8.1 项目1: 图像处理库](#81-项目1-图像处理库)
    - [8.2 项目2: 高性能矩阵运算库](#82-项目2-高性能矩阵运算库)
  - [9. 常见问题与解决方案](#9-常见问题与解决方案)
    - [9.1 编译问题](#91-编译问题)
    - [9.2 运行时问题](#92-运行时问题)
    - [9.3 性能优化建议](#93-性能优化建议)
  - [10. 参考资源](#10-参考资源)
    - [官方文档](#官方文档)
    - [工具和库](#工具和库)
    - [学习资源](#学习资源)
  - [附录：完整的命令速查表](#附录完整的命令速查表)
  - [深入理解](#深入理解)
    - [核心原理](#核心原理)
    - [实践应用](#实践应用)
    - [最佳实践](#最佳实践)

---

## 1. WebAssembly 概述

### 1.1 什么是 WebAssembly

WebAssembly（简称 WASM）是一种现代的、高效的、低级的字节码格式，专为浏览器和执行环境设计。它是一种可移植、大小和加载高效的二进制代码格式，可以作为 C、C++、Rust 等高级语言的编译目标。

WebAssembly 的核心特点包括：

| 特性 | 描述 | 优势 |
|------|------|------|
| **二进制格式** | 紧凑的二进制表示 | 加载速度快，解析效率高 |
| **文本格式** | 可读的文本表示（WAT） | 便于调试和学习 |
| **栈式虚拟机** | 基于栈的执行模型 | 执行效率高，易于验证 |
| **沙箱环境** | 安全的执行环境 | 防止恶意代码执行 |
| **语言无关** | 多种语言的编译目标 | 灵活性高 |
| **高性能** | 接近原生的执行速度 | 适合计算密集型任务 |

### 1.2 WebAssembly 的历史与发展

WebAssembly 的发展历程：

```
2015年 - WebAssembly 社区组成立
    ↓
2017年3月 - MVP（最小可行产品）版本发布
    ↓
2017年11月 - Chrome, Firefox, Safari, Edge 四大浏览器支持
    ↓
2019年12月 - WebAssembly 成为 W3C 推荐标准
    ↓
2021年 - SIMD 提案标准化
    ↓
2022年 - 异常处理、线程等特性逐步完善
    ↓
2023年 - GC（垃圾回收）、尾调用等特性推进
    ↓
2024年 - WASI（WebAssembly System Interface）持续发展
```

### 1.3 WebAssembly 的技术定位

WebAssembly 在现代 Web 技术栈中的位置：

```
┌─────────────────────────────────────────────────────────────┐
│                      Web Application                        │
├─────────────────────────────────────────────────────────────┤
│  HTML5  │  CSS3  │  JavaScript  │  WebAssembly  │  Web API  │
├─────────────────────────────────────────────────────────────┤
│                    JavaScript Engine                        │
│         ┌─────────────┐    ┌─────────────┐                  │
│         │  WASM VM    │    │  JS VM      │                  │
│         └─────────────┘    └─────────────┘                  │
├─────────────────────────────────────────────────────────────┤
│                     Browser Runtime                         │
├─────────────────────────────────────────────────────────────┤
│              Operating System / Hardware                    │
└─────────────────────────────────────────────────────────────┘
```

### 1.4 WebAssembly 的应用场景

WebAssembly 非常适合以下场景：

1. **图像/视频处理**
   - 图像滤镜、格式转换
   - 视频编解码
   - 计算机视觉算法

2. **游戏开发**
   - 3D 游戏引擎
   - 物理模拟
   - 游戏逻辑计算

3. **科学计算**
   - 数值分析
   - 仿真模拟
   - 数据可视化

4. **音频处理**
   - 音频合成
   - 音效处理
   - 音乐制作工具

5. **密码学**
   - 加密解密操作
   - 哈希计算
   - 区块链应用

6. **编程语言实现**
   - 在浏览器中运行其他语言
   - 在线 IDE 和编译器

---

## 2. WebAssembly 架构深度解析

### 2.1 WebAssembly 模块结构

一个 WebAssembly 模块由以下部分组成：

```
┌─────────────────────────────────────────────────────────────┐
│                    WebAssembly Module                       │
├─────────────────────────────────────────────────────────────┤
│  Magic Number    │  0x00 0x61 0x73 0x6D ("\0asm")           │
│  Version         │  0x01 0x00 0x00 0x00 (版本1)              │
├─────────────────────────────────────────────────────────────┤
│  Section 1: Type      │  函数签名声明                          │
│  Section 2: Import    │  导入函数、内存、表、全局变量           │
│  Section 3: Function  │  函数体声明                            │
│  Section 4: Table     │  间接函数调用表                        │
│  Section 5: Memory    │  线性内存声明                          │
│  Section 6: Global    │  全局变量声明                          │
│  Section 7: Export    │  导出函数、内存、表、全局变量           │
│  Section 8: Start     │  启动函数                              │
│  Section 9: Element   │  表初始化元素                          │
│  Section 10: Code     │  函数体代码                            │
│  Section 11: Data     │  内存初始化数据                        │
│  Section 12: DataCount│  数据段计数（用于验证）                 │
└─────────────────────────────────────────────────────────────┘
```

### 2.2 线性内存模型

WebAssembly 使用线性内存（Linear Memory）作为其主要存储机制：

```
线性内存布局（64位地址空间概念图）

┌─────────────────────────────────────────────────────────────┐
│  地址 0x00000000                                            │
│  ┌───────────────────────────────────────────────────────┐  │
│  │  Stack (栈) - 向下增长                                  │  │
│  │  局部变量、函数调用信息                                  │  │
│  └───────────────────────────────────────────────────────┘  │
│                        ↓                                    │
│                        ...                                  │
│                        ↑                                    │
│  ┌───────────────────────────────────────────────────────┐  │
│  │  Heap (堆) - 向上增长                                   │  │
│  │  动态分配的内存 (malloc/free)                           │  │
│  └───────────────────────────────────────────────────────┘  │
│                        ↑                                    │
│  ┌───────────────────────────────────────────────────────┐  │
│  │  Static Data (静态数据)                                 │  │
│  │  全局变量、字符串常量、静态数组                           │  │
│  └───────────────────────────────────────────────────────┘  │
│                                                             │
│  最大内存: 2GB (默认) / 4GB (最大)                           │
└─────────────────────────────────────────────────────────────┘
```

### 2.3 栈式虚拟机执行模型

WebAssembly 采用基于栈的执行模型：

```c
// C 代码示例
int add(int a, int b) {
    return a + b;
}

// 对应的 WebAssembly 文本格式 (WAT)
(module
  (func $add (param i32 i32) (result i32)
    local.get 0    // 将第一个参数压入栈
    local.get 1    // 将第二个参数压入栈
    i32.add        // 弹出两个值，相加，压入结果
  )
  (export "add" (func $add))
)

// 执行栈状态变化
// 步骤1: local.get 0    → 栈: [a]
// 步骤2: local.get 1    → 栈: [a, b]
// 步骤3: i32.add        → 栈: [a+b]
```

### 2.4 类型系统

WebAssembly 的类型系统相对简单但高效：

| 类型 | 描述 | C语言对应 |
|------|------|-----------|
| `i32` | 32位整数 | `int32_t` |
| `i64` | 64位整数 | `int64_t` |
| `f32` | 32位浮点数 | `float` |
| `f64` | 64位浮点数 | `double` |
| `v128` | 128位SIMD向量 | `__m128` (SIMD) |
| `funcref` | 函数引用 | 函数指针 |
| `externref` | 外部引用 | 任意JS对象 |

### 2.5 指令集概述

WebAssembly 指令分类：

```
┌─────────────────────────────────────────────────────────────┐
│                  WebAssembly Instructions                   │
├─────────────────────────────────────────────────────────────┤
│  控制流指令                                                 │
│  ├─ block, loop, if, else, end                              │
│  ├─ br, br_if, br_table                                     │
│  └─ return, unreachable                                     │
├─────────────────────────────────────────────────────────────┤
│  参数指令                                                   │
│  ├─ drop, select                                            │
│  └─ local.get, local.set, local.tee                         │
│  └─ global.get, global.set                                  │
├─────────────────────────────────────────────────────────────┤
│  内存指令                                                   │
│  ├─ i32.load, i64.load, f32.load, f64.load                  │
│  ├─ i32.store, i64.store, f32.store, f64.store              │
│  ├─ memory.size, memory.grow                                │
│  └─ memory.fill, memory.copy, memory.init                    │
├─────────────────────────────────────────────────────────────┤
│  数值指令                                                   │
│  ├─ i32.const, i64.const, f32.const, f64.const              │
│  ├─ i32.add, i32.sub, i32.mul, i32.div_s/u                  │
│  ├─ i32.and, i32.or, i32.xor, i32.shl, i32.shr              │
│  ├─ f32.add, f32.sub, f32.mul, f32.div, f32.sqrt            │
│  └─ 类型转换: i32.wrap_i64, f32.convert_i32_s 等             │
├─────────────────────────────────────────────────────────────┤
│  引用指令                                                   │
│  ├─ ref.null, ref.is_null, ref.func                         │
│  └─ ref.eq (GC提案)                                          │
├─────────────────────────────────────────────────────────────┤
│  SIMD指令 (128位向量操作)                                    │
│  ├─ v128.load, v128.store                                   │
│  ├─ i8x16.add, i16x8.add, i32x4.add                         │
│  ├─ f32x4.add, f32x4.mul, f32x4.sqrt                        │
│  └─ 比较、移位、逻辑操作等                                    │
└─────────────────────────────────────────────────────────────┘
```

---

## 3. 为什么选择 C/C++ 开发 WASM

### 3.1 C/C++ 作为源语言的优势

| 优势 | 说明 |
|------|------|
| **成熟的工具链** | Emscripten 提供了完整的编译和运行时支持 |
| **性能接近原生** | C/C++ 编译的 WASM 性能损耗极小 |
| **内存控制** | 直接操作线性内存，适合性能敏感场景 |
| **现有代码库** | 可以复用大量现有的 C/C++ 库和算法 |
| **跨平台** | 一套代码可编译到多个平台 |
| **标准化** | C/C++ 是工业标准，文档丰富 |

### 3.2 与其他语言的对比

| 语言 | 编译目标 | 性能 | 成熟度 | 适用场景 |
|------|----------|------|--------|----------|
| C/C++ | Emscripten | ★★★★★ | ★★★★★ | 高性能计算、游戏、图像处理 |
| Rust | wasm-pack | ★★★★★ | ★★★★☆ | 系统编程、安全敏感应用 |
| Go | tinygo | ★★★☆☆ | ★★★☆☆ | 服务端应用移植 |
| AssemblyScript | 直接编译 | ★★★★☆ | ★★★★☆ | TypeScript开发者入门 |
| C# | Blazor | ★★★☆☆ | ★★★★☆ | .NET 生态系统应用 |
| Java | CheerpJ/JWebAssembly | ★★☆☆☆ | ★★☆☆☆ | Java 应用移植 |

### 3.3 C/C++ 到 WebAssembly 的编译流程

```
┌─────────────────────────────────────────────────────────────┐
│                  C/C++ 源代码                               │
│         (.c / .cpp / .h / .hpp)                             │
└─────────────────────────────────────────────────────────────┘
                            ↓
┌─────────────────────────────────────────────────────────────┐
│              Clang/LLVM 前端编译                             │
│  ├─ 预处理 (Preprocessor)                                    │
│  ├─ 词法分析 (Lexical Analysis)                              │
│  ├─ 语法分析 (Parsing)                                       │
│  ├─ 语义分析 (Semantic Analysis)                             │
│  └─ 生成 LLVM IR (中间表示)                                   │
└─────────────────────────────────────────────────────────────┘
                            ↓
┌─────────────────────────────────────────────────────────────┐
│              LLVM 优化和代码生成                             │
│  ├─ LLVM IR 优化 (Pass)                                      │
│  └─ LLVM WebAssembly Backend                                 │
│         └─ 生成 .wasm 对象文件                               │
└─────────────────────────────────────────────────────────────┘
                            ↓
┌─────────────────────────────────────────────────────────────┐
│              Emscripten 链接和打包                           │
│  ├─ 链接系统库 (libc, libc++, etc.)                          │
│  ├─ 生成 JavaScript 胶水代码                                 │
│  ├─ 生成 HTML 启动文件                                       │
│  └─ 可选: 生成 Worker 线程支持                               │
└─────────────────────────────────────────────────────────────┘
                            ↓
┌─────────────────────────────────────────────────────────────┐
│                  输出文件                                    │
│  ├─ program.wasm    (WebAssembly 二进制模块)                 │
│  ├─ program.js      (JavaScript 胶水代码)                    │
│  └─ program.html    (HTML 入口文件)                          │
└─────────────────────────────────────────────────────────────┘
```

### 3.4 C/C++ 特定优化

Emscripten 针对 C/C++ 提供了专门的优化：

```c
// 示例: EMSCRIPTEN_KEEPALIVE 宏
#include <emscripten/emscripten.h>

// 标记此函数需要从 WASM 导出，即使看起来未被使用
EMSCRIPTEN_KEEPALIVE
int fibonacci(int n) {
    if (n <= 1) return n;
    return fibonacci(n - 1) + fibonacci(n - 2);
}

// 示例: 使用 Embind 进行类型绑定
#include <emscripten/bind.h>

class Calculator {
public:
    int add(int a, int b) { return a + b; }
    int multiply(int a, int b) { return a * b; }
};

EMSCRIPTEN_BINDINGS(my_module) {
    emscripten::class_<Calculator>("Calculator")
        .constructor()
        .function("add", &Calculator::add)
        .function("multiply", &Calculator::multiply);
}
```

---

## 4. Emscripten 工具链详解

### 4.1 Emscripten 架构

```
┌─────────────────────────────────────────────────────────────┐
│                    Emscripten SDK                           │
├─────────────────────────────────────────────────────────────┤
│  ┌───────────────────────────────────────────────────────┐  │
│  │  emcc / em++ (编译器前端)                                │  │
│  │  ├─ 命令行参数解析                                        │  │
│  │  ├─ 调用 Clang/LLVM                                     │  │
│  │  └─ 调用 Emscripten 链接器                               │  │
│  └───────────────────────────────────────────────────────┘  │
│                            ↓                                │
│  ┌───────────────────────────────────────────────────────┐  │
│  │  Emscripten 系统库                                       │  │
│  │  ├─ libc (musl libc 移植)                               │  │
│  │  ├─ libc++ (C++ 标准库)                                 │  │
│  │  ├- SDL/SDL2 (多媒体库)                                 │  │
│  │  ├─ OpenGL ES (图形库)                                  │  │
│  │  ├─ pthreads (线程支持)                                 │  │
│  │  └─ 文件系统库 (MEMFS, IDBFS, NODEFS)                    │  │
│  └───────────────────────────────────────────────────────┘  │
│                            ↓                                │
│  ┌───────────────────────────────────────────────────────┐  │
│  │  JavaScript 运行时                                       │  │
│  │  ├─ 模块加载和实例化                                     │  │
│  │  ├─ 内存管理接口                                         │  │
│  │  ├─ 文件系统模拟                                         │  │
│  │  ├─ 浏览器 API 封装                                       │  │
│  │  └─ 与 JavaScript 的互操作接口                           │  │
│  └───────────────────────────────────────────────────────┘  │
└─────────────────────────────────────────────────────────────┘
```

### 4.2 安装 Emscripten SDK

#### 4.2.1 使用 Git 安装（推荐）

```bash
## 1. 克隆 emsdk 仓库

---

## 🔗 全面知识关联体系

### 【全局层】知识库导航
| 维度 | 目标文档 | 导航作用 |
|:-----|:---------|:---------|
| **总索引** | [../00_GLOBAL_INDEX.md](../00_GLOBAL_INDEX.md) | 完整知识图谱入口，全局视角 |
| **本模块** | [../README.md](../README.md) | 模块总览与目录导航 |
| **学习路径** | [../06_Thinking_Representation/06_Learning_Paths/README.md](../06_Thinking_Representation/06_Learning_Paths/README.md) | 阶段化学习路线规划 |
| **概念映射** | [../06_Thinking_Representation/05_Concept_Mappings/README.md](../06_Thinking_Representation/05_Concept_Mappings/README.md) | 核心概念等价关系图 |

### 【阶段层】学习定位
**当前模块**: 知识库
**难度等级**: L1-L6
**前置依赖**: 核心知识体系
**后续延伸**: 持续学习

```

学习阶段金字塔:
    L6 专家层 [形式验证、编译器]
    L5 高级层 [并发、系统编程] ⬅️ 可能在此
    L4 进阶层 [指针、内存管理]
    L3 基础层 [函数、结构体]
    L2 入门层 [语法、数据类型]
    L1 零基础 [环境搭建]

```

### 【层次层】纵向知识链
| 层级 | 关联文档 | 层次关系 |
|:-----|:---------|:---------|
| **理论基础** | [../02_Formal_Semantics_and_Physics/00_Core_Semantics_Foundations/README.md](../02_Formal_Semantics_and_Physics/00_Core_Semantics_Foundations/README.md) | 语义学理论基础 |
| **核心机制** | [../01_Core_Knowledge_System/02_Core_Layer/README.md](../01_Core_Knowledge_System/02_Core_Layer/README.md) | C语言核心机制 |
| **标准接口** | [../01_Core_Knowledge_System/04_Standard_Library_Layer/README.md](../01_Core_Knowledge_System/04_Standard_Library_Layer/README.md) | 标准库API |
| **系统实现** | [../03_System_Technology_Domains/README.md](../03_System_Technology_Domains/README.md) | 系统级实现 |

### 【局部层】横向关联网
| 关联类型 | 目标文档 | 关联说明 |
|:---------|:---------|:---------|
| **技术扩展** | [../03_System_Technology_Domains/14_Concurrency_Parallelism/README.md](../03_System_Technology_Domains/14_Concurrency_Parallelism/README.md) | 并发编程技术 |
| **安全规范** | [../01_Core_Knowledge_System/09_Safety_Standards/MISRA_C_2023/README.md](../01_Core_Knowledge_System/09_Safety_Standards/MISRA_C_2023/README.md) | 安全编码标准 |
| **工具支持** | [../07_Modern_Toolchain/README.md](../07_Modern_Toolchain/README.md) | 现代开发工具链 |
| **实践案例** | [../04_Industrial_Scenarios/README.md](../04_Industrial_Scenarios/README.md) | 工业实践场景 |

### 【总体层】知识体系架构
```

┌─────────────────────────────────────────────────────────────┐
│                     总体知识体系架构                          │
├─────────────────────────────────────────────────────────────┤
│  01 Core Knowledge          → 核心概念与机制                  │
│  02 Formal Semantics        → 理论与物理基础                  │
│  03 System Technology       → 系统级技术领域                  │
│  04 Industrial Scenarios    → 工业应用场景                    │
│  05 Deep Structure          → 深层结构与元物理                │
│  06 Thinking Representation → 思维表征与学习                  │
│  07 Modern Toolchain        → 现代工具链                      │
└─────────────────────────────────────────────────────────────┘

```

### 【决策层】学习路径选择
| 目标 | 推荐路径 | 关键文档 |
|:-----|:---------|:---------|
| **系统学习** | 01 → 02 → 03 → 04 | 按顺序阅读各模块 |
| **问题导向** | 06决策树 → 相关模块 | [决策树目录](../06_Thinking_Representation/01_Decision_Trees/README.md) |
| **项目驱动** | 04案例 → 所需知识 | [工业场景](../04_Industrial_Scenarios/README.md) |
| **深入研究** | 02形式语义 → 11CompCert | [形式语义](../02_Formal_Semantics_and_Physics/README.md) |

---

git clone https://github.com/emscripten-core/emsdk.git

## 2. 进入目录
cd emsdk

## 3. 获取最新版本
./emsdk install latest

### Windows
### emsdk install latest

## 4. 激活版本
./emsdk activate latest

### Windows
### emsdk activate latest

## 5. 设置环境变量
source ./emsdk_env.sh

### Windows (PowerShell)
### .\emsdk_env.ps1
### Windows (CMD)
### emsdk_env.bat
```

#### 4.2.2 验证安装

```bash
### 检查版本
emcc --version

### 预期输出类似：
### emcc (Emscripten gcc/clang-like replacement + linker emulating GNU ld) 3.1.45
### clang version 17.0.0
### Target: wasm32-unknown-emscripten
### Thread model: posix

### 检查环境
echo $EMSDK

### Windows
### echo %EMSDK%

### 运行诊断工具
emcc --check
```

### 4.3 Emscripten 目录结构

```
emsdk/
├── emsdk.py              # 主管理脚本
├── emsdk_env.sh          # Unix 环境设置
├── emsdk_env.bat         # Windows CMD 环境设置
├── emsdk_env.ps1         # Windows PowerShell 环境设置
├── upstream/             # LLVM/Clang 工具链
│   ├── bin/              # 编译器可执行文件
│   │   ├── clang
│   │   ├── clang++
│   │   ├── llvm-ar
│   │   └── wasm-ld
│   ├── emscripten/       # Emscripten 主目录
│   │   ├── emcc.py       # C 编译器
│   │   ├── em++.py       # C++ 编译器
│   │   ├── system/       # 系统库和头文件
│   │   │   ├── lib/      # 预编译库
│   │   │   └── include/  # 头文件
│   │   ├── src/          # JavaScript 运行时源代码
│   │   └── cache/        # 编译缓存
│   └── lib/              # 库文件
└── node/                 # 捆绑的 Node.js (可选)
```

### 4.4 核心工具介绍

| 工具 | 用途 | 示例 |
|------|------|------|
| `emcc` | C 编译器 | `emcc hello.c -o hello.html` |
| `em++` | C++ 编译器 | `em++ hello.cpp -o hello.html` |
| `emar` | 归档工具 | `emar rcs libfoo.a foo.o` |
| `emranlib` | 索引归档 | `emranlib libfoo.a` |
| `emconfigure` | 配置脚本包装 | `emconfigure ./configure` |
| `emmake` | Make 包装 | `emmake make` |
| `emcmake` | CMake 包装 | `emcmake cmake ..` |
| `emnm` | 符号表查看 | `emnm module.wasm` |
| `emsize` | 查看大小 | `emsize module.wasm` |
| `wasm-dis` | WASM 反汇编 | `wasm-dis module.wasm -o module.wat` |
| `wasm-as` | WASM 汇编 | `wasm-as module.wat -o module.wasm` |

---

## 5. 从 C/C++ 编译到 WASM 的完整流程

### 5.1 基础编译命令

```bash
### ========== 基本编译 ==========

### 编译 C 文件为 WASM + HTML + JS
emcc hello.c -o hello.html

### 编译 C++ 文件
em++ hello.cpp -o hello.html

### 只生成 WASM + JS（无 HTML）
emcc hello.c -o hello.js

### 生成独立 WASM 模块（最小输出）
emcc hello.c -o hello.wasm

### ========== 优化级别 ==========

### 无优化（调试）
emcc hello.c -O0 -o hello.html

### 基本优化
emcc hello.c -O1 -o hello.html

### 更多优化
emcc hello.c -O2 -o hello.html

### 激进优化（推荐用于发布）
emcc hello.c -O3 -o hello.html

### 优化体积
emcc hello.c -Os -o hello.html

### 优化性能和体积平衡
emcc hello.c -Oz -o hello.html

### ========== 调试信息 ==========

### 包含调试信息
emcc hello.c -g -o hello.html

### 使用 DWARF 调试格式
emcc hello.c -g3 -o hello.html

### 生成 Source Map
emcc hello.c -g4 -o hello.html

### ========== 编译和链接分离 ==========

### 编译为目标文件
emcc -c hello.c -o hello.o
emcc -c world.c -o world.o

### 链接生成最终输出
emcc hello.o world.o -o app.html

### 创建静态库
emar rcs libmylib.a hello.o world.o

### 链接静态库
emcc main.c libmylib.a -o app.html
### 或
emcc main.c -L. -lmylib -o app.html
```

### 5.2 编译选项详解

```bash
### ========== 输出格式选项 ==========

### 生成 HTML 文件（包含嵌入式 WASM）
-sSINGLE_FILE=1

### 生成 ES6 模块
-sEXPORT_ES6=1

### 指定环境
-sENVIRONMENT=web        # 仅浏览器
-sENVIRONMENT=node       # 仅 Node.js
-sENVIRONMENT=web,node   # 两者都支持
-sENVIRONMENT=worker     # Web Worker

### ========== 运行时选项 ==========

### 初始内存大小（64KB 的倍数，单位：页）
-sINITIAL_MEMORY=16MB
-sINITIAL_MEMORY=67108864  # 64MB = 64 * 1024 * 1024

### 最大内存大小
-sMAXIMUM_MEMORY=256MB
-sALLOW_MEMORY_GROWTH=1   # 允许内存增长

### 栈大小
-sSTACK_SIZE=5MB

### 导出函数
-sEXPORTED_FUNCTIONS="['_main', '_myfunc']"

### 导出运行时方法
-sEXPORTED_RUNTIME_METHODS="['ccall', 'cwrap', 'UTF8ToString']"

### ========== 高级优化 ==========

### 启用 Closure Compiler 优化 JS
--closure 1

### 链接时优化 (LTO)
-flto
-sLTO=1

### 死代码消除
-sDEAD_CODE_STRIPPING=1

### 最小化导出
-sEXPORT_ALL=0

### WASM 异步编译
-sWASM_ASYNC_COMPILATION=1

### ========== 模块化选项 ==========

### 生成 ES6 模块
-sEXPORT_ES6=1
-sMODULARIZE=1
-sEXPORT_NAME="MyModule"

### 使用模块化实例化
-sMODULARIZE=1 -sEXPORT_NAME="createModule"

### ========== 多线程支持 ==========

### 启用 pthreads
-sUSE_PTHREADS=1
-sPTHREAD_POOL_SIZE=4

### SharedArrayBuffer
-sINITIAL_MEMORY=64MB
-sMAXIMUM_MEMORY=256MB
-sALLOW_MEMORY_GROWTH=1

### ========== 文件系统 ==========

### 完全禁用文件系统（减少体积）
-sFILESYSTEM=0

### 预加载文件到 MEMFS
--preload-file assets/

### 嵌入单个文件
--embed-file config.txt

### 使用 IndexedDB 文件系统
-lidbfs.js

### ========== 网络/Web API ==========

### 允许同步 XHR（用于文件系统）
-sALLOW_SYNC_XHR_IN_MAIN_THREAD=1

### 使用 WebGL
-sUSE_WEBGL2=1
-sFULL_ES3=1
-sMIN_WEBGL_VERSION=2
-sMAX_WEBGL_VERSION=2

### WebSocket 支持
-lwebsocket.js

### ========== 库链接 ==========

### 链接 SDL2
-sUSE_SDL=2

### 链接 GLFW
-sUSE_GLFW=3

### 链接 OpenAL
-sUSE_OPENAL=1

### 链接 zlib
-sUSE_ZLIB=1

### 链接 libpng
-sUSE_LIBPNG=1

### 链接 libjpeg
-sUSE_LIBJPEG=1

### 链接 freetype
-sUSE_FREETYPE=1

### 链接 harfbuzz
-sUSE_HARFBUZZ=1

### 链接 ICU
-sUSE_ICU=1
```

### 5.3 编译流程图

```
┌─────────────────────────────────────────────────────────────┐
│                   源代码文件                                 │
│       main.c  utils.c  helper.cpp  header.h                 │
└─────────────────────────────────────────────────────────────┘
                            ↓
                    ┌───────────────┐
                    │   预处理阶段   │
                    │   (Preprocess) │
                    └───────────────┘
                            ↓
┌─────────────────────────────────────────────────────────────┐
│                 预处理后源代码                               │
│    宏展开完成 / 头文件包含 / 条件编译处理                      │
└─────────────────────────────────────────────────────────────┘
                            ↓
                    ┌───────────────┐
                    │   编译阶段     │
                    │   (Compile)    │
                    │   emcc/em++    │
                    └───────────────┘
                            ↓
┌─────────────────────────────────────────────────────────────┐
│                   LLVM IR (中间代码)                        │
│         平台无关的中间表示形式                                 │
└─────────────────────────────────────────────────────────────┘
                            ↓
                    ┌───────────────┐
                    │   优化阶段     │
                    │   (Optimize)   │
                    │   LLVM Passes  │
                    └───────────────┘
                            ↓
┌─────────────────────────────────────────────────────────────┐
│               优化后的 LLVM IR                              │
│              死代码消除 / 循环优化 / 内联等                    │
└─────────────────────────────────────────────────────────────┘
                            ↓
                    ┌───────────────┐
                    │   代码生成     │
                    │   (Code Gen)   │
                    │   wasm-ld      │
                    └───────────────┘
                            ↓
┌─────────────────────────────────────────────────────────────┐
│               WASM 对象文件 (.o)                            │
│        多个源文件编译为多个对象文件                            │
└─────────────────────────────────────────────────────────────┘
                            ↓
                    ┌───────────────┐
                    │   链接阶段     │
                    │   (Link)       │
                    │   wasm-ld      │
                    └───────────────┘
                            ↓
┌─────────────────────────────────────────────────────────────┐
│              未优化 WASM 模块                                │
│       包含所有导入的库代码                                     │
└─────────────────────────────────────────────────────────────┘
                            ↓
                    ┌───────────────┐
                    │   后链接优化   │
                    │   wasm-opt     │
                    │   (Binaryen)   │
                    └───────────────┘
                            ↓
┌─────────────────────────────────────────────────────────────┐
│              优化后 WASM 模块                                │
│    ┌───────────────────────────────────────────────────────┐│
│    │  可选: 生成 JavaScript 胶水代码                        ││
│    │  ├─ 运行时初始化代码                                    ││
│    │  ├─ 内存管理接口                                        ││
│    │  ├─ 文件系统模拟                                        ││
│    │  └─ API 导出封装                                        ││
│    └───────────────────────────────────────────────────────┘│
│    ┌───────────────────────────────────────────────────────┐│
│    │  可选: 生成 HTML 启动文件                              ││
│    │  ├─ Canvas 元素 (用于图形)                             ││
│    │  ├─ 脚本加载                                            ││
│    │  └─ 样式设置                                            ││
│    └───────────────────────────────────────────────────────┘│
└─────────────────────────────────────────────────────────────┘
```

---

## 6. 第一个 "Hello, WASM" 程序

### 6.1 最简单的 C 程序

创建文件 `hello.c`：

```c
#include <stdio.h>

int main() {
    printf("Hello, WebAssembly!\n");
    printf("你好，WebAssembly！\n");
    return 0;
}
```

编译命令：

```bash
### 基础编译 - 生成 HTML 文件
emcc hello.c -o hello.html

### 编译输出：
### - hello.wasm    (32KB - WASM 模块)
### - hello.js      (280KB - JavaScript 胶水代码)
### - hello.html    (8KB - HTML 入口文件)

### 使用优化编译
emcc hello.c -O3 -o hello.html

### 优化后输出：
### - hello.wasm    (8KB)
### - hello.js      (180KB)
### - hello.html    (8KB)
```

### 6.2 运行程序

```bash
### 方法1: 使用 emrun（推荐）
emrun hello.html

### 方法2: 使用 Python 简单 HTTP 服务器
python3 -m http.server 8000
### 浏览器访问: http://localhost:8000/hello.html

### 方法3: 使用 Node.js
node hello.js
### 注意: 需要添加编译选项 -sENVIRONMENT=node

### 方法4: 使用 VS Code Live Server 扩展
### 右键点击 hello.html → "Open with Live Server"
```

### 6.3 程序结构解析

```
┌─────────────────────────────────────────────────────────────┐
│                     hello.html                              │
│  ┌───────────────────────────────────────────────────────┐  │
│  │  1. 加载 hello.js (JavaScript 胶水代码)                  │  │
│  │  2. 初始化 Canvas (如果存在图形)                          │  │
│  │  3. 创建输出显示区域                                      │  │
│  └───────────────────────────────────────────────────────┘  │
└─────────────────────────────────────────────────────────────┘
                            ↓
┌─────────────────────────────────────────────────────────────┐
│                     hello.js                                │
│  ┌───────────────────────────────────────────────────────┐  │
│  │  1. 配置模块参数 (内存大小、环境等)                        │  │
│  │  2. 异步加载 hello.wasm                                 │  │
│  │  3. 实例化 WASM 模块                                     │  │
│  │  4. 设置 JavaScript 运行时环境                            │  │
│  │  5. 调用 main() 函数                                     │  │
│  └───────────────────────────────────────────────────────┘  │
└─────────────────────────────────────────────────────────────┘
                            ↓
┌─────────────────────────────────────────────────────────────┐
│                     hello.wasm                              │
│  ┌───────────────────────────────────────────────────────┐  │
│  │  1. _start() 启动函数 (运行时初始化)                       │  │
│  │  2. main() 函数                                         │  │
│  │  3. printf() 实现 (通过系统调用)                          │  │
│  │     ├─ 格式化字符串                                      │  │
│  │     ├─ 写入 stdout (文件描述符1)                         │  │
│  │     └─ JavaScript 捕获输出并显示                          │  │
│  │  4. 调用 wasi_unstable.fd_write                          │  │
│  │  5. JavaScript 输出到 console/HTML                       │  │
│  └───────────────────────────────────────────────────────┘  │
└─────────────────────────────────────────────────────────────┘
```

### 6.4 导出自定义函数

创建文件 `math_lib.c`：

```c
#include <emscripten/emscripten.h>

// 标记此函数需要导出
EMSCRIPTEN_KEEPALIVE
int add(int a, int b) {
    return a + b;
}

EMSCRIPTEN_KEEPALIVE
int subtract(int a, int b) {
    return a - b;
}

EMSCRIPTEN_KEEPALIVE
int multiply(int a, int b) {
    return a * b;
}

EMSCRIPTEN_KEEPALIVE
double divide(double a, double b) {
    if (b == 0.0) return 0.0;
    return a / b;
}

EMSCRIPTEN_KEEPALIVE
unsigned long long factorial(int n) {
    if (n < 0) return 0;
    if (n <= 1) return 1;
    unsigned long long result = 1;
    for (int i = 2; i <= n; i++) {
        result *= i;
    }
    return result;
}

EMSCRIPTEN_KEEPALIVE
int fibonacci(int n) {
    if (n <= 0) return 0;
    if (n == 1) return 1;
    return fibonacci(n - 1) + fibonacci(n - 2);
}

// 计算圆周率 - 莱布尼茨公式
EMSCRIPTEN_KEEPALIVE
double calculate_pi(int iterations) {
    double pi = 0.0;
    for (int i = 0; i < iterations; i++) {
        double term = 1.0 / (2.0 * i + 1.0);
        if (i % 2 == 0) {
            pi += term;
        } else {
            pi -= term;
        }
    }
    return pi * 4.0;
}

// 数组求和
EMSCRIPTEN_KEEPALIVE
int sum_array(int* arr, int length) {
    int sum = 0;
    for (int i = 0; i < length; i++) {
        sum += arr[i];
    }
    return sum;
}

// 求平均值
EMSCRIPTEN_KEEPALIVE
double average_array(int* arr, int length) {
    if (length <= 0) return 0.0;
    return (double)sum_array(arr, length) / length;
}
```

编译命令：

```bash
### 编译为库（没有 main 函数）
emcc math_lib.c -O3 \
    -sEXPORTED_FUNCTIONS="['_add', '_subtract', '_multiply', '_divide', '_factorial', '_fibonacci', '_calculate_pi', '_sum_array', '_average_array']" \
    -sEXPORTED_RUNTIME_METHODS="['ccall', 'cwrap']" \
    -o math_lib.js

### 精简版本（无文件系统）
emcc math_lib.c -O3 \
    -sEXPORTED_FUNCTIONS="['_add', '_subtract', '_multiply', '_divide', '_factorial', '_fibonacci', '_calculate_pi', '_sum_array', '_average_array']" \
    -sEXPORTED_RUNTIME_METHODS="['ccall', 'cwrap']" \
    -sFILESYSTEM=0 \
    -sMODULARIZE=1 \
    -sEXPORT_NAME="MathLib" \
    -o math_lib.js
```

### 6.5 在 HTML 中使用导出的函数

创建 `math_demo.html`：

```html
<!DOCTYPE html>
<html lang="zh-CN">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>WebAssembly Math Library Demo</title>
    <style>
        * {
            margin: 0;
            padding: 0;
            box-sizing: border-box;
        }
        body {
            font-family: -apple-system, BlinkMacSystemFont, 'Segoe UI', Roboto, sans-serif;
            background: linear-gradient(135deg, #667eea 0%, #764ba2 100%);
            min-height: 100vh;
            padding: 20px;
        }
        .container {
            max-width: 900px;
            margin: 0 auto;
            background: white;
            border-radius: 16px;
            box-shadow: 0 20px 60px rgba(0,0,0,0.3);
            overflow: hidden;
        }
        header {
            background: linear-gradient(90deg, #667eea 0%, #764ba2 100%);
            color: white;
            padding: 30px;
            text-align: center;
        }
        h1 {
            font-size: 2em;
            margin-bottom: 10px;
        }
        .subtitle {
            opacity: 0.9;
        }
        .content {
            padding: 30px;
        }
        .section {
            margin-bottom: 30px;
            padding: 20px;
            background: #f8f9fa;
            border-radius: 8px;
        }
        .section h2 {
            color: #667eea;
            margin-bottom: 15px;
            font-size: 1.3em;
        }
        .input-group {
            display: flex;
            gap: 10px;
            margin-bottom: 15px;
            flex-wrap: wrap;
            align-items: center;
        }
        input[type="number"] {
            padding: 10px 15px;
            border: 2px solid #e0e0e0;
            border-radius: 6px;
            font-size: 16px;
            width: 120px;
            transition: border-color 0.3s;
        }
        input[type="number"]:focus {
            outline: none;
            border-color: #667eea;
        }
        button {
            padding: 10px 20px;
            background: #667eea;
            color: white;
            border: none;
            border-radius: 6px;
            cursor: pointer;
            font-size: 14px;
            transition: all 0.3s;
        }
        button:hover {
            background: #5a6fd6;
            transform: translateY(-2px);
        }
        button:disabled {
            background: #ccc;
            cursor: not-allowed;
            transform: none;
        }
        .result {
            padding: 15px;
            background: white;
            border-radius: 6px;
            border-left: 4px solid #667eea;
            margin-top: 10px;
        }
        .result-label {
            font-weight: bold;
            color: #666;
            margin-bottom: 5px;
        }
        .result-value {
            font-size: 1.5em;
            color: #333;
        }
        .error {
            border-left-color: #e74c3c !important;
            color: #e74c3c;
        }
        .success {
            border-left-color: #27ae60 !important;
        }
        .code-block {
            background: #2d2d2d;
            color: #f8f8f2;
            padding: 15px;
            border-radius: 6px;
            font-family: 'Consolas', 'Monaco', monospace;
            font-size: 13px;
            overflow-x: auto;
            margin-top: 10px;
        }
        .loading {
            text-align: center;
            padding: 40px;
            color: #666;
        }
        .spinner {
            border: 3px solid #f3f3f3;
            border-top: 3px solid #667eea;
            border-radius: 50%;
            width: 40px;
            height: 40px;
            animation: spin 1s linear infinite;
            margin: 0 auto 15px;
        }
        @keyframes spin {
            0% { transform: rotate(0deg); }
            100% { transform: rotate(360deg); }
        }
        .stats {
            display: grid;
            grid-template-columns: repeat(auto-fit, minmax(150px, 1fr));
            gap: 15px;
            margin-top: 15px;
        }
        .stat-card {
            background: white;
            padding: 15px;
            border-radius: 8px;
            text-align: center;
            box-shadow: 0 2px 4px rgba(0,0,0,0.1);
        }
        .stat-value {
            font-size: 1.8em;
            font-weight: bold;
            color: #667eea;
        }
        .stat-label {
            color: #666;
            font-size: 0.9em;
            margin-top: 5px;
        }
    </style>
</head>
<body>
    <div class="container">
        <header>
            <h1>🧮 WebAssembly Math Library</h1>
            <p class="subtitle">高性能数学计算 - C 编译为 WebAssembly</p>
        </header>

        <div class="content">
            <div id="loading" class="loading">
                <div class="spinner"></div>
                <p>正在加载 WebAssembly 模块...</p>
            </div>

            <div id="app" style="display: none;">
                <!-- 基础运算 -->
                <div class="section">
                    <h2>🔢 基础运算</h2>
                    <div class="input-group">
                        <input type="number" id="num1" placeholder="数字 A" value="10">
                        <span>和</span>
                        <input type="number" id="num2" placeholder="数字 B" value="5">
                    </div>
                    <div class="input-group">
                        <button onclick="calculate('add')">加法 (+)</button>
                        <button onclick="calculate('subtract')">减法 (-)</button>
                        <button onclick="calculate('multiply')">乘法 (×)</button>
                        <button onclick="calculate('divide')">除法 (÷)</button>
                    </div>
                    <div id="basic-result" class="result" style="display: none;">
                        <div class="result-label">计算结果</div>
                        <div class="result-value" id="basic-value">-</div>
                    </div>
                </div>

                <!-- 阶乘计算 -->
                <div class="section">
                    <h2>📊 阶乘计算</h2>
                    <div class="input-group">
                        <input type="number" id="fact-input" placeholder="输入 n" value="20" min="0" max="20">
                        <button onclick="calculateFactorial()">计算 n!</button>
                    </div>
                    <div id="fact-result" class="result" style="display: none;">
                        <div class="result-label">阶乘结果</div>
                        <div class="result-value" id="fact-value">-</div>
                    </div>
                </div>

                <!-- 斐波那契数列 -->
                <div class="section">
                    <h2>🌀 斐波那契数列</h2>
                    <div class="input-group">
                        <input type="number" id="fib-input" placeholder="输入 n" value="40" min="0" max="45">
                        <button onclick="calculateFibonacci()">计算 Fib(n)</button>
                    </div>
                    <div id="fib-result" class="result" style="display: none;">
                        <div class="result-label">斐波那契数</div>
                        <div class="result-value" id="fib-value">-</div>
                        <div style="margin-top: 10px; color: #666; font-size: 0.9em;">
                            计算时间: <span id="fib-time">-</span> ms
                        </div>
                    </div>
                </div>

                <!-- 圆周率计算 -->
                <div class="section">
                    <h2>🥧 圆周率 π 计算</h2>
                    <div class="input-group">
                        <input type="number" id="pi-input" placeholder="迭代次数" value="10000000" min="1000" step="1000000">
                        <button onclick="calculatePi()">计算 π</button>
                    </div>
                    <div id="pi-result" class="result" style="display: none;">
                        <div class="result-label">计算结果</div>
                        <div class="result-value" id="pi-value">-</div>
                        <div style="margin-top: 10px;">
                            误差: <span id="pi-error">-</span>
                        </div>
                        <div style="margin-top: 5px; color: #666; font-size: 0.9em;">
                            计算时间: <span id="pi-time">-</span> ms
                        </div>
                    </div>
                </div>

                <!-- 性能对比 -->
                <div class="section">
                    <h2>⚡ 性能对比: JavaScript vs WebAssembly</h2>
                    <div class="input-group">
                        <input type="number" id="bench-n" placeholder="n" value="35" min="10" max="45">
                        <button onclick="runBenchmark()">运行对比测试</button>
                    </div>
                    <div class="stats" id="benchmark-stats" style="display: none;">
                        <div class="stat-card">
                            <div class="stat-value" id="js-time">-</div>
                            <div class="stat-label">JavaScript (ms)</div>
                        </div>
                        <div class="stat-card">
                            <div class="stat-value" id="wasm-time">-</div>
                            <div class="stat-label">WebAssembly (ms)</div>
                        </div>
                        <div class="stat-card">
                            <div class="stat-value" id="speedup">-</div>
                            <div class="stat-label">加速比</div>
                        </div>
                    </div>
                </div>

                <!-- 模块信息 -->
                <div class="section">
                    <h2>📦 模块信息</h2>
                    <div class="code-block" id="module-info">
加载中...
                    </div>
                </div>
            </div>
        </div>
    </div>

    <script>
        // 全局模块实例
        let Module;

        // 导出函数的包装器
        let add, subtract, multiply, divide;
        let factorial, fibonacci, calculatePi;

        // 初始化模块
        async function initModule() {
            try {
                // 加载 WASM 模块
                Module = await createModule();

                // 使用 cwrap 创建便捷函数
                add = Module.cwrap('add', 'number', ['number', 'number']);
                subtract = Module.cwrap('subtract', 'number', ['number', 'number']);
                multiply = Module.cwrap('multiply', 'number', ['number', 'number']);
                divide = Module.cwrap('divide', 'number', ['number', 'number']);
                factorial = Module.cwrap('factorial', 'number', ['number']);
                fibonacci = Module.cwrap('fibonacci', 'number', ['number']);
                calculatePi = Module.cwrap('calculate_pi', 'number', ['number']);

                // 显示应用
                document.getElementById('loading').style.display = 'none';
                document.getElementById('app').style.display = 'block';

                // 显示模块信息
                showModuleInfo();

                console.log('WebAssembly 模块加载成功！');
            } catch (error) {
                console.error('加载失败:', error);
                document.getElementById('loading').innerHTML = `
                    <p style="color: #e74c3c;">加载失败: ${error.message}</p>
                `;
            }
        }

        // 显示模块信息
        function showModuleInfo() {
            const info = `
内存信息:
- 初始内存: ${Module.INITIAL_MEMORY / 1024 / 1024} MB
- 当前内存: ${Module.buffer.byteLength / 1024 / 1024} MB

导出函数:
- add(a, b): 整数加法
- subtract(a, b): 整数减法
- multiply(a, b): 整数乘法
- divide(a, b): 浮点除法
- factorial(n): 阶乘计算
- fibonacci(n): 斐波那契数列
- calculate_pi(iterations): 莱布尼茨公式计算 π
            `;
            document.getElementById('module-info').textContent = info;
        }

        // 基础计算
        function calculate(operation) {
            const a = parseFloat(document.getElementById('num1').value);
            const b = parseFloat(document.getElementById('num2').value);
            let result;

            switch(operation) {
                case 'add': result = add(a, b); break;
                case 'subtract': result = subtract(a, b); break;
                case 'multiply': result = multiply(a, b); break;
                case 'divide': result = divide(a, b); break;
            }

            const resultDiv = document.getElementById('basic-result');
            const valueDiv = document.getElementById('basic-value');

            resultDiv.style.display = 'block';
            resultDiv.className = 'result success';
            valueDiv.textContent = result;
        }

        // 阶乘计算
        function calculateFactorial() {
            const n = parseInt(document.getElementById('fact-input').value);
            const result = factorial(n);

            const resultDiv = document.getElementById('fact-result');
            const valueDiv = document.getElementById('fact-value');

            resultDiv.style.display = 'block';
            resultDiv.className = 'result success';
            valueDiv.textContent = result.toLocaleString();
        }

        // 斐波那契计算
        function calculateFibonacci() {
            const n = parseInt(document.getElementById('fib-input').value);

            const startTime = performance.now();
            const result = fibonacci(n);
            const endTime = performance.now();

            const resultDiv = document.getElementById('fib-result');
            resultDiv.style.display = 'block';
            resultDiv.className = 'result success';
            document.getElementById('fib-value').textContent = result.toLocaleString();
            document.getElementById('fib-time').textContent = (endTime - startTime).toFixed(2);
        }

        // 圆周率计算
        function calculatePi() {
            const iterations = parseInt(document.getElementById('pi-input').value);

            const startTime = performance.now();
            const result = calculatePi(iterations);
            const endTime = performance.now();

            const error = Math.abs(result - Math.PI);

            const resultDiv = document.getElementById('pi-result');
            resultDiv.style.display = 'block';
            resultDiv.className = 'result success';
            document.getElementById('pi-value').textContent = result.toFixed(15);
            document.getElementById('pi-error').textContent = error.toExponential(6);
            document.getElementById('pi-time').textContent = (endTime - startTime).toFixed(2);
        }

        // JavaScript 斐波那契（用于对比）
        function jsFibonacci(n) {
            if (n <= 0) return 0;
            if (n === 1) return 1;
            return jsFibonacci(n - 1) + jsFibonacci(n - 2);
        }

        // 性能对比测试
        function runBenchmark() {
            const n = parseInt(document.getElementById('bench-n').value);

            // 预热
            fibonacci(10);
            jsFibonacci(10);

            // 测试 JavaScript
            const jsStart = performance.now();
            const jsResult = jsFibonacci(n);
            const jsEnd = performance.now();
            const jsTime = jsEnd - jsStart;

            // 测试 WebAssembly
            const wasmStart = performance.now();
            const wasmResult = fibonacci(n);
            const wasmEnd = performance.now();
            const wasmTime = wasmEnd - wasmStart;

            // 显示结果
            document.getElementById('benchmark-stats').style.display = 'grid';
            document.getElementById('js-time').textContent = jsTime.toFixed(2);
            document.getElementById('wasm-time').textContent = wasmTime.toFixed(2);
            document.getElementById('speedup').textContent = (jsTime / wasmTime).toFixed(2) + 'x';

            // 验证结果一致性
            console.log(`JS result: ${jsResult}, WASM result: ${wasmResult}`);
        }

        // 页面加载完成后初始化
        document.addEventListener('DOMContentLoaded', initModule);
    </script>
    <script src="math_lib.js"></script>
</body>
</html>
```

---


## 7. 浏览器中运行 WASM 的方法

### 7.1 使用 JavaScript API 加载 WASM

#### 7.1.1 基本加载方法

```javascript
// ========== 方法1: 使用 WebAssembly.instantiateStreaming (推荐) ==========

async function loadWasmStreaming(url) {
    try {
        // 直接使用 fetch 和 instantiateStreaming
        const result = await WebAssembly.instantiateStreaming(
            fetch(url),
            {
                // 导入对象 - WASM 模块需要的外部函数
                env: {
                    memory: new WebAssembly.Memory({ initial: 256, maximum: 512 }),
                    table: new WebAssembly.Table({ initial: 0, element: 'anyfunc' }),
                    __memory_base: 0,
                    __table_base: 0,
                    abort: () => console.log('Abort called')
                },
                wasi_snapshot_preview1: {
                    // WASI 系统调用实现
                    proc_exit: (code) => { console.log(`Exit code: ${code}`); }
                }
            }
        );

        return result.instance;
    } catch (error) {
        console.error('加载 WASM 失败:', error);
        throw error;
    }
}

// 使用
const instance = await loadWasmStreaming('module.wasm');
console.log(instance.exports.add(1, 2));  // 调用导出的函数

// ========== 方法2: 使用 fetch + ArrayBuffer ==========

async function loadWasmBuffer(url) {
    try {
        // 1. 获取 WASM 文件
        const response = await fetch(url);

        // 2. 转换为 ArrayBuffer
        const buffer = await response.arrayBuffer();

        // 3. 编译 WASM 模块
        const module = await WebAssembly.compile(buffer);

        // 4. 实例化模块
        const instance = await WebAssembly.instantiate(module, {
            env: {
                memory: new WebAssembly.Memory({ initial: 256 }),
                abort: () => console.log('Abort')
            }
        });

        return instance;
    } catch (error) {
        console.error('加载失败:', error);
        throw error;
    }
}

// ========== 方法3: 同步编译（仅适用于较小的模块） ==========

function loadWasmSync(buffer) {
    // 同步编译（会阻塞主线程）
    const module = new WebAssembly.Module(buffer);
    const instance = new WebAssembly.Instance(module, {
        env: { memory: new WebAssembly.Memory({ initial: 256 }) }
    });
    return instance;
}

// 使用 XMLHttpRequest 获取 ArrayBuffer（传统方式）
function loadWasmXHR(url, callback) {
    const xhr = new XMLHttpRequest();
    xhr.open('GET', url, true);
    xhr.responseType = 'arraybuffer';

    xhr.onload = function() {
        if (xhr.status === 200) {
            const buffer = xhr.response;
            const module = new WebAssembly.Module(buffer);
            const instance = new WebAssembly.Instance(module, {
                env: { memory: new WebAssembly.Memory({ initial: 256 }) }
            });
            callback(null, instance);
        } else {
            callback(new Error('加载失败'));
        }
    };

    xhr.onerror = () => callback(new Error('网络错误'));
    xhr.send();
}
```

#### 7.1.2 完整的 WASM 加载器类

```javascript
// wasm-loader.js
class WasmLoader {
    constructor(options = {}) {
        this.options = {
            initialMemory: options.initialMemory || 256,  // 页数 (64KB/页)
            maximumMemory: options.maximumMemory || 512,
            importObject: options.importObject || {},
            debug: options.debug || false
        };
        this.instance = null;
        this.module = null;
        this.memory = null;
    }

    log(...args) {
        if (this.options.debug) {
            console.log('[WasmLoader]', ...args);
        }
    }

    // 创建标准导入对象
    createImportObject() {
        // 创建共享内存
        this.memory = new WebAssembly.Memory({
            initial: this.options.initialMemory,
            maximum: this.options.maximumMemory,
            shared: true  // 启用 SharedArrayBuffer 支持
        });

        // 创建函数表（用于间接调用）
        const table = new WebAssembly.Table({
            initial: 10,
            maximum: 100,
            element: 'anyfunc'
        });

        return {
            env: {
                memory: this.memory,
                table: table,
                __memory_base: 1024,
                __table_base: 0,
                abort: this.abort.bind(this),
                abortOnCannotGrowMemory: this.abortOnCannotGrowMemory.bind(this),
                _emscripten_resize_heap: this.resizeHeap.bind(this),
                emscripten_get_now: () => performance.now(),
                emscripten_random: () => Math.random(),
                ...this.options.importObject.env
            },
            wasi_snapshot_preview1: {
                proc_exit: (code) => {
                    this.log('Process exit with code:', code);
                },
                fd_write: (fd, iov, iovcnt, pnum) => {
                    return this.fdWrite(fd, iov, iovcnt, pnum);
                },
                fd_close: () => 0,
                fd_seek: () => 0,
                ...this.options.importObject.wasi_snapshot_preview1
            }
        };
    }

    // 标准输出处理
    fdWrite(fd, iov, iovcnt, pnum) {
        let num = 0;
        const memory = this.memory;
        const view = new DataView(memory.buffer);

        for (let i = 0; i < iovcnt; i++) {
            const ptr = view.getUint32(iov + i * 8, true);
            const len = view.getUint32(iov + i * 8 + 4, true);

            const bytes = new Uint8Array(memory.buffer, ptr, len);
            const text = new TextDecoder().decode(bytes);

            if (fd === 1) {  // stdout
                console.log(text);
            } else if (fd === 2) {  // stderr
                console.error(text);
            }

            num += len;
        }

        view.setUint32(pnum, num, true);
        return 0;
    }

    abort() {
        throw new Error('WASM aborted');
    }

    abortOnCannotGrowMemory() {
        throw new Error('Cannot grow memory');
    }

    resizeHeap(requestedSize) {
        const currentSize = this.memory.buffer.byteLength;
        const maxSize = this.options.maximumMemory * 64 * 1024;

        if (requestedSize > maxSize) {
            return -1;
        }

        try {
            this.memory.grow(Math.ceil((requestedSize - currentSize) / (64 * 1024)));
            return 0;
        } catch (e) {
            return -1;
        }
    }

    // 加载 WASM 模块
    async load(url) {
        this.log('Loading WASM from:', url);

        try {
            // 尝试使用流式编译
            if (WebAssembly.instantiateStreaming) {
                this.log('Using instantiateStreaming');
                const result = await WebAssembly.instantiateStreaming(
                    fetch(url),
                    this.createImportObject()
                );
                this.module = result.module;
                this.instance = result.instance;
            } else {
                // 降级方案
                this.log('Using fallback loading');
                const response = await fetch(url);
                const buffer = await response.arrayBuffer();
                this.module = await WebAssembly.compile(buffer);
                this.instance = await WebAssembly.instantiate(
                    this.module,
                    this.createImportObject()
                );
            }

            this.log('WASM loaded successfully');
            return this.instance.exports;
        } catch (error) {
            this.log('Failed to load WASM:', error);
            throw error;
        }
    }

    // 从 ArrayBuffer 加载
    async loadFromBuffer(buffer) {
        this.log('Loading WASM from ArrayBuffer');

        try {
            this.module = await WebAssembly.compile(buffer);
            this.instance = await WebAssembly.instantiate(
                this.module,
                this.createImportObject()
            );

            this.log('WASM loaded successfully');
            return this.instance.exports;
        } catch (error) {
            this.log('Failed to load WASM:', error);
            throw error;
        }
    }

    // 从 Base64 字符串加载
    async loadFromBase64(base64String) {
        const binaryString = atob(base64String);
        const bytes = new Uint8Array(binaryString.length);
        for (let i = 0; i < binaryString.length; i++) {
            bytes[i] = binaryString.charCodeAt(i);
        }
        return this.loadFromBuffer(bytes.buffer);
    }

    // 获取导出函数
    get exports() {
        return this.instance ? this.instance.exports : null;
    }

    // 获取内存
    getMemory() {
        return this.memory;
    }

    // 读取内存中的字符串
    readString(ptr, maxLength = 1024) {
        const memory = this.memory.buffer;
        const bytes = new Uint8Array(memory);
        let end = ptr;
        while (bytes[end] !== 0 && end - ptr < maxLength) {
            end++;
        }
        return new TextDecoder().decode(bytes.slice(ptr, end));
    }

    // 写入字符串到内存
    writeString(ptr, str) {
        const encoder = new TextEncoder();
        const bytes = encoder.encode(str + '\0');
        const memory = new Uint8Array(this.memory.buffer);
        memory.set(bytes, ptr);
        return bytes.length;
    }

    // 分配内存
    malloc(size) {
        if (this.instance.exports.malloc) {
            return this.instance.exports.malloc(size);
        }
        throw new Error('malloc not exported');
    }

    // 释放内存
    free(ptr) {
        if (this.instance.exports.free) {
            this.instance.exports.free(ptr);
        }
    }
}

// 导出
if (typeof module !== 'undefined' && module.exports) {
    module.exports = WasmLoader;
}
```

### 7.2 在不同环境中的运行

#### 7.2.1 在浏览器中运行

```html
<!DOCTYPE html>
<html>
<head>
    <title>WASM in Browser</title>
</head>
<body>
    <h1>WebAssembly Browser Demo</h1>
    <div id="output"></div>

    <script type="module">
        import WasmLoader from './wasm-loader.js';

        const output = document.getElementById('output');

        async function main() {
            const loader = new WasmLoader({
                initialMemory: 256,
                maximumMemory: 1024,
                debug: true
            });

            try {
                const exports = await loader.load('app.wasm');

                // 调用导出函数
                const result = exports.add(5, 3);
                output.innerHTML = `<p>5 + 3 = ${result}</p>`;

                // 处理字符串
                const strPtr = exports.get_hello_string();
                const str = loader.readString(strPtr);
                output.innerHTML += `<p>Message: ${str}</p>`;

            } catch (error) {
                output.innerHTML = `<p style="color: red">Error: ${error.message}</p>`;
            }
        }

        main();
    </script>
</body>
</html>
```

#### 7.2.2 在 Web Worker 中运行

```javascript
// wasm-worker.js
self.importScripts('wasm-loader.js');

let wasmExports = null;
let wasmLoader = null;

self.onmessage = async function(e) {
    const { type, data } = e.data;

    switch (type) {
        case 'init':
            try {
                wasmLoader = new WasmLoader({
                    initialMemory: data.memory || 256,
                    debug: data.debug || false
                });
                wasmExports = await wasmLoader.load(data.url);
                self.postMessage({ type: 'initialized', success: true });
            } catch (error) {
                self.postMessage({ type: 'initialized', success: false, error: error.message });
            }
            break;

        case 'compute':
            if (!wasmExports) {
                self.postMessage({ type: 'error', error: 'WASM not initialized' });
                return;
            }

            try {
                const startTime = performance.now();
                const result = wasmExports[data.function](...data.args);
                const endTime = performance.now();

                self.postMessage({
                    type: 'result',
                    result: result,
                    duration: endTime - startTime
                });
            } catch (error) {
                self.postMessage({ type: 'error', error: error.message });
            }
            break;

        case 'computeArray':
            if (!wasmExports || !wasmLoader) {
                self.postMessage({ type: 'error', error: 'WASM not initialized' });
                return;
            }

            try {
                const { function: funcName, array, elementSize } = data;

                // 分配 WASM 内存
                const ptr = wasmLoader.malloc(array.length * elementSize);

                // 写入数据
                const memory = new DataView(wasmLoader.getMemory().buffer);
                if (elementSize === 4) {
                    const intArray = new Int32Array(array);
                    for (let i = 0; i < intArray.length; i++) {
                        memory.setInt32(ptr + i * 4, intArray[i], true);
                    }
                } else if (elementSize === 8) {
                    const floatArray = new Float64Array(array);
                    for (let i = 0; i < floatArray.length; i++) {
                        memory.setFloat64(ptr + i * 8, floatArray[i], true);
                    }
                }

                // 调用 WASM 函数
                const startTime = performance.now();
                const result = wasmExports[funcName](ptr, array.length);
                const endTime = performance.now();

                // 释放内存
                wasmLoader.free(ptr);

                self.postMessage({
                    type: 'result',
                    result: result,
                    duration: endTime - startTime
                });
            } catch (error) {
                self.postMessage({ type: 'error', error: error.message });
            }
            break;
    }
};
```

```javascript
// main.js - 主线程代码
const worker = new Worker('wasm-worker.js');

// 初始化 WASM
worker.postMessage({
    type: 'init',
    data: {
        url: 'compute.wasm',
        memory: 256,
        debug: true
    }
});

// 监听消息
worker.onmessage = function(e) {
    const { type, result, duration, error } = e.data;

    switch (type) {
        case 'initialized':
            console.log('WASM initialized:', e.data.success);
            if (e.data.success) {
                // 开始计算
                runComputation();
            }
            break;
        case 'result':
            console.log(`Result: ${result}, Time: ${duration.toFixed(2)}ms`);
            break;
        case 'error':
            console.error('Error:', error);
            break;
    }
};

function runComputation() {
    // 在 Worker 中执行计算
    worker.postMessage({
        type: 'compute',
        data: {
            function: 'fibonacci',
            args: [40]
        }
    });

    // 数组计算
    worker.postMessage({
        type: 'computeArray',
        data: {
            function: 'sum_array',
            array: [1, 2, 3, 4, 5, 6, 7, 8, 9, 10],
            elementSize: 4
        }
    });
}

// 终止 Worker
// worker.terminate();
```

#### 7.2.3 在 Node.js 中运行

```javascript
// node-wasm.js
const fs = require('fs');
const path = require('path');

class NodeWasmLoader {
    constructor(options = {}) {
        this.options = {
            initialMemory: options.initialMemory || 256,
            maximumMemory: options.maximumMemory || 512
        };
        this.memory = null;
    }

    createImportObject() {
        this.memory = new WebAssembly.Memory({
            initial: this.options.initialMemory,
            maximum: this.options.maximumMemory
        });

        return {
            env: {
                memory: this.memory,
                __memory_base: 1024,
                __table_base: 0,
                abort: () => { throw new Error('Abort'); },
                emscripten_resize_heap: (size) => {
                    const current = this.memory.buffer.byteLength;
                    const delta = size - current;
                    const pages = Math.ceil(delta / (64 * 1024));
                    this.memory.grow(pages);
                    return 0;
                }
            },
            wasi_snapshot_preview1: {
                proc_exit: (code) => process.exit(code),
                fd_write: (fd, iov, iovcnt, pnum) => {
                    let written = 0;
                    const view = new DataView(this.memory.buffer);

                    for (let i = 0; i < iovcnt; i++) {
                        const ptr = view.getUint32(iov + i * 8, true);
                        const len = view.getUint32(iov + i * 8 + 4, true);
                        const bytes = new Uint8Array(this.memory.buffer, ptr, len);
                        const str = Buffer.from(bytes).toString('utf8');

                        if (fd === 1) {
                            process.stdout.write(str);
                        } else if (fd === 2) {
                            process.stderr.write(str);
                        }
                        written += len;
                    }

                    view.setUint32(pnum, written, true);
                    return 0;
                }
            }
        };
    }

    async load(filePath) {
        const buffer = fs.readFileSync(filePath);
        const module = await WebAssembly.compile(buffer);
        const instance = await WebAssembly.instantiate(module, this.createImportObject());
        return instance.exports;
    }
}

// 使用示例
async function main() {
    const loader = new NodeWasmLoader();
    const exports = await loader.load('./math_lib.wasm');

    console.log('2 + 3 =', exports.add(2, 3));
    console.log('10 - 4 =', exports.subtract(10, 4));
    console.log('6 * 7 =', exports.multiply(6, 7));
    console.log('20 / 4 =', exports.divide(20, 4));
    console.log('5! =', exports.factorial(5));
    console.log('fib(10) =', exports.fibonacci(10));
}

main().catch(console.error);
```

### 7.3 与 JavaScript 的交互模式

```
┌─────────────────────────────────────────────────────────────┐
│                JavaScript ↔ WebAssembly 交互模式             │
├─────────────────────────────────────────────────────────────┤
│                                                             │
│  模式1: 直接函数调用                                         │
│  ┌─────────────┐     调用      ┌─────────────┐             │
│  │  JavaScript │ ────────────> │    WASM     │             │
│  │  exports.fn()│              │   export    │             │
│  │             │ <──────────── │   function  │             │
│  └─────────────┘    返回值      └─────────────┘             │
│                                                             │
│  模式2: 共享内存访问                                         │
│  ┌─────────────┐              ┌─────────────┐              │
│  │  JavaScript │              │    WASM     │              │
│  │  读写内存    │ <──────────> │  内存操作    │              │
│  │  (ArrayBuffer)│  共享内存  │  (load/store)│             │
│  └─────────────┘              └─────────────┘              │
│                                                             │
│  模式3: 回调函数 (导入)                                       │
│  ┌─────────────┐              ┌─────────────┐              │
│  │  JavaScript │              │    WASM     │              │
│  │   JS函数    │ <──────────  │  调用导入函数 │              │
│  │  (import)   │   回调       │  (indirect) │              │
│  └─────────────┘              └─────────────┘              │
│                                                             │
│  模式4: 表调用 (动态分发)                                     │
│  ┌─────────────┐              ┌─────────────┐              │
│  │  JavaScript │              │    WASM     │              │
│  │  table.get()│ <──────────  │  call_indirect│             │
│  │  获取函数   │   函数表     │  通过索引调用 │              │
│  └─────────────┘              └─────────────┘              │
│                                                             │
└─────────────────────────────────────────────────────────────┘
```

---

## 8. 实战项目

### 8.1 项目1: 图像处理库

```c
// image_processor.c
#include <emscripten/emscripten.h>
#include <stdlib.h>
#include <string.h>

// 图像结构体
typedef struct {
    unsigned char* data;
    int width;
    int height;
    int channels;
} Image;

// 在 WASM 内存中分配图像
EMSCRIPTEN_KEEPALIVE
Image* create_image(int width, int height, int channels) {
    Image* img = (Image*)malloc(sizeof(Image));
    if (!img) return NULL;

    img->width = width;
    img->height = height;
    img->channels = channels;
    img->data = (unsigned char*)malloc(width * height * channels);

    if (!img->data) {
        free(img);
        return NULL;
    }

    return img;
}

// 释放图像
EMSCRIPTEN_KEEPALIVE
void destroy_image(Image* img) {
    if (img) {
        if (img->data) free(img->data);
        free(img);
    }
}

// 获取图像数据指针
EMSCRIPTEN_KEEPALIVE
unsigned char* get_image_data(Image* img) {
    return img ? img->data : NULL;
}

// 灰度转换
EMSCRIPTEN_KEEPALIVE
void grayscale(Image* src, Image* dst) {
    if (!src || !dst || src->width != dst->width || src->height != dst->height) return;

    int pixelCount = src->width * src->height;

    for (int i = 0; i < pixelCount; i++) {
        int idx = i * src->channels;
        unsigned char r = src->data[idx];
        unsigned char g = src->data[idx + 1];
        unsigned char b = src->data[idx + 2];

        // 加权灰度: 0.299*R + 0.587*G + 0.114*B
        unsigned char gray = (unsigned char)(0.299f * r + 0.587f * g + 0.114f * b);

        dst->data[i] = gray;
    }
}

// 亮度调整
EMSCRIPTEN_KEEPALIVE
void adjust_brightness(Image* img, int brightness) {
    if (!img) return;

    int size = img->width * img->height * img->channels;

    for (int i = 0; i < size; i++) {
        int val = img->data[i] + brightness;
        img->data[i] = (unsigned char)(val < 0 ? 0 : (val > 255 ? 255 : val));
    }
}

// 对比度调整
EMSCRIPTEN_KEEPALIVE
void adjust_contrast(Image* img, float contrast) {
    if (!img) return;

    int size = img->width * img->height * img->channels;
    float factor = (259.0f * (contrast + 255.0f)) / (255.0f * (259.0f - contrast));

    for (int i = 0; i < size; i++) {
        float val = factor * (img->data[i] - 128) + 128;
        img->data[i] = (unsigned char)(val < 0 ? 0 : (val > 255 ? 255 : val));
    }
}

// 高斯模糊 (简化版 - 3x3 核)
EMSCRIPTEN_KEEPALIVE
void blur(Image* src, Image* dst) {
    if (!src || !dst || src->width != dst->width || src->height != dst->height) return;

    float kernel[3][3] = {
        {1.0f/16, 2.0f/16, 1.0f/16},
        {2.0f/16, 4.0f/16, 2.0f/16},
        {1.0f/16, 2.0f/16, 1.0f/16}
    };

    int w = src->width;
    int h = src->height;
    int c = src->channels;

    for (int y = 1; y < h - 1; y++) {
        for (int x = 1; x < w - 1; x++) {
            for (int ch = 0; ch < c; ch++) {
                float sum = 0;
                for (int ky = -1; ky <= 1; ky++) {
                    for (int kx = -1; kx <= 1; kx++) {
                        int idx = ((y + ky) * w + (x + kx)) * c + ch;
                        sum += src->data[idx] * kernel[ky + 1][kx + 1];
                    }
                }
                int dstIdx = (y * w + x) * c + ch;
                dst->data[dstIdx] = (unsigned char)sum;
            }
        }
    }
}

// 边缘检测 (Sobel 算子)
EMSCRIPTEN_KEEPALIVE
void sobel_edge_detect(Image* src, Image* dst) {
    if (!src || !dst || src->width != dst->width || src->height != dst->height) return;

    int w = src->width;
    int h = src->height;

    int gx[3][3] = {{-1, 0, 1}, {-2, 0, 2}, {-1, 0, 1}};
    int gy[3][3] = {{-1, -2, -1}, {0, 0, 0}, {1, 2, 1}};

    for (int y = 1; y < h - 1; y++) {
        for (int x = 1; x < w - 1; x++) {
            int sumX = 0, sumY = 0;

            for (int ky = -1; ky <= 1; ky++) {
                for (int kx = -1; kx <= 1; kx++) {
                    int idx = ((y + ky) * w + (x + kx)) * src->channels;
                    unsigned char gray = (src->data[idx] + src->data[idx + 1] + src->data[idx + 2]) / 3;
                    sumX += gray * gx[ky + 1][kx + 1];
                    sumY += gray * gy[ky + 1][kx + 1];
                }
            }

            int magnitude = (int)sqrt(sumX * sumX + sumY * sumY);
            if (magnitude > 255) magnitude = 255;

            int dstIdx = (y * w + x);
            dst->data[dstIdx] = (unsigned char)magnitude;
        }
    }
}

// 颜色反转
EMSCRIPTEN_KEEPALIVE
void invert(Image* img) {
    if (!img) return;

    int size = img->width * img->height * img->channels;
    for (int i = 0; i < size; i++) {
        img->data[i] = 255 - img->data[i];
    }
}

// 图像缩略图生成 (最近邻插值)
EMSCRIPTEN_KEEPALIVE
void resize_nearest(Image* src, Image* dst, int newWidth, int newHeight) {
    if (!src || !dst) return;

    float xRatio = (float)src->width / newWidth;
    float yRatio = (float)src->height / newHeight;
    int c = src->channels;

    for (int y = 0; y < newHeight; y++) {
        for (int x = 0; x < newWidth; x++) {
            int srcX = (int)(x * xRatio);
            int srcY = (int)(y * yRatio);

            int srcIdx = (srcY * src->width + srcX) * c;
            int dstIdx = (y * newWidth + x) * c;

            for (int ch = 0; ch < c; ch++) {
                dst->data[dstIdx + ch] = src->data[srcIdx + ch];
            }
        }
    }
}

// 获取图像尺寸信息
EMSCRIPTEN_KEEPALIVE
int get_image_width(Image* img) { return img ? img->width : 0; }
EMSCRIPTEN_KEEPALIVE
int get_image_height(Image* img) { return img ? img->height : 0; }
EMSCRIPTEN_KEEPALIVE
int get_image_channels(Image* img) { return img ? img->channels : 0; }
```

编译命令：

```bash
emcc image_processor.c -O3 \
    -sEXPORTED_FUNCTIONS="['_create_image', '_destroy_image', '_get_image_data', '_grayscale', '_adjust_brightness', '_adjust_contrast', '_blur', '_sobel_edge_detect', '_invert', '_resize_nearest', '_get_image_width', '_get_image_height', '_get_image_channels', '_malloc', '_free']" \
    -sEXPORTED_RUNTIME_METHODS="['ccall', 'cwrap', 'getValue', 'setValue']" \
    -sALLOW_MEMORY_GROWTH=1 \
    -sINITIAL_MEMORY=32MB \
    -sMAXIMUM_MEMORY=256MB \
    -sMODULARIZE=1 \
    -sEXPORT_NAME="ImageProcessor" \
    -o image_processor.js
```

### 8.2 项目2: 高性能矩阵运算库

```c
// matrix.c
#include <emscripten/emscripten.h>
#include <stdlib.h>
#include <math.h>

// 矩阵结构
typedef struct {
    float* data;
    int rows;
    int cols;
} Matrix;

// 创建矩阵
EMSCRIPTEN_KEEPALIVE
Matrix* matrix_create(int rows, int cols) {
    Matrix* mat = (Matrix*)malloc(sizeof(Matrix));
    if (!mat) return NULL;

    mat->rows = rows;
    mat->cols = cols;
    mat->data = (float*)calloc(rows * cols, sizeof(float));

    if (!mat->data) {
        free(mat);
        return NULL;
    }

    return mat;
}

// 销毁矩阵
EMSCRIPTEN_KEEPALIVE
void matrix_destroy(Matrix* mat) {
    if (mat) {
        if (mat->data) free(mat->data);
        free(mat);
    }
}

// 获取/设置元素
EMSCRIPTEN_KEEPALIVE
float matrix_get(Matrix* mat, int row, int col) {
    if (!mat || row < 0 || row >= mat->rows || col < 0 || col >= mat->cols) return 0;
    return mat->data[row * mat->cols + col];
}

EMSCRIPTEN_KEEPALIVE
void matrix_set(Matrix* mat, int row, int col, float value) {
    if (!mat || row < 0 || row >= mat->rows || col < 0 || col >= mat->cols) return;
    mat->data[row * mat->cols + col] = value;
}

// 获取数据指针
EMSCRIPTEN_KEEPALIVE
float* matrix_get_data(Matrix* mat) {
    return mat ? mat->data : NULL;
}

// 矩阵加法: C = A + B
EMSCRIPTEN_KEEPALIVE
void matrix_add(const Matrix* A, const Matrix* B, Matrix* C) {
    if (!A || !B || !C) return;
    if (A->rows != B->rows || A->cols != B->cols) return;
    if (A->rows != C->rows || A->cols != C->cols) return;

    int size = A->rows * A->cols;
    for (int i = 0; i < size; i++) {
        C->data[i] = A->data[i] + B->data[i];
    }
}

// 矩阵减法: C = A - B
EMSCRIPTEN_KEEPALIVE
void matrix_subtract(const Matrix* A, const Matrix* B, Matrix* C) {
    if (!A || !B || !C) return;
    if (A->rows != B->rows || A->cols != B->cols) return;
    if (A->rows != C->rows || A->cols != C->cols) return;

    int size = A->rows * A->cols;
    for (int i = 0; i < size; i++) {
        C->data[i] = A->data[i] - B->data[i];
    }
}

// 矩阵乘法: C = A × B
EMSCRIPTEN_KEEPALIVE
void matrix_multiply(const Matrix* A, const Matrix* B, Matrix* C) {
    if (!A || !B || !C) return;
    if (A->cols != B->rows) return;
    if (C->rows != A->rows || C->cols != B->cols) return;

    int m = A->rows;
    int n = A->cols;
    int p = B->cols;

    // 初始化 C 为零
    for (int i = 0; i < m * p; i++) {
        C->data[i] = 0;
    }

    // 矩阵乘法
    for (int i = 0; i < m; i++) {
        for (int k = 0; k < n; k++) {
            float a_ik = A->data[i * n + k];
            for (int j = 0; j < p; j++) {
                C->data[i * p + j] += a_ik * B->data[k * p + j];
            }
        }
    }
}

// 矩阵转置
EMSCRIPTEN_KEEPALIVE
void matrix_transpose(const Matrix* A, Matrix* B) {
    if (!A || !B) return;
    if (B->rows != A->cols || B->cols != A->rows) return;

    for (int i = 0; i < A->rows; i++) {
        for (int j = 0; j < A->cols; j++) {
            B->data[j * A->rows + i] = A->data[i * A->cols + j];
        }
    }
}

// 标量乘法
EMSCRIPTEN_KEEPALIVE
void matrix_scale(Matrix* A, float scalar) {
    if (!A) return;

    int size = A->rows * A->cols;
    for (int i = 0; i < size; i++) {
        A->data[i] *= scalar;
    }
}

// 矩阵填充随机值
EMSCRIPTEN_KEEPALIVE
void matrix_randomize(Matrix* A) {
    if (!A) return;

    int size = A->rows * A->cols;
    for (int i = 0; i < size; i++) {
        A->data[i] = (float)rand() / RAND_MAX * 2.0f - 1.0f;
    }
}

// 单位矩阵
EMSCRIPTEN_KEEPALIVE
void matrix_identity(Matrix* A) {
    if (!A || A->rows != A->cols) return;

    int n = A->rows;
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            A->data[i * n + j] = (i == j) ? 1.0f : 0.0f;
        }
    }
}

// 高斯消元法求逆矩阵
EMSCRIPTEN_KEEPALIVE
int matrix_inverse(const Matrix* A, Matrix* B) {
    if (!A || !B || A->rows != A->cols || B->rows != B->cols) return 0;
    if (A->rows != B->rows) return 0;

    int n = A->rows;

    // 创建增广矩阵 [A | I]
    float* aug = (float*)malloc(n * 2 * n * sizeof(float));
    if (!aug) return 0;

    // 初始化增广矩阵
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            aug[i * 2 * n + j] = A->data[i * n + j];
            aug[i * 2 * n + j + n] = (i == j) ? 1.0f : 0.0f;
        }
    }

    // 高斯消元
    for (int i = 0; i < n; i++) {
        // 找主元
        float maxVal = fabs(aug[i * 2 * n + i]);
        int maxRow = i;
        for (int k = i + 1; k < n; k++) {
            float val = fabs(aug[k * 2 * n + i]);
            if (val > maxVal) {
                maxVal = val;
                maxRow = k;
            }
        }

        if (maxVal < 1e-10) {
            free(aug);
            return 0;  // 奇异矩阵
        }

        // 交换行
        if (maxRow != i) {
            for (int j = 0; j < 2 * n; j++) {
                float temp = aug[i * 2 * n + j];
                aug[i * 2 * n + j] = aug[maxRow * 2 * n + j];
                aug[maxRow * 2 * n + j] = temp;
            }
        }

        // 归一化主元行
        float pivot = aug[i * 2 * n + i];
        for (int j = 0; j < 2 * n; j++) {
            aug[i * 2 * n + j] /= pivot;
        }

        // 消去其他行
        for (int k = 0; k < n; k++) {
            if (k != i) {
                float factor = aug[k * 2 * n + i];
                for (int j = 0; j < 2 * n; j++) {
                    aug[k * 2 * n + j] -= factor * aug[i * 2 * n + j];
                }
            }
        }
    }

    // 提取逆矩阵
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            B->data[i * n + j] = aug[i * 2 * n + j + n];
        }
    }

    free(aug);
    return 1;
}

// 矩阵行列式
EMSCRIPTEN_KEEPALIVE
float matrix_determinant(const Matrix* A) {
    if (!A || A->rows != A->cols) return 0;

    int n = A->rows;
    if (n == 1) return A->data[0];
    if (n == 2) {
        return A->data[0] * A->data[3] - A->data[1] * A->data[2];
    }

    // 复制矩阵用于LU分解
    float* temp = (float*)malloc(n * n * sizeof(float));
    if (!temp) return 0;
    memcpy(temp, A->data, n * n * sizeof(float));

    float det = 1.0f;
    int swaps = 0;

    for (int i = 0; i < n; i++) {
        // 找主元
        float maxVal = fabs(temp[i * n + i]);
        int maxRow = i;
        for (int k = i + 1; k < n; k++) {
            float val = fabs(temp[k * n + i]);
            if (val > maxVal) {
                maxVal = val;
                maxRow = k;
            }
        }

        if (maxVal < 1e-10) {
            free(temp);
            return 0;
        }

        // 交换行
        if (maxRow != i) {
            for (int j = 0; j < n; j++) {
                float t = temp[i * n + j];
                temp[i * n + j] = temp[maxRow * n + j];
                temp[maxRow * n + j] = t;
            }
            swaps++;
        }

        // 消元
        for (int k = i + 1; k < n; k++) {
            float factor = temp[k * n + i] / temp[i * n + i];
            for (int j = i; j < n; j++) {
                temp[k * n + j] -= factor * temp[i * n + j];
            }
        }

        det *= temp[i * n + i];
    }

    free(temp);
    return (swaps % 2 == 0) ? det : -det;
}

// 求特征值（简化版 - 幂迭代法求最大特征值）
EMSCRIPTEN_KEEPALIVE
float matrix_max_eigenvalue(const Matrix* A, int maxIterations) {
    if (!A || A->rows != A->cols) return 0;

    int n = A->rows;
    float* b = (float*)malloc(n * sizeof(float));
    float* Ab = (float*)malloc(n * sizeof(float));
    if (!b || !Ab) return 0;

    // 初始向量
    for (int i = 0; i < n; i++) b[i] = 1.0f;

    float eigenvalue = 0;

    for (int iter = 0; iter < maxIterations; iter++) {
        // 计算 Ab = A * b
        for (int i = 0; i < n; i++) {
            Ab[i] = 0;
            for (int j = 0; j < n; j++) {
                Ab[i] += A->data[i * n + j] * b[j];
            }
        }

        // 归一化
        float norm = 0;
        for (int i = 0; i < n; i++) norm += Ab[i] * Ab[i];
        norm = sqrt(norm);

        eigenvalue = norm;

        for (int i = 0; i < n; i++) {
            b[i] = Ab[i] / norm;
        }
    }

    free(b);
    free(Ab);
    return eigenvalue;
}

// 获取矩阵维度
EMSCRIPTEN_KEEPALIVE
int matrix_get_rows(const Matrix* mat) { return mat ? mat->rows : 0; }
EMSCRIPTEN_KEEPALIVE
int matrix_get_cols(const Matrix* mat) { return mat ? mat->cols : 0; }
```

编译命令：

```bash
emcc matrix.c -O3 \
    -sEXPORTED_FUNCTIONS="['_matrix_create', '_matrix_destroy', '_matrix_get', '_matrix_set', '_matrix_get_data', '_matrix_add', '_matrix_subtract', '_matrix_multiply', '_matrix_transpose', '_matrix_scale', '_matrix_randomize', '_matrix_identity', '_matrix_inverse', '_matrix_determinant', '_matrix_max_eigenvalue', '_matrix_get_rows', '_matrix_get_cols', '_malloc', '_free']" \
    -sEXPORTED_RUNTIME_METHODS="['ccall', 'cwrap']" \
    -sALLOW_MEMORY_GROWTH=1 \
    -sINITIAL_MEMORY=64MB \
    -sMODULARIZE=1 \
    -sEXPORT_NAME="MatrixLib" \
    -o matrix.js
```

---

## 9. 常见问题与解决方案

### 9.1 编译问题

| 问题 | 原因 | 解决方案 |
|------|------|----------|
| `undefined symbol: main` | 库文件没有 main 函数 | 添加 `-sEXPORTED_RUNTIME_METHODS` 或定义 main |
| `memory access out of bounds` | 内存越界访问 | 检查指针和数组边界，增加初始内存 |
| `function signature mismatch` | 导入/导出函数签名不匹配 | 检查参数类型和数量 |
| `unreachable` | WASM 执行到 unreachable 指令 | 检查代码逻辑，避免未定义行为 |

### 9.2 运行时问题

```javascript
// 问题1: SharedArrayBuffer 未定义
// 解决方案：配置正确的 HTTP 响应头
/*
Cross-Origin-Opener-Policy: same-origin
Cross-Origin-Embedder-Policy: require-corp
*/

// 问题2: 内存不足
// 解决方案：增加内存限制
-sINITIAL_MEMORY=128MB \
-sMAXIMUM_MEMORY=2GB \
-sALLOW_MEMORY_GROWTH=1

// 问题3: 调用导出函数失败
// 确保使用正确的名称（带下划线前缀）
Module._myFunction();  // C 函数名为 myFunction
Module.ccall('myFunction', ...);  // 不带下划线

// 问题4: 字符串传递问题
// 使用正确的编码和解码
function stringToNewUTF8(str) {
    const size = lengthBytesUTF8(str) + 1;
    const ptr = Module._malloc(size);
    stringToUTF8(str, ptr, size);
    return ptr;
}

function freeString(ptr) {
    Module._free(ptr);
}
```

### 9.3 性能优化建议

| 优化方向 | 具体措施 |
|----------|----------|
| 编译优化 | 使用 `-O3` 或 `-Oz`，启用 LTO |
| 内存管理 | 预分配足够内存，避免频繁增长 |
| 减少边界检查 | 使用 `-sALLOW_MEMORY_GROWTH=0` 固定内存 |
| 批量数据传输 | 减少 JS/WASM 边界跨越次数 |
| 使用 Worker | 将计算放到 Web Worker |
| 启用 SIMD | 使用 `-msimd128` |

---

## 10. 参考资源

### 官方文档

- [WebAssembly 官方规范](https://webassembly.github.io/spec/)
- [Emscripten 文档](https://emscripten.org/docs/)
- [MDN WebAssembly 指南](https://developer.mozilla.org/zh-CN/docs/WebAssembly)

### 工具和库

| 工具 | 用途 |
|------|------|
| [wasm2wat](https://github.com/WebAssembly/wabt) | WASM 反汇编为 WAT |
| [wat2wasm](https://github.com/WebAssembly/wabt) | WAT 汇编为 WASM |
| [wasm-opt](https://github.com/WebAssembly/binaryen) | WASM 优化工具 |
| [wasmer](https://wasmer.io/) | 独立 WASM 运行时 |
| [wasmtime](https://wasmtime.dev/) | 另一个 WASM 运行时 |

### 学习资源

- [WebAssembly Weekly](https://wasmweekly.news/) - 周刊
- [Awesome WebAssembly](https://github.com/mbasso/awesome-wasm) - 资源汇总
- [Wasm By Example](https://wasmbyexample.dev/) - 示例教程

---

## 附录：完整的命令速查表

```bash
### ========== 基础编译 ==========
emcc input.c -o output.html                    # 生成 HTML + JS + WASM
emcc input.c -o output.js                      # 生成 JS + WASM
emcc input.c -o output.wasm                    # 仅 WASM

### ========== 优化选项 ==========
emcc input.c -O0 -o output.js                  # 无优化（调试）
emcc input.c -O1 -o output.js                  # 基本优化
emcc input.c -O2 -o output.js                  # 更多优化
emcc input.c -O3 -o output.js                  # 激进优化（推荐）
emcc input.c -Os -o output.js                  # 优化体积
emcc input.c -Oz -o output.js                  # 极致体积优化

### ========== 调试选项 ==========
emcc input.c -g -o output.js                   # 包含调试信息
emcc input.c -g3 -o output.js                  # DWARF 格式
emcc input.c -g4 -o output.js                  # Source Map

### ========== 内存选项 ==========
emcc input.c -sINITIAL_MEMORY=64MB -o output.js
emcc input.c -sMAXIMUM_MEMORY=256MB -o output.js
emcc input.c -sALLOW_MEMORY_GROWTH=1 -o output.js
emcc input.c -sSTACK_SIZE=8MB -o output.js

### ========== 导出选项 ==========
emcc input.c -sEXPORTED_FUNCTIONS="['_main', '_foo']" -o output.js
emcc input.c -sEXPORTED_RUNTIME_METHODS="['ccall', 'cwrap']" -o output.js

### ========== 模块化选项 ==========
emcc input.c -sMODULARIZE=1 -sEXPORT_NAME="MyModule" -o output.js
emcc input.c -sEXPORT_ES6=1 -o output.js

### ========== 文件系统 ==========
emcc input.c --preload-file assets/ -o output.js
emcc input.c --embed-file data.txt -o output.js
emcc input.c -sFILESYSTEM=0 -o output.js       # 禁用文件系统

### ========== 库链接 ==========
emcc input.c -sUSE_SDL=2 -o output.js
emcc input.c -sUSE_SDL=2 -sUSE_SDL_IMAGE=2 -o output.js
emcc input.c -sUSE_SDL_MIXER=2 -o output.js
emcc input.c -sUSE_SDL_TTF=2 -o output.js
emcc input.c -sUSE_GLFW=3 -o output.js
emcc input.c -sUSE_WEBGL2=1 -o output.js
emcc input.c -sUSE_PTHREADS=1 -o output.js

### ========== 网络选项 ==========
emcc input.c -sFETCH=1 -o output.js
emcc input.c -sALLOW_SYNC_XHR_IN_MAIN_THREAD=1 -o output.js

### ========== 高级优化 ==========
emcc input.c -flto -o output.js                # 链接时优化
emcc input.c --closure 1 -o output.js          # JS 闭包优化
emcc input.c -sWASM=1 -sSIDE_MODULE=1 -o output.wasm  # 动态链接
```

---

*文档版本: 1.0*
*最后更新: 2024年*
*作者: C_Lang 项目团队*


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
