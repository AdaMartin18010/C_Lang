#!/usr/bin/env python3
"""
贡献者统计脚本

功能：
- 统计项目贡献者数据
- 生成贡献报告
- 统计指标：提交数、文档更新、代码示例等
- 支持多种输出格式

用法：
    python contributor_stats.py [--format {text,json,markdown}] [--output FILE]
    python contributor_stats.py --since 2026-01-01 --until 2026-03-31
    python contributor_stats.py --hall-of-fame

作者: C_Lang 社区
更新: 2026-03-19
"""

import argparse
import json
import os
import re
import subprocess
import sys
from collections import defaultdict
from datetime import datetime, timedelta
from pathlib import Path
from typing import Dict, List, Optional, Tuple


class ContributorStats:
    """贡献者统计类"""
    
    def __init__(self, repo_path: str = "."):
        self.repo_path = Path(repo_path).resolve()
        self.stats = defaultdict(lambda: {
            'commits': 0,
            'files_changed': 0,
            'insertions': 0,
            'deletions': 0,
            'prs': {'opened': 0, 'merged': 0, 'closed': 0},
            'issues': {'opened': 0, 'closed': 0},
            'code_examples': 0,
            'docs': 0,
            'reviews': 0
        })
        
    def run_git_command(self, cmd: List[str]) -> str:
        """运行 Git 命令并返回输出"""
        try:
            result = subprocess.run(
                ['git'] + cmd,
                cwd=self.repo_path,
                capture_output=True,
                text=True,
                encoding='utf-8'
            )
            if result.returncode != 0:
                print(f"Git 命令错误: {result.stderr}", file=sys.stderr)
                return ""
            return result.stdout
        except Exception as e:
            print(f"运行 Git 命令失败: {e}", file=sys.stderr)
            return ""
    
    def get_commit_stats(self, since: Optional[str] = None, 
                         until: Optional[str] = None) -> Dict:
        """获取提交统计"""
        cmd = ['log', '--pretty=format:%H|%an|%ae|%ad|%s', '--date=short', '--numstat']
        
        if since:
            cmd.extend(['--since', since])
        if until:
            cmd.extend(['--until', until])
        
        output = self.run_git_command(cmd)
        if not output:
            return {}
        
        current_commit = None
        current_author = None
        
        for line in output.split('\n'):
            if '|' in line and not line.startswith(' '):
                # 提交信息行
                parts = line.split('|')
                if len(parts) >= 5:
                    commit_hash = parts[0]
                    author = parts[1]
                    email = parts[2]
                    date = parts[3]
                    subject = parts[4]
                    
                    current_commit = commit_hash
                    current_author = author
                    self.stats[author]['commits'] += 1
                    
                    # 分析提交类型
                    if any(kw in subject.lower() for kw in ['doc', 'doc:', 'docs', 'docs:', '📚']):
                        self.stats[author]['docs'] += 1
                    if any(kw in subject.lower() for kw in ['feat', 'feature', 'add', '新']):
                        if 'example' in subject.lower() or '代码' in subject:
                            self.stats[author]['code_examples'] += 1
            
            elif line.strip() and current_author:
                # 文件统计行
                parts = line.split('\t')
                if len(parts) >= 3:
                    insertions = parts[0] if parts[0] != '-' else 0
                    deletions = parts[1] if parts[1] != '-' else 0
                    filename = parts[2]
                    
                    try:
                        self.stats[current_author]['insertions'] += int(insertions)
                        self.stats[current_author]['deletions'] += int(deletions)
                        self.stats[current_author]['files_changed'] += 1
                    except ValueError:
                        pass
        
        return dict(self.stats)
    
    def get_file_type_stats(self) -> Dict[str, Dict[str, int]]:
        """按文件类型统计贡献"""
        file_stats = defaultdict(lambda: {'authors': set(), 'commits': 0})
        
        cmd = ['log', '--pretty=format:%H %an', '--name-only']
        output = self.run_git_command(cmd)
        
        current_author = None
        for line in output.split('\n'):
            if ' ' in line and not line.startswith(' '):
                parts = line.split(' ', 1)
                if len(parts) == 2:
                    current_author = parts[1]
            elif line.strip() and current_author:
                ext = Path(line).suffix.lower()
                if ext:
                    file_stats[ext]['authors'].add(current_author)
                    file_stats[ext]['commits'] += 1
        
        # 转换 set 为 count
        return {k: {'authors': len(v['authors']), 'commits': v['commits']} 
                for k, v in file_stats.items()}
    
    def get_module_stats(self) -> Dict[str, Dict[str, int]]:
        """按知识模块统计贡献"""
        module_stats = defaultdict(lambda: {'files': 0, 'authors': set()})
        
        knowledge_path = self.repo_path / 'knowledge'
        if not knowledge_path.exists():
            return {}
        
        # 统计各模块文件
        for module_dir in knowledge_path.iterdir():
            if module_dir.is_dir() and module_dir.name.startswith(('0', '1', '2')):
                module_name = module_dir.name
                md_files = list(module_dir.rglob('*.md'))
                module_stats[module_name]['files'] = len(md_files)
                
                # 获取每个文件的作者
                for md_file in md_files:
                    rel_path = md_file.relative_to(self.repo_path).as_posix()
                    cmd = ['log', '--follow', '--pretty=format:%an', '--', rel_path]
                    output = self.run_git_command(cmd)
                    for author in output.strip().split('\n'):
                        if author:
                            module_stats[module_name]['authors'].add(author)
        
        return {k: {'files': v['files'], 'authors': len(v['authors'])} 
                for k, v in module_stats.items()}
    
    def get_code_example_stats(self) -> Dict[str, int]:
        """统计代码示例"""
        stats = defaultdict(int)
        
        knowledge_path = self.repo_path / 'knowledge'
        if not knowledge_path.exists():
            return {}
        
        for md_file in knowledge_path.rglob('*.md'):
            try:
                content = md_file.read_text(encoding='utf-8')
                
                # 统计 C 代码块
                c_blocks = len(re.findall(r'```c\n', content))
                if c_blocks > 0:
                    # 获取最后修改者
                    rel_path = md_file.relative_to(self.repo_path).as_posix()
                    cmd = ['log', '-1', '--pretty=format:%an', '--', rel_path]
                    author = self.run_git_command(cmd).strip()
                    if author:
                        stats[author] += c_blocks
            except Exception:
                continue
        
        return dict(stats)
    
    def generate_report(self, since: Optional[str] = None,
                       until: Optional[str] = None,
                       format_type: str = 'text') -> str:
        """生成贡献报告"""
        # 获取所有统计数据
        self.get_commit_stats(since, until)
        file_type_stats = self.get_file_type_stats()
        module_stats = self.get_module_stats()
        code_stats = self.get_code_example_stats()
        
        # 合并代码示例统计
        for author, count in code_stats.items():
            if author in self.stats:
                self.stats[author]['code_examples'] = count
        
        if format_type == 'json':
            return self._generate_json_report(since, until)
        elif format_type == 'markdown':
            return self._generate_markdown_report(since, until, file_type_stats, module_stats)
        else:
            return self._generate_text_report(since, until, file_type_stats, module_stats)
    
    def _generate_text_report(self, since: Optional[str], until: Optional[str],
                              file_type_stats: Dict, module_stats: Dict) -> str:
        """生成文本格式报告"""
        lines = []
        lines.append("=" * 70)
        lines.append("C_Lang 知识库 - 贡献者统计报告")
        lines.append("=" * 70)
        lines.append(f"生成时间: {datetime.now().strftime('%Y-%m-%d %H:%M:%S')}")
        if since:
            lines.append(f"统计起始: {since}")
        if until:
            lines.append(f"统计截止: {until}")
        lines.append("")
        
        # 总体统计
        total_commits = sum(s['commits'] for s in self.stats.values())
        total_insertions = sum(s['insertions'] for s in self.stats.values())
        total_deletions = sum(s['deletions'] for s in self.stats.values())
        total_files = sum(s['files_changed'] for s in self.stats.values())
        
        lines.append("-" * 70)
        lines.append("总体统计")
        lines.append("-" * 70)
        lines.append(f"贡献者总数: {len(self.stats)}")
        lines.append(f"总提交数: {total_commits}")
        lines.append(f"总增行数: {total_insertions}")
        lines.append(f"总删行数: {total_deletions}")
        lines.append(f"修改文件数: {total_files}")
        lines.append("")
        
        # 贡献者排名
        lines.append("-" * 70)
        lines.append("贡献者排名 (按提交数)")
        lines.append("-" * 70)
        sorted_contributors = sorted(self.stats.items(), 
                                     key=lambda x: x[1]['commits'], 
                                     reverse=True)
        
        lines.append(f"{'排名':<6}{'贡献者':<20}{'提交':<10}{'增行':<10}{'删行':<10}{'文件':<10}")
        lines.append("-" * 70)
        for i, (author, stats) in enumerate(sorted_contributors[:20], 1):
            lines.append(f"{i:<6}{author:<20}{stats['commits']:<10}"
                        f"{stats['insertions']:<10}{stats['deletions']:<10}"
                        f"{stats['files_changed']:<10}")
        lines.append("")
        
        # 文件类型统计
        lines.append("-" * 70)
        lines.append("文件类型统计")
        lines.append("-" * 70)
        sorted_types = sorted(file_type_stats.items(), 
                             key=lambda x: x[1]['commits'], 
                             reverse=True)
        lines.append(f"{'类型':<15}{'作者数':<10}{'提交数':<10}")
        lines.append("-" * 70)
        for ext, stats in sorted_types[:10]:
            lines.append(f"{ext:<15}{stats['authors']:<10}{stats['commits']:<10}")
        lines.append("")
        
        # 模块统计
        lines.append("-" * 70)
        lines.append("知识模块统计")
        lines.append("-" * 70)
        sorted_modules = sorted(module_stats.items(), 
                               key=lambda x: x[1]['files'], 
                               reverse=True)
        lines.append(f"{'模块':<30}{'文件数':<10}{'贡献者数':<10}")
        lines.append("-" * 70)
        for module, stats in sorted_modules:
            module_short = module[:28]
            lines.append(f"{module_short:<30}{stats['files']:<10}{stats['authors']:<10}")
        lines.append("")
        
        lines.append("=" * 70)
        lines.append("报告结束")
        lines.append("=" * 70)
        
        return '\n'.join(lines)
    
    def _generate_markdown_report(self, since: Optional[str], until: Optional[str],
                                  file_type_stats: Dict, module_stats: Dict) -> str:
        """生成 Markdown 格式报告"""
        lines = []
        lines.append("# 贡献者统计报告")
        lines.append("")
        lines.append(f"> 生成时间: {datetime.now().strftime('%Y-%m-%d %H:%M:%S')}")
        if since:
            lines.append(f"> 统计起始: {since}")
        if until:
            lines.append(f"> 统计截止: {until}")
        lines.append("")
        
        # 总体统计
        total_commits = sum(s['commits'] for s in self.stats.values())
        total_insertions = sum(s['insertions'] for s in self.stats.values())
        
        lines.append("## 总体统计")
        lines.append("")
        lines.append("| 指标 | 数值 |")
        lines.append("|:-----|-----:|")
        lines.append(f"| 贡献者总数 | {len(self.stats)} |")
        lines.append(f"| 总提交数 | {total_commits} |")
        lines.append(f"| 总增行数 | {total_insertions} |")
        lines.append("")
        
        # 贡献者排名
        lines.append("## 贡献者排名")
        lines.append("")
        sorted_contributors = sorted(self.stats.items(), 
                                     key=lambda x: x[1]['commits'], 
                                     reverse=True)
        
        lines.append("| 排名 | 贡献者 | 提交 | 增行 | 删行 | 文件 |")
        lines.append("|:---:|:---|:---:|:---:|:---:|:---:|")
        for i, (author, stats) in enumerate(sorted_contributors[:20], 1):
            lines.append(f"| {i} | {author} | {stats['commits']} | "
                        f"{stats['insertions']} | {stats['deletions']} | "
                        f"{stats['files_changed']} |")
        lines.append("")
        
        return '\n'.join(lines)
    
    def _generate_json_report(self, since: Optional[str], until: Optional[str]) -> str:
        """生成 JSON 格式报告"""
        report = {
            'generated_at': datetime.now().isoformat(),
            'period': {
                'since': since,
                'until': until or datetime.now().strftime('%Y-%m-%d')
            },
            'summary': {
                'total_contributors': len(self.stats),
                'total_commits': sum(s['commits'] for s in self.stats.values()),
                'total_insertions': sum(s['insertions'] for s in self.stats.values()),
                'total_deletions': sum(s['deletions'] for s in self.stats.values()),
            },
            'contributors': [
                {
                    'name': author,
                    'commits': stats['commits'],
                    'files_changed': stats['files_changed'],
                    'insertions': stats['insertions'],
                    'deletions': stats['deletions'],
                    'docs': stats['docs'],
                    'code_examples': stats['code_examples']
                }
                for author, stats in sorted(
                    self.stats.items(),
                    key=lambda x: x[1]['commits'],
                    reverse=True
                )
            ]
        }
        return json.dumps(report, indent=2, ensure_ascii=False)
    
    def generate_hall_of_fame(self) -> str:
        """生成荣誉榜数据"""
        self.get_commit_stats()
        code_stats = self.get_code_example_stats()
        
        for author, count in code_stats.items():
            if author in self.stats:
                self.stats[author]['code_examples'] = count
        
        lines = []
        lines.append("# 荣誉榜数据")
        lines.append("")
        lines.append(f"更新时间: {datetime.now().strftime('%Y-%m-%d')}")
        lines.append("")
        
        # 按不同维度排序
        by_commits = sorted(self.stats.items(), key=lambda x: x[1]['commits'], reverse=True)
        by_code = sorted(self.stats.items(), key=lambda x: x[1]['code_examples'], reverse=True)
        by_docs = sorted(self.stats.items(), key=lambda x: x[1]['docs'], reverse=True)
        
        lines.append("## 总提交榜 Top 10")
        lines.append("")
        for i, (author, stats) in enumerate(by_commits[:10], 1):
            lines.append(f"{i}. **{author}** - {stats['commits']} 次提交")
        lines.append("")
        
        lines.append("## 代码示例榜 Top 10")
        lines.append("")
        for i, (author, stats) in enumerate(by_code[:10], 1):
            lines.append(f"{i}. **{author}** - {stats['code_examples']} 个代码块")
        lines.append("")
        
        lines.append("## 文档贡献榜 Top 10")
        lines.append("")
        for i, (author, stats) in enumerate(by_docs[:10], 1):
            lines.append(f"{i}. **{author}** - {stats['docs']} 次文档提交")
        lines.append("")
        
        return '\n'.join(lines)


