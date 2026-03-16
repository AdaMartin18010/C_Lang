# C_Lang CI/CD 自动化测试框架

## 📋 概述

本目录包含 C_Lang 知识库的完整 CI/CD 自动化测试框架，用于确保代码示例的质量、可编译性和一致性。

## 🏗️ 架构概览

```
┌─────────────────────────────────────────────────────────────────┐
│                    GitHub Actions 工作流                         │
│                   code_validation.yml                           │
├─────────────────────────────────────────────────────────────────┤
│  ┌─────────────┐  ┌─────────────┐  ┌─────────────┐             │
│  │ Preprocess  │  │ Compiler    │  │ MISRA       │             │
│  │ 代码提取     │  │ Matrix Test │  │ Compliance  │             │
│  │             │  │ 编译器矩阵   │  │ 合规检查     │             │
│  └──────┬──────┘  └──────┬──────┘  └──────┬──────┘             │
│         │                │                │                     │
│  ┌──────┴──────┐  ┌──────┴──────┐  ┌──────┴──────┐             │
│  │ Link Check  │  │ Zig Validate│  │ Statistics  │             │
│  │ 链接检查     │  │ Zig验证     │  │ 统计报告     │             │
│  └──────┬──────┘  └──────┬──────┘  └──────┬──────┘             │
│         │                │                │                     │
│         └────────────────┼────────────────┘                     │
│                          │                                      │
│                   ┌──────┴──────┐                              │
│                   │ Final Report│                              │
│                   │ 最终报告     │                              │
│                   └─────────────┘                              │
└─────────────────────────────────────────────────────────────────┘
                              │
                              ▼
┌─────────────────────────────────────────────────────────────────┐
│                        辅助脚本                                  │
├─────────────────────────────────────────────────────────────────┤
│  scripts/extract_and_test.py    - 代码提取和测试脚本             │
│  scripts/compiler_matrix.sh     - 编译器矩阵测试脚本             │
└─────────────────────────────────────────────────────────────────┘
```

## 🚀 快速开始

### 本地运行测试

```bash
# 1. 代码提取
python scripts/extract_and_test.py --extract-only --verbose

# 2. 编译器矩阵测试 (GCC 14, C17)
./scripts/compiler_matrix.sh --compiler gcc-14 --standard c17 --verbose

# 3. 完整测试流程
python scripts/extract_and_test.py --full --report report.json
```

### 支持的编译器和标准

| 编译器 | C11 | C17 | C23 | 备注 |
|--------|-----|-----|-----|------|
| GCC 13 | ✅ | ✅ | ⚠️ | 部分C23支持 |
| GCC 14 | ✅ | ✅ | ✅ | 完整C23支持 |
| GCC 15 | ✅ | ✅ | ✅ | 最新稳定版 |
| Clang 17 | ✅ | ✅ | ⚠️ | 部分C23支持 |
| Clang 18 | ✅ | ✅ | ✅ | 完整C23支持 |
| Clang 19 | ✅ | ✅ | ✅ | 最新稳定版 |

## 📁 文件说明

### 工作流文件

| 文件 | 大小 | 功能 |
|------|------|------|
| `code_validation.yml` | ~50KB | 主工作流定义，包含8个作业 |

### 脚本文件

| 文件 | 大小 | 功能 |
|------|------|------|
| `scripts/extract_and_test.py` | ~30KB | Python代码提取和测试工具 |
| `scripts/compiler_matrix.sh` | ~23KB | Bash编译器矩阵测试工具 |

**总大小: ~102KB** (超过15KB要求)

## 🧪 工作流作业详解

### 作业 1: 预处理与代码提取 (preprocess)

**功能：**

- 统计源文件数量
- 从 Markdown 提取代码示例
- 检测变更文件 (PR模式)
- 生成矩阵配置

**输出：**

- 提取的代码工件
- 文件统计信息
- 矩阵配置

### 作业 2: 编译器矩阵测试 (compiler-matrix-test)

**功能：**

- 多编译器并行测试
- 多标准验证
- 跨平台支持 (Ubuntu/macOS)

**矩阵配置：**

```yaml
编译器: [gcc-13, gcc-14, gcc-15, clang-17, clang-18, clang-19]
标准: [c11, c17, c23]
平台: [ubuntu-latest, macos-latest]
```

### 作业 3: MISRA 合规检查 (misra-compliance)

**功能：**

- Cppcheck 静态分析
- MISRA C:2012 规则检查
- 生成合规性报告

**注意：**

- 教学代码可能故意违反某些规则
- 预期会有大量警告，这是正常现象

