# 编译器内部原理 (Compiler Internals)

> **层级定位**: 02 Formal Semantics and Physics / 11 Compiler Internals
> **主题**: 编译器后端管道：从 IR 到机器码的完整旅程
> **难度级别**: L5 专家
> **预估学习时间**: 25-35 小时

## 概述

编译器后端是将中间表示（IR）转换为目标机器码的核心阶段。
与前端关注语言结构和语义不同，后端直接面向硬件特性，决定最终代码的性能和体积。
本模块系统阐述从 LLVM IR/SSA 到 SelectionDAG、指令选择、调度、寄存器分配、直至代码发射的完整后端管道，对齐 LLVM 官方文档和 CompCert 验证编译器的实现体系。

---

## 编译器后端完整管道

```
┌─────────────────────────────────────────────────────────────┐
│  Frontend IR (LLVM IR / GIMPLE / RTL)                        │
└──────────────────────────┬──────────────────────────────────┘
                           ▼
┌─────────────────────────────────────────────────────────────┐
│  1. 指令选择 (Instruction Selection)                          │
│     • DAG 构建 (SelectionDAG)                                 │
│     • 模式匹配 (Pattern Matching: LLVM TableGen)              │
│     • DAG 组合与合法化 (Combine + Legalize)                   │
└──────────────────────────┬──────────────────────────────────┘
                           ▼
┌─────────────────────────────────────────────────────────────┐
│  2. 指令调度 (Instruction Scheduling)                         │
│     • 列表调度 (List Scheduling)                              │
│     • 软件流水线 (Software Pipelining)                        │
│     • 指令级并行 (ILP) 优化                                   │
└──────────────────────────┬──────────────────────────────────┘
                           ▼
┌─────────────────────────────────────────────────────────────┐
│  3. 寄存器分配 (Register Allocation)                          │
│     • 图着色 (Graph Coloring: Chaitin-Briggs)                 │
│     • 线性扫描 (Linear Scan)                                  │
│     • 活跃变量分析 (Liveness Analysis)                        │
└──────────────────────────┬──────────────────────────────────┘
                           ▼
┌─────────────────────────────────────────────────────────────┐
│  4. 代码发射 (Code Emission)                                  │
│     • 机器指令生成 (MachineInstr → MCInst)                    │
│     • 汇编/目标文件输出 (Assembly / Object File)              │
│     • 函数 prologue/epilogue                                  │
└──────────────────────────┬──────────────────────────────────┘
                           ▼
┌─────────────────────────────────────────────────────────────┐
│  Target Machine Code (x86-64 / ARM64 / RISC-V)               │
└─────────────────────────────────────────────────────────────┘
```

---

## 1. 指令选择 (Instruction Selection)

### SelectionDAG

LLVM 使用有向无环图（DAG）表示基本块内的数据依赖和计算。

```
C 代码:  a = (b + c) * 2

SelectionDAG:
    t4: i32 = mul t3, Constant:i32<2>
          │
    t3: i32 = add t1, t2
         /   \
   t1: i32    t2: i32
     (b)       (c)
```

### 模式匹配与 TableGen

LLVM 使用 TableGen 定义目标架构的指令模式：

```tablegen
// x86 ADD32rr 指令定义
def ADD32rr : Instruction {
  let Pattern = [(set GPR32:$dst, (add GPR32:$src1, GPR32:$src2))];
  let Inst{31-0} = 0x01;  // opcode
}
```

### 合法化 (Legalization)

将目标不支持的 IR 操作转换为支持的形式：

- **类型合法化**: `i128` → 两个 `i64`
- **操作合法化**: 浮点除法 → 库函数调用
- **常量合法化**: 大立即数 → 寄存器加载

---

## 2. 指令调度 (Instruction Scheduling)

### 列表调度 (List Scheduling)

基于优先级的基本块内指令重排，最大化指令级并行。

```
原始代码:          调度后:
  LOAD r1, [a]      LOAD r1, [a]
  LOAD r2, [b]      LOAD r2, [b]
  ADD  r3, r1, r2   LOAD r3, [c]   ← 隐藏延迟
  LOAD r4, [c]      ADD  r3, r1, r2
  MUL  r5, r3, r4   MUL  r5, r3, r4
```

### 关键概念

