# Compiler Explorer 使用指南

> 版本: 1.0
> 更新日期: 2026-03-19

## 什么是 Compiler Explorer

Compiler Explorer (godbolt.org) 是一个在线编译器探索工具，由 Matt Godbolt 创建。它允许你：

- 在浏览器中编译代码
- 实时查看生成的汇编代码
- 对比不同编译器和优化级别的输出
- 分享代码片段
- 在教学中直观展示编译过程

## 基础使用

### 界面介绍

Compiler Explorer 主界面分为几个区域：

```
┌─────────────────────────────────────────────────────────────┐
│  工具栏: 编译器选择 | 优化级别 | 编译选项 | 分享 | 设置       │
├──────────────────────┬──────────────────────────────────────┤
│                      │                                      │
│   源代码编辑器        │         汇编输出显示                  │
│                      │                                      │
│   (C/C++/Rust/...)   │         (彩色高亮, 行号对应)           │
│                      │                                      │
├──────────────────────┴──────────────────────────────────────┤
│  底部: 执行输出 | 编译信息 | 错误提示                         │
└─────────────────────────────────────────────────────────────┘
```

### 快速开始

1. 访问 <https://godbolt.org>
2. 在左侧编辑器输入C代码
3. 选择编译器 (如 x86-64 gcc 12.2)
4. 选择优化级别 (-O0, -O1, -O2, -O3)
5. 右侧自动显示汇编输出

### 快捷键

| 快捷键 | 功能 |
|-------|------|
| Ctrl + Enter | 手动编译 |
| Ctrl + S | 保存到本地 |
| Ctrl + Z | 撤销 |
| F1 | 显示所有快捷键 |

## 教学中的应用

### 1. 展示编译器优化

**教学目标**: 让学生理解编译器优化的效果

**示例**: 展示循环展开

```c
// 源代码
int sum(int *arr, int n) {
    int s = 0;
    for (int i = 0; i < n; i++) {
        s += arr[i];
    }
    return s;
}
```

**教学活动**:

1. 先使用 `-O0` 查看未优化代码
2. 切换到 `-O2` 查看优化后的汇编
3. 让学生对比差异
4. 讨论循环展开、寄存器分配等优化

### 2. 理解指针和数组

**教学目标**: 展示指针和数组在汇编层的等价性

**示例**:

```c
void ptr_access(int *p) {
    *p = 42;
    *(p + 1) = 43;
}

void array_access(int arr[]) {
    arr[0] = 42;
    arr[1] = 43;
}
```

**教学活动**:

1. 编译两个函数
2. 对比生成的汇编代码
3. 学生会发现两者几乎相同
4. 强化"数组就是指针"的概念

### 3. 函数调用约定

**教学目标**: 理解参数传递和栈帧

**示例**:

```c
int add(int a, int b, int c, int d, int e, int f, int g) {
    return a + b + c + d + e + f + g;
}
```

**教学活动**:

1. 查看汇编代码
2. 识别寄存器传参 (RDI, RSI, RDX, RCX, R8, R9)
3. 观察栈上传参 (第7个参数)
4. 讨论不同平台的调用约定差异

### 4. 内存布局可视化

**教学目标**: 理解结构体内存布局

**示例**:

```c
struct Bad {
    char a;   // 1 byte
    int b;    // 4 bytes
    char c;   // 1 byte
};

struct Good {
    int b;    // 4 bytes
    char a;   // 1 byte
    char c;   // 1 byte
};
```

**教学活动**:

1. 使用 `sizeof` 在代码中打印
2. 也可以查看汇编中的偏移量
3. 讨论内存对齐和填充

## 嵌入代码示例

### 使用 iframe 嵌入

可以在教学网页中嵌入 Compiler Explorer：

```html
<iframe
    src="https://godbolt.org/e?hideEditorToolbars=true&readOnly=true&..."
    width="800"
    height="600"
    frameborder="0">
</iframe>
```

### 创建分享链接

1. 编写示例代码
2. 点击 "Share" 按钮
3. 选择分享方式：
   - **Short Link**: 短链接 (推荐)
   - **Full Link**: 完整链接 (包含所有设置)
   - **Embed**: HTML嵌入代码
   - **CppInsights**: 转发到 CppInsights

### 示例链接格式

```
https://godbolt.org/z/[short-code]
```

## 配置编译选项

### 常用编译选项

| 选项 | 说明 |
|-----|------|
| `-O0` | 无优化，调试友好 |
| `-O1` | 基本优化 |
| `-O2` | 标准优化 (推荐) |
| `-O3` | 激进优化 |
| `-Os` | 优化代码大小 |
| `-g` | 包含调试信息 |
| `-Wall` | 显示所有警告 |
| `-Wextra` | 显示额外警告 |
| `-march=native` | 针对当前CPU优化 |
| `-S` | 只编译到汇编 |
| `-masm=intel` | Intel汇编语法 |

### 查看特定优化

```bash
# 查看向量化报告
-fopt-info-vec-all

# 查看内联报告
-fopt-info-inline

# 启用LTO
-flto
```

