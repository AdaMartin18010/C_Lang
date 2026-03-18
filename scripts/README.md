# 自动化内容同步脚本

本目录包含用于监控和同步C语言知识库内容的自动化脚本。

## 📁 脚本概览

| 脚本 | 功能 | 说明 |
|------|------|------|
| `content_sync.py` | 主控脚本 | 协调所有监控脚本 |
| `standard_monitor.py` | 标准监控 | ISO/IEC 9899、WG14、MISRA |
| `compiler_monitor.py` | 编译器监控 | GCC、Clang、Zig |
| `security_advisory_aggregator.py` | 安全公告 | CVE、CERT、CISA |

## 🚀 快速开始

### 环境要求

- Python 3.8+
- 依赖包：`requests`, `pyyaml`

### 安装依赖

```bash
pip install requests pyyaml
```

### 运行所有监控

```bash
cd scripts
python content_sync.py
```

## 📋 脚本使用说明

### 1. 标准监控 (standard_monitor.py)

监控C语言标准的发布和更新。

```bash
# 基本使用
python standard_monitor.py

# 使用自定义配置
python standard_monitor.py -c config.yml

# 指定输出文件
python standard_monitor.py -o report.md

# 使用代理
python standard_monitor.py --proxy http://proxy:8080

# 创建示例配置
python standard_monitor.py --init-config
```

**监控源包括：**
- ISO/IEC 9899 C17/C23
- WG14开放标准文档
- MISRA C编码规范
- CERT C编码标准

### 2. 编译器监控 (compiler_monitor.py)

监控编译器版本发布和新特性。

```bash
# 基本使用
python compiler_monitor.py

# 使用GitHub API Token（推荐）
python compiler_monitor.py --token YOUR_GITHUB_TOKEN

# 指定配置
python compiler_monitor.py -c compiler_config.yml
```

**支持的编译器：**
- GCC
- Clang/LLVM
- Zig
- MSVC（有限支持）

**功能：**
- 获取最新版本信息
- 解析发布说明
- 提取新特性和修复
- 更新C标准支持矩阵

### 3. 安全公告聚合 (security_advisory_aggregator.py)

聚合与C语言相关的安全公告。

```bash
# 基本使用
python security_advisory_aggregator.py

# 使用NVD API Key（提高速率限制）
python security_advisory_aggregator.py --key YOUR_NVD_API_KEY

# 查看过去14天的公告
python security_advisory_aggregator.py --days 14

# 仅显示高严重级别
python security_advisory_aggregator.py --severity critical,high
```

**数据来源：**
- NVD (National Vulnerability Database)
- CERT/CC 公告
- CISA KEV (Known Exploited Vulnerabilities)

**C相关筛选：**
- 缓冲区溢出
- 内存损坏
- 格式字符串漏洞
- C标准库漏洞
- 关键C项目（Linux、OpenSSL等）

### 4. 内容同步主控 (content_sync.py)

协调所有监控脚本的执行。

```bash
# 运行所有监控
python content_sync.py

# 运行指定脚本
python content_sync.py --scripts standard_monitor,security_advisory

# 指定输出目录
python content_sync.py -o reports/

# 生成GitHub Actions工作流
python content_sync.py --generate-workflow > .github/workflows/content_sync.yml

# 生成PR模板
python content_sync.py --generate-pr-template
```

## ⚙️ 配置方法

### 创建配置文件

每个脚本都支持通过 `--init-config` 创建示例配置：

```bash
python standard_monitor.py --init-config
python compiler_monitor.py --init-config
python security_advisory_aggregator.py --init-config
python content_sync.py --init-config
```

### 配置示例

#### standard_monitor_config.yml

```yaml
state_file: .standard_state.json
output_format: markdown

sources:
  - name: ISO/IEC 9899 C17
    url: https://www.iso.org/standard/74528.html
    type: iso
    check_interval_days: 30
    enabled: true

  - name: WG14 Documents
    url: https://www.open-std.org/jtc1/sc22/wg14/www/docs/
    type: wg14
    check_interval_days: 7
    enabled: true

notification:
  enabled: false
  webhook_url: null
```

#### compiler_monitor_config.yml

```yaml
compilers:
  - gcc
  - clang
  - zig

check_prerelease: false
output_format: markdown
support_matrix_file: compiler_support_matrix.json

min_version:
  gcc: "9.0.0"
  clang: "9.0.0"
  zig: "0.9.0"
```

#### security_advisory_config.yml

