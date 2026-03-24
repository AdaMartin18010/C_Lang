# C_Lang 定理依赖网络构建工具

## 概述

本目录包含构建C_Lang知识库248个定理依赖关系网络的完整工具链。

## 脚本说明

### 1. extract_theorems.py

**功能**: 扫描所有.md和.lean文件，提取定理定义

**输入**:

- `knowledge/**/*.md` - Markdown文档
- `knowledge/**/*.lean` - Lean 4源代码

**输出**:

- `theorems_raw.json` - 原始定理数据

**提取的定理类型**:

- 桥梁定理 (BT-1至BT-5及其辅助引理)
- 操作语义定理 (类型保持、进展、安全性等)
- 指称语义定理
- 公理语义定理 (Hoare逻辑)
- 分布式系统定理 (Raft安全性)
- 类型理论定理 (MLTT, CoC, HoTT)
- 编译器相关定理

### 2. enhance_network.py

**功能**: 增强定理依赖关系

**输入**:

- `theorems_raw.json`

**输出**:

- `theorems_enhanced.json` - 增强后的定理数据
- `network_248.json` - 248个核心定理的网络数据

**主要功能**:

- 定义已知的定理依赖链
- 构建增强的依赖关系
- 选择248个核心定理

### 3. build_network.py

**功能**: 构建定理依赖网络（备用脚本）

### 4. generate_report.py

**功能**: 生成完整的定理网络报告和Mermaid可视化

**输入**:

- `network_248.json`

**输出**:

- `../../knowledge/02_Formal_Semantics_and_Physics/THEOREM_NETWORK.md` - 主报告
- `../../knowledge/02_Formal_Semantics_and_Physics/diagrams/*.mmd` - 8个Mermaid图表

## 生成的图表

| 文件名 | 描述 |
|--------|------|
| `theorem_network_full.mmd` | 完整定理网络（Top 150节点） |
| `theorem_network_critical_path.mmd` | 关键路径高亮 |
| `theorem_network_bridge_flow.mmd` | 桥梁定理依赖流 |
| `theorem_network_semantic_triangle.mmd` | 语义三角关系 |
| `theorem_network_distributed.mmd` | 分布式系统定理 |
| `theorem_network_桥梁定理.mmd` | 桥梁定理子网络 |
| `theorem_network_类型理论.mmd` | 类型理论子网络 |
| `theorem_network_操作语义.mmd` | 操作语义子网络 |
| `theorem_network_指称语义.mmd` | 指称语义子网络 |
| `theorem_network_公理语义.mmd` | 公理语义子网络 |
| `theorem_network_分布式系统.mmd` | 分布式系统子网络 |

## 使用方法

### 完整构建流程

```bash
cd e:/_src/C_Lang

# 步骤1: 提取定理
python scripts/theorem_network/extract_theorems.py

# 步骤2: 增强网络
python scripts/theorem_network/enhance_network.py

# 步骤3: 生成报告
python scripts/theorem_network/generate_report.py
```

### 单独运行

```bash
# 仅提取定理
python scripts/theorem_network/extract_theorems.py

# 仅生成报告（需已有network_248.json）
python scripts/theorem_network/generate_report.py
```

## 网络统计

构建的248个定理网络包含：

- **节点数**: 248个定理
- **边数**: 189个依赖关系
- **依赖密度**: 0.76 边/节点
- **关键路径长度**: 6
- **最大依赖深度**: 5
- **孤立节点**: 166个（主要是基础定义）

## 定理类别分布

| 类别 | 数量 | 占比 |
|------|------|------|
| 分布式系统 | 196 | 79.0% |
| 桥梁定理 | 20 | 8.1% |
| 公理语义 | 17 | 6.9% |
| 操作语义 | 15 | 6.0% |

## 核心定理识别

### 中心性最高的定理

1. **leaderCompleteness** (领导者完备性) - 中心性: 15
2. **assignRule** (赋值规则) - 中心性: 12
3. **stateMachineSafety** (状态机安全性) - 中心性: 10
4. **raftSafety** (Raft安全性综合) - 中心性: 9
5. **preservation** (类型保持) - 中心性: 8

### 关键依赖链

最长的依赖链包含6个定理：

```
indirectCommit → commitRuleCurrentTerm → leaderCompleteness
→ electionSafety → voteTermConsistency → voteUniqueness
```

## 技术说明

### 依赖关系定义

依赖关系通过以下方式定义：

1. **显式依赖**: 文档中明确引用的定理（如"由引理 BT-1-A"）
2. **手动定义**: 已知的定理依赖链（如类型保持依赖于替换引理）
3. **结构依赖**: 基于定理类别的层次结构

### 定理选择标准

248个核心定理的选择标准：

1. 桥梁定理（优先级最高）
2. 操作/指称/公理语义核心定理
3. 分布式系统安全性定理
4. 类型理论基础定理
5. 编译器正确性定理

## 维护

如需更新定理网络：

1. 修改 `enhance_network.py` 中的依赖链定义
2. 重新运行完整构建流程
3. 检查生成的报告和图表

## 许可证

同C_Lang项目主许可证
