# 语义规范标准

> **版本**: 1.0
> **目标**: 建立形式化语义的标准规范格式
> **范围**: 所有语义学文档

---

## 1. 引言

本文档定义了编写形式化语义的标准规范，确保所有文档的一致性、可读性和可验证性。

### 规范目标

- **一致性**: 统一的符号、术语和结构
- **可读性**: 清晰的组织、适当的抽象层次
- **可验证性**: 可机械检查的规范和证明
- **可扩展性**: 易于添加新语言特性

---

## 2. 文档结构规范

### 2.1 标准章节顺序

```
1. 引言
   1.1 目标
   1.2 范围
   1.3 前置知识
   1.4 符号约定

2. 语法定义
   2.1 抽象语法
   2.2 具体语法（如适用）
   2.3 语法糖（如适用）

3. 语义定义
   3.1 操作语义 / 指称语义 / 公理语义
   3.2 辅助定义
   3.3 示例

4. 类型系统（如适用）
   4.1 类型规则
   4.2 类型健全性

5. 元理论性质
   5.1 主要定理陈述
   5.2 证明概要

6. 实现（Lean 4）
   6.1 核心定义
   6.2 关键函数
   6.3 验证代码

7. 参考文献

8. 交叉引用
```

### 2.2 文档元数据

每篇文档必须包含：

```markdown
# 文档标题

> **版本**: x.y
> **目标**: 一句话描述
> **前置**: 前置知识要求

---

## 变更日志

| 版本 | 日期 | 变更 |
|:-----|:-----|:-----|
| 1.0 | 2026-03-24 | 初始版本 |

---
```

---

## 3. 符号规范

### 3.1 数学符号

| 概念 | 标准符号 | 替代符号 | 避免使用 |
|:-----|:---------|:---------|:---------|
| 归约 | M → N | M ⟶ N | M => N |
| 多步归约 | M →* N | M ⟶* N | M =>* N |
| 求值 | M ⇓ v | M ↓ v | M evalto v |
| 指称 | ⟦M⟧ | [[M]] | C⟦M⟧ |
| 环境 | ρ, σ | env, s | gamma |
| 类型判断 | Γ ⊢ M : T | Γ |- M : T | Gamma |- M : T |
| 霍尔三元组 | {P} C {Q} | <P> C <Q> | {P}C{Q} |

### 3.2 命名规范

**语言构造子**:

- 小写: `exp`, `cmd`, `typ`
- 首字母大写: `Expr`, `Command`, `Type`
- 驼峰式: `AbstractSyntax`, `TypeChecker`

**元变量**:

- 表达式: M, N, P, Q
- 值: v, w
- 类型: S, T, U
- 上下文: Γ, Δ
- 环境: ρ, σ

**函数**:

- 小写: `subst`, `eval`, `typecheck`
- 谓词: `isValue`, `isWellTyped`

### 3.3 Lean 4代码规范

```lean
-- 命名空间使用小写
namespace operational_semantics

-- 归纳类型使用大写开头
inductive Expr where
  | var (x : String)
  | lam (x : String) (body : Expr)
  | app (fn arg : Expr)
  deriving Repr, BEq

-- 函数使用小写，描述性名称
def substitute (x : String) (v : Expr) (body : Expr) : Expr :=
  -- 实现
  sorry

-- 定理使用描述性名称
theorem substitution_preserves_typing :
  ∀ {Γ x v body T},
    Γ ⊢ v : T →
    (Γ.insert x T) ⊢ body : U →
    Γ ⊢ substitute x v body : U :=
  sorry

end operational_semantics
```

---

## 4. 语义定义规范

### 4.1 操作语义

**推理规则格式**:

```
前提1    前提2    ...    前提n
──────────────────────────────── (规则名称)
          结论
```

**要求**:

- 每个规则有名称
- 前提左对齐
- 结论在横线下居中
- 横线长度适中

**示例**:

