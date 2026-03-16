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

class MarkdownCodeExtractor:
    """Markdown代码块提取器"""
    
    def __init__(self):
        self.c_snippets = []  # [(file_path, line_num, code), ...]
        self.zig_snippets = []
    
    def extract_code_blocks(self):
        """从所有Markdown文件中提取代码块"""
        print_status("从Markdown提取代码块...", "info")
        
        md_files = list(KNOWLEDGE_DIR.rglob("*.md"))
        
        for md_file in md_files:
            content = md_file.read_text(encoding='utf-8')
            lines = content.split('\n')
            
            in_code_block = False
            code_lang = None
            code_content = []
            start_line = 0
            
            for i, line in enumerate(lines):
                if line.startswith('```'):
                    if not in_code_block:
                        # 开始代码块
                        in_code_block = True
                        code_lang = line[3:].strip().lower()
                        code_content = []
                        start_line = i + 1
                    else:
                        # 结束代码块
                        in_code_block = False
                        code = '\n'.join(code_content)
                        
                        # 只提取足够长的代码块（至少3行，排除单行示例）
                        if len(code_content) >= 3:
                            if code_lang in ('c', 'cpp', 'c++', 'c99', 'c11', 'c17', 'c23'):
                                self.c_snippets.append((md_file, start_line, code))
                            elif code_lang in ('zig', 'zon'):
                                self.zig_snippets.append((md_file, start_line, code))
                        
                        code_lang = None
                        code_content = []
                elif in_code_block:
                    code_content.append(line)
        
        print_status(f"提取到 {len(self.c_snippets)} 个C代码块, {len(self.zig_snippets)} 个Zig代码块", "success")
    
    def validate_c_snippets(self) -> Dict:
        """验证C代码片段"""
        results = {"total": 0, "success": 0, "failed": []}
        
        if not self.c_snippets:
            return results
        
        print_status("验证C代码片段语法...", "info")
        
        import tempfile
        import os
        
        for md_file, line_num, code in self.c_snippets[:50]:  # 限制前50个以节省时间
            results["total"] += 1
            
            # 创建临时文件
            with tempfile.NamedTemporaryFile(mode='w', suffix='.c', delete=False) as f:
                f.write(code)
                temp_path = f.name
            
            try:
                # 尝试编译（语法检查）
                result = subprocess.run(
                    ['gcc', '-std=c11', '-fsyntax-only', '-c', temp_path],
                    capture_output=True,
                    timeout=5
                )
                if result.returncode == 0:
                    results["success"] += 1
                else:
                    # 可能是缺少头文件，尝试添加常见头文件
                    enhanced_code = f"#include <stdio.h>\n#include <stdlib.h>\n#include <string.h>\n{code}"
                    with open(temp_path, 'w') as f:
                        f.write(enhanced_code)
                    
                    result = subprocess.run(
                        ['gcc', '-std=c11', '-fsyntax-only', '-c', temp_path],
                        capture_output=True,
                        timeout=5
                    )
                    if result.returncode == 0:
                        results["success"] += 1
                    else:
                        results["failed"].append((md_file, line_num, result.stderr.decode()[:100]))
            except Exception as e:
                results["failed"].append((md_file, line_num, str(e)))
            finally:
                os.unlink(temp_path)
        
        return results
    
    def validate_zig_snippets(self) -> Dict:
        """验证Zig代码片段"""
        results = {"total": 0, "success": 0, "failed": []}
        
        if not self.zig_snippets:
            return results
        
        print_status("验证Zig代码片段语法...", "info")
        
        import tempfile
        import os
        
        for md_file, line_num, code in self.zig_snippets[:20]:  # 限制前20个
            results["total"] += 1
            
            with tempfile.NamedTemporaryFile(mode='w', suffix='.zig', delete=False) as f:
                f.write(code)
                temp_path = f.name
            
            try:
                result = subprocess.run(
                    ['zig', 'fmt', '--check', temp_path],
                    capture_output=True,
                    timeout=5
                )
                if result.returncode == 0:
                    results["success"] += 1
                else:
                    results["failed"].append((md_file, line_num, "Format error"))
            except Exception as e:
                results["failed"].append((md_file, line_num, str(e)))
            finally:
                try:
                    os.unlink(temp_path)
                except:
                    pass
        
        return results

class CodeValidator:
    """代码示例验证器"""
    
    def __init__(self):
        self.results = {
            "c_files": {"total": 0, "success": 0, "failed": []},
            "zig_files": {"total": 0, "success": 0, "failed": []},
            "c_snippets": {"total": 0, "success": 0, "failed": []},
            "zig_snippets": {"total": 0, "success": 0, "failed": []},
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
        
        # C代码文件报告
        c_total = self.results["c_files"]["total"]
        c_success = self.results["c_files"]["success"]
        c_rate = (c_success / c_total * 100) if c_total > 0 else 0
        
        print(f"独立C文件:")
        print(f"  总数: {c_total}, 成功: {c_success}, 成功率: {c_rate:.1f}%")
        
        # C代码片段报告
        cs_total = self.results["c_snippets"]["total"]
        cs_success = self.results["c_snippets"]["success"]
        cs_rate = (cs_success / cs_total * 100) if cs_total > 0 else 0
        
        print(f"\nMarkdown C代码片段 (抽查前50个):")
        print(f"  检查: {cs_total}, 成功: {cs_success}, 成功率: {cs_rate:.1f}%")
        if cs_rate >= 70:
            print_status(f"C代码片段验证通过 (>70%)", "success")
        else:
            print_status(f"C代码片段验证需关注 (<70%)", "warning" if cs_total > 0 else "info")
        
        if self.results["c_snippets"]["failed"]:
            print(f"  失败片段 ({len(self.results['c_snippets']['failed'])}):")
            for f, line, err in self.results["c_snippets"]["failed"][:3]:
                print(f"    - {f.name}:{line}")
        
        # Zig代码文件报告
        z_total = self.results["zig_files"]["total"]
        z_success = self.results["zig_files"]["success"]
        z_rate = (z_success / z_total * 100) if z_total > 0 else 0
        
        print(f"\n独立Zig文件:")
        print(f"  总数: {z_total}, 成功: {z_success}, 成功率: {z_rate:.1f}%")
        
        # Zig代码片段报告
        zs_total = self.results["zig_snippets"]["total"]
        zs_success = self.results["zig_snippets"]["success"]
        zs_rate = (zs_success / zs_total * 100) if zs_total > 0 else 0
        
        print(f"\nMarkdown Zig代码片段 (抽查前20个):")
        print(f"  检查: {zs_total}, 成功: {zs_success}, 成功率: {zs_rate:.1f}%")
        
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
    
    # 4. 提取并验证Markdown代码片段
    code_extractor = MarkdownCodeExtractor()
    code_extractor.extract_code_blocks()
    validator.results["c_snippets"] = code_extractor.validate_c_snippets()
    validator.results["zig_snippets"] = code_extractor.validate_zig_snippets()
    validator.print_report()
    
    # 5. 生成报告
    generate_report(stats, link_checker, validator)
    
    print("="*60)
    print("✨ 维护检查完成")
    print("="*60)

if __name__ == "__main__":
    main()
