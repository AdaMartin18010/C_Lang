# C语言知识图谱 - Web版

> 交互式知识可视化导航系统

![版本](https://img.shields.io/badge/版本-1.0.0-blue)
![技术栈](https://img.shields.io/badge/技术栈-HTML/CSS/JS-green)
![许可证](https://img.shields.io/badge/许可证-MIT-yellow)

## 📖 项目简介

这是一个基于Web的交互式C语言知识图谱可视化系统，使用纯前端技术（HTML/CSS/JavaScript + D3.js）构建。它将庞大的C语言知识库转化为可视化的知识网络，帮助学习者：

- 🔍 快速定位和搜索知识点
- 🌐 直观理解知识之间的关联
- 📚 规划个性化的学习路径
- 🎯 按需筛选和探索内容

## ✨ 主要特性

### 1. 交互式知识图谱
- 使用 D3.js 力导向图可视化知识网络
- 支持节点拖拽、缩放、平移
- 点击查看节点详情
- 双击展开/折叠子节点
- 关系线动画效果

### 2. 全文搜索与过滤
- 实时搜索建议与自动完成
- 支持按节点类型过滤（模块/主题/概念）
- 支持按难度级别过滤（初级/中级/高级/专家）
- 搜索结果高亮显示

### 3. 学习路径规划
- 智能学习路径生成
- 支持多种路径策略：
  - **最短路径**：最少节点到达目标
  - **初学路线**：从易到难循序渐进
  - **全面路线**：按知识模块系统学习
- 路径可视化高亮

### 4. 响应式设计
- 支持桌面端、平板、手机访问
- 自适应布局
- 深色/浅色主题切换

### 5. 现代化UI
- 深色主题设计，护眼舒适
- 流畅的动画过渡
- 直观的操作反馈
- 完整的键盘快捷键支持

## 🚀 快速开始

### 本地运行

#### 方式1：直接打开（最简单）
```bash
# 进入项目目录
cd e:\_src\C_Lang\web

# 用浏览器打开 index.html
# 或者双击 index.html 文件
```

#### 方式2：使用本地服务器（推荐）
使用 Python 启动本地服务器：

```bash
# Python 3
cd e:\_src\C_Lang\web
python -m http.server 8080

# 浏览器访问 http://localhost:8080
```

或使用 Node.js 的 http-server：

```bash
# 安装 http-server
npm install -g http-server

# 启动服务器
cd e:\_src\C_Lang\web
http-server -p 8080

# 浏览器访问 http://localhost:8080
```

#### 方式3：使用 VS Code Live Server
1. 在 VS Code 中安装 "Live Server" 扩展
2. 右键点击 `index.html`
3. 选择 "Open with Live Server"

## 🌐 GitHub Pages 部署

### 自动部署（推荐）

1. **Fork 或克隆仓库到 GitHub**
   ```bash
   # 确保 web/ 目录在仓库根目录下
   git add web/
   git commit -m "添加知识图谱Web版"
   git push origin main
   ```

2. **启用 GitHub Pages**
   - 进入仓库 Settings → Pages
   - Source 选择 "Deploy from a branch"
   - Branch 选择 `main`，文件夹选择 `/ (root)` 或 `/web`
   - 点击 Save

3. **访问站点**
   - 等待几分钟后，访问 `https://<username>.github.io/<repo>/web/`

### 使用 GitHub Actions 自动部署

创建 `.github/workflows/deploy.yml`：

```yaml
name: Deploy to GitHub Pages

on:
  push:
    branches: [ main ]

jobs:
  deploy:
    runs-on: ubuntu-latest
    steps:
    - uses: actions/checkout@v3
    
    - name: Deploy to GitHub Pages
      uses: peaceiris/actions-gh-pages@v3
      with:
        github_token: ${{ secrets.GITHUB_TOKEN }}
        publish_dir: ./web
```

## 📁 项目结构

```
web/
├── index.html              # 主页面入口
├── README.md               # 本文件
├── css/
│   └── style.css           # 样式文件（深色主题、响应式）
├── js/
│   ├── data.js             # 数据管理模块（加载、搜索、缓存）
│   ├── graph.js            # 图谱可视化模块（D3.js）
│   └── search.js           # 搜索与过滤模块
└── data/
    └── knowledge_graph.json # 知识图谱数据文件
```

## 🛠️ 技术栈

| 技术 | 用途 | 版本 |
|------|------|------|
| HTML5 | 页面结构 | - |
| CSS3 | 样式与布局 | - |
| JavaScript (ES6+) | 交互逻辑 | - |
| D3.js | 数据可视化 | v7 |
| Font Awesome | 图标库 | v6 |

## 📝 数据更新流程

### 从知识库生成图谱数据

当知识库内容更新时，需要同步更新 `knowledge_graph.json`：

1. **手动更新**
   - 编辑 `data/knowledge_graph.json`
   - 添加/修改节点和关系
   - 更新元数据信息

2. **节点数据结构**
   ```json
   {
     "id": "唯一标识符",
     "name": "显示名称",
     "type": "module|topic|concept",
     "module": "所属模块",
     "difficulty": "beginner|intermediate|advanced|expert",
     "estimatedTime": 预估学习时间(小时),
     "description": "描述",
     "keywords": ["关键词1", "关键词2"],
     "filePath": "对应文档路径"
   }
   ```

3. **关系数据结构**
   ```json
   {
     "source": "源节点ID",
     "target": "目标节点ID",
     "type": "contains|prerequisite|related"
   }
   ```

### 数据验证

使用以下命令验证 JSON 格式：

```bash
# 使用 Node.js
node -e "JSON.parse(require('fs').readFileSync('data/knowledge_graph.json'))"

# 或使用 Python
python -m json.tool data/knowledge_graph.json > /dev/null && echo "Valid JSON"
```

## 🎨 自定义主题

### 修改颜色变量

编辑 `css/style.css` 中的 CSS 变量：

```css
:root {
  /* 背景色 */
  --bg-primary: #0d1117;
  --bg-secondary: #161b22;
  
  /* 文字色 */
  --text-primary: #e6edf3;
  --text-secondary: #7d8590;
  
  /* 节点颜色 */
  --color-module: #f78166;
  --color-topic: #a371f7;
  --color-concept: #3fb950;
}
```

### 切换主题

点击页面右上角的月亮/太阳图标切换深色/浅色主题。

## ⌨️ 快捷键

| 快捷键 | 功能 |
|--------|------|
| `Ctrl/Cmd + F` | 聚焦搜索框 |
| `Ctrl/Cmd + 0` | 重置视图 |
| `ESC` | 关闭弹窗/面板 |
| `↑/↓` | 搜索建议导航 |
| `Enter` | 选择搜索建议 |

## 🔧 开发计划

- [ ] 添加更多知识节点（目标 200+ 节点）
- [ ] 实现知识节点编辑功能
- [ ] 添加学习进度追踪
- [ ] 集成 Markdown 文档预览
- [ ] 添加知识测验功能
- [ ] 支持导出学习路径为 PDF
- [ ] 添加多语言支持

## 🤝 贡献指南

1. Fork 本仓库
2. 创建功能分支 (`git checkout -b feature/AmazingFeature`)
3. 提交更改 (`git commit -m 'Add some AmazingFeature'`)
4. 推送到分支 (`git push origin feature/AmazingFeature`)
5. 创建 Pull Request

### 代码规范

- 使用 2 空格缩进
- 使用单引号
- 添加适当的注释
- 遵循 ESLint 规范

## 📄 许可证

本项目采用 [MIT 许可证](LICENSE)。

## 🙏 致谢

- [D3.js](https://d3js.org/) - 强大的数据可视化库
- [Font Awesome](https://fontawesome.com/) - 精美的图标库
- 所有为 C语言知识库 做出贡献的人

## 📮 反馈与支持

如有问题或建议，欢迎：

- 提交 [Issue](../../issues)
- 发送邮件至项目维护者
- 加入讨论社区

---

**知识库版本**: v4.0  
**最后更新**: 2026-03-19
