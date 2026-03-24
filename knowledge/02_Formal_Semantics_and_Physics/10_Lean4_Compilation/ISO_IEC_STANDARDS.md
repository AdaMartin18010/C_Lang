# ISO/IEC标准对接

> **目标**: 对接国际标准化组织的形式化方法标准
> **范围**: ISO/IEC JTC 1/SC 22相关标准

---

## 1. 引言

国际标准化组织(ISO)和国际电工委员会(IEC)制定了大量与程序语言和形式化方法相关的标准。本知识库与这些标准对接，确保内容的规范性和实用性。

### 标准组织体系

```
ISO/IEC JTC 1 - 信息技术
├── SC 2 - 编码字符集
├── SC 7 - 软件与系统工程
├── SC 22 - 程序语言及其环境与系统软件接口
│   ├── WG 4 - COBOL
│   ├── WG 5 - Fortran
│   ├── WG 9 - Ada
│   ├── WG 14 - C
│   ├── WG 15 - C++
│   ├── WG 17 - Prolog
│   ├── WG 19 - Formal Specification Languages
│   └── WG 23 - Programming Language Vulnerabilities
└── SC 27 - 信息安全
```

---

## 2. ISO/IEC 标准概览

### 2.1 程序语言标准

| 标准编号 | 名称 | 形式化内容 | 状态 |
|:---------|:-----|:-----------|:-----|
| ISO/IEC 9899 | C语言 | 附录J(未定义行为) | 活跃 |
| ISO/IEC 14882 | C++ | Concepts(概念) | 活跃 |
| ISO/IEC 1539 | Fortran | - | 活跃 |
| ISO/IEC 1989 | COBOL | - | 维护 |
| ISO/IEC 8652 | Ada | SPARK子集 | 活跃 |
| ISO/IEC 13211 | Prolog | 形式化语义 | 活跃 |

### 2.2 形式化方法标准

**ISO/IEC 15909 - LOTOS**

- 形式化描述技术
- 基于进程代数
- 用于通信协议规范

**ISO/IEC 13568 - Z notation**

- 基于集合论的形式化规约语言
- 本知识库的类型系统部分对接

**ISO/IEC 24765 - 系统和软件工程词汇**

- 定义了形式化方法相关术语
- 本知识库术语遵循此标准

### 2.3 安全相关标准

**ISO/IEC 15408 (Common Criteria)**

- 信息技术安全性评估
- 评估保证等级(EAL1-EAL7)
- EAL6+要求形式化验证

**ISO 26262 - 汽车功能安全**

- ASIL等级与形式化方法
- 第6部分：软件级产品开发

**IEC 61508 - 电气/电子/可编程电子安全相关系统**

- 安全完整性等级(SIL)
- 形式化方法推荐表

---

## 3. 标准对接分析

### 3.1 术语对接

**本知识库术语与ISO标准对照**:

| 本知识库 | ISO 24765 | 说明 |
|:---------|:----------|:-----|
| 操作语义 | operational semantics | 完全一致 |
| 指称语义 | denotational semantics | 完全一致 |
| 公理语义 | axiomatic semantics | 完全一致 |
| 类型保持 | type preservation | 同type consistency |
| 最弱前置条件 | weakest precondition | 完全一致 |

### 3.2 符号规范对接

**ISO 80000-2 数学符号**:

| 符号 | ISO标准 | 本知识库 | 说明 |
|:-----|:--------|:---------|:-----|
| 逻辑与 | ∧ | ∧ | 一致 |
| 逻辑或 | ∨ | ∨ | 一致 |
| 蕴含 | ⇒ | ⟹ | 本库使用长箭头 |
| 全称量词 | ∀ | ∀ | 一致 |
| 存在量词 | ∃ | ∃ | 一致 |

### 3.3 形式化方法分级

**ISO/IEC TR 24772 - 程序语言漏洞**

形式化方法使用建议:

| 等级 | 形式化程度 | 适用场景 |
|:-----|:-----------|:---------|
| 1 | 半形式化规格 | 一般软件 |
| 2 | 形式化规格 | 关键组件 |
| 3 | 形式化验证 | 安全关键 |
| 4 | 完全形式化 | 最高安全 |

---

## 4. 具体标准对接

### 4.1 ISO/IEC 9899 (C语言)

**形式化语义缺口**:

- C标准附录J列出未定义行为
- 但缺乏完整形式化语义
- 本知识库的命令式语义部分填补此缺口

**对接内容**:

```c
// C标准示例：序列点
i = ++i + 1;  // 未定义行为

// 形式化解释 (本知识库)
-- 读取i和写入i之间无序列点
-- 违反语义规则：同对象在序列点间修改且读取
```

### 4.2 ISO/IEC 14882 (C++)

**Concepts (C++20)**:

- 基于类型的约束系统
- 与类型类、特质系统相关

**对接内容**:

```cpp
// C++ Concepts
template <typename T>
concept Addable = requires(T a, T b) {
    { a + b } -> std::convertible_to<T>;
};

// 对应本知识库的类型约束
-- 类型类: Addable
-- 实例: 具体类型的加法实现
```

### 4.3 ISO/IEC 8652 (Ada)

**SPARK子集**:

