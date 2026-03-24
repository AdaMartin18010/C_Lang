# CS242 Programming Languages: 完整设计解析

## 1. 课程概览

### 1.1 基本信息

- **学校**: Stanford University
- **学期**: 秋季学期 (Fall)
- **学分**: 3学分
- **先修要求**: CS107 Programming Paradigms, CS103 Mathematical Foundations of Computing
- **授课教师 (历年)**:
  - John Mitchell (创始人)
  - Alex Aiken
  - Dawson Engler
  - Various visiting professors

### 1.2 课程定位

#### 在培养方案中的位置

CS242是Stanford编程语言理论的核心研究生课程，涵盖形式语义、类型系统、程序分析等理论基础。本课程为学生提供设计和分析编程语言的理论工具。

#### 与前后续课程关系图

```
前置课程:
├── CS103 计算数学基础 (逻辑、集合、归纳)
├── CS107 编程范式 (C, C++, Python, Scheme)
└── CS110 计算机系统原理

      ↓ (本课程)

CS242 Programming Languages
      ├── 操作语义
      ├── 公理语义
      ├── 指称语义
      ├── 类型系统
      ├── 多态性
      ├── 子类型
      ├── 模块系统
      └── 程序分析

      ↓ (后续课程)

├── CS243 Program Analysis and Optimization
├── CS344 Topics in Computer Systems
├── CS349 Topics in Programming Systems
└── 博士资格考准备
```

#### 核心问题驱动

**"如何用数学方法精确描述和推理程序的行为？"**

### 1.3 教学目标

#### 知识目标

1. 掌握操作语义、公理语义、指称语义三种主要语义方法
2. 理解类型系统的形式化理论和类型安全性
3. 掌握多态类型系统的实现原理
4. 理解子类型关系和面向对象类型理论
5. 了解程序分析的基本技术

#### 能力目标

1. 能够为编程语言定义形式化语义
2. 能够设计和证明类型系统的性质
3. 能够使用定理证明器验证程序性质
4. 能够阅读和评估PL研究论文

#### 素养目标

1. 培养数学化思维
2. 培养对程序语言设计原理的深入理解
3. 培养严谨的证明能力
4. 培养创新语言特性的能力

### 1.4 教学理念

#### 理论 vs 实践平衡

- **理论占比**: 80%
  - 形式语义
  - 类型理论
  - 逻辑与证明

- **实践占比**: 20%
  - Coq/Lean证明
  - 小型语言实现
  - 论文复现

#### 讲授 vs 自学比例

- **课堂讲授**: 每周3小时 × 10周 = 30小时 (Stanford Quarter制)
- **讨论课**: 每周1小时 × 10周 = 10小时
- **预期自学时间**: 每周12-15小时

#### 考核哲学

- **强调形式化**: 考试以形式化定义和证明为主
- **强调原创**: 鼓励学生提出新的类型规则
- **强调深度**: 深入理解而非广度覆盖

---

## 2. 教学大纲详解

### 2.1 周次安排表

| 周次 | 主题 | 学习目标 | 阅读 | 作业 | 备注 |
|:-----|:-----|:---------|:-----|:-----|:-----|
| 1 | 课程导论与IMP语言 | 理解PL研究范畴;掌握IMP语法 | Winskel Ch.1-2 |  |  |
| 2 | 操作语义:大步/小步 | 定义IMP语义;证明等价性 | Winskel Ch.2 | HW1发布 |  |
| 3 | 归纳与推理 | 结构化归纳;规则归纳 | Winskel Ch.3 |  |  |
| 4 | 公理语义:Hoare逻辑 | Hoare三元组;最弱前置条件 | Winskel Ch.6 | HW1截止 |  |
| 5 | Hoare逻辑完备性 | 相对完备性;不变式 | Winskel Ch.6 | HW2发布 |  |
| 6 | 指称语义 | 完全偏序;连续函数;不动点 | Winskel Ch.8 |  | 期中考试 |
| 7 | 简单类型lambda演算 | 类型规则;类型安全性 | Pierce Ch.9 | HW2截止 |  |
| 8 | 多态与System F | 参数多态;类型推导 | Pierce Ch.23 | HW3发布 |  |
| 9 | 子类型与面向对象 | 记录子类型;函数子类型 | Pierce Ch.15 |  |  |
| 10 | 高级主题 | 依赖类型;线性类型 | 论文 | HW3截止 | 期末项目 |

