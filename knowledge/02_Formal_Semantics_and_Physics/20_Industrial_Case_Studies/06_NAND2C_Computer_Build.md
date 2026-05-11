# 从 NAND 门到 C 编译器：完整计算机构建指南

> **层级定位**: 02 Formal Semantics and Physics / 20 Industrial Case Studies
> **主题**: 从零开始构建完整计算机系统
> **难度级别**: L2 理解 → L5 专家（渐进式）
> **预估学习时间**: 60-100 小时
> **对标课程**: NAND2Tetris (Hebrew University) + Stanford CS107E

## 概述

本项目指导你**从最基本的 NAND 门开始**，逐步构建一个完整的计算机系统，最终能够运行用 C 语言编写的程序。
这不是理论练习——你将亲手设计数字电路、编写汇编器、实现虚拟机、构建编译器，并理解每一层抽象是如何建立在更底层之上的。

这是理解"硬件到 C"最完整的学习路径。

---

## 项目总览

```
NAND 门 → 逻辑门 → ALU → 寄存器 → CPU → 机器语言 → 汇编器 → 虚拟机 → 编译器 → OS → C 程序
   │         │       │      │       │       │          │         │         │       │      │
   └─────────┴───────┴──────┴───────┴───────┴──────────┴─────────┴─────────┴───────┴──────┘
   Project 1   2      3      4       5       6          7         8         9      10
```

---

## Project 1: 布尔逻辑与算术逻辑单元 (ALU)

### 目标

仅用 NAND 门构建所有基本逻辑门和 16 位 ALU。

### 核心组件

```verilog
// 基本逻辑门（从 NAND 构建）
NOT(x)   = NAND(x, x)
AND(x,y) = NOT(NAND(x, y))
OR(x,y)  = NAND(NOT(x), NOT(y))
XOR(x,y) = OR(AND(x, NOT(y)), AND(NOT(x), y))

// 16 位 ALU 操作
// zx, nx, zy, ny, f, no: 6 位控制信号
// out = f ? (x op y) : (x & y)
// 支持: 加法、减法、与、或、零检测、负数检测
```

### 输出

- `And16`, `Or16`, `Mux16`, `Add16`, `ALU` 的 HDL 实现
- ALU 测试向量验证全部 18 种操作组合

---

## Project 2: 顺序逻辑与内存

### 目标

构建触发器、寄存器和分层内存系统。

### 核心组件

```verilog
// D 触发器（1 位存储）
DFF(in, clock) → out(t+1) = in(t)

// 1 位寄存器（带加载控制）
Bit(in, load, clock) →
    if load: out = in
    else:    out = out  (保持)

// 16 位寄存器
Register16(in[16], load, clock) → out[16]

// 分层 RAM
RAM8   (3 位地址, 8 个寄存器)
RAM64  (6 位地址, 64 个寄存器)
RAM512 (9 位地址, 512 个寄存器)
RAM4K  (12 位地址)
RAM16K (14 位地址)
```

---

## Project 3: 处理器架构 (CPU)

### 目标

设计一个完整的 16 位 CPU，支持完整的指令集。

### 指令集设计 (Hack ISA)

```
A 指令 (地址指令): 0vvv vvvv vvvv vvvv  → 将常数 v 加载到 A 寄存器
C 指令 (计算指令): 111a c1c2c3c4c5c6 d1d2 j1j2j3
    a: 选择 ALU 输入 (A 或 M)
    c1-c6: ALU 控制位
    d1-d2: 目标寄存器 (A, D, M)
    j1-j3: 跳转条件 (<0, =0, >0)
```

### CPU 数据通路

```
     ┌─────────┐
     │   PC    │ ──→ 指令内存地址
     └────┬────┘
          │
     ┌────▼────┐     ┌─────────┐
     │  ROM32K │ ──→ │ 指令寄存器│
     └─────────┘     └────┬────┘
                          │
                    ┌─────┴─────┐
                    │  控制单元   │
                    │ (解码 c,d,j)│
                    └─────┬─────┘
          ┌───────────────┼───────────────┐
          ▼               ▼               ▼
     ┌─────────┐    ┌─────────┐    ┌─────────┐
     │   A寄存器 │    │   D寄存器 │    │   ALU   │
     └────┬────┘    └────┬────┘    └────┬────┘
          │              │              │
          └──────────────┴──────────────┘
                         │
                    ┌────▼────┐
                    │  Mux    │ ──→ M (数据内存)
                    └─────────┘
```

---

## Project 4: 机器语言与汇编器

### 目标

为 Hack ISA 编写汇编器，将助记符转换为二进制机器码。

### 汇编器核心逻辑

