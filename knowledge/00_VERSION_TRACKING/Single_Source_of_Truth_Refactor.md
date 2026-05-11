# 单一真相源重构计划

> **文档版本**: 1.0.0
> **最后更新**: 2026-05-10
> **目标**: 消除 `00_VERSION_TRACKING/` 中的内容冗余和版本冲突

---

## 问题诊断

### 当前状态

`00_VERSION_TRACKING/` 目录包含 **15+** 个C23/C2y相关文件，内容严重重叠：

| 文件 | 主题 | 最后更新 | 问题 |
|------|------|----------|------|
| `C23_C2Y_ROADMAP.md` | 主路线图 | 2026-05-10 | 3373行，过于庞大 |
| `C23_C2Y_SUPPORT_MATRIX.md` | 编译器支持矩阵 | 2026-05-10 | 280行，独立 |
| `C23_C2y_Comprehensive_Guide.md` | 综合指南 | 2026-05-10 | 与ROADMAP重叠 |
| `C23_to_C2y_Roadmap.md` | 简化路线图 | 2026-03 | 与主路线图功能重复 |
| `C2y_Feature_Previews.md` | 特性预览 | 2026-03 | 与C2y_Preview/目录重复 |
| `C2y_Proposal_Tracker.md` | 提案追踪 | 2026-03 | 与New_Proposals重复 |
| `C2y_New_Proposals_2026.md` | 新提案2026 | 2026-03 | 与Tracker重复 |
| `GCC_15_C23_Default.md` | GCC 15 | 2026-03 | 信息已整合到ROADMAP |
| `LLVM_21_C23_Update.md` | LLVM 21 | 2026-03 | 版本过时 |
| `LLVM_22_C23_Update.md` | LLVM 22 | 2026-03 | 版本号错误 |
| `C2y_to_C34_Roadmap.md` | C34展望 | 2026-03 | 远期规划，缺乏细节 |

### 核心问题

1. **同一信息多处维护**：defer状态在至少5个文件中存在
2. **更新不同步**：修改ROADMAP后，Tracker/Guide/Preview未能同步
3. **读者困惑**：不知道该以哪个文件为准

---

## 重构方案

### 方案A：主从架构（推荐）

**设计**：确立一个"主文档"，其他文档通过引用或链接指向主文档。

```
00_VERSION_TRACKING/
├── C23_C2Y_MASTER_INDEX.md          ← 单一入口（目录+状态摘要）
├── C23_C2Y_SUPPORT_MATRIX.md        ← 保留（独立、高频查询）
├── C23_C2Y_ROADMAP.md               ← 保留（详细路线图）
├── C2y_Spring_2026_Update.md        ← 保留（会议纪要及时性）
├── C23_Default_Standard_Migration_Guide.md ← 保留（独立主题）
├── C_Memory_Safety_Politics_2026.md ← 保留（独立主题）
├── C2y_Proposal_Tracker.md          ← 重构为自动生成的索引
└── archive/                          ← 将过时文件移入归档
    ├── C23_C2y_Comprehensive_Guide.md
    ├── C23_to_C2y_Roadmap.md
    ├── C2y_Feature_Previews.md
    ├── C2y_New_Proposals_2026.md
    ├── GCC_15_C23_Default.md
    ├── LLVM_21_C23_Update.md
    ├── LLVM_22_C23_Update.md
    └── ...
```

### 方案B：合并精简

**设计**：将多个重叠文件合并为2-3个核心文档。

```
00_VERSION_TRACKING/
├── 01_C23_Status.md                  ← C23现状+编译器支持
├── 02_C2y_Roadmap.md                 ← C2y路线图+提案追踪
├── 03_Migration_Guides.md            ← 迁移指南合集
└── 04_Meeting_Notes/                 ← 按会议日期组织
    ├── 2026_Spring.md
    ├── 2026_Summer.md
    └── ...
```

---

## 实施步骤

### 阶段1：建立主索引（1周内）

1. 创建 `C23_C2Y_MASTER_INDEX.md`
2. 明确标注每个子文档的职责边界
3. 在所有子文档头部添加指向主索引的链接

### 阶段2：归档冗余文件（1周内）

1. 创建 `archive/` 子目录
2. 将功能重复的过时文件移入
3. 在原位置放置重定向说明

### 阶段3：自动化索引生成（长期）

1. 开发脚本扫描所有C23/C2y文档
2. 自动提取：最后更新日期、提案状态、编译器支持状态
3. 生成 `C2y_Proposal_Tracker.md` 的自动更新版本

---

## 文档职责边界（草案）

| 文档 | 职责 | 不做什么 |
|------|------|----------|
| `C23_C2Y_ROADMAP.md` | C23回顾+C2y提案详细分析+迁移指南 | 不重复编译器支持矩阵 |
| `C23_C2Y_SUPPORT_MATRIX.md` | 编译器特性支持状态速查 | 不做语义分析 |
| `C2y_Spring_2026_Update.md` | 特定会议的增量更新 | 不做历史综述 |
| `C2y_Proposal_Tracker.md` | 提案状态一览表 | 不做技术深度分析 |
| `C23_Default_Standard_Migration_Guide.md` | GCC 15迁移实操 | 不做标准语义讲解 |

---

## 参考

- [00_VERSION_TRACKING/README.md](README.md)
- C23/C2y Roadmap