## 多编译器对比

### 添加多个面板

1. 点击 "Add New..." → "Compiler"
2. 选择不同的编译器
3. 可以同时对比 GCC、Clang、MSVC

### 教学示例：对比优化能力

设置三个面板：

- GCC 11.2 -O2
- Clang 14.0 -O2
- MSVC 19.29 -O2

输入同一段代码，对比生成的汇编质量。

## 执行代码

### 启用执行

1. 点击 "Output" 按钮
2. 勾选 "Execute the code"
3. 底部会显示执行结果

### 添加输入

```c
#include <stdio.h>

int main() {
    int x;
    scanf("%d", &x);
    printf("You entered: %d\n", x);
    return 0;
}
```

在 "Stdin" 框中输入测试数据。

## 高级功能

### 1. 查看预处理输出

添加编译器选项：`-E`

### 2. 查看 LLVM IR

选择 Clang 编译器，在 "Add Tool" 中选择 "LLVM IR"

### 3. 控制流图

在 "Add Tool" 中选择 "Graph" → "Control Flow Graph"

### 4. 性能分析

使用 "Execution Only" 模式和计时工具。

## 在课堂中的最佳实践

### 课前准备

1. 预先编写好示例代码
2. 生成短链接
3. 测试在不同浏览器中的显示
4. 准备备选方案 (网络问题)

### 课堂使用技巧

1. **渐进式展示**
   - 先写简单版本
   - 逐步添加复杂性
   - 观察汇编变化

2. **对比教学**
   - 好代码 vs 坏代码
   - 不同优化级别
   - 不同编译器

3. **互动提问**
   - "猜猜这段代码生成多少汇编？"
   - "这个优化是安全吗？"
   - "为什么编译器这样做？"

4. **保存和分享**
   - 课后分享链接
   - 学生可以自行探索
   - 鼓励修改和实验

### 常见教学场景

#### 场景1：理解 undefined behavior

```c
int foo(int x) {
    return x + 1 > x;  // 有符号溢出是 UB
}
```

- 使用不同优化级别编译
- 观察编译器如何假设无溢出

#### 场景2：volatile 的作用

```c
volatile int flag = 0;

void wait() {
    while (!flag);  // 不会优化掉
}
```

- 对比有/无 volatile 的汇编
- 理解内存可见性

#### 场景3：内联的效果

```c
static int add(int a, int b) {
    return a + b;
}

int calc() {
    return add(1, 2) + add(3, 4);
}
```

- 使用 `-O0`：看到函数调用
- 使用 `-O2`：看到直接计算结果

## 故障排除

### 常见问题

1. **汇编太长看不完**
   - 使用 "Filter..." 筛选特定函数
   - 勾选 "Directives" 过滤掉编译器指令

2. **看不懂汇编**
   - 启用 "Compile to Binary" 查看机器码
   - 使用 "Colorize" 高亮对应源码

3. **链接错误**
   - 添加 `-c` 选项只编译
   - 或者提供 `main` 函数

4. **执行无输出**
   - 检查是否启用 "Execute the code"
   - 检查程序是否返回0

## 相关资源

### 官方资源

- 网站: <https://godbolt.org>
- GitHub: <https://github.com/compiler-explorer/compiler-explorer>
- Patreon: 支持项目开发

### 学习资源

- Matt Godbolt 的 CppCon 演讲
- Compiler Explorer 官方文档
- 《Computer Systems: A Programmer's Perspective》

### 类似工具

- **CppInsights**: 查看模板展开等
- **Quick C++ Benchmark**: 快速性能测试
- **Wandbox**: 多语言在线编译
- **Coliru**: 简单C++编译

## 示例代码库

以下是适合教学的代码示例：

### 示例1：函数调用开销

```c
int global;

void by_value(int x) {
    global = x;
}

void by_pointer(int *x) {
    global = *x;
}

void by_reference(int &x) {
    global = x;
}
```

### 示例2：循环优化

```c
int sum_array(int *arr, int n) {
    int sum = 0;
    for (int i = 0; i < n; i++) {
        sum += arr[i];
    }
    return sum;
}
```

### 示例3：条件分支

```c
int max(int a, int b) {
    if (a > b) return a;
    return b;
}

// 对比
int max_conditional(int a, int b) {
    return (a > b) ? a : b;
}
```

### 示例4：尾递归

```c
// 非尾递归
int factorial(int n) {
    if (n <= 1) return 1;
    return n * factorial(n - 1);
}

// 尾递归
int factorial_tail(int n, int acc) {
    if (n <= 1) return acc;
    return factorial_tail(n - 1, n * acc);
}
```

## 结语

Compiler Explorer 是理解编译过程和代码优化的强大工具。在教学中合理使用，可以：

- 让抽象的编译原理具体化
- 培养学生对代码质量的直觉
- 激发对底层实现的兴趣
- 辅助调试和优化实践

建议每位C语言学习者都熟练使用这个工具。


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
