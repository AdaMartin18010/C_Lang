# 形式语义学 (Formal Semantics)

## 目录概述

本目录系统阐述**程序设计语言形式语义学**的三大经典范式：操作语义、公理语义和指称语义。
从元数学视角探讨程序行为的严格定义方法，建立程序正确性验证的理论基础。
内容涵盖从操作语义的计算过程建模到指称语义的数学对象映射，以及Hoare逻辑的公理化推理体系。

---

## 理论背景

### 形式语义学的三大支柱

程序语义学的核心问题：**程序的含义是什么？**

形式语义学提供三种互补的解答途径：

```text
                    程序 P
                      │
        ┌─────────────┼─────────────┐
        ↓             ↓             ↓
   操作语义      公理语义       指称语义
   (怎么做)     (正确性)       (是什么)
        ↓             ↓             ↓
   抽象机器      逻辑推理       数学对象
   状态转换      前后条件       域论构造
```

### 1. 操作语义 (Operational Semantics)

**核心思想**: 通过抽象状态机的**计算步骤**定义程序含义。

**结构化操作语义 (SOS)**:
由Plotkin提出，使用推理规则定义转换关系。

**小步语义**:
$$\langle S, \sigma \rangle \rightarrow \langle S', \sigma' \rangle$$

表示语句 $S$ 在状态 $\sigma$ 下执行一步，到达语句 $S'$ 和状态 $\sigma'$。

**大步语义**:
$$\langle S, \sigma \rangle \Downarrow \sigma'$$

表示语句 $S$ 在状态 $\sigma$ 下执行终止于状态 $\sigma'$。

**示例 (赋值语句的小步语义)**:
$$\frac{\langle a, \sigma \rangle \rightarrow_a n}{\langle x := a, \sigma \rangle \rightarrow \langle \text{skip}, \sigma[x \mapsto n] \rangle}$$

### 2. 公理语义 (Axiomatic Semantics)

**核心思想**: 通过**逻辑断言**描述程序执行前后的状态关系。

**Hoare三元组**:
$$\{P\} S \{Q\}$$

表示：若前置条件 $P$ 在 $S$ 执行前成立，且 $S$ 终止，则后置条件 $Q$ 在 $S$ 执行后成立。

**推理规则**:

**赋值规则**:
$$\{Q[x \mapsto E]\} x := E \{Q\}$$

**顺序规则**:
$$\frac{\{P\} S_1 \{R\} \quad \{R\} S_2 \{Q\}}{\{P\} S_1; S_2 \{Q\}}$$

**条件规则**:
$$\frac{\{P \land b\} S_1 \{Q\} \quad \{P \land \neg b\} S_2 \{Q\}}{\{P\} \text{if } b \text{ then } S_1 \text{ else } S_2 \{Q\}}$$

**循环规则 (部分正确性)**:
$$\frac{\{I \land b\} S \{I\}}{\{I\} \text{while } b \text{ do } S \{I \land \neg b\}}$$

其中 $I$ 是**循环不变式(Loop Invariant)**。

### 3. 指称语义 (Denotational Semantics)

**核心思想**: 将程序映射为**数学函数/对象**，组合性定义。

**基本框架**:
$$\llbracket \cdot \rrbracket : \text{Syntax} \rightarrow \text{Semantics}$$

**语义域(Domain)**:

- 状态域：$State = Var \rightarrow \mathbb{Z}_{\perp}$
- 程序含义：$\llbracket S \rrbracket : State \rightarrow State_{\perp}$

其中 $\perp$ 表示**非终止(bottom)**，使用**提升域(Lifted Domain)**。

**递归程序的语义**:
使用**最小不动点(Least Fixed Point)**：
$$\llbracket \text{while } b \text{ do } S \rrbracket = \text{fix}(\Phi)$$

其中 $\Phi$ 是函数泛函：
$$\Phi(f) = \lambda \sigma. \text{if } \llbracket b \rrbracket \sigma \text{ then } f(\llbracket S \rrbracket \sigma) \text{ else } \sigma$$

根据**Kleene不动点定理**，最小不动点存在且唯一：
$$\text{fix}(\Phi) = \bigsqcup_{n \geq 0} \Phi^n(\bot)$$

### 数学基础对照

| 语义类型 | 数学工具 | 核心概念 |
|----------|----------|----------|
| 操作语义 | 关系代数、归纳定义 | 转换关系、可达性 |
| 公理语义 | 谓词逻辑、模态逻辑 | Hoare逻辑、最弱前置条件 |
| 指称语义 | 域论、范畴论 | 连续函数、不动点 |

---

## 文件结构

| 文件名 | 难度 | 核心内容 |
|--------|------|----------|
| `01_Operational_Semantics.md` | ⭐⭐⭐⭐ | 结构化操作语义(SOS)理论，小步/大步语义，类型安全定理，上下文等价 |
| `02_Axiomatic_Semantics.md` | ⭐⭐⭐⭐⭐ | Hoare逻辑完整理论，最弱前置条件演算，程序验证方法，相对完备性定理 |
| `03_Denotational_Semantics.md` | ⭐⭐⭐⭐⭐ | 域论基础，Scott域，语义解释函数，完全抽象问题，Kripke逻辑关系 |