```python
# 两遍汇编器
class Assembler:
    def __init__(self):
        self.symbol_table = {
            'SP': 0, 'LCL': 1, 'ARG': 2, 'THIS': 3, 'THAT': 4,
            'R0': 0, 'R1': 1, ... 'R15': 15,
            'SCREEN': 16384, 'KBD': 24576,
        }
        self.next_var = 16

    def first_pass(self, lines):
        # 记录标签位置 (LABEL)
        for i, line in enumerate(lines):
            if line.startswith('('):
                symbol = line[1:-1]
                self.symbol_table[symbol] = i

    def second_pass(self, lines):
        # 转换指令
        for line in lines:
            if line.startswith('@'):
                yield self.compile_a_instruction(line)
            else:
                yield self.compile_c_instruction(line)
```

---

## Project 5: 虚拟机 (VM) 翻译器

### 目标

将基于栈的 VM 代码翻译为 Hack 汇编。

### VM 指令集

```
# 内存访问
push segment index    # 将 segment[index] 压栈
pop  segment index    # 弹栈到 segment[index]

# 算术/逻辑
add, sub, neg, eq, gt, lt, and, or, not

# 程序流
label LABEL
goto LABEL
if-goto LABEL

# 函数调用
function FnName nLocals
call FnName nArgs
return
```

### VM 到汇编翻译示例

```vm
// VM 代码
push constant 7
push constant 8
add

// 翻译后的 Hack 汇编
@7
D=A
@SP
A=M
M=D
@SP
M=M+1

@8
D=A
@SP
A=M
M=D
@SP
M=M+1

@SP
AM=M-1
D=M
A=A-1
M=M+D
```

---

## Project 6: 编译器前端（Jack → VM）

### 目标

构建一个完整编译器，将类 C 语言（Jack）编译为 VM 代码。

### 编译器架构

```
Jack 源代码
    │
    ▼
Tokenizer (词法分析) → XML/Token 流
    │
    ▼
Parser (语法分析) → AST (XML 表示)
    │
    ▼
Code Generator (代码生成) → VM 代码
```

### Jack 语言语法（类 C 子集）

```jack
// Jack 程序示例
class Main {
    function void main() {
        var int x, y;
        let x = 3;
        let y = 5;
        do Output.printInt(x + y);
        return;
    }
}
```

---

## Project 7: 操作系统基础

### 目标

构建一个最小操作系统，提供内存管理、数学库、字符串处理和 I/O 驱动。

### OS 模块

| 模块 | 功能 |
|:-----|:-----|
| `Sys` | 系统初始化、halt |
| `Memory` | 内存分配/释放（基于链表） |
| `Math` | 乘、除、平方根（位运算实现） |
| `String` | 字符串操作 |
| `Array` | 数组分配 |
| `Output` | 屏幕输出（字符映射） |
| `Keyboard` | 键盘输入 |

---

## Project 8: C 语言移植（进阶）

### 目标

在 Hack 平台上运行真正的 C 程序。

### 扩展路径

```
Hack 平台 (16-bit) → RISC-V (32/64-bit)
    │                      │
    ▼                      ▼
Jack 编译器          RISC-V GCC 交叉编译
Jack OS              Newlib / 裸机运行时
    │                      │
    └──────────┬───────────┘
               ▼
         运行 C 程序！
```

### 从 Jack 到 C 的关键升级

| 方面 | Jack | C |
|:-----|:-----|:--|
| 类型系统 | 仅有 int, boolean, char | 完整类型系统 + 指针 |
| 内存模型 | 无指针，数组即对象 | 指针算术、地址操作 |
| 预处理器 | 无 | `#include`, `#define`, 宏 |
| 标准库 | 最小 OS 库 | libc + POSIX |
| 编译器 | 教学用 | GCC/Clang/LLVM |

---

## 学习路径建议

| 阶段 | 项目 | 时间 | 前置知识 |
|:-----|:-----|:-----|:---------|
| 1 | Project 1-2 (逻辑门→内存) | 10h | 布尔代数基础 |
| 2 | Project 3-4 (CPU→汇编器) | 15h | 数字逻辑、二进制运算 |
| 3 | Project 5 (VM 翻译器) | 10h | 栈数据结构 |
| 4 | Project 6 (编译器前端) | 20h | 递归下降解析 |
| 5 | Project 7 (操作系统) | 15h | 内存管理概念 |
| 6 | Project 8 (C 移植) | 20h | C 语言、链接脚本 |

---

## 权威参考

- **NAND2Tetris** (Shimon Schocken & Noam Nisan): 本项目直接对标
- **The Elements of Computing Systems** (NAND2Tetris 教材)
- **Stanford CS107E**: 在真实 RISC-V 硬件上的裸机 C 编程
- **RISC-V Reader**: 开源 ISA 的权威入门
- **"Programming from the Ground Up"** (Jonathan Bartlett): x86 汇编与系统编程

---

*最后更新: 2026-05-10*