### 2.2 知识点依赖图

```
[数学基础]
├── 集合论
├── 归纳原理
└── 偏序与格论
      ↓
[IMP语言]
   ├── 抽象语法
   ├── 存储模型
   └── 求值关系
      ↓
[语义方法]
   ├── 操作语义 (大步/小步)
   ├── 公理语义 (Hoare逻辑)
   └── 指称语义 (CPO/连续函数)
      ↓
[类型系统]
   ├── 简单类型lambda演算
   ├── 多态类型 (System F)
   ├── 子类型系统
   └── 类型安全性证明
      ↓
[高级主题]
   ├── 模块系统
   ├── 依赖类型
   └── 线性/仿射类型
```

---

## 3. 作业设计详解

### 3.1 理论作业

#### 作业1: 操作语义与归纳

- **发布时间**: 第2周
- **截止时间**: 第4周
- **预期用时**: 15-20小时

**题目1: IMP求值关系 (25分)**

- 定义IMP大步操作语义的完整规则集
- 证明给定程序的执行结果
- 展示完整的推导树

**题目2: 大步vs小步等价性 (25分)**

- 证明大步语义和小步语义的等价性
- 形式化定义等价关系
- 完成双向证明

**题目3: 结构化归纳 (25分)**

- 使用结构化归纳证明IMP求值的确定性
- 设计适当的归纳假设
- 处理所有语法构造

**题目4: 扩展IMP (15分)**

- 为IMP添加异常处理机制
- 定义扩展后的语义规则
- 讨论语义选择的设计空间

**题目5: 证明练习 (10分)**

- 证明IMP程序的等价变换保持语义

---

#### 作业2: 公理语义与Hoare逻辑

- **发布时间**: 第5周
- **截止时间**: 第7周
- **预期用时**: 18-22小时

**题目1: Hoare逻辑规则 (20分)**

- 为IMP的所有构造写出Hoare规则
- 讨论循环规则的设计
- 处理数组和过程

**题目2: 程序验证 (25分)**

- 使用Hoare逻辑验证给定程序的正确性
- 找出并证明循环不变式
- 展示完整的证明树

**题目3: 最弱前置条件 (25分)**

- 定义最弱前置条件生成函数
- 证明wp(P, Q)是最弱的
- 与Hoare逻辑的关系

**题目4: 相对完备性 (20分)**

- 理解Gödel不完备性的影响
- 讨论算术表达式的可表达性
- 分析相对完备性定理

**题目5: 自动化验证 (10分)**

- 讨论程序验证的自动化挑战
- 比较Dijkstra最弱前置条件与Hoare逻辑

---

#### 作业3: 类型系统

- **发布时间**: 第7周
- **截止时间**: 第10周
- **预期用时**: 20-25小时

**题目1: 简单类型lambda演算 (25分)**

- 定义完整类型规则
- 实现类型推导算法
- 证明类型唯一性

**题目2: 类型安全性 (30分)**

- 证明Progress定理
- 证明Preservation定理
- 讨论类型安全性与求值策略

**题目3: System F (25分)**

- 定义多态类型规则
- 实现类型擦除语义
- 讨论参数多态与特设多态

**题目4: 子类型 (20分)**

- 定义记录子类型规则
- 讨论函数子类型的协变/逆变
- 分析子类型的实现技术

---

### 3.2 编程/证明作业

#### Lab 1: Coq/Lean中的IMP形式化

- **环境要求**: Coq 8.16+ 或 Lean 4
- **任务**: 形式化IMP语言及其语义
- **要求**:
  - 定义抽象语法
  - 定义操作语义
  - 证明基本性质

**评分标准**:

- 定义完整性 (40%)
- 证明正确性 (40%)
- 文档质量 (20%)

---

#### Lab 2: 类型检查器实现

- **任务**: 为简单函数式语言实现类型检查器
- **要求**:
  - 支持基本类型
  - 支持函数类型
  - 良好的错误报告

---

## 4. 课程项目设计

### 4.1 项目概述

- **项目名称**: 编程语言研究项目
- **项目类型**: 个人或小组(2人)
- **工作量**: 整个学期
- **占成绩比例**: 35%

