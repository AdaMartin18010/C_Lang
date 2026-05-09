# 外部事件驱动更新机制设计

> **文档版本**: 1.0.0
> **最后更新**: 2026-05-10
> **目标**: 建立自动化的标准演进跟踪体系

---

## 问题陈述

当前项目更新依赖人工检查，存在以下问题：

- WG14发布新文档后，平均延迟 **2-4周** 才同步到项目
- 编译器新版本发布后，支持矩阵未能及时更新
- MISRA/CERT等安全标准更新缺乏触发机制
- 大量时间花费在"发现变化"而非"分析变化"

---

## 触发源清单

### A. 标准组织

| 来源 | URL | 检查频率 | 关键指标 |
|------|-----|----------|----------|
| WG14文档日志 | open-std.org/jtc1/sc22/wg14 | 每周 | 新文档编号(Nxxxx) |
| ISO C23勘误 | iso.org | 每月 | 技术勘误发布 |
| MISRA公告 | misra.org.uk | 每季度 | 新版本/修订 |
| CERT C更新 | sei.cmu.edu | 每季度 | 规则变更 |

### B. 编译器/工具链

| 来源 | 检查方式 | 关键指标 |
|------|----------|----------|
| GCC Release | gcc.gnu.org/releases.html | 主版本发布 |
| Clang/LLVM Release | releases.llvm.org | 主版本发布 |
| MSVC C博客 | devblogs.microsoft.com/cppblog | /std:clatest 更新 |
| glibc Release | sourceware.org/glibc | 版本发布说明 |
| musl Release | musl.libc.org | 版本更新 |

### C. 运行时/库

| 来源 | 检查频率 | 关键指标 |
|------|----------|----------|
| OpenSSL | 每月 | EOL/LTS状态变更 |
| Linux Kernel | 每季度 | DRM/Rust政策变化 |
| Zephyr/FreeRTOS | 每季度 | C23支持声明 |

### D. 安全/漏洞

| 来源 | 检查频率 | 关键指标 |
|------|----------|----------|
| NVD (CVE) | 每周 | C语言相关高危CVE |
| CISA Alerts | 每周 | 内存安全通告 |
| DARPA项目 | 每季度 | "Hardening Legacy C"结果 |

---

## 自动化工具链设计

### 阶段1：信息抓取（已实现原型）

```python
# scripts/fetch_wg14_log.py
import requests
from bs4 import BeautifulSoup

def fetch_wg14_documents():
    url = "https://www.open-std.org/jtc1/sc22/wg14/www/wg14_document_log"
    soup = BeautifulSoup(requests.get(url).text, 'html.parser')
    # 提取新文档编号和日期
    return parse_document_table(soup)

def check_new_documents(since_date):
    docs = fetch_wg14_documents()
    return [d for d in docs if d['date'] > since_date]
```

### 阶段2：差异分析

```python
# scripts/diff_standard_status.py
def compare_with_project(docs):
    """对比WG14新文档与项目现有内容"""
    for doc in docs:
        if is_defer_related(doc):
            current_status = get_project_defer_status()
            if current_status != doc['status']:
                yield Alert(doc, current_status)
```

### 阶段3：生成任务卡片

```markdown
<!-- .github/ISSUE_TEMPLATE/standard_update.md -->
---
title: "[标准更新] {{ document.title }}"
labels: ["standard-tracking", "C2y"]
---

## 新文档信息
- **编号**: {{ document.number }}
- **标题**: {{ document.title }}
- **作者**: {{ document.author }}
- **日期**: {{ document.date }}

## 与项目现状的差异
{{ diff_summary }}

## 建议操作
- [ ] 阅读原始文档
- [ ] 更新相关Markdown文件
- [ ] 更新编译器支持矩阵
- [ ] 添加/更新代码示例
```

---

## GitHub Actions 集成

```yaml
# .github/workflows/standard_tracking.yml
name: Standard Evolution Tracking
on:
  schedule:
    - cron: '0 0 * * 1'  # 每周一运行

jobs:
  check-wg14:
    runs-on: ubuntu-latest
    steps:
      - uses: actions/checkout@v4
      - name: Check WG14 Updates
        run: python scripts/check_wg14_updates.py
      - name: Create Issues for New Documents
        run: python scripts/create_tracking_issues.py
```

---

## 人工审查流程

自动化工具生成的是**提醒**，而非**自动修改**。每次触发后需要人工：

1. **验证**：确认自动化提取的信息准确
2. **评估**：判断该变化对项目的影响等级（P0/P1/P2/P3）
3. **规划**：确定需要修改哪些文件
4. **执行**：进行内容更新
5. **审核**：交叉验证修改的正确性

---

## 参考

- [C23/C2y Roadmap](../05_C_Language_Standards/C23_C2Y_ROADMAP.md)
- [2026-05-10 Major Update Log](2026-05-10_Major_Update_Log.md)