| 概念 | 说明 |
|:-----|:-----|
| **关键路径** | DAG 中最长的依赖链，决定执行时间下限 |
| **发射宽度** | 每周期可同时发射的指令数 (x86-64: 4-6) |
| **延迟隐藏** | 通过指令重排覆盖 LOAD/MUL 延迟 |
| **读后写 (RAW)** | 真正的数据依赖，不可重排 |
| **写后读 (WAR)** | 反依赖，可通过寄存器重命名消除 |

---

## 3. 寄存器分配 (Register Allocation)

### 图着色算法 (Chaitin-Briggs)

将活跃变量分析结果构建为干扰图，用最少的颜色（物理寄存器）着色。

```
变量干扰图:
    a --- b
    |     |
    c --- d --- e

若只有 3 个寄存器: 必须溢出 (spill) 至少一个变量到栈
```

### 寄存器分配策略对比

| 算法 | 复杂度 | 质量 | 适用场景 |
|:-----|:-------|:-----|:---------|
| **图着色** | O(n²) | 高 | 静态编译器 (GCC, LLVM) |
| **线性扫描** | O(n) | 中 | JIT 编译器 (V8, Java HotSpot) |
| **PBQP** | O(n³) | 很高 | 特殊寄存器约束场景 |

### 活跃变量分析

```c
// 计算每个变量的活跃范围 (Live Range)
// 变量在 def 处诞生，在最后一个 use 处死亡

int foo(int x) {      // x: live-in
    int a = x + 1;    // a: born here
    int b = a * 2;    // b: born here
    return b;         // a dies before here, b dies at return
}
```

---

## 4. 代码发射 (Code Emission)

### 机器指令表示

LLVM 内部的指令表示层级：

```
LLVM IR      →  SelectionDAG   →  MachineInstr  →  MCInst    →  字节码
(目标无关)      (DAG 节点)        (机器指令)        (MC 层)     (目标文件)
```

### 函数 Prologue/Epilogue

```asm
; x86-64 函数框架
push    rbp
mov     rbp, rsp
sub     rsp, 32          ; 分配栈帧
...                      ; 函数体
mov     rsp, rbp         ; 恢复栈指针
pop     rbp
ret
```

### 调用约定 (Calling Convention)

| 架构 | 整数参数寄存器 | 浮点参数寄存器 | 返回值 | 栈清理 |
|:-----|:---------------|:---------------|:-------|:-------|
| **x86-64 System V** | RDI, RSI, RDX, RCX, R8, R9 | XMM0-7 | RAX/XMM0 | 调用者 |
| **x86-64 Windows** | RCX, RDX, R8, R9 | XMM0-3 | RAX/XMM0 | 调用者 |
| **ARM64 (AAPCS)** | X0-X7 | V0-V7 | X0/V0 | 调用者 |
| **RISC-V** | A0-A7 | FA0-FA7 | A0/FA0 | 调用者 |

---

## 5. 验证编译器：CompCert 后端

CompCert 验证编译器证明了从 Clight 到汇编的每一步语义保持：

```
Clight → Csharpminor → Cminor → CminorSel → RTL → LTL → Linear → Mach → Assembly
   │         │           │         │         │     │      │       │       │
   └─────────┴───────────┴─────────┴─────────┴─────┴──────┴───────┴───────┘
                           共 18 个验证遍
```

每个中间语言都有形式化语义，每一步转换都附带 Coq 正确性证明。

---

## 子文档索引

| 文件 | 主题 | 深度 |
|:-----|:-----|:-----|
| [01_AST_and_IR.md](01_AST_and_IR.md) | AST 设计与 IR 表示 | 中 |
| [02_Backend_Code_Generation.md](02_Backend_Code_Generation.md) | 后端代码生成 | 中 |
| [03_Optimization_Techniques.md](03_Optimization_Techniques.md) | 编译器优化技术 | 深 |
| 04_CompCert_Verified_Compiler.md | 验证编译器 | 深 |

## 权威参考

- **LLVM Documentation**: [Code Generator](https://llvm.org/docs/CodeGenerator.html)
- **CompCert Manual**: Backend passes and intermediate languages
- **Engineering a Compiler** (Cooper & Torczon): Ch.12-13 (Instruction Selection, Scheduling, Register Allocation)
- **Advanced Compiler Design and Implementation** (Muchnick): 经典后端教材

## 待补充内容

- [ ] 特定后端（x86-64 / ARM64 / RISC-V）的指令选择细节
- [ ] LLVM MachineScheduler 的调度策略详解
- [ ] 寄存器压力分析与 spills 优化
- [ ] 链接时优化 (LTO) 和 全程序优化

---

*最后更新: 2026-05-10*
