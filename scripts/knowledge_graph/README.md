# C_Lang Knowledge Graph System

智能知识图谱分析系统，用于C_Lang知识库的可视化和分析。

## 功能特性

### 1. 文档依赖分析
- 解析所有Markdown文件的内部链接
- 构建文档-文档关系矩阵
- 计算PageRank风格的核心度排名
- 识别孤立文档和断链

### 2. 知识图谱可视化
- 整体知识图谱（Top 50核心文档）
- 主题子图谱（形式语义、工业应用等）
- 学习路径流程图
- 定理依赖网络
- 系统架构图

### 3. 分析报告
- 图谱统计（节点、边、密度）
- 主题聚类分析
- 核心文档排名
- 改进建议（断链修复、新链接建议）

## 文件结构

```
scripts/knowledge_graph/
├── README.md                      # 本文件
├── analyze_knowledge_graph.py     # 主分析脚本
├── generate_enhanced_graphs.py    # 增强可视化脚本
└── output/                        # 输出目录（运行时创建）

knowledge/00_VERSION_TRACKING/
├── KNOWLEDGE_GRAPH.md             # 完整分析报告
└── diagrams/                      # Mermaid图表文件
    ├── overall_graph.mmd          # 整体知识图谱
    ├── formal_semantics_graph.mmd # 形式语义子图
    ├── industrial_graph.mmd       # 工业应用子图
    ├── topic_distribution.mmd     # 主题分布饼图
    ├── core_knowledge_detailed.mmd    # 核心知识详细图
    ├── formal_semantics_detailed.mmd  # 形式语义详细图
    ├── system_tech_detailed.mmd       # 系统技术详细图
    ├── industrial_detailed.mmd        # 工业场景详细图
    ├── thinking_representation.mmd    # 思维表示图
    ├── learning_path_flow.mmd         # 学习路径流程图
    ├── theorem_dependency.mmd         # 定理依赖图
    └── system_architecture.mmd        # 系统架构图
```

## 使用方法

### 运行完整分析

```bash
python scripts/knowledge_graph/analyze_knowledge_graph.py
```

这将生成：
- 完整的知识图谱分析报告 (`knowledge/00_VERSION_TRACKING/KNOWLEDGE_GRAPH.md`)
- 4个基础Mermaid图表文件

### 生成增强可视化

```bash
python scripts/knowledge_graph/generate_enhanced_graphs.py
```

这将生成：
- 5个主题详细图谱
- 学习路径流程图
- 定理依赖网络
- 系统架构图

## 分析报告内容

### 统计概览
- 文档总数：961
- 内部链接总数：12,437
- 图密度：0.0135
- 主题聚类：9个

### 核心枢纽文档（Top 5）
1. 00_GLOBAL_INDEX.md (Hub Score: 167.49)
2. 02_Formal_Semantics_and_Physics/README.md (Hub Score: 157.44)
3. 03_System_Technology_Domains/README.md (Hub Score: 156.29)
4. 07_Modern_Toolchain/README.md (Hub Score: 155.39)
5. 04_Industrial_Scenarios/README.md (Hub Score: 153.88)

### 主题分布
- Core Knowledge: 149 文档 (15.5%)
- Formal Semantics: 139 文档 (14.5%)
- Thinking & Representation: 117 文档 (12.2%)
- Industrial: 106 文档 (11.0%)
- System Technology: 82 文档 (8.5%)
- 其他主题: 268 文档 (27.9%)

## 图表说明

### 1. overall_graph.mmd
整体知识图谱，展示Top 50核心文档及其连接关系。

### 2. formal_semantics_graph.mmd
形式语义与物理层子图谱，包含：
- 操作语义/指称语义/公理语义
- 类型理论
- C-汇编映射
- 微架构

### 3. industrial_graph.mmd
工业应用场景子图谱，包含：
- 汽车系统
- 航空电子
- 高频交易
- 嵌入式IoT

### 4. topic_distribution.mmd
主题分布饼图，可视化各主题文档占比。

### 5. learning_path_flow.mmd
核心知识学习路径流程图，从基础到高级的推荐学习顺序。

### 6. theorem_dependency.mmd
定理依赖网络，展示形式化证明中的定理依赖关系。

## 改进建议

### 高优先级
1. **修复断链**: 5,696个断链需要修复
2. **连接源文档**: 312个仅有出链的文档需要增加入链
3. **跨主题连接**: 增加核心知识到工业应用的链接

### 中优先级
4. **枢纽强化**: 增强Top 10枢纽文档的连接
5. **标准库集成**: 增加标准库相关的交叉引用

## 技术细节

### PageRank计算
使用阻尼系数d=0.85，迭代100次收敛。

### 枢纽评分公式
```
HubScore = 0.4 × InDegree + 0.2 × OutDegree + 0.4 × PageRank × 1000
```

### 主题分类规则
基于文件路径模式匹配：
- `01_Core` → Core Knowledge
- `02_Formal` → Formal Semantics
- `03_System` → System Technology
- `04_Industrial` → Industrial
- `05_Deep/Standards` → Standards
- `06_Thinking` → Thinking & Representation
- `07_Toolchain` → Modern Toolchain

## 依赖项

- Python 3.8+
- 标准库：`json`, `re`, `pathlib`, `collections`
- 输入文件：`association_analysis_graph.json`, `association_analysis_stats.json`

## 更新日志

### 2026-03-25
- 初始版本发布
- 完成基础分析功能
- 生成9个可视化图表
- 识别5696个断链，提供50条改进建议