def main():
    parser = argparse.ArgumentParser(
        description='C_Lang 知识库贡献者统计工具',
        formatter_class=argparse.RawDescriptionHelpFormatter,
        epilog="""
示例:
  python contributor_stats.py                    # 生成默认文本报告
  python contributor_stats.py --format json      # 生成 JSON 报告
  python contributor_stats.py --format markdown --output report.md
  python contributor_stats.py --since 2026-01-01 --until 2026-03-31
  python contributor_stats.py --hall-of-fame     # 生成荣誉榜数据
        """
    )
    
    parser.add_argument('--format', choices=['text', 'json', 'markdown'],
                       default='text', help='输出格式 (默认: text)')
    parser.add_argument('--output', '-o', type=str,
                       help='输出文件路径 (默认: 输出到 stdout)')
    parser.add_argument('--since', type=str,
                       help='统计起始日期 (格式: YYYY-MM-DD)')
    parser.add_argument('--until', type=str,
                       help='统计截止日期 (格式: YYYY-MM-DD)')
    parser.add_argument('--hall-of-fame', action='store_true',
                       help='生成荣誉榜数据')
    parser.add_argument('--repo', type=str, default='.',
                       help='仓库路径 (默认: 当前目录)')
    
    args = parser.parse_args()
    
    # 检查是否在 git 仓库中
    repo_path = Path(args.repo).resolve()
    git_dir = repo_path / '.git'
    if not git_dir.exists():
        print(f"错误: {repo_path} 不是一个 git 仓库", file=sys.stderr)
        sys.exit(1)
    
    stats = ContributorStats(args.repo)
    
    if args.hall_of_fame:
        report = stats.generate_hall_of_fame()
    else:
        report = stats.generate_report(args.since, args.until, args.format)
    
    if args.output:
        with open(args.output, 'w', encoding='utf-8') as f:
            f.write(report)
        print(f"报告已保存到: {args.output}")
    else:
        print(report)


if __name__ == '__main__':
    main()
