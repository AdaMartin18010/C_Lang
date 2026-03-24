# C_Lang知识库可视化系统使用指南

> **文档版本**: 1.0  
> **最后更新**: 2026-03-25  
> **作者**: C_Lang知识库团队

---

## 📋 目录

1. [系统概述](#系统概述)
2. [可视化类型](#可视化类型)
3. [使用说明](#使用说明)
4. [数据更新](#数据更新)
5. [部署指南](#部署指南)
6. [故障排除](#故障排除)

---

## 1. 系统概述

C_Lang知识库可视化系统提供了三个交互式D3.js可视化，帮助用户直观地探索知识库的结构和内容：

| 可视化 | 描述 | 数据规模 |
|--------|------|----------|
| **定理网络** | 248个定理的依赖关系网络 | 403节点，125边 |
| **Lean生态** | Lean 4生态系统层次结构 | 30节点，29边 |
| **知识图谱** | Top 200核心文档的关系网络 | 200节点，927边 |

### 1.1 系统架构

```
web/visualization/
├── theorem_network.html      # 定理网络可视化
├── lean_ecosystem.html       # Lean生态可视化
├── knowledge_graph.html      # 知识图谱可视化
├── css/
│   └── common.css           # 共享样式
├── js/                      # 可选的JS模块
└── data/
    ├── theorem_network.json  # 定理网络数据
    ├── lean_ecosystem.json   # Lean生态数据
    ├── knowledge_graph.json  # 知识图谱数据
    └── summary.json          # 统计摘要
```

---

## 2. 可视化类型

### 2.1 定理网络可视化 (theorem_network.html)

**功能特点：**

- **力导向图布局**：基于D3.js力导向算法自动排列节点
- **类别着色**：
  - 🟢 操作语义 (绿色)
  - 🔵 指称语义 (蓝色)
  - 🟠 公理语义 (橙色)
  - 🩵 分布式系统 (青色)
  - 🔴 桥梁定理 (红色)
- **节点大小**：基于中心性得分
- **交互功能**：
  - 悬停显示定理详情
  - 点击高亮依赖链（上游+下游）
  - 缩放和平移
  - 按类别过滤
  - 搜索定理名称
  - 关键路径高亮模式

**关键路径定理：**
1. indirectCommit → commitRuleCurrentTerm → leaderCompleteness
2. electionSafety → voteTermConsistency → voteUniqueness

### 2.2 Lean生态可视化 (lean_ecosystem.html)

**功能特点：**

- **三种视图模式**：
  - 🌳 树状视图：层次化展示
  - ⭕ 径向视图：圆形布局
  - ☀️ 旭日图：数学领域覆盖
- **组件类型**：
  - 语言核心 (蓝色)
  - 数学库 (红色)
  - 核心组件 (绿色)
  - 领域框架 (橙色)
  - 形式化项目 (紫色)
- **交互功能**：
  - 点击展开/折叠子树
  - 缩放和平移
  - 搜索组件
  - 切换布局模式

**数学覆盖领域：**
- 代数 (Algebra)
- 分析 (Analysis)
- 拓扑 (Topology)
- 数论 (NumberTheory)
- 线性代数 (LinearAlgebra)
- 组合 (Combinatorics)
- 几何 (Geometry)
- 逻辑 (Logic)

### 2.3 知识图谱可视化 (knowledge_graph.html)

**功能特点：**

- **力导向图布局**：基于PageRank得分的节点大小
- **主题聚类着色**（9个主题）：
  - 核心知识 (蓝色)
  - 形式语义 (绿色)
  - 系统技术 (红色)
  - 工业场景 (橙色)
  - 标准规范 (紫色)
  - 深层结构 (青色)
  - 思维表示 (粉色)
  - 嵌入式 (深灰)
  - 其他 (灰色)
- **聚类边界**：显示主题聚类的凸包边界
- **交互功能**：
  - 悬停显示文档信息
  - 点击查看文档预览
  - 主题过滤
  - 搜索文档
  - 显示/隐藏标签
  - 高亮Hub节点

---

## 3. 使用说明

### 3.1 基本操作

| 操作 | 说明 |
|------|------|
| **悬停** | 将鼠标悬停在节点上查看详细信息 |
| **点击** | 点击节点查看详情面板或执行特定操作 |
| **拖拽** | 拖拽节点可以调整位置（力导向图中） |
| **滚轮** | 滚动鼠标滚轮缩放视图 |
| **平移** | 按住空白处拖拽可以平移视图 |

### 3.2 控制按钮

每个可视化页面都包含以下控制按钮：

- **+** / **−**：放大/缩小视图
- **⊡**：适应屏幕（重置缩放）
- **++** / **−−**（Lean生态）：全部展开/折叠

### 3.3 搜索功能

所有可视化都支持搜索：

1. 在侧边栏的搜索框中输入关键词
2. 系统会自动高亮匹配的节点
3. 按Enter或等待自动定位

搜索支持模糊匹配，不区分大小写。

### 3.4 过滤功能

**定理网络过滤：**
- 按类别过滤：勾选/取消勾选类别复选框
- 特殊模式：点击"显示关键路径"按钮

**Lean生态过滤：**
- 按组件类型过滤
- 切换三种视图模式

**知识图谱过滤：**
- 按主题聚类过滤
- 显示/隐藏聚类边界
- 显示/隐藏节点标签
- 高亮Hub节点（PageRank > 15）

---

## 4. 数据更新

### 4.1 更新数据

当知识库内容发生变化时，需要重新导出可视化数据：

```bash
# 进入项目根目录
cd e:\_src\C_Lang

# 运行数据导出脚本
python scripts/export_for_viz.py
```

脚本会自动：
1. 从定理网络提取JSON数据
2. 从知识图谱提取JSON数据
3. 从Lean生态提取JSON数据
4. 生成统计摘要

### 4.2 数据文件说明

| 文件 | 描述 | 更新频率 |
|------|------|----------|
| `theorem_network.json` | 定理依赖关系 | 每月 |
| `lean_ecosystem.json` | Lean生态结构 | 每季度 |
| `knowledge_graph.json` | 文档关系网络 | 每周 |
| `summary.json` | 统计摘要 | 每次更新 |

### 4.3 自定义数据

如果需要手动修改数据，JSON文件结构如下：

**定理网络数据格式：**
```json
{
  "nodes": [
    {
      "id": "theorem_name",
      "name": "Theorem Name",
      "category": "操作语义",
      "color": "#2ecc71",
      "centrality": 10,
      "is_critical": false
    }
  ],
  "links": [
    {
      "source": "theorem_a",
      "target": "theorem_b",
      "type": "depends_on"
    }
  ],
  "stats": {
    "total_nodes": 403,
    "total_links": 125
  }
}
```

---

## 5. 部署指南

### 5.1 本地查看

最简单的方式是在本地打开HTML文件：

```bash
# 使用Python启动简单HTTP服务器
cd web/visualization
python -m http.server 8080

# 然后访问 http://localhost:8080
```

或使用VS Code的Live Server扩展。

### 5.2 部署到GitHub Pages

**方法一：使用gh-pages分支**

```bash
# 创建gh-pages分支
git checkout --orphan gh-pages

# 复制可视化文件
cp -r web/visualization/* .

# 提交并推送
git add .
git commit -m "Deploy visualization to GitHub Pages"
git push origin gh-pages

# 返回主分支
git checkout main
```

**方法二：使用GitHub Actions自动部署**

创建 `.github/workflows/deploy-viz.yml`：

```yaml
name: Deploy Visualization

on:
  push:
    branches: [ main ]
    paths:
      - 'web/visualization/**'
      - 'scripts/export_for_viz.py'

jobs:
  deploy:
    runs-on: ubuntu-latest
    steps:
    - uses: actions/checkout@v3
    
    - name: Setup Python
      uses: actions/setup-python@v4
      with:
        python-version: '3.11'
    
    - name: Export Data
      run: python scripts/export_for_viz.py
    
    - name: Deploy to GitHub Pages
      uses: peaceiris/actions-gh-pages@v3
      with:
        github_token: ${{ secrets.GITHUB_TOKEN }}
        publish_dir: ./web/visualization
```

**配置GitHub Pages：**

1. 进入仓库 Settings → Pages
2. Source 选择 "Deploy from a branch"
3. Branch 选择 "gh-pages" / "/ (root)"
4. 点击 Save

### 5.3 自定义域名（可选）

1. 在 `web/visualization/` 目录创建 `CNAME` 文件
2. 内容为你的域名，如 `viz.c-lang-kb.org`
3. 在DNS提供商处添加CNAME记录指向 `<username>.github.io`

---

## 6. 故障排除

### 6.1 常见问题

**Q: 页面显示空白，没有可视化内容**

A: 检查以下几点：
- 确保通过HTTP服务器访问，而非直接打开文件
- 检查浏览器控制台是否有CORS错误
- 确认 `data/*.json` 文件存在且格式正确

**Q: 数据加载失败**

A: 运行数据导出脚本重新生成数据：
```bash
python scripts/export_for_viz.py
```

**Q: 节点位置错乱或重叠**

A: 点击"重置视图"按钮或刷新页面。力导向图需要一定的计算时间才能稳定。

**Q: 搜索没有结果**

A: 检查搜索关键词是否正确，尝试使用更通用的关键词。

### 6.2 浏览器兼容性

| 浏览器 | 支持状态 |
|--------|----------|
| Chrome 90+ | ✅ 完全支持 |
| Firefox 88+ | ✅ 完全支持 |
| Safari 14+ | ✅ 完全支持 |
| Edge 90+ | ✅ 完全支持 |
| IE 11 | ❌ 不支持 |

### 6.3 性能优化

对于大型数据集（>1000节点）：

1. **减少节点标签显示**：只在高分节点显示标签
2. **启用碰撞检测**：防止节点重叠
3. **降低模拟精度**：减少tick次数
4. **使用WebGL渲染**：对于超大规模图考虑使用WebGL库

### 6.4 调试技巧

1. **查看控制台**：按F12打开开发者工具查看错误信息
2. **检查数据**：在控制台运行 `console.log(nodes)` 检查数据结构
3. **网络请求**：在Network标签页检查JSON文件是否成功加载
4. **元素检查**：使用Elements标签检查SVG元素是否正确生成

---

## 附录

### A. 文件清单

```
web/visualization/
├── theorem_network.html      (23KB)
├── lean_ecosystem.html       (33KB)
├── knowledge_graph.html      (28KB)
├── css/common.css           (10KB)
├── data/
│   ├── theorem_network.json
│   ├── lean_ecosystem.json
│   ├── knowledge_graph.json
│   └── summary.json
└── [可选] js/modules/
```

### B. 外部依赖

- **D3.js v7**: https://d3js.org/d3.v7.min.js
- 通过CDN加载，无需本地安装

### C. 更新日志

| 版本 | 日期 | 变更内容 |
|------|------|----------|
| 1.0 | 2026-03-25 | 初始版本，三个可视化完成 |

### D. 反馈与支持

如有问题或建议，请通过以下方式联系：
- 提交GitHub Issue
- 发送邮件至维护团队

---

**文档结束**

*本指南是C_Lang知识库项目的一部分，遵循MIT许可证。*
