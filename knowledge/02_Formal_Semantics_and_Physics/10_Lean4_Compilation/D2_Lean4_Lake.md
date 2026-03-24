# Lake：Lean 4构建系统的理论与实践

> **层级**: L5 (原理层)
> **引用**: Lake文档, Nix论文, Vargo (2022)
> **课程对标**: Stanford CS242, MIT 6.035

---

## 一、构建系统的理论基础

### 1.1 增量构建的数学模型

**定义 1.1** (构建图)

构建图是DAG G = (V, E)，其中：

- V：文件（源文件、目标文件）
- E：依赖关系

**定义 1.2** (增量构建)

给定变更集合Δ ⊆ V，
需要重新计算的节点：

```
Rebuild(Δ) = {v ∈ V | ∃u ∈ Δ. u →* v}
```

其中 →* 是依赖的传递闭包。

### 1.2 确定性构建

**定理 1.1** (构建确定性)

如果输入相同，则输出相同（bit-for-bit identical）。

Lean 4/Lake通过：

- 固定的编译器版本
- 确定的编译顺序
- 禁止非确定性操作（如随机数、时间戳）

---

## 二、Lake的架构

### 2.1 包管理的形式化

**定义 2.1** (包)

包P是四元组 (Name, Version, Dependencies, Artifacts)：

- Name：包标识符
- Version：语义化版本
- Dependencies：依赖包集合
- Artifacts：构建产物

**依赖解析**：
给定依赖约束集合，
Lake求解满足所有约束的版本分配。

### 2.2 lakefile.lean的语义

**配置DSL**：

```lean
package «my-pkg» where
  version := "1.0.0"
  leanVersion := "4.0.0"
  dependencies := #[
    { name := "std", src := Source.git "..." "main" }
  ]
```

**语义**：声明式描述构建配置，
Lake计算闭包并执行构建。

---

## 三、依赖管理理论

### 3.1 语义化版本

**定义 3.1** (语义化版本)

版本号 = MAJOR.MINOR.PATCH

**兼容性规则**：

- MAJOR变化：不兼容的API变更
- MINOR变化：向后兼容的功能添加
- PATCH变化：向后兼容的问题修复

### 3.2 依赖求解

**问题**：给定包P的依赖树，
找到满足所有版本约束的解。

**算法**：SAT求解 / 约束传播

Lake使用简单的约束传播：

- 首先选择最新兼容版本
- 遇到冲突时回溯

---

## 四、与C构建系统的集成

### 4.1 外部库依赖

**形式化**：

```lean
target ffi.o : FilePath := do
  let src := "ffi.c"
  let out := "ffi.o"
  exec "gcc" #["-c", src, "-o", out]
  return out
```

**构建流程**：

1. Lake编译Lean代码
2. 调用C编译器编译外部库
3. 链接所有目标文件

### 4.2 FFI构建的正确性

**定理 4.1** (FFI构建正确性)

如果C代码符合FFI契约，
Lake构建确保：

- C对象文件最新
- Lean代码正确链接C库
- 运行时库路径正确

---

## 五、最新特性（Lean 4.3+）

### 5.1 Workspace支持

Lake支持多包工作区：

```
workspace/
├── pkg1/
│   └── lakefile.lean
├── pkg2/
│   └── lakefile.lean
└── lakefile.lean  # 工作区根
```

### 5.2 Precompile Modules

支持预编译模块加速构建：

```lean
precompileModules := true
```

---

**参考文献**：

- Lake Documentation (leanprover.github.io)
- Vargo, S. "Lake: A Build System for Lean 4" (2022)
