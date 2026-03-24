# CS143 Compilers: 完整设计解析

## 1. 课程概览

### 1.1 基本信息

- **学校**: Stanford University
- **学期**: 秋季学期 (Fall)
- **学分**: 3学分
- **先修要求**: CS107 Programming Paradigms, CS109 Probability
- **授课教师 (历年)**:
  - Alex Aiken
  - Dawson Engler
  - Various TAs

### 1.2 课程定位

CS143是Stanford本科编译器课程，以完整的COOL (Classroom Object Oriented Language) 编译器项目为核心。

#### 核心问题驱动

**"如何从头到尾构建一个完整的面向对象语言编译器？"**

### 1.3 教学目标

1. 理解编译器各阶段的设计和实现
2. 掌握面向对象语言的编译技术
3. 能够独立完成编译器项目
4. 培养大规模系统编程能力

### 1.4 教学理念

- **项目驱动**: 整个课程围绕一个大型编译器项目
- **自底向上**: 从词法分析到代码生成的完整流程
- **工具使用**: 大量使用自动生成工具

---

## 2. 教学大纲详解

### 2.1 周次安排表

| 周次 | 主题 | 作业 | 项目阶段 |
|:-----|:-----|:-----|:---------|
| 1 | 编译器概览;COOL语言介绍 | HW1 | PA1 |
| 2 | 词法分析;Lex/Flex |  |  |
| 3 | 语法分析;Yacc/Bison | HW2 | PA2 |
| 4 | 抽象语法树 |  |  |
| 5 | 语义分析;类型检查 | HW3 | PA3 |
| 6 | 运行时系统 |  |  |
| 7 | 中间代码 | HW4 | PA4 |
| 8 | 代码优化 |  | 期中考试 |
| 9 | 代码生成 | HW5 | PA5 |
| 10 | 高级主题 |  | 项目截止 |

### 2.2 知识点依赖

```
[COOL语言]
      ↓
[词法分析] → Flex
      ↓
[语法分析] → Bison
      ↓
[语义分析]
   ├── 符号表
   ├── 类型检查
   └── 继承分析
      ↓
[代码生成]
   ├── 三地址码
   ├── MIPS汇编
   └── 运行时支持
```

---

## 3. 作业设计详解

### 3.1 理论作业

#### 作业1: 词法分析

- **发布时间**: 第1周
- **截止时间**: 第2周
- **设计目的**: 理解词法分析原理

**题目1**: 为COOL语言构造词法规范
**题目2**: 正则表达式到DFA转换练习
**题目3**: 词法错误处理策略

---

#### 作业2: 语法分析

- **发布时间**: 第3周
- **截止时间**: 第4周

**题目1**: COOL文法设计与二义性消除
**题目2**: FIRST/FOLLOW集计算
**题目3**: LR分析表构造

---

#### 作业3: 语义分析

- **发布时间**: 第5周
- **截止时间**: 第6周

**题目1**: 面向对象类型系统规则
**题目2**: 方法分派(dispatch)的语义
**题目3**: 继承关系的类型检查

---

### 3.2 编程作业 (Programming Assignments)

#### PA1: 词法分析器

- **任务**: 使用Flex实现COOL scanner
- **输入**: .cl源文件
- **输出**: token序列
- **挑战**: 复杂的字符串和注释处理
- **评分**: 自动测试100%

---

#### PA2: 语法分析器

- **任务**: 使用Bison实现COOL parser
- **输入**: token序列
- **输出**: AST
- **挑战**: 错误恢复

---

#### PA3: 语义分析器

- **任务**: 实现类型检查和符号表
- **功能**:
  - 类继承图构建
  - 属性与方法的环境
  - 类型推导和检查
- **挑战**: SELF_TYPE处理

---

#### PA4: 代码生成

- **任务**: 生成MIPS汇编代码
- **功能**:
  - 表达式翻译
  - 方法调用
  - 对象创建

---

#### PA5: 优化 (可选)

- **任务**: 实现基本优化pass
- **可选优化**:
  - 常量传播
  - 死代码消除
  - 内联展开

---

## 4. 课程项目设计

### 4.1 项目概述

- **项目名称**: COOL编译器
- **项目类型**: 个人
- **工作量**: 贯穿整个学期
- **占成绩比例**: 50% (各PA合计)

### 4.2 项目评分

| PA | 内容 | 占比 | 截止日期 |
|:---|:-----|:-----|:---------|
| PA1 | Scanner | 10% | 第2周 |
| PA2 | Parser | 15% | 第4周 |
| PA3 | Semantic | 20% | 第7周 |
| PA4 | CodeGen | 20% | 第10周 |
| PA5 | Optimizer | 10% | 第10周 |

---

## 5. 考试设计

### 5.1 期中考试

- **时间**: 第8周
- **形式**: 开卷
- **时长**: 90分钟
- **范围**: 词法、语法、语义

### 5.2 期末考试

- **时间**: 期末考试周
- **形式**: 开卷
- **时长**: 180分钟
- **范围**: 全课程

---

## 6. 教学资源

### 6.1 教材

**主要**:

- *Modern Compiler Implementation in C* by Andrew Appel
- COOL语言参考手册

### 6.2 工具

- **词法分析**: Flex
- **语法分析**: Bison
- **目标平台**: MIPS汇编/SPIM模拟器
- **语言**: C++

---

## 7. 与Lean 4的整合

### 7.1 可用Lean形式化的内容

1. **COOL类型系统形式化**
   - 类层次结构
   - 方法类型检查
   - 类型安全性

2. **优化正确性**
   - 数据流分析正确性
   - 转换保持语义

**示例代码**:

```lean4
-- COOL类层次
structure ClassDecl :=
  name : String
  parent : Option String
  attributes : List (String × Type)
  methods : List MethodDecl

-- 类型检查
def typeCheck (env : TypeEnv) (e : Expr) : Except String Type :=
  match e with
  | Expr.int _ => pure Type.int
  | Expr.plus e1 e2 => do
      let t1 ← typeCheck env e1
      let t2 ← typeCheck env e2
      if t1 = Type.int ∧ t2 = Type.int
      then pure Type.int
      else throw "Type error in plus"
  -- ...

-- 语义等价
def SemEquiv (e1 e2 : Expr) : Prop :=
  ∀ env, eval e1 env = eval e2 env

-- 优化正确性
theorem constant_folding_correct {e} :
  SemEquiv e (constantFold e) := by
  sorry
```

---

## 8-9. 其他章节

### 8.1 课程演化

| 年份 | 改革 |
|:-----|:-----|
| 2000 | 从C转为C++ |
| 2010 | 更新SPIM版本 |
| 2020 | 增加LLVM后端选项 |

### 9. 学习建议

- **提前准备**: 复习C++和面向对象编程
- **测试驱动**: 频繁运行测试套件
- **寻求帮助**: 充分利用Office Hour

---

## 参考资料

1. Stanford CS143 Course Website
2. The Cool Reference Manual
3. Appel: Modern Compiler Implementation in C
4. SPIM MIPS Simulator文档