---

## 前置知识

- **数理逻辑**: 一阶谓词演算、自然演绎
- **集合论**: 函数、关系、偏序集
- **离散数学**: 归纳法、不动点理论
- **编程语言基础**: 语法、类型系统

---

## 学术参考文献

### 经典专著

1. **Winskel, G.** (1993). *The Formal Semantics of Programming Languages*.
   MIT Press. (形式语义学标准教材)

2. **Nielson, H. R., & Nielson, F.** (2007). *Semantics with Applications:
   An Appetizer*. Springer. (语义学入门，含在线资源)

3. **Pierce, B. C.** (2002). *Types and Programming Languages*. MIT Press.
   (类型理论与操作语义)

4. **Gunter, C. A.** (1992). *Semantics of Programming Languages: Structures
   and Techniques*. MIT Press. (指称语义学专著)

### 奠基性论文

1. **Plotkin, G. D.** (1981). *A Structural Approach to Operational Semantics*.
   Technical Report DAIMI FN-19, Aarhus University. (SOS奠基之作)

2. **Hoare, C. A. R.** (1969). *An Axiomatic Basis for Computer Programming*.
   Communications of the ACM, 12(10), 576-580. (Hoare逻辑奠基)

3. **Floyd, R. W.** (1967). *Assigning Meanings to Programs*. In
   Proceedings of Symposia in Applied Mathematics, 19, 19-32.
   (程序验证方法先驱)

4. **Scott, D., & Strachey, C.** (1971). *Toward a Mathematical Semantics
   for Computer Languages*. Technical Report PRG-6, Oxford.
   (Scott-Strachey指称语义)

5. **Scott, D.** (1976). *Data Types as Lattices*. SIAM Journal on Computing,
   5(3), 522-587. (域论基础)

### 高级主题

1. **Dreyer, D., Ahmed, A., & Birkedal, L.** (2009). *Logical Step-Indexed
    Logical Relations*. In LICS '09. (Kripke逻辑关系)

2. **Pitts, A. M.** (1996). *Relational Properties of Domains*.
    Information and Computation, 127(2), 66-90. (关系指称语义)

3. **Milner, R.** (1977). *Fully Abstract Models of Typed λ-Calculi*.
    Theoretical Computer Science, 4(1), 1-22. (完全抽象问题)

### 程序验证

1. **Dijkstra, E. W.** (1975). *Guarded Commands, Nondeterminacy and
    Formal Derivation of Programs*. Communications of the ACM, 18(8), 453-457.
    (最弱前置条件演算)

2. **Apt, K. R.** (1981). *Ten Years of Hoare's Logic: A Survey — Part I*.
    ACM TOPLAS, 3(4), 431-483. (Hoare逻辑综述)

3. **O'Hearn, P. W.** (2019). *Incorrectness Logic*. In POPL '20.
    (程序错误检测新方向)

---

## 学习路径建议

```text
数理逻辑基础 → 操作语义 → 公理语义 → 指称语义
       ↓              ↓            ↓            ↓
   谓词演算      状态转换      Hoare逻辑    域论构造
       ↓              ↓            ↓            ↓
   归纳原理      类型安全      程序验证    完全抽象
```

---

## 语义等价关系

### 操作语义与指称语义的关系

**定理 (语义的对应)**:
对于良类型程序 $P$，以下等价：
$$\langle P, \sigma \rangle \Downarrow \sigma' \Leftrightarrow \llbracket P \rrbracket(\sigma) = \sigma'$$

**充分性(Adequacy)**: 操作终止蕴含指称非底元
**完备性(Completeness)**: 指称非底元蕴含操作终止

### 完全抽象(Full Abstraction)

**定义**: 指称语义是完全抽象的，当且仅当：
$$\llbracket P_1 \rrbracket = \llbracket P_2 \rrbracket \Leftrightarrow P_1 \approx_{ctx} P_2$$

其中 $\approx_{ctx}$ 是**上下文等价(Contextual Equivalence)**。

---

## 相关链接

- [父目录: 深层结构与元物理学](../README.md)
- [C到汇编映射](../../02_Formal_Semantics_and_Physics/06_C_Assembly_Mapping/README.md)
- [类型系统与逻辑](../../03_Type_Systems_and_Logic/README.md)
- [自修改代码](../04_Self_Modifying_Code/README.md)

---

## 数学符号速查

| 符号 | 含义 |
|------|------|
| $\langle S, \sigma \rangle \rightarrow \sigma'$ | 小步语义转换 |
| $\langle S, \sigma \rangle \Downarrow \sigma'$ | 大步语义求值 |
| $\{P\} S \{Q\}$ | Hoare三元组 |
| $\llbracket P \rrbracket$ | 指称语义解释 |
| $\perp$ | 底元(非终止) |
| $\text{fix}(f)$ | 最小不动点 |
| $\sqsubseteq$ | 域的偏序 |
| $\bigsqcup$ | 最小上界 |
| $\sigma[x \mapsto v]$ | 状态更新 |

---

*本目录内容适合形式化方法研究者、程序验证工程师及编程语言理论学者阅读。*

*最后更新: 2026-03-10*
