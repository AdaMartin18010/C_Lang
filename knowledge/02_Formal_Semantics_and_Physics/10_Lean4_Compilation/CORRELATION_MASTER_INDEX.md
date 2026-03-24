# 关联论证总索引

> **目的**: 整合所有层次、模型、定理、领域间的关联论证
> **使用**: 作为理解知识网络结构的入口点

---

## 一、关联文档体系

### 1.1 核心关联文档

| 文档 | 主题 | 关键关联 | 阅读顺序 |
|:-----|:-----|:---------|:---------|
| CORRELATION_ARGUMENT.md | 层次、模型与定理关联总纲 | 全面概述 | 1 |
| SEMANTICS_UNIFICATION.md | 语义学三元组统一 | 操作/指称/公理 | 2 |
| TYPE_THEORY_CORRELATION.md | 类型论内部关联 | MLTT/CoC/HoTT | 3 |
| COMPILATION_PHASE_CORRELATION.md | 编译阶段关联 | 端到端正确性 | 4 |
| THEOREM_DEPENDENCY_GRAPH.md | 定理依赖网络 | 证明结构 | 5 |
| CROSS_DOMAIN_UNIFICATION.md | 跨领域统一 | Curry-Howard-Lambek | 6 |

### 1.2 与Z系列文档的关联

```
关联文档                    Z系列扩展文档
─────────────────────────────────────────────
CORRELATION_ARGUMENT    ↔   Z1-Z10 (作为论据)
SEMANTICS_UNIFICATION   ↔   Z1, Z2, Z10
TYPE_THEORY_CORRELATION ↔   Z1_MLTT, Z2_CoC, Z10_HoTT
COMPILATION_PHASE       ↔   Z3, Z4, Z5, Z6, Z7
THEOREM_DEPENDENCY      ↔   所有*_Extended.md
CROSS_DOMAIN            ↔   Z1, Z2, Z10
```

---

## 二、关联网络总览

### 2.1 五维关联空间

```
维度1: 层次关联 (L1-L6)
   └── 递进关系、保持性链

维度2: 语义关联 (Op/Den/Ax)
   └── Galois连接、逻辑关系

维度3: 类型论关联 (MLTT/CoC/HoTT)
   └── 忠实嵌入、保守扩展

维度4: 编译关联 (阶段间)
   └── 局部保持、端到端正确

维度5: 领域关联 (类型/逻辑/范畴)
   └── Curry-Howard-Lambek
```

### 2.2 关联强度矩阵

| | L层次 | 语义 | 类型论 | 编译 | 跨领域 |
|:--|:------|:-----|:-------|:-----|:-------|
| L层次 | - | 强 | 中 | 强 | 弱 |
| 语义 | 强 | - | 强 | 中 | 强 |
| 类型论 | 中 | 强 | - | 中 | 强 |
| 编译 | 强 | 中 | 中 | - | 弱 |
| 跨领域 | 弱 | 强 | 强 | 弱 | - |

---

## 三、关键关联定理索引

### 3.1 层次间定理

| 定理 | 关联层次 | 所在文档 |
|:-----|:---------|:---------|
| 解析正确性 | L1→L2 | COMPILATION_PHASE |
| 详细化正确性 | L2→L3 | COMPILATION_PHASE |
| 类型保持 | L3内部 | TYPE_THEORY_CORRELATION |
| IR生成保持 | L3→L4 | COMPILATION_PHASE |
| 代码生成正确 | L4→L5 | COMPILATION_PHASE |

### 3.2 模型间定理

| 定理 | 关联模型 | 所在文档 |
|:-----|:---------|:---------|
| 操作-指称等价 | Op ↔ Den | SEMANTICS_UNIFICATION |
| 最弱前置条件 | Den → Ax | SEMANTICS_UNIFICATION |
| Hoare完备性 | Ax ↔ Den | SEMANTICS_UNIFICATION |

### 3.3 理论间定理