```
M₁ → M₁'
──────────────── (E-App1)
M₁ M₂ → M₁' M₂

(λx.M) v
──────────────── (E-AppAbs)
M[x:=v]
```

### 4.2 指称语义

**语义函数定义**:

```
⟦_⟧ : Expr → Env → Value

⟦n⟧ρ = n
⟦x⟧ρ = ρ(x)
⟦λx.M⟧ρ = λv. ⟦M⟧ρ[x↦v]
⟦M N⟧ρ = ⟦M⟧ρ (⟦N⟧ρ)
```

**要求**:

- 明确定义域和陪域
- 环境处理清晰
- 递归情况明确

### 4.3 公理语义

**霍尔规则格式**:

```
{P} skip {P}                          (Skip)

{P[x:=E]} x:=E {P}                    (Assign)

{P} C₁ {Q}    {Q} C₂ {R}
─────────────────────────── (Seq)
{P} C₁; C₂ {R}
```

---

## 5. 证明规范

### 5.1 证明结构

标准证明结构:

```markdown
**定理 (名称)**: 陈述

**证明**:

*方法*: 证明方法（归纳/反证/构造等）

*基本情况*:
- 情况分析
- 每个情况的证明

*归纳步骤*:
- 归纳假设
- 推导过程

*结论*。

**证毕**。
```

### 5.2 证明技巧标记

| 技巧 | 标记 | 说明 |
|:-----|:-----|:-----|
| 结构归纳 | 对M的结构归纳 | 明确归纳对象 |
| 规则归纳 | 对推导归纳 | 说明是对哪个关系 |
| 互归纳 | 互归纳于M和N | 多个对象的联合归纳 |
| 反证法 | 假设相反...导出矛盾 | 明确矛盾点 |
| 构造性证明 | 构造如下... | 给出具体构造 |

### 5.3 证明状态记录

复杂证明应记录中间状态:

```
当前目标: ⊢ ∀n, n + 0 = n

应用策略: intro n
新目标: n : Nat ⊢ n + 0 = n

应用策略: induction n
子目标1: ⊢ 0 + 0 = 0
子目标2: n : Nat, ih : n + 0 = n ⊢ (n+1) + 0 = n+1
```

---

## 6. Lean 4代码规范

### 6.1 文件组织

```
project/
├── Basic.lean          -- 基础定义
├── Syntax.lean         -- 语法定义
├── Operational.lean    -- 操作语义
├── Denotational.lean   -- 指称语义
├── Axiomatic.lean      -- 公理语义
├── Types.lean          -- 类型系统
├── Metatheory.lean     -- 元理论
└── Tests.lean          -- 测试
```

### 6.2 注释规范

```lean
/-- 函数文档：简要描述函数功能 -/
def importantFunction (x : Type) : Result :=
  sorry

-- 行内注释：解释特定代码段
let result := complexOperation input  -- 注意边界条件

/-
 块注释：解释复杂逻辑
  1. 步骤1
  2. 步骤2
  3. 步骤3
-/
```

### 6.3 证明脚本规范

```lean
-- 推荐：结构化证明
theorem example_theorem : P := by
  intro x          -- 引入假设
  cases x with     -- 情况分析
  | case1 =>       -- 情况1
    simp          -- 简化
    exact h       -- 应用假设
  | case2 =>       -- 情况2
    apply lemma   -- 应用引理
    assumption    -- 自动匹配

-- 避免：过于紧凑的证明
theorem bad_example : P := by intro x; cases x with | case1 => simp; exact h | case2 => apply lemma; assumption
```

---

## 7. 图表规范

### 7.1 语法树

```
    App
   /   \
  Abs   Num 5
  / \
 x   Add
    /   \
   Var x Var x
```

### 7.2 状态转换图

```
       a
  ┌─────────┐
  ↓         │
  P ───────→ P'
  │    τ    ↑
  │         │
  └────Q────┘
       b
```

### 7.3 类型推导树

