# 共享模板：知识关联体系

> **注意**: 以下为各模块README的通用底部导航模板。
> 为减少维护负担，建议在新文档中引用此模板，而非全文复制。
> 旧文档将分批迁移。

---

## 🔗 知识关联体系

### 【全局层】知识库导航

| 维度 | 目标文档 | 导航作用 |
|:-----|:---------|:---------|
| **总索引** | [../00_INDEX.md](../00_INDEX.md) | 完整知识图谱入口 |
| **学习路径** | [../06_Thinking_Representation/06_Learning_Paths/README.md](../06_Thinking_Representation/06_Learning_Paths/README.md) | 阶段化学习路线 |
| **概念映射** | [../06_Thinking_Representation/05_Concept_Mappings/README.md](../06_Thinking_Representation/05_Concept_Mappings/README.md) | 核心概念等价关系 |

### 【阶段层】学习定位

**难度等级**: L1-L6 | **前置依赖**: 核心知识体系 | **后续延伸**: 持续学习

```
学习阶段金字塔:
    L6 专家层 [形式验证、编译器]
    L5 高级层 [并发、系统编程]
    L4 进阶层 [指针、内存管理]
    L3 基础层 [函数、结构体]
    L2 入门层 [语法、数据类型]
    L1 零基础 [环境搭建]
```

### 【层次层】纵向知识链

| 层级 | 关联文档 | 层次关系 |
|:-----|:---------|:---------|
| **理论基础** | [../02_Formal_Semantics_and_Physics/README.md](../02_Formal_Semantics_and_Physics/README.md) | 语义学理论基础 |
| **核心机制** | [../01_Core_Knowledge_System/README.md](../01_Core_Knowledge_System/README.md) | C语言核心机制 |
| **标准接口** | [../01_Core_Knowledge_System/04_Standard_Library_Layer/README.md](../01_Core_Knowledge_System/04_Standard_Library_Layer/README.md) | 标准库API |
| **系统实现** | [../03_System_Technology_Domains/README.md](../03_System_Technology_Domains/README.md) | 系统级实现 |

### 【总体层】知识体系架构

```
┌─────────────────────────────────────────────────────────────┐
│                     总体知识体系架构                          │
├─────────────────────────────────────────────────────────────┤
│  01 Core Knowledge          → 核心概念与机制                  │
│  02 Formal Semantics        → 理论与物理基础                  │
│  03 System Technology       → 系统级技术领域                  │
│  04 Industrial Scenarios    → 工业应用场景                    │
│  05 Deep Structure          → 深层结构与元物理                │
│  06 Thinking Representation → 思维表征与学习                  │
│  07 Modern Toolchain        → 现代工具链                      │
└─────────────────────────────────────────────────────────────┘
```

### 【决策层】学习路径选择

| 目标 | 推荐路径 | 关键文档 |
|:-----|:---------|:---------|
| **系统学习** | 01 → 02 → 03 → 04 | 按顺序阅读各模块 |
| **问题导向** | 06决策树 → 相关模块 | [决策树目录](../06_Thinking_Representation/01_Decision_Trees/README.md) |
| **项目驱动** | 04案例 → 所需知识 | [工业场景](../04_Industrial_Scenarios/README.md) |
| **深入研究** | 02形式语义 → CompCert | [形式语义](../02_Formal_Semantics_and_Physics/README.md) |

---

*此模板用于统一各模块README底部结构，减少重复内容。*
