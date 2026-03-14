#!/usr/bin/env python3
"""
C_Lang 知识库自动化维护工具
功能:
1. 统计文件和内容
2. 检查内部链接
3. 验证代码示例语法
4. 生成维护报告

作者: C_Lang Knowledge Base Team
更新: 2026-03-15
"""

import os
import re
import sys
import json
import subprocess
from pathlib import Path
from datetime import datetime
from typing import List, Dict, Tuple, Optional

# 配置
KNOWLEDGE_DIR = Path("knowledge")
REPORT_FILE = "maintenance_report.md"

class Colors:
    """终端颜色输出"""
    GREEN = '\033[92m'
    YELLOW = '\033[93m'
    RED = '\033[91m'
    BLUE = '\033[94m'
    END = '\033[0m'

def print_status(message: str, status: str = "info"):
    """打印带颜色的状态消息"""
    if status == "success":
        print(f"{Colors.GREEN}✅ {message}{Colors.END}")
    elif status == "warning":
        print(f"{Colors.YELLOW}⚠️  {message}{Colors.END}")
    elif status == "error":
        print(f"{Colors.RED}❌ {message}{Colors.END}")
    else:
        print(f"{Colors.BLUE}ℹ️  {message}{Colors.END}")

class KnowledgeBaseStats:
    """知识库统计"""
    
    def __init__(self):
        self.stats = {
            "total_files": 0,
            "total_dirs": 0,
            "markdown_files": 0,
            "c_files": 0,
            "zig_files": 0,
            "total_lines": 0,
            "total_size": 0,
            "readme_coverage": 0,
            "avg_file_size": 0,
        }
    
    def collect(self) -> Dict:
        """收集统计信息"""
        print_status("收集知识库统计信息...", "info")
        
        md_files = list(KNOWLEDGE_DIR.rglob("*.md"))
        c_files = list(KNOWLEDGE_DIR.rglob("*.c"))
        zig_files = list(KNOWLEDGE_DIR.rglob("*.zig"))
        all_files = list(KNOWLEDGE_DIR.rglob("*"))
        all_dirs = [d for d in KNOWLEDGE_DIR.rglob("*") if d.is_dir()]
        
        self.stats["markdown_files"] = len(md_files)
        self.stats["c_files"] = len(c_files)
        self.stats["zig_files"] = len(zig_files)
        self.stats["total_files"] = len([f for f in all_files if f.is_file()])
        self.stats["total_dirs"] = len(all_dirs)
        
        # 计算总行数和大小
        total_lines = 0
        total_size = 0
        for f in md_files:
            try:
                content = f.read_text(encoding='utf-8')
                total_lines += len(content.split('\n'))
                total_size += f.stat().st_size
            except Exception as e:
                print_status(f"无法读取 {f}: {e}", "warning")
        
        self.stats["total_lines"] = total_lines
        self.stats["total_size"] = total_size
        self.stats["avg_file_size"] = total_size // len(md_files) if md_files else 0
        
        # README覆盖率
        dirs_with_readme = sum(1 for d in all_dirs if (d / "README.md").exists())
        self.stats["readme_coverage"] = (dirs_with_readme / len(all_dirs) * 100) if all_dirs else 0
        
        return self.stats
    
    def print_summary(self):
        """打印统计摘要"""
        print("\n" + "="*60)
        print("📊 知识库统计摘要")
        print("="*60)
        print(f"📄 Markdown文件: {self.stats['markdown_files']}")
        print(f"📁 总目录数: {self.stats['total_dirs']}")
        print(f"💻 C代码文件: {self.stats['c_files']}")
        print(f"⚡ Zig代码文件: {self.stats['zig_files']}")
        print(f"📝 总行数: {self.stats['total_lines']:,}")
        print(f"💾 总大小: {self.stats['total_size'] / 1024 / 1024:.2f} MB")
        print(f"📋 README覆盖率: {self.stats['readme_coverage']:.1f}%")
        print(f"📊 平均文件大小: {self.stats['avg_file_size'] / 1024:.2f} KB")
        print("="*60 + "\n")