```
  Γ ⊢ M : A → B    Γ ⊢ N : A
  ─────────────────────────── (→E)
         Γ ⊢ M N : B
```

---

## 8. 术语规范

### 8.1 标准术语

| 中文 | 英文 | 缩写 | 说明 |
|:-----|:-----|:-----|:-----|
| 抽象语法树 | Abstract Syntax Tree | AST | 程序的树形表示 |
| 绑定的变量 | Bound Variable | | 被λ/∀绑定的变量 |
| 自由变量 | Free Variable | FV | 未被绑定的变量 |
| 代换 | Substitution | | 变量替换 |
| 归约 | Reduction | | 计算步骤 |
| 范式 | Normal Form | | 无法继续归约的项 |
| 合流性 | Confluence | CR | Church-Rosser性质 |
| 类型保持 | Type Preservation | | 归约保持类型 |
| 进展性 | Progress | | 良类型项可继续 |

### 8.2 避免术语

| 避免使用 | 推荐使用 | 原因 |
|:---------|:---------|:-----|
| 运算 | 归约/求值 | 更精确 |
| 变量替换 | 代换/替换 | 标准化 |
| 停机 | 终止 | 更专业 |
| 类型推导 | 类型检查/类型推断 | 区分算法 |

---

## 9. 交叉引用规范

### 9.1 内部引用

```markdown
参见 [操作语义定义](#31-操作语义)。

详见 [定理2.1](#定理-21-操作与指称语义等价)。

完整代码见 [Lean实现](#6-lean-4实现)。
```

### 9.2 外部引用

```markdown
- [SEMANTIC_NOTATIONS.md](./SEMANTIC_NOTATIONS.md) - 符号标准
- [SEMANTICS_UNIFIED_REVISED.md](./SEMANTICS_UNIFIED_REVISED.md) - 统一框架
- Pierce, TAPL [1] - 类型和编程语言
```

### 9.3 文献引用格式

```markdown
[1] Benjamin C. Pierce. Types and Programming Languages. MIT Press, 2002.

[2] John C. Reynolds. Theories of Programming Languages. Cambridge University Press, 1998.

[3] Glynn Winskel. The Formal Semantics of Programming Languages. MIT Press, 1993.
```

---

## 10. 质量保证

### 10.1 审查清单

发布前检查:

- [ ] 符号一致性检查
- [ ] 数学公式正确性
- [ ] Lean代码可运行
- [ ] 交叉引用有效性
- [ ] 拼写和语法检查
- [ ] 格式统一性

### 10.2 自动化检查

```bash
# 符号检查
./scripts/check_symbols.sh

# Lean代码检查
lake build

# 链接检查
./scripts/check_links.sh

# 格式检查
./scripts/check_format.sh
```

### 10.3 版本控制

- 语义重大变化: 主版本号+1
- 新增内容: 次版本号+1
- 错误修复: 修订号+1

---

## 11. 扩展规范

### 11.1 添加新语言

1. 在 `Syntax.lean` 中定义语法
2. 在 `Operational.lean` 中定义操作语义
3. 在 `Types.lean` 中定义类型系统
4. 在 `Metatheory.lean` 中证明性质
5. 更新交叉引用

### 11.2 添加新语义范式

1. 定义符号（更新 SEMANTIC_NOTATIONS.md）
2. 定义语义（创建新文档或在统一框架中添加）
3. 建立与现有范式的联系
4. 提供Lean实现
5. 添加示例和练习

---

## 12. 参考文献

[1] Benjamin C. Pierce. Types and Programming Languages. MIT Press, 2002.

[2] Glynn Winskel. The Formal Semantics of Programming Languages. MIT Press, 1993.

[3] The Lean Manual. <https://lean-lang.org/lean4/doc/>

[4] Markdown Style Guide. <https://www.markdownguide.org/>

---

**Maintainer**: Lean 4 Semantics Standards Committee
**Version**: 1.0
**Last Updated**: 2026-03-24
