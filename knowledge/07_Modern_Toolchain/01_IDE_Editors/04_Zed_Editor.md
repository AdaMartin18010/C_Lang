# Zed 编辑器C语言开发配置

> **层级定位**: 07 Modern Toolchain / 01 IDE Editors
> **难度级别**: L2 应用
> **预估学习时间**: 2-3小时

---

## 📋 Zed简介

Zed是2024年新兴的代码编辑器，特点：

- **极致性能**: Rust编写，GPU加速
- **AI原生**: 内置AI辅助编程
- **协作编辑**: 实时多人协作
- **现代设计**: 简洁美观的界面

---

## 🚀 安装与配置

### 安装

```bash
# macOS
curl -f https://zed.dev/install.sh | sh

# Linux (即将支持)
# 查看官网最新信息
```

### C语言支持配置

Zed通过Language Server Protocol支持C语言：

**安装clangd**:

```bash
brew install llvm  # macOS
```

**配置settings.json**:

```json
{
  "lsp": {
    "clangd": {
      "binary": {
        "path": "/usr/local/bin/clangd",
        "arguments": ["--background-index"]
      }
    }
  }
}
```

---

## 📚 核心功能

### AI辅助

Zed内置AI支持：

- `Ctrl+Enter` - 打开AI助手
- 代码生成
- 代码解释
- 错误诊断

### 快捷键

| 快捷键 | 功能 |
|:-------|:-----|
| `Cmd+P` | 文件查找 |
| `Cmd+Shift+F` | 全局搜索 |
| `Cmd+Shift+P` | 命令面板 |
| `Cmd+D` | 多光标选择 |
| `Cmd+Shift+D` | 跳过当前选择 |

### 协作功能

1. 点击右上角Share
2. 复制链接邀请协作者
3. 实时同步编辑

---

## ✅ 验证清单

- [ ] Zed安装完成
- [ ] clangd安装
- [ ] LSP配置正确
- [ ] 代码补全工作

---

**← [返回工具链主页](../README.md)**
