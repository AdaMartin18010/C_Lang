# 社区贡献体系索引

> 本文档汇总 C_Lang 项目的完整社区贡献体系，帮助贡献者快速找到所需信息。

---

## 📚 贡献文档导航

### 必读文档

| 文档 | 说明 | 必读人群 |
|:-----|:-----|:---------|
| [CONTRIBUTING.md](CONTRIBUTING.md) | 完整的贡献指南 | ⭐⭐⭐ 所有贡献者 |
| [CONTENT_STYLE_GUIDE.md](CONTENT_STYLE_GUIDE.md) | 内容编写规范 | ⭐⭐⭐ 内容贡献者 |
| [REVIEW_PROCESS.md](REVIEW_PROCESS.md) | PR 审核流程 | ⭐⭐ 审核者、维护者 |

### 参考文档

| 文档 | 说明 | 适用场景 |
|:-----|:-----|:---------|
| [COMMUNITY_HALL_OF_FAME.md](COMMUNITY_HALL_OF_FAME.md) | 贡献者认可计划 | 了解贡献奖励 |
| [QUARTERLY_REVIEW_TEMPLATE.md](QUARTERLY_REVIEW_TEMPLATE.md) | 季度评审模板 | 维护者季度评审 |

### GitHub 模板

| 模板 | 用途 | 链接 |
|:-----|:-----|:-----|
| PR 模板 | Pull Request 描述 | [.github/PULL_REQUEST_TEMPLATE.md](.github/PULL_REQUEST_TEMPLATE.md) |
| Bug 报告 | 报告错误 | [Bug Report](.github/ISSUE_TEMPLATE/bug_report.md) |
| 功能请求 | 建议新功能 | [Feature Request](.github/ISSUE_TEMPLATE/feature_request.md) |
| 内容更新 | 更新现有内容 | [Content Update](.github/ISSUE_TEMPLATE/content_update.md) |
| 新模块提案 | 提议新模块 | [New Module Proposal](.github/ISSUE_TEMPLATE/new_module_proposal.md) |

### 工具脚本

| 脚本 | 功能 | 用法 |
|:-----|:-----|:-----|
| [contributor_stats.py](scripts/contributor_stats.py) | 贡献者统计 | `python scripts/contributor_stats.py` |
| [maintenance_tool.py](scripts/maintenance_tool.py) | 维护检查 | `python scripts/maintenance_tool.py` |

---

## 🚀 贡献流程快速指南

### 首次贡献流程

```
1. 阅读贡献指南
   └─> [CONTRIBUTING.md](CONTRIBUTING.md)

2. 了解编写规范
   └─> [CONTENT_STYLE_GUIDE.md](CONTENT_STYLE_GUIDE.md)

3. 选择贡献类型
   ├─> 修复错误 → 使用 Bug 报告模板
   ├─> 改进内容 → 使用内容更新模板
   ├─> 新功能 → 使用功能请求模板
   └─> 新模块 → 使用新模块提案模板

4. 提交 Issue 讨论
   └─> 等待社区反馈

5. Fork 并创建分支
   └─> git checkout -b feature/your-feature

6. 编写内容
   └─> 遵循内容风格指南

7. 本地验证
   └─> python scripts/maintenance_tool.py

8. 提交 PR
   └─> 使用 PR 模板

9. 等待审核
   └─> 参考 [REVIEW_PROCESS.md](REVIEW_PROCESS.md)

10. 合并发布
    └─> 获得认可!
```

---

## 📋 按场景查找

### 我想报告问题

→ 使用 [Bug 报告模板](.github/ISSUE_TEMPLATE/bug_report.md)

### 我想建议新功能

→ 使用 [功能请求模板](.github/ISSUE_TEMPLATE/feature_request.md)

### 我想改进现有内容

→ 使用 [内容更新模板](.github/ISSUE_TEMPLATE/content_update.md)

### 我想贡献新模块

1. 阅读 [新领域贡献流程](CONTRIBUTING.md#新领域贡献流程)
2. 使用 [新模块提案模板](.github/ISSUE_TEMPLATE/new_module_proposal.md)
3. 等待方案批准
4. 开始开发

### 我想翻译内容

→ 阅读 [翻译与国际化指南](CONTRIBUTING.md#翻译与国际化指南)

### 我想参与审核

→ 阅读 [审核流程](REVIEW_PROCESS.md) 和 [审核者指南](REVIEW_PROCESS.md#审核者指南)

---

## 🏆 贡献者成长路径

```
🌱 新手贡献者
    ├─ 完成首次贡献
    ├─ 熟悉贡献流程
    └─ 获得 1-3 个徽章

    ↓

🌿 活跃贡献者
    ├─ 5+ 次合并 PR
    ├─ 参与社区讨论
    └─ 获得 4-6 个徽章

    ↓

🌳 核心贡献者
    ├─ 10+ 次合并 PR
    ├─ 协助审核他人 PR
    └─ 获得 7+ 个徽章

    ↓

👑 维护者
    ├─ 持续高质量贡献
    ├─ 参与项目决策
    └─ 获得社区认可
```

了解详情: [COMMUNITY_HALL_OF_FAME.md](COMMUNITY_HALL_OF_FAME.md)

---

## 📞 获取帮助

### 社区支持

- **GitHub Issues**: [提交问题](../../issues)
- **GitHub Discussions**: [参与讨论](../../discussions)

### 常见问题

查看 [CONTRIBUTING.md#常见问题](CONTRIBUTING.md#常见问题)

### 直接联系

- 维护者团队: （通过 GitHub 联系）

---

## 📊 社区统计

*数据由 [contributor_stats.py](scripts/contributor_stats.py) 生成*

```bash
# 生成贡献者统计报告
python scripts/contributor_stats.py --format markdown --output stats.md

# 生成荣誉榜数据
python scripts/contributor_stats.py --hall-of-fame

# 统计指定时间段
python scripts/contributor_stats.py --since 2026-01-01 --until 2026-03-31
```

---

## 🔄 维护计划

| 活动 | 频率 | 负责人 | 文档 |
|:-----|:-----|:-------|:-----|
| 内容质量检查 | 每周 | 自动化脚本 | CI/CD |
| 社区健康检查 | 每月 | 维护团队 | - |
| 季度内容评审 | 每季度 | 维护团队 | [季度评审模板](QUARTERLY_REVIEW_TEMPLATE.md) |
| 年度规划 | 每年 | 核心团队 | ROADMAP |

---

> **感谢您对 C_Lang 社区的贡献！**
>
> 无论贡献大小，您都在帮助构建更好的 C 语言学习资源。

---

**最后更新**: 2026-03-19
**维护**: C_Lang 社区团队
