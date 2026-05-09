# 程序验证 (Program Verification)

> **层级定位**: 02 Formal Semantics and Physics / 05 Program Verification
> **难度级别**: L6 创造
> **预估学习时间**: 20+ 小时

## 概述

程序验证是使用形式化方法证明程序满足其规格说明的过程。与测试不同，验证提供的是数学上的正确性保证，覆盖所有可能的执行路径。

## 主要方法

### 1. 霍尔逻辑 (Hoare Logic)
- **三元组**: `{P} C {Q}` (前置条件、命令、后置条件)
- **最弱前置条件 (Weakest Precondition)**: Dijkstra 的谓词转换器
- **验证条件生成 (VCG)**: 自动推导证明义务

### 2. 模型检测 (Model Checking)
- **时序逻辑**: LTL, CTL
- **状态空间爆炸**: 符号模型检测、偏序规约
- **工具**: SPIN, NuSMV, TLA+

### 3. 定理证明 (Theorem Proving)
- **交互式证明**: Coq, Isabelle/HOL, Lean4
- **自动定理证明**: Z3, CVC5
- **程序逻辑**: 分离逻辑 (Separation Logic), Iris

### 4. 抽象解释 (Abstract Interpretation)
- **Cousot 框架**: 抽象域、具体化、加宽算子
- **静态分析工具**: Astrée, Polyspace

## 工业应用

| 领域 | 工具/方法 | 应用 |
|:-----|:----------|:-----|
| 操作系统内核 | seL4 验证 | 功能正确性证明 |
| 编译器 | CompCert | 编译正确性证明 |
| 设备驱动 | Z3 + Boogie | 驱动验证 |
| 安全协议 | Tamarin, ProVerif | 密码协议分析 |

## 待补充内容

- [ ] 分离逻辑与内存推理
- [ ] Frama-C 和 ACSL 注释
- [ ] VST (Verified Software Toolchain) 使用
- [ ] 符号执行与 KLEE

---

*最后更新: 2026-05-10*