```yaml
nvd_api_key: null  # 从环境变量NVD_API_KEY读取
days_back: 7
severity_filter:
  - critical
  - high
  - medium

min_cvss_score: 0.0
max_results: 100

sources:
  - nvd
  - cert
  - cisa
```

#### content_sync_config.yml

```yaml
scripts_dir: scripts
output_dir: sync_reports
run_all: true
parallel: false

scripts:
  standard_monitor:
    enabled: true
    verbose: false
  
  compiler_monitor:
    enabled: true
    verbose: false
  
  security_advisory:
    enabled: true
    verbose: false
    config: security_advisory_config.yml

github:
  enabled: false
  create_issue: true
  create_pr: false
  auto_merge: false

notification:
  enabled: false
  webhook: null
  email: null
```

## 🔧 环境变量

| 变量 | 说明 | 脚本 |
|------|------|------|
| `NVD_API_KEY` | NVD API访问密钥 | security_advisory_aggregator |
| `GITHUB_TOKEN` | GitHub API Token | compiler_monitor, content_sync |
| `HTTP_PROXY` | HTTP代理地址 | 所有脚本 |
| `HTTPS_PROXY` | HTTPS代理地址 | 所有脚本 |

## 🔄 CI/CD集成

### GitHub Actions

工作流文件已包含在 `.github/workflows/content_sync.yml`：

```yaml
# 定时触发（每周一）
on:
  schedule:
    - cron: '0 0 * * 1'
  workflow_dispatch:
```

**工作流功能：**
- 自动运行所有监控脚本
- 上传报告为Artifacts
- 检测变更时创建Issue
- 可选：自动创建PR（草稿模式）
- 清理旧报告

### 手动触发

在GitHub Actions页面手动触发：

1. 进入 Actions → Content Sync
2. 点击 "Run workflow"
3. 可选：指定要运行的脚本

### 退出代码

| 代码 | 含义 |
|------|------|
| 0 | 成功，无变更 |
| 1 | 成功，有变更或警告 |
| 2 | 发现严重安全公告 |
| 130 | 用户取消 |
| 其他 | 错误 |

## 📊 输出格式

### Markdown报告

所有脚本默认生成Markdown格式的报告，包含：
- 执行摘要
- 检测到的变更详情
- 统计信息
- 行动建议

### JSON报告

使用 `--format json` 可生成结构化JSON：

```bash
python standard_monitor.py --format json -o report.json
```

## 🔒 安全注意事项

1. **API密钥保护**
   - 将敏感密钥存储在环境变量或GitHub Secrets中
   - 永远不要提交密钥到版本控制

2. **代理配置**
   - 企业环境可能需要代理配置
   - 使用 `--proxy` 或环境变量设置

3. **速率限制**
   - NVD API有速率限制，建议使用API Key
   - GitHub API也需要Token以避免限制

## 🐛 故障排除

### 常见问题

#### 1. 脚本执行超时

```bash
# 增加超时时间
python security_advisory_aggregator.py --timeout 120
```

#### 2. SSL证书错误

```bash
# 临时禁用SSL验证（不推荐用于生产）
export PYTHONWARNINGS="ignore:Unverified HTTPS request"
```

#### 3. 网络代理问题

```bash
# 检查代理设置
echo $HTTP_PROXY
echo $HTTPS_PROXY

# 或显式指定
python content_sync.py --proxy http://proxy.company.com:8080
```

#### 4. 依赖缺失

```bash
# 重新安装依赖
pip install --upgrade requests pyyaml
```

### 日志查看

每个脚本都会生成日志文件：
- `standard_monitor.log`
- `compiler_monitor.log`
- `security_advisory.log`
- `content_sync.log`

使用 `-v` 或 `--verbose` 查看详细日志。

## 📝 开发指南

### 添加新的监控源

在配置文件的 `sources` 部分添加：

```yaml
sources:
  - name: 新监控源名称
    url: https://example.com/source
    type: custom
    check_interval_days: 7
    enabled: true
    headers:
      Authorization: Bearer token
```

### 扩展脚本

1. 继承基类或遵循现有模式
2. 实现 `run()` 和 `generate_report()` 方法
3. 添加到 `content_sync.py` 的 `MONITOR_SCRIPTS` 列表

## 📄 许可证

这些脚本与主项目使用相同的许可证。

## 🤝 贡献

欢迎提交Issue和PR来改进这些脚本。

---

*最后更新: 2026-03-19*
