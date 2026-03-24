# 附录：完整参考手册

> **内容**: 所有记号、定义、定理的快速参考

---

## A. 记号约定

### A.1 逻辑记号

| 记号 | 含义 | 读法 |
|:-----|:-----|:-----|
| ⊢ | 推出 | proves |
| ≡ | 定义等价 | equivalent |
| → | 蕴含 | implies |
| ∀ | 全称量词 | for all |
| ∃ | 存在量词 | exists |
| λ | 抽象 | lambda |
| ⟦·⟧ | 语义括号 | denotation |

### A.2 类型记号

| 记号 | 含义 |
|:-----|:-----|
| Πx:A.B | 依赖函数类型 |
| Σx:A.B | 依赖对类型 |
| A × B | 积类型 |
| A + B | 和类型 |
| A → B | 函数类型 |
| ⊥ | 空类型 |
| ⊤ | 单位类型 |

---

## B. 核心定义索引

### B.1 类型论

**定义B.1.1 (上下文)**:

```
Γ ::= ∅ | Γ, x:A
```

**定义B.1.2 (类型判断)**:

```
Γ ⊢ M : A  (在Γ中M具有类型A)
```

### B.2 语义学

**定义B.2.1 (操作语义)**:

```
M → N  (M一步归约到N)
```

**定义B.2.2 (指称语义)**:

```
C⟦M⟧ρ  (M在环境ρ中的指称)
```

---

## C. 定理索引

### C.1 类型论语义 (41定理)

| 定理 | 位置 | 关键内容 |
|:-----|:-----|:---------|
| 替换引理 | Z1_MLTT_Extended | 替换保持类型 |
| 类型保持 | Z1_MLTT_Extended | 归约保持类型 |
| 强规范化 | Z1_MLTT_Extended | 所有项终止 |
| Church-Rosser | Z2_CoC_Extended | 合流性 |
| 一致性 | Z2_CoC_Extended | 无矛盾 |

### C.2 编译理论 (32定理)

| 定理 | 位置 | 关键内容 |
|:-----|:-----|:---------|
| 解析正确性 | COMPILATION_PHASE | 解析无歧义 |
| 详细化正确性 | COMPILATION_PHASE | 表层到核心 |
| IR生成保持 | Z3_Extended | 语义保持 |
| 闭包转换正确 | Z4_Extended | 闭包语义 |
| TCO正确性 | Z5_Extended | 尾调用优化 |

### C.3 内存与FFI (21定理)

| 定理 | 位置 | 关键内容 |
|:-----|:-----|:---------|
| 线性逻辑割消 | Z6_Extended | 证明简化 |
| 借用安全性 | Z6_Extended | 无悬垂指针 |
| RC正确性 | Z6_Extended | 引用计数 |
| FFI边界安全 | Z7_Extended | 跨语言安全 |

---

## D. 证明技术索引

### D.1 归纳技术

| 技术 | 适用场景 | 示例 |
|:-----|:---------|:-----|
| 结构归纳 | 语法结构 | 类型保持 |
| 良基归纳 | 递归定义 | 强规范化 |
| 规则归纳 | 推导规则 | 语义等价 |
| 路径归纳 | 相等证明 | 同伦类型论 |

### D.2 逻辑关系

| 关系 | 定义 | 用途 |
|:-----|:-----|:-----|
| 逻辑谓词 | V⟦τ⟧ ⊆ Value | 类型解释 |
| 可计算性 | ⟦τ⟧ ⊆ Term | 规范化证明 |
| 双模拟 | R ⊆ State × State | 语义等价 |

---

## E. 引用文献

### E.1 类型论

1. Martin-Löf, P. "Intuitionistic Type Theory" (1984)
2. Coquand, T. & Huet, G. "The Calculus of Constructions" (1988)
3. The Univalent Foundations Program. "Homotopy Type Theory" (2013)

### E.2 编译

1. Appel, A. "Compiling with Continuations" (1992)
2. Leroy, X. "Formal Verification of a Realistic Compiler" (2009)
3. de Moura, L. & Ullrich, S. "The Lean 4 Theorem Prover" (2021)

### E.3 语义学

1. Winskel, G. "The Formal Semantics of Programming Languages" (1993)
2. Pierce, B. "Types and Programming Languages" (2002)
3. Harper, R. "Practical Foundations for Programming Languages" (2016)

---

## F. 课程映射

### F.1 Stanford CS242

| 主题 | 对应文档 |
|:-----|:---------|
| λ演算 | Z1, Z2 |
| 类型系统 | Z1, Z2, Z6 |
| 多态 | Z2 |
| HoTT | Z10 |

### F.2 MIT 6.035

| 主题 | 对应文档 |
|:-----|:---------|
| 编译器 | Z3, Z4, Z5 |
| 优化 | Z4, Z5, Z6 |
| 代码生成 | Z3, Z7 |

### F.3 CMU 15-411/15-312

| 主题 | 对应文档 |
|:-----|:---------|
| 类型安全 | Z1, Z2 |
| IR | Z3 |
| 闭包 | Z4 |
| GC | Z6 |
