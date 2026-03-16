# 贡献指南

> **感谢您对 C_Lang 知识库的兴趣！**
> 本指南将帮助您参与项目贡献。

---

## 📋 目录

- [贡献指南](#贡献指南)
  - [📋 目录](#-目录)
  - [行为准则](#行为准则)
    - [我们的承诺](#我们的承诺)
    - [不可接受的行为](#不可接受的行为)
  - [如何贡献](#如何贡献)
    - [方式一: 提交 Issue](#方式一-提交-issue)
    - [方式二: 提交 Pull Request](#方式二-提交-pull-request)
      - [准备工作](#准备工作)
      - [修改内容](#修改内容)
      - [创建 PR](#创建-pr)
  - [内容标准](#内容标准)
    - [文档规范](#文档规范)
      - [文件命名](#文件命名)
      - [文档结构](#文档结构)
      - [代码示例](#代码示例)
    - [内容质量](#内容质量)
      - [准确性](#准确性)
      - [完整性](#完整性)
      - [可读性](#可读性)
  - [提交规范](#提交规范)
    - [Commit Message 格式](#commit-message-格式)
      - [Type 类型](#type-类型)
      - [示例](#示例)
    - [PR 模板](#pr-模板)
  - [审核流程](#审核流程)
    - [审核标准](#审核标准)
    - [审核时间](#审核时间)
    - [合并条件](#合并条件)
  - [开发指南](#开发指南)
    - [本地开发](#本地开发)
    - [目录结构](#目录结构)
    - [添加新内容](#添加新内容)
    - [更新检查清单](#更新检查清单)
  - [常见问题](#常见问题)
    - [Q: 发现文档中有错误，但不确定如何修复？](#q-发现文档中有错误但不确定如何修复)
    - [Q: 想添加新主题，但不确定目录位置？](#q-想添加新主题但不确定目录位置)
    - [Q: 代码示例需要特定环境才能运行？](#q-代码示例需要特定环境才能运行)
    - [Q: 如何追踪文档的更新状态？](#q-如何追踪文档的更新状态)
  - [联系方式](#联系方式)
  - [致谢](#致谢)

---

## 行为准则

### 我们的承诺

- 友善、包容的社区环境
- 尊重不同的观点和经验
- 建设性的批评与反馈
- 关注社区最佳利益

### 不可接受的行为

- 歧视、骚扰或侮辱性言论
- 恶意破坏或攻击性行为
- 发布不当内容
- 侵犯他人隐私

---

## 如何贡献

### 方式一: 提交 Issue

发现错误或建议改进：

1. 点击 [New Issue](../../issues/new)
2. 选择模板:
   - 🐛 **Bug Report** - 报告错误
   - 💡 **Feature Request** - 功能建议
   - 📚 **Content Improvement** - 内容改进
3. 详细描述问题或建议
4. 如有必要，提供截图或示例

### 方式二: 提交 Pull Request

#### 准备工作

```bash
# 1. Fork 本仓库
# 2. 克隆您的Fork
git clone https://github.com/YOUR_USERNAME/C_Lang.git
cd C_Lang

# 3. 添加上游仓库
git remote add upstream https://github.com/ORIGINAL_OWNER/C_Lang.git

# 4. 创建特性分支
git checkout -b feature/your-feature-name
```

#### 修改内容

```bash
# 进行修改...
# 编辑文件

# 运行维护工具检查
python scripts/maintenance_tool.py

# 提交更改
git add .
git commit -m "feat: 添加XXX内容"
git push origin feature/your-feature-name
```

#### 创建 PR

1. 访问您的Fork页面
2. 点击 "Compare & pull request"
3. 填写PR模板
4. 等待审核

---

## 内容标准

### 文档规范

#### 文件命名

```
✅ 推荐:
  - 01_C23_Core_Features.md
  - 02_Memory_Management.md
  - README.md

❌ 避免:
  - doc1.md
  - 临时文件.md
  - New Document.md
```

#### 文档结构

```markdown
# 标题

> **元信息**: 难度 | 标准 | 更新日期

---

## 目录

- [标题](#标题)
  - [目录](#目录)
  - [章节1](#章节1)
  - [章节2](#章节2)

---

## 章节1

内容...

## 章节2

内容...

---

> **参考**: 相关链接
```

#### 代码示例

- 所有代码示例必须可编译
- 使用语法高亮
- 包含必要的注释
- 标注适用的C标准版本

```c
// ✅ 好的示例
#include <stdio.h>
#include <stdbool.h>

// C23: 使用nullptr
int main(void) {
    void* ptr = nullptr;  // C23特性
    if (ptr == nullptr) {
        printf("Null pointer\n");
    }
    return 0;
}
```

### 内容质量

#### 准确性

- ✅ 引用权威来源 (ISO标准、官方文档)
- ✅ 代码示例经过测试
- ✅ 版本信息准确
- ✅ 及时更新过时内容

#### 完整性

- ✅ 概念解释清晰
- ✅ 包含示例代码
- ✅ 提供参考资料
- ✅ 与其他文档交叉引用

#### 可读性

- ✅ 使用简洁清晰的语言
- ✅ 合理使用表格和图表
- ✅ 适当分段，避免长篇大论
- ✅ 为外文术语提供中文解释

---

## 提交规范

### Commit Message 格式

```
<type>(<scope>): <subject>

<body>

<footer>
```

#### Type 类型

| 类型 | 说明 | 示例 |
|:-----|:-----|:-----|
| `feat` | 新功能/内容 | `feat: 添加C2y defer特性文档` |
| `fix` | 修复错误 | `fix: 修正Zig版本声明` |
| `docs` | 文档改进 | `docs: 优化内存模型说明` |
| `style` | 格式调整 | `style: 统一代码格式` |
| `refactor` | 重构 | `refactor: 重组形式语义章节` |
| `test` | 测试相关 | `test: 添加代码验证` |
| `chore` | 维护 | `chore: 更新依赖` |

#### 示例

```
feat(c23): 添加constexpr函数详解

- 添加编译时计算示例
- 对比C++ constexpr差异
- 提供性能测试数据

Closes #123
```

### PR 模板

```markdown
## 描述
简要描述此次更改的内容。

## 类型
- [ ] Bug修复
- [ ] 新功能
- [ ] 文档改进
- [ ] 内容更新
- [ ] 其他

## 检查清单
- [ ] 代码示例经过验证
- [ ] 内部链接有效
- [ ] 新增内容已添加到索引
- [ ] 版本日期已更新

## 相关Issue
Fixes #(issue编号)
```

---

## 审核流程

### 审核标准

1. **准确性**: 内容技术正确
2. **完整性**: 覆盖必要信息
3. **一致性**: 与现有内容风格一致
4. **可维护性**: 易于后续更新

### 审核时间

- 一般PR: 3-5个工作日
- 重大更改: 5-7个工作日
- 紧急修复: 1-2个工作日

### 合并条件

- [ ] 通过自动化检查 (CI/CD)
- [ ] 至少1位维护者审核通过
- [ ] 无冲突需要解决
- [ ] 符合内容标准

---

## 开发指南

### 本地开发

```bash
# 克隆仓库
git clone https://github.com/YOUR_USERNAME/C_Lang.git
cd C_Lang

# 安装依赖 (如需)
# Python 3.8+ 用于维护工具

# 运行维护工具
python scripts/maintenance_tool.py
```

### 目录结构

```
knowledge/
├── 00_INDEX.md              # 全局索引
├── 00_VERSION_TRACKING/     # 版本追踪
├── 01_Core_Knowledge_System/
│   ├── 01_Basic_Layer/      # 基础层
│   ├── 02_Core_Layer/       # 核心层
│   └── ...
└── ...
```

### 添加新内容

1. **确定位置**: 根据主题选择合适的目录
2. **创建文件**: 使用规范命名
3. **更新索引**: 修改目录README和全局索引
4. **添加链接**: 建立与其他文档的交叉引用
5. **运行检查**: `python scripts/maintenance_tool.py`

### 更新检查清单

```markdown
## 内容更新检查清单

- [ ] 文档已添加到正确目录
- [ ] 使用正确的文件命名
- [ ] 包含必要的元信息
- [ ] 代码示例可编译
- [ ] 目录README已更新
- [ ] 全局索引已更新 (00_INDEX.md)
- [ ] 相关交叉引用已添加
- [ ] 运行维护工具无错误
- [ ] 提交信息符合规范
```

---

## 常见问题

### Q: 发现文档中有错误，但不确定如何修复？

A: 直接提交 [Issue](../../issues/new) 描述问题，维护者会处理。

### Q: 想添加新主题，但不确定目录位置？

A: 先提交 Issue 讨论，确定最佳位置后再编写。

### Q: 代码示例需要特定环境才能运行？

A: 在文档中注明环境要求，并尽量提供通用示例。

### Q: 如何追踪文档的更新状态？

A: 查看 [CHANGELOG.md](CHANGELOG.md) 和 [PROJECT_STATUS.md](PROJECT_STATUS.md)

---

## 联系方式

- **Issues**: [GitHub Issues](../../issues)
- **讨论**: [GitHub Discussions](../../discussions)
- **邮件**: (如需公开邮箱)

---

## 致谢

感谢所有贡献者！

<a href="../../graphs/contributors">
  <img src="https://contrib.rocks/image?repo=YOUR_USERNAME/C_Lang" />
</a>

---

**再次感谢您的贡献！** 🎉
