# Lean 4 示例代码集合

本目录包含10个独立的Lean 4示例文件，展示Lean生态系统的各个方面。

## 文件列表

| 文件名 | 主题 | 覆盖内容 |
|--------|------|----------|
| `01_basic_group_theory.lean` | 基础群论 | 群定义、同态、ZMod n |
| `02_induction_proofs.lean` | 归纳证明 | 自然数归纳、结构归纳、强归纳 |
| `03_type_classes.lean` | 类型类 | 类型类定义、实例、层次结构 |
| `04_metaprogramming.lean` | 元编程 | 宏、策略编写、MetaM |
| `05_algebraic_structures.lean` | 代数结构 | 环、域、多项式、矩阵 |
| `06_analysis_basics.lean` | 分析基础 | 连续性、微分、积分、级数 |
| `07_topology_basics.lean` | 拓扑基础 | 拓扑空间、紧性、度量空间 |
| `08_number_theory.lean` | 数论 | 素数、模运算、p-adic数 |
| `09_category_theory.lean` | 范畴论 | 范畴、函子、极限、Yoneda |
| `10_advanced_tactics.lean` | 高级策略 | linarith, ring, field_simp等 |

## 使用方式

### 方法1: 在VS Code中运行

1. 安装Lean 4 VS Code扩展
2. 打开任意`.lean`文件
3. 光标放在代码中按`Ctrl+Shift+Enter`执行

### 方法2: 命令行运行

```bash
# 安装依赖
lake update

# 构建项目
lake build

# 运行示例
lake exe runExamples
```

### 方法3: 在线运行

访问 [Lean 4 Web Editor](https://lean.math.hhu.de/) 复制粘贴代码运行。

## 前置要求

- Lean 4 (版本 ≥ 4.7.0)
- Mathlib4
- Aesop (可选)

## 学习目标

完成这些示例后，您将掌握：

1. **基础数学形式化** - 群、环、域的定义和证明
2. **证明技巧** - 归纳法、分类讨论、反证法
3. **类型系统** - 类型类、依赖类型、宇宙层次
4. **元编程** - 编写自定义策略和宏
5. **数学分析** - 连续、微分、积分的形式化
6. **抽象代数** - 范畴论、同调代数基础

## 相关资源

- [Lean 4 官方文档](https://lean-lang.org/lean4/doc/)
- [Mathlib4 文档](https://leanprover-community.github.io/mathlib4_docs/)
- [Theorem Proving in Lean 4](https://leanprover.github.io/theorem_proving_in_lean4/)
- [Mathematics in Lean](https://leanprover-community.github.io/mathematics_in_lean/)

## 许可证

MIT License - 与Lean 4和Mathlib4保持一致。