class LinkChecker:
    """内部链接检查器"""
    
    def __init__(self):
        self.broken_links: List[Tuple[Path, str]] = []
        self.checked = 0
    
    def check_links(self) -> List[Tuple[Path, str]]:
        """检查所有Markdown文件中的内部链接"""
        print_status("检查内部链接...", "info")
        
        md_files = list(KNOWLEDGE_DIR.rglob("*.md"))
        all_md_names = {f.name for f in md_files}
        
        link_pattern = re.compile(r'\[([^\]]+)\]\(([^)]+)\)')
        
        for md_file in md_files:
            content = md_file.read_text(encoding='utf-8')
            
            for match in link_pattern.finditer(content):
                link_text = match.group(1)
                link_target = match.group(2)
                
                # 跳过外部链接
                if link_target.startswith(('http://', 'https://', 'mailto:')):
                    continue
                
                # 跳过锚点链接
                if link_target.startswith('#'):
                    continue
                
                self.checked += 1
                
                # 解析相对路径
                if link_target.endswith('.md'):
                    target_path = md_file.parent / link_target
                    if not target_path.exists():
                        self.broken_links.append((md_file, link_target))
        
        return self.broken_links
    
    def print_report(self):
        """打印链接检查报告"""
        print(f"\n🔗 链接检查报告 (检查 {self.checked} 个内部链接)")
        print("="*60)
        
        if not self.broken_links:
            print_status("所有内部链接有效!", "success")
        else:
            print_status(f"发现 {len(self.broken_links)} 个失效链接:", "error")
            for file, link in self.broken_links[:10]:  # 只显示前10个
                print(f"  📄 {file}")
                print(f"     └─ 失效链接: {link}")
            if len(self.broken_links) > 10:
                print(f"  ... 还有 {len(self.broken_links) - 10} 个")
        print("="*60 + "\n")

class CodeValidator:
    """代码示例验证器"""
    
    def __init__(self):
        self.results = {
            "c_files": {"total": 0, "success": 0, "failed": []},
            "zig_files": {"total": 0, "success": 0, "failed": []},
        }
    
    def check_compilers(self) -> bool:
        """检查编译器是否可用"""
        compilers_available = True
        
        # 检查GCC
        try:
            subprocess.run(['gcc', '--version'], capture_output=True, check=True)
            print_status("GCC 可用", "success")
        except (subprocess.CalledProcessError, FileNotFoundError):
            print_status("GCC 不可用", "warning")
            compilers_available = False
        
        # 检查Clang
        try:
            subprocess.run(['clang', '--version'], capture_output=True, check=True)
            print_status("Clang 可用", "success")
        except (subprocess.CalledProcessError, FileNotFoundError):
            print_status("Clang 不可用", "warning")
        
        # 检查Zig
        try:
            subprocess.run(['zig', 'version'], capture_output=True, check=True)
            print_status("Zig 可用", "success")
        except (subprocess.CalledProcessError, FileNotFoundError):
            print_status("Zig 不可用 (跳过Zig验证)", "warning")
        
        return compilers_available
    
    def validate_c_files(self):
        """验证C代码文件语法"""
        print_status("验证C代码示例...", "info")
        
        c_files = list(KNOWLEDGE_DIR.rglob("*.c"))
        self.results["c_files"]["total"] = len(c_files)
        
        for c_file in c_files:
            try:
                # 尝试语法检查 (不链接)
                result = subprocess.run(
                    ['gcc', '-fsyntax-only', '-c', str(c_file)],
                    capture_output=True,
                    timeout=10
                )
                if result.returncode == 0:
                    self.results["c_files"]["success"] += 1
                else:
                    # 可能是标准问题，尝试C11
                    result = subprocess.run(
                        ['gcc', '-std=c11', '-fsyntax-only', '-c', str(c_file)],
                        capture_output=True,
                        timeout=10
                    )
                    if result.returncode == 0:
                        self.results["c_files"]["success"] += 1
                    else:
                        self.results["c_files"]["failed"].append((c_file, result.stderr.decode()))
            except subprocess.TimeoutExpired:
                self.results["c_files"]["failed"].append((c_file, "Timeout"))
            except Exception as e:
                self.results["c_files"]["failed"].append((c_file, str(e)))
    
    def validate_zig_files(self):
        """验证Zig代码文件"""
        print_status("验证Zig代码示例...", "info")
        
        zig_files = list(KNOWLEDGE_DIR.rglob("*.zig"))
        self.results["zig_files"]["total"] = len(zig_files)
        
        for zig_file in zig_files:
            try:
                # 格式检查
                result = subprocess.run(
                    ['zig', 'fmt', '--check', str(zig_file)],
                    capture_output=True,
                    timeout=10
                )
                if result.returncode == 0:
                    self.results["zig_files"]["success"] += 1
                else:
                    # 格式错误
                    self.results["zig_files"]["failed"].append((zig_file, "Format error"))
            except Exception as e:
                # Zig不可用或出错
                self.results["zig_files"]["failed"].append((zig_file, str(e)))
    
    def print_report(self):
        """打印验证报告"""
        print("\n💻 代码验证报告")
        print("="*60)
        
        # C代码报告
        c_total = self.results["c_files"]["total"]
        c_success = self.results["c_files"]["success"]
        c_rate = (c_success / c_total * 100) if c_total > 0 else 0
        
        print(f"C代码:")
        print(f"  总数: {c_total}, 成功: {c_success}, 成功率: {c_rate:.1f}%")
        if c_rate >= 80:
            print_status(f"C代码验证通过 (>80%)", "success")
        else:
            print_status(f"C代码验证未通过 (<80%)", "error")
        
        if self.results["c_files"]["failed"]:
            print(f"  失败文件 ({len(self.results['c_files']['failed'])}):")
            for f, err in self.results["c_files"]["failed"][:5]:
                print(f"    - {f.name}")
        
        # Zig代码报告
        z_total = self.results["zig_files"]["total"]
        z_success = self.results["zig_files"]["success"]
        z_rate = (z_success / z_total * 100) if z_total > 0 else 0
        
        print(f"\nZig代码:")
        print(f"  总数: {z_total}, 成功: {z_success}, 成功率: {z_rate:.1f}%")
        if z_rate >= 85:
            print_status(f"Zig代码验证通过 (>85%)", "success")
        else:
            print_status(f"Zig代码验证未通过 (<85%)", "warning" if z_total > 0 else "info")
        
        print("="*60 + "\n")