| 定理 | 关联理论 | 所在文档 |
|:-----|:---------|:---------|
| MLTT忠实嵌入 | MLTT → CoC | TYPE_THEORY_CORRELATION |
| CoC保守扩展 | CoC ⊃ MLTT | TYPE_THEORY_CORRELATION |
| UA蕴含FunExt | HoTT ⊃ CoC | TYPE_THEORY_CORRELATION |

---

## 四、阅读路径推荐

### 路径A: 理论基础

```
CORRELATION_ARGUMENT (概览)
    ↓
CROSS_DOMAIN_UNIFICATION (CHL对应)
    ↓
TYPE_THEORY_CORRELATION (类型论递进)
    ↓
SEMANTICS_UNIFICATION (语义统一)
```

### 路径B: 编译实践

```
CORRELATION_ARGUMENT (概览)
    ↓
COMPILATION_PHASE (阶段关联)
    ↓
THEOREM_DEPENDENCY (证明网络)
    ↓
Z3-Z7_Extended (具体实现)
```

### 路径C: 全面理解

```
CORRELATION_ARGUMENT
    ├─→ SEMANTICS_UNIFICATION
    ├─→ TYPE_THEORY_CORRELATION
    ├─→ COMPILATION_PHASE
    ├─→ THEOREM_DEPENDENCY
    └─→ CROSS_DOMAIN_UNIFICATION
```

---

## 五、关联使用指南

### 5.1 诊断知识缺口

| 症状 | 缺失关联 | 推荐阅读 |
|:-----|:---------|:---------|
| 不理解编译为何正确 | 层次关联 | COMPILATION_PHASE |
| 语义概念混淆 | 语义关联 | SEMANTICS_UNIFICATION |
| 类型论概念孤立 | 类型论关联 | TYPE_THEORY_CORRELATION |
| 证明结构不清 | 定理依赖 | THEOREM_DEPENDENCY |
| 跨领域理解困难 | 跨领域映射 | CROSS_DOMAIN |

### 5.2 建立新关联的方法

1. **识别关联类型**: 层次/模型/理论/领域
2. **寻找对应**: 找出两侧的对应概念
3. **建立映射**: 定义显式转换函数
4. **验证保持性**: 证明性质在映射下保持
5. **文档化**: 添加到关联文档体系

---

## 六、关联维护

### 6.1 维护责任

| 文档 | 维护重点 | 更新触发 |
|:-----|:---------|:---------|
| CORRELATION_ARGUMENT | 总纲一致性 | 任何重大变更 |
| SEMANTICS_UNIFICATION | 语义等价性 | 新语义模型 |
| TYPE_THEORY_CORRELATION | 理论递进 | 新类型论 |
| COMPILATION_PHASE | 阶段映射 | 编译器变更 |
| THEOREM_DEPENDENCY | 依赖准确性 | 新定理/证明 |
| CROSS_DOMAIN | 对应完整性 | 新领域工具 |

### 6.2 质量检查清单

- [ ] 所有Z系列文档与关联文档对应
- [ ] 定理依赖无循环
- [ ] 层次递进关系明确
- [ ] 模型映射是函数(单值)
- [ ] 跨领域对应双向验证

---

## 七、未来扩展

### 7.1 计划中的关联

1. **并发模型关联**: 进程代数与类型系统
2. **效应系统关联**: 代数效应与单子
3. **机器验证关联**: Coq/Lean形式化与文档

### 7.2 工具化设想

- **可视化工具**: 生成交互式关联图
- **验证工具**: 自动检查关联一致性
- **导航工具**: 基于关联的知识检索

---

## 八、总结

本关联文档体系建立了：

1. **层次递进**: L1-L6的保持性链
2. **语义统一**: 三种语义学的等价框架
3. **理论递进**: MLTT→CoC→HoTT的扩展路径
4. **编译正确性**: 端到端的保持性证明
5. **定理网络**: 明确的依赖关系图
6. **跨领域映射**: Curry-Howard-Lambek完整对应

这些关联将原本孤立的文档转化为有机的知识网络，使得每个概念都能在多维空间中找到位置，每个定理都能在依赖网络中找到支撑。