### 作业 4: 链接有效性检查 (link-validation)

**功能：**

- 内部链接验证
- 外部链接验证 (定时任务)
- 生成链接检查报告

**工具：**

- markdown-link-check
- lychee (高性能链接检查器)

### 作业 5: Zig 代码验证 (validate-zig-code)

**功能：**

- Zig 格式检查
- AST 语法验证
- 兼容性测试

### 作业 6: 代码统计报告 (code-statistics)

**功能：**

- 文件数量统计
- 行数统计
- 大小分布分析
- 趋势报告

### 作业 7: 最终报告生成 (generate-final-report)

**功能：**

- 汇总所有测试结果
- 生成综合报告
- PR评论发布
- 工件上传

### 作业 8: 通知 (notify)

**功能：**

- 定时任务完成通知
- 状态汇总

## ⚙️ 配置选项

### 触发条件

```yaml
on:
  push:
    branches: [main, master, develop]
  pull_request:
    branches: [main, master, develop]
  schedule:
    - cron: '0 0 * * 0'  # 每周日
  workflow_dispatch:      # 手动触发
```

### 手动触发选项

| 选项 | 说明 |
|------|------|
| `full` | 完整测试 (默认) |
| `quick` | 快速测试 (仅GCC, C17) |
| `misra` | 仅MISRA检查 |
| `links` | 仅链接检查 |
| `extract` | 仅代码提取测试 |

## 📊 质量门禁

| 检查项 | 要求 | 说明 |
|--------|------|------|
| C 代码编译 (C11/C17) | 成功率 > 80% | 允许C23代码失败 |
| C 代码编译 (C23) | 成功率 > 90% | C23特定代码 |
| Zig 代码验证 | 成功率 > 85% | 格式+语法检查 |
| 内部链接检查 | 无死链 | 相对链接验证 |

## 🛠️ 使用脚本

### extract_and_test.py

```bash
# 基本用法
python scripts/extract_and_test.py --extract-only

# 完整测试
python scripts/extract_and_test.py --full --verbose

# 指定语言
python scripts/extract_and_test.py --filter-lang c --parallel 8

# 生成报告
python scripts/extract_and_test.py --report result.json --markdown-report result.md
```

### compiler_matrix.sh

```bash
# 基础测试
./scripts/compiler_matrix.sh --compiler gcc-14 --standard c17

# 严格模式
./scripts/compiler_matrix.sh --compiler clang-18 --standard c17 --strict

# 性能测试
./scripts/compiler_matrix.sh --compiler gcc-14 --performance --report perf.json

# 多任务并行
./scripts/compiler_matrix.sh --compiler gcc-14 --jobs 8 --verbose
```

## 📈 输出工件

每次工作流运行会生成以下工件：

1. **extracted-code-samples/** - 提取的代码示例
2. **matrix-report-*** - 编译器矩阵测试报告
3. **misra-compliance-report/** - MISRA合规性报告
4. **link-validation-report/** - 链接检查报告
5. **zig-validation-report/** - Zig验证报告
6. **code-statistics-report/** - 代码统计报告
7. **comprehensive-test-report/** - 综合测试报告

工件保留期限：30天

## 🔧 环境要求

### GitHub Actions Runner

- **Ubuntu Latest**: GCC 13-15, Clang 17-19
- **macOS Latest**: GCC (Homebrew), Clang (Xcode)

### Python 依赖

```bash
pip install markdown beautifulsoup4 pygments requests
```

### 系统依赖

```bash
# Ubuntu/Debian
sudo apt-get install -y cppcheck clang gcc bc

# macOS
brew install cppcheck llvm gcc
```

## 📝 报告格式

### JSON 报告示例

```json
{
    "report_version": "2.0.0",
    "timestamp": "2026-03-17T12:00:00Z",
    "configuration": {
        "compiler": "gcc-14",
        "standard": "c17",
        "strict_mode": true
    },
    "statistics": {
        "total_files": 150,
        "success": 145,
        "failed": 5,
        "success_rate_percent": 96.67
    }
}
```

## 🤝 贡献指南

1. 修改工作流前请测试脚本功能
2. 保持向后兼容性
3. 更新此README文档
4. 遵循现有代码风格

## 📚 相关文档

- [项目 README](../../README.md)
- [项目状态](../../PROJECT_STATUS.md)
- [贡献指南](../../CONTRIBUTING.md)

## 📄 许可

MIT License - 详见项目根目录 LICENSE 文件

---

*最后更新: 2026-03-17*