### 4.2 项目选项

| 类型 | 描述 | 难度 |
|:-----|:-----|:-----|
| 语言扩展 | 为现有语言添加新特性 | 中等 |
| 形式化验证 | 在Coq/Lean中验证算法 | 困难 |
| 类型系统 | 设计并实现新类型系统 | 中等 |
| 论文复现 | 复现近期PL研究论文 | 可变 |

### 4.3 历年项目题目

- **2023**: 在Lean中形式化Rust的借用检查器
- **2022**: 线性类型系统的实现与验证
- **2021**: 渐进类型的形式化语义
- **2020**: 效果处理器(Effect Handlers)的类型系统

---

## 5. 考试设计

### 5.1 期中考试

- **时间**: 第6周
- **形式**: 开卷
- **时长**: 120分钟
- **范围**: 操作语义、公理语义

**题型**:

- 定义题 (3题, 30分)
- 证明题 (3题, 45分)
- 设计题 (1题, 25分)

### 5.2 期末考试

- **时间**: 第10周
- **形式**: 开卷
- **时长**: 180分钟
- **范围**: 全课程

---

## 6. 教学资源

### 6.1 教材

**主要教材**:

- *The Formal Semantics of Programming Languages* by Glynn Winskel
- *Types and Programming Languages* by Benjamin Pierce

**参考**:

- *Practical Foundations for Programming Languages* by Robert Harper
- *Semantics with Applications* by Nielson & Nielson

### 6.2 论文

| 周次 | 论文 | 作者 |
|:-----|:-----|:-----|
| 8 | "Types and Programming Languages" | Pierce |
| 9 | "Compiling with Continuations" | Appel |
| 10 | "Dependent Types at Work" | Bove & Dybjer |

### 6.3 工具

- **证明助手**: Coq, Lean 4, Agda
- **语法分析**: OCamllex/Menhir

---

## 7. 与Lean 4的整合

### 7.1 可用Lean形式化的内容

1. **IMP语言完整形式化**
   - 抽象语法
   - 大步/小步语义
   - 等价性证明

2. **Hoare逻辑验证**
   - 验证条件生成器
   - 程序正确性证明

3. **类型系统**
   - 简单类型lambda演算
   - System F
   - 类型安全性证明

**示例代码**:

```lean4
-- IMP抽象语法
inductive Expr
  | const : Nat → Expr
  | var : String → Expr
  | add : Expr → Expr → Expr
  | mul : Expr → Expr → Expr

inductive Stmt
  | skip
  | assign : String → Expr → Stmt
  | seq : Stmt → Stmt → Stmt
  | ifThenElse : Expr → Stmt → Stmt → Stmt
  | while : Expr → Stmt → Stmt

-- 大步操作语义
inductive BigStep : Stmt → State → State → Prop
  | skip : BigStep skip s s
  | assign : BigStep (assign x e) s (s.update x (eval e s))
  | seq : BigStep s1 s s' → BigStep s2 s' s'' → BigStep (seq s1 s2) s s''
  -- ... 其他规则

-- 类型系统
inductive Ty
  | nat
  | bool
  | arrow : Ty → Ty → Ty

inductive HasType : Context → Expr → Ty → Prop
  -- ... 类型规则

-- 类型安全性
theorem type_safety {Γ e τ} :
  HasType Γ e τ →
  (Value e ∨ ∃ e', Step e e') := by
  -- Progress + Preservation
  sorry
```

### 7.2 教学建议

- 使用Lean替代部分Coq作业
- 将类型安全性证明作为Lean项目
- 探索Lean的元编程功能用于PL实现

---

## 8-9. 其他章节

### 8.1 课程演化

| 年份 | 改革 |
|:-----|:-----|
| 2005 | 引入Coq形式化 |
| 2015 | 增加依赖类型内容 |
| 2022 | 引入Lean 4选项 |

### 9. 学习建议

- **数学准备**: 强化离散数学和逻辑基础
- **证明练习**: 多做归纳证明
- **工具学习**: 尽早熟悉Coq或Lean

---

## 参考资料

1. Stanford CS242 Course Website
2. Winskel: Formal Semantics of Programming Languages
3. Pierce: Types and Programming Languages
4. Software Foundations (Coq教材)
5. Theorem Proving in Lean 4