def generate_report(stats: Dict, link_checker: LinkChecker, validator: CodeValidator):
    """生成维护报告"""
    report = f"""# C_Lang 知识库维护报告

> **生成时间**: {datetime.now().strftime("%Y-%m-%d %H:%M:%S")}
> **工具版本**: 1.0.0

---

## 📊 统计摘要

| 指标 | 数值 |
|:-----|:-----|
| Markdown文件 | {stats['markdown_files']} |
| 总目录数 | {stats['total_dirs']} |
| C代码文件 | {stats['c_files']} |
| Zig代码文件 | {stats['zig_files']} |
| 总行数 | {stats['total_lines']:,} |
| 总大小 | {stats['total_size'] / 1024 / 1024:.2f} MB |
| README覆盖率 | {stats['readme_coverage']:.1f}% |

## 🔗 链接检查

- 检查内部链接: {link_checker.checked} 个
- 失效链接: {len(link_checker.broken_links)} 个
- 状态: {'✅ 通过' if not link_checker.broken_links else '❌ 需要修复'}

## 💻 代码验证

### C代码
- 总数: {validator.results['c_files']['total']}
- 成功: {validator.results['c_files']['success']}
- 成功率: {(validator.results['c_files']['success'] / validator.results['c_files']['total'] * 100) if validator.results['c_files']['total'] > 0 else 0:.1f}%

### Zig代码
- 总数: {validator.results['zig_files']['total']}
- 成功: {validator.results['zig_files']['success']}
- 成功率: {(validator.results['zig_files']['success'] / validator.results['zig_files']['total'] * 100) if validator.results['zig_files']['total'] > 0 else 0:.1f}%

---

## 📝 维护建议

"""
    
    # 添加建议
    suggestions = []
    if stats['readme_coverage'] < 100:
        suggestions.append(f"- 补充缺失的README.md文件 (当前覆盖率 {stats['readme_coverage']:.1f}%)")
    if link_checker.broken_links:
        suggestions.append(f"- 修复 {len(link_checker.broken_links)} 个失效内部链接")
    if validator.results['c_files']['total'] > 0:
        c_rate = validator.results['c_files']['success'] / validator.results['c_files']['total'] * 100
        if c_rate < 80:
            suggestions.append(f"- 修复C代码示例 (当前成功率 {c_rate:.1f}% < 80%)")
    
    if suggestions:
        report += "\n".join(suggestions)
    else:
        report += "- 知识库状态良好，无需特别维护"
    
    report += "\n\n---\n\n*报告由 maintenance_tool.py 自动生成*\n"
    
    # 写入报告
    with open(REPORT_FILE, 'w', encoding='utf-8') as f:
        f.write(report)
    
    print_status(f"报告已生成: {REPORT_FILE}", "success")

def main():
    """主函数"""
    print("="*60)
    print("🔧 C_Lang 知识库自动化维护工具")
    print("="*60)
    print()
    
    # 检查知识库目录
    if not KNOWLEDGE_DIR.exists():
        print_status(f"错误: 找不到目录 {KNOWLEDGE_DIR}", "error")
        sys.exit(1)
    
    # 1. 收集统计
    stats_collector = KnowledgeBaseStats()
    stats = stats_collector.collect()
    stats_collector.print_summary()
    
    # 2. 检查链接
    link_checker = LinkChecker()
    link_checker.check_links()
    link_checker.print_report()
    
    # 3. 验证代码
    validator = CodeValidator()
    validator.check_compilers()
    validator.validate_c_files()
    validator.validate_zig_files()
    validator.print_report()
    
    # 4. 生成报告
    generate_report(stats, link_checker, validator)
    
    print("="*60)
    print("✨ 维护检查完成")
    print("="*60)

if __name__ == "__main__":
    main()
