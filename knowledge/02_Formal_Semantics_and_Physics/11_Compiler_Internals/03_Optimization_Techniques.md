# 编译器优化技术 (Optimization Techniques)

> **层级定位**: 02 Formal Semantics and Physics / 11 Compiler Internals
> **难度级别**: L5 专家
> **预估学习时间**: 15-20 小时

## 概述

编译器优化是将在保持程序语义等价的前提下，将中间表示 (IR) 转换为更高效形式的过程。现代编译器（GCC、LLVM/Clang）实现了数百种优化遍 (optimization passes)。

## 核心优化分类

### 1. 局部优化 (Local Optimizations)
- **常量折叠 (Constant Folding)**: `2 + 3` → `5`
- **常量传播 (Constant Propagation)**: 替换已知的常量值
- **死代码消除 (Dead Code Elimination)**: 移除不可达代码
- **强度削弱 (Strength Reduction)**: `x * 2` → `x << 1`

### 2. 循环优化 (Loop Optimizations)
- **循环展开 (Loop Unrolling)**: 减少循环开销
- **循环不变量外提 (Loop-Invariant Code Motion)**: 将不变计算移出循环
- **归纳变量优化 (Induction Variable Optimization)**
- **向量化 (Vectorization)**: SIMD 指令生成

### 3. 全局优化 (Global Optimizations)
- **内联展开 (Function Inlining)**: 消除函数调用开销
- **尾调用优化 (Tail Call Optimization)**
- **公共子表达式消除 (CSE)**
- **全局值编号 (GVN)**

### 4. 内存优化
- **标量替换 (Scalar Replacement of Aggregates)**
- **内存到寄存器提升 (Mem2Reg / PromoteMemoryToRegister)**
- **别名分析 (Alias Analysis)**

## LLVM 优化遍示例

```bash
# 查看所有优化遍
opt -passes=help

# 运行特定优化
opt -passes="mem2reg,simplifycfg,gvn" input.bc -o output.bc
```

## GCC 优化等级

| 等级 | 标志 | 说明 |
|:-----|:-----|:-----|
| O0 | 默认 | 无优化，便于调试 |
| O1 | `-O1` | 基本优化，不增加编译时间 |
| O2 | `-O2` | 标准优化，推荐用于生产 |
| O3 | `-O3` | 激进优化，可能增大代码体积 |
| Os | `-Os` | 优化代码大小 |
| Ofast | `-Ofast` | 不遵守严格标准，最大性能 |

## 待补充内容

- [ ] SSA 形式与优化
- [ ] 数据流分析框架
- [ ] 指针分析和别名分析深入
- [ ] 自动并行化和 OpenMP 生成

---

*最后更新: 2026-05-10*
