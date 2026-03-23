#!/usr/bin/env python3
"""
Markdown链接检查工具
检查知识库中所有Markdown文件的链接有效性
"""

import os
import re
import sys
from pathlib import Path
from collections import defaultdict
from dataclasses import dataclass
from typing import List, Dict, Tuple, Set


@dataclass
class LinkIssue:
    """链接问题"""
    file_path: str
    line_number: int
    link_text: str
    link_target: str
    issue_type: str  # 'broken', 'missing_anchor', 'external', 'empty'


class LinkChecker:
    """Markdown链接检查器"""
    
    def __init__(self, knowledge_dir: str = "knowledge"):
        self.knowledge_dir = Path(knowledge_dir)
        self.issues: List[LinkIssue] = []
        self.stats = {
            "total_files": 0,
            "total_links": 0,
            "broken_links": 0,
            "external_links": 0,
            "internal_links": 0,
            "anchor_links": 0,
        }
        self.all_files: Set[str] = set()
        
    def scan_all_files(self):
        """扫描所有Markdown文件"""
        print("🔍 扫描所有Markdown文件...")
        for md_file in self.knowledge_dir.rglob("*.md"):
            self.all_files.add(str(md_file.relative_to(self.knowledge_dir)))
        self.stats["total_files"] = len(self.all_files)
        print(f"   找到 {self.stats['total_files']} 个Markdown文件")
        
    def extract_links(self, md_file: Path) -> List[Tuple[int, str, str]]:
        """从Markdown文件提取链接"""
        links = []
        content = md_file.read_text(encoding='utf-8')
        
        # 匹配Markdown链接: [text](target)
        pattern = r'\[([^\]]+)\]\(([^)]+)\)'
        
        for match in re.finditer(pattern, content):
            line_num = content[:match.start()].count('\n') + 1
            link_text = match.group(1)
            link_target = match.group(2)
            links.append((line_num, link_text, link_target))
            
        return links
        
    def check_link(self, source_file: Path, line_num: int, 
                   link_text: str, link_target: str):
        """检查单个链接"""
        self.stats["total_links"] += 1
        
        # 忽略URL链接
        if link_target.startswith(('http://', 'https://', 'mailto:')):
            self.stats["external_links"] += 1
            return
            
        # 处理锚点链接
        if '#' in link_target:
            self.stats["anchor_links"] += 1
            target_file = link_target.split('#')[0]
            anchor = link_target.split('#')[1]
        else:
            target_file = link_target
            anchor = None
            
        # 空链接
        if not target_file:
            self.stats["broken_links"] += 1
            self.issues.append(LinkIssue(
                str(source_file.relative_to(self.knowledge_dir)),
                line_num, link_text, link_target, 'empty'
            ))
            return
            
        self.stats["internal_links"] += 1
        
        # 解析相对路径
        source_dir = source_file.parent
        
        if target_file.startswith('/'):
            # 绝对路径（相对于knowledge目录）
            full_target = self.knowledge_dir / target_file[1:]
        else:
            # 相对路径
            full_target = source_dir / target_file
            
        # 规范化路径
        try:
            full_target = full_target.resolve()
            relative_target = str(full_target.relative_to(self.knowledge_dir.resolve()))
        except (ValueError, OSError):
            # 路径无法解析或超出knowledge目录
            self.stats["broken_links"] += 1
            self.issues.append(LinkIssue(
                str(source_file.relative_to(self.knowledge_dir)),
                line_num, link_text, link_target, 'broken'
            ))
            return
            
        # 检查文件是否存在
        if not full_target.exists():
            self.stats["broken_links"] += 1
            self.issues.append(LinkIssue(
                str(source_file.relative_to(self.knowledge_dir)),
                line_num, link_text, link_target, 'broken'
            ))
            return
            
        # 检查锚点是否存在
        if anchor:
            target_content = full_target.read_text(encoding='utf-8')
            # 检查标题锚点
            anchor_patterns = [
                f'# {link_text}',  # 精确匹配标题
                f'## {link_text}',
                f'<a name="{anchor}">',
                f'<a id="{anchor}">',
            ]
            # 锚点格式: #section-name
            anchor_id = anchor.lower().replace(' ', '-')
            if not re.search(f'^[#]+ .*{re.escape(anchor_id)}', target_content, re.MULTILINE | re.IGNORECASE):
                # 锚点可能不存在，但暂不报告为错误（因为Markdown渲染器有多种锚点生成方式）
                pass
                
    def check_all_files(self):
        """检查所有文件的链接"""
        print("\n🔍 检查所有链接...")
        
        md_files = list(self.knowledge_dir.rglob("*.md"))
        for i, md_file in enumerate(md_files, 1):
            if i % 100 == 0:
                print(f"   已检查 {i}/{len(md_files)} 个文件...")
                
            links = self.extract_links(md_file)
            for line_num, link_text, link_target in links:
                self.check_link(md_file, line_num, link_text, link_target)
                
    def print_report(self):
        """打印检查报告"""
        print("\n" + "=" * 70)
        print("📊 Markdown链接检查报告")
        print("=" * 70)
        
        print(f"\n📁 文件统计:")
        print(f"   总Markdown文件: {self.stats['total_files']}")
        
        print(f"\n🔗 链接统计:")
        print(f"   总链接数: {self.stats['total_links']}")
        print(f"   内部链接: {self.stats['internal_links']}")
        print(f"   外部链接: {self.stats['external_links']}")
        print(f"   锚点链接: {self.stats['anchor_links']}")
        print(f"   失效链接: {self.stats['broken_links']}")
        
        if self.stats['total_links'] > 0:
            health_rate = (self.stats['total_links'] - self.stats['broken_links']) / self.stats['total_links'] * 100
            print(f"\n💚 链接健康度: {health_rate:.1f}%")
            
        if self.issues:
            print(f"\n❌ 发现 {len(self.issues)} 个问题链接:")
            # 按文件分组
            issues_by_file = defaultdict(list)
            for issue in self.issues:
                issues_by_file[issue.file_path].append(issue)
                
            for file_path, issues in sorted(issues_by_file.items()):
                print(f"\n   📄 {file_path}")
                for issue in issues[:3]:  # 每个文件最多显示3个
                    print(f"      行{issue.line_number}: [{issue.link_text}]({issue.link_target})")
                    print(f"      问题: {self._issue_type_desc(issue.issue_type)}")
                if len(issues) > 3:
                    print(f"      ... 还有 {len(issues) - 3} 个")
        else:
            print("\n✅ 没有发现链接问题！")
            
        print("\n" + "=" * 70)
        
    def _issue_type_desc(self, issue_type: str) -> str:
        """获取问题类型描述"""
        descriptions = {
            'broken': '目标文件不存在',
            'missing_anchor': '锚点不存在',
            'external': '外部链接(未检查)',
            'empty': '空链接',
        }
        return descriptions.get(issue_type, issue_type)
        
    def export_issues(self, output_file: str = "link_issues.txt"):
        """导出问题列表"""
        with open(output_file, 'w', encoding='utf-8') as f:
            f.write("# Markdown链接问题列表\n\n")
            for issue in self.issues:
                f.write(f"- [{issue.issue_type}] {issue.file_path}:{issue.line_number}\n")
                f.write(f"  链接: [{issue.link_text}]({issue.link_target})\n\n")
        print(f"\n📝 问题列表已导出到: {output_file}")


def main():
    checker = LinkChecker()
    
    print("🔧 Markdown链接检查工具")
    print("=" * 70)
    
    # 扫描所有文件
    checker.scan_all_files()
    
    # 检查所有链接
    checker.check_all_files()
    
    # 打印报告
    checker.print_report()
    
    # 导出问题
    if checker.issues:
        checker.export_issues()
        
    # 返回非零退出码如果有问题
    sys.exit(1 if checker.issues else 0)


if __name__ == "__main__":
    main()