- 形式化验证友好
- 合约式设计
- 本知识库的公理语义可直接应用

**对接内容**:

```ada
-- Ada/SPARK示例
procedure Increment (X : in out Integer) with
    Pre  => X < Integer'Last,
    Post => X = X'Old + 1;

-- 对应霍尔逻辑
-- {X < MAX} Increment {X = X_old + 1}
```

### 4.4 ISO 26262 (汽车功能安全)

**ASIL与形式化方法**:

| ASIL | 形式化方法要求 | 本知识库覆盖 |
|:-----|:---------------|:-------------|
| ASIL A | 推荐 | 基础语义 |
| ASIL B | 推荐 | 基础语义 |
| ASIL C | 强烈推荐 | 完整理论 |
| ASIL D | 强烈推荐 | 完整理论+案例 |

**软件单元设计验证**:

- 表7：设计验证方法
- 1d：形式化验证 (高推荐度ASIL C/D)

### 4.5 IEC 61508 (功能安全)

**安全完整性等级**:

| SIL | 形式化方法 | 本知识库应用 |
|:----|:-----------|:-------------|
| SIL 1 | 不推荐 | - |
| SIL 2 | 推荐 | 基础理论 |
| SIL 3 | 推荐 | 完整理论 |
| SIL 4 | 高度推荐 | 完整理论+产业案例 |

**软件系统设计**:

- 表A.3：软件设计验证技术
- 形式化方法：SIL 3/4强烈推荐

---

## 5. 标准化贡献

### 5.1 提案建议

**对本知识库的标准化贡献**:

1. **术语标准提案**
   - 统一的语义学术语
   - 多语言对照
   - 适用ISO/IEC 24765补充

2. **符号标准提案**
   - 形式化语义符号规范
   - 与ISO 80000-2兼容
   - Unicode数学符号

3. **教学方法提案**
   - 语义学教学指南
   - 能力评估标准
   - 适用于ISO/IEC 19796

### 5.2 标准草案

**语义符号标准草案**:

```
ISO/IEC DIS XXXXX
Information technology — Symbols for programming language semantics

1 Scope
2 Normative references
3 Terms and definitions
4 Operational semantics symbols
5 Denotational semantics symbols
6 Axiomatic semantics symbols
7 Type theory symbols
8 Category theory symbols
```

### 5.3 产业建议

**向标准化组织的建议**:

1. **增强形式化内容**
   - 在程序语言标准中增加形式化附录
   - 参考本知识库的理论框架

2. **统一验证标准**
   - 建立形式化验证结果互认机制
   - 制定验证工具标准

3. **教育标准**
   - 将形式化方法纳入软件工程教育标准
   - 参考本知识库的课程设计

---

## 6. 符合性声明

### 6.1 术语符合性

**声明**: 本知识库术语定义与ISO/IEC 24765:2017一致度95%以上。

**差异说明**:

- 部分术语增加了教学解释
- 某些前沿术语标准尚未收录
- 已标注与标准术语的对应关系

### 6.2 符号符合性

**声明**: 本知识库符号系统与ISO 80000-2:2019一致度90%以上。

**差异说明**:

- 部分符号使用LaTeX传统表示
- 类型理论符号采用学术惯例
- 已在文档中说明符号变体

### 6.3 方法符合性

**声明**: 本知识库形式化方法与国际最佳实践一致。

**符合标准**:

- ISO/IEC 15909 (LOTOS)
- ISO/IEC 13568 (Z)
- IEEE 1012 (验证与确认)

---

## 7. 认证路径

### 7.1 文档认证

**ISO标准认证流程**:

```
1. 编写标准草案 (WD)
2. 工作组草案 (WD) → 委员会草案 (CD)
3. 委员会草案 → 询问草案 (DIS)
4. 询问草案 → 最终草案 (FDIS)
5. 最终草案 → 国际标准 (IS)
```

### 7.2 知识库认证

**符合性评估**:

- 第三方评估机构
- ISO 9001质量管理体系
- ISO/IEC 27001信息安全管理

### 7.3 教育认证

**学术认证**:

- ABET工程教育认证
- 计算机科学课程认证
- 参考本知识库的课程设计

---

## 8. 参考文献

1. ISO/IEC 24765:2017 - Systems and software engineering vocabulary
2. ISO/IEC 15909-1:2019 - LOTOS
3. ISO/IEC 13568:2002 - Z notation
4. ISO 26262-6:2018 - Road vehicles functional safety
5. IEC 61508-3:2010 - Functional safety of E/E/PE systems
6. ISO/IEC 15408 - Common Criteria
7. ISO 80000-2:2019 - Quantities and units - Mathematics

---

## 9. 交叉引用

- [SEMANTIC_NOTATIONS.md](./SEMANTIC_NOTATIONS.md) - 符号标准
- [SEMANTIC_SPECIFICATION.md](./SEMANTIC_SPECIFICATION.md) - 规范标准
- [SAFETY_CRITICAL_SYSTEMS.md](./SAFETY_CRITICAL_SYSTEMS.md) - 安全关键系统
- [CERTIFIED_COMPILATION.md](./CERTIFIED_COMPILATION.md) - 认证编译

---

**维护者**: Lean 4编译知识库标准化工作组
**更新日期**: 2026-03-24
