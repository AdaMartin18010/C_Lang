#!/usr/bin/env python3
"""
C_Lang 知识库链接检查脚本
用于检测和报告Markdown文件中的断链问题

使用方法:
    python scripts/check_links.py [knowledge_directory]
"""

import os
import re
import sys
from pathlib import Path
from collections import defaultdict

# 断链模式检查规则
BROKEN_PATTERNS = {
    'path_abbreviation': [
        ('01_Core/', '01_Core_Knowledge_System/'),
        ('02_Formal/', '02_Formal_Semantics_and_Physics/'),
        ('03_System/', '03_System_Technology_Domains/'),
        ('04_Industrial/', '04_Industrial_Scenarios/'),
        ('05_Standards/', '05_C_Language_Standards/'),
        ('06_Thinking/', '06_Thinking_Representation/'),
    ],
    'case_issue': [
        '/README.md',
    ],
}

# Markdown链接正则
LINK_PATTERN = re.compile(r'\[([^\]]+)\]\(([^)]+)\)')


def check_file(filepath, base_dir):
    """检查单个文件的链接问题"""
    issues = []
    
    try:
        with open(filepath, 'r', encoding='utf-8') as f:
            content = f.read()
            lines = content.split('\n')
    except Exception as e:
        return [(f"无法读取文件: {e}", 0, '')]
    
    for line_num, line in enumerate(lines, 1):
        # 检查路径缩写问题
        for wrong, correct in BROKEN_PATTERNS['path_abbreviation']:
            if wrong in line and correct not in line:
                issues.append((f"路径缩写: {wrong}", line_num, line.strip()))
        
        # 检查大小写问题
        if '/README.md' in line and '/readme.md' not in line.lower():
            issues.append(("大小写问题: README.md", line_num, line.strip()))
        
        # 检查链接目标是否存在
        for match in LINK_PATTERN.finditer(line):
            link_text = match.group(1)
            link_target = match.group(2)
            
            # 跳过外部链接和锚点
            if link_target.startswith(('http://', 'https://', 'mailto:', '#')):
                continue
            
            # 解析相对路径
            if link_target.startswith('/'):
                target_path = os.path.join(base_dir, link_target.lstrip('/'))
            else:
                target_path = os.path.join(os.path.dirname(filepath), link_target)
            
            target_path = os.path.normpath(target_path)
            
            # 检查文件是否存在
            if not os.path.exists(target_path):
                # 尝试其他变体
                variants = [
                    target_path,
                    target_path.replace('README.md', 'readme.md'),
                    target_path.replace('readme.md', 'README.md'),
                ]
                if not any(os.path.exists(v) for v in variants):
                    issues.append((f"断链: [{link_text}]({link_target})", line_num, line.strip()))
    
    return issues


def main():
    """主函数"""
    if len(sys.argv) > 1:
        knowledge_dir = sys.argv[1]
    else:
        knowledge_dir = 'knowledge'
    
    if not os.path.exists(knowledge_dir):
        print(f"错误: 目录不存在: {knowledge_dir}")
        sys.exit(1)
    
    print(f"正在检查目录: {knowledge_dir}")
    print("=" * 70)
    
    all_issues = defaultdict(list)
    file_count = 0
    
    for root, dirs, files in os.walk(knowledge_dir):
        for file in files:
            if file.endswith('.md'):
                file_path = os.path.join(root, file)
                file_count += 1
                
                issues = check_file(file_path, knowledge_dir)
                if issues:
                    rel_path = os.path.relpath(file_path, knowledge_dir)
                    all_issues[rel_path] = issues
    
    # 输出结果
    if not all_issues:
        print(f"✓ 恭喜！检查了 {file_count} 个文件，未发现断链问题。")
        return
    
    print(f"检查了 {file_count} 个文件，发现 {len(all_issues)} 个文件存在问题:\n")
    
    total_issues = 0
    for filepath, issues in sorted(all_issues.items()):
        print(f"\n{filepath}")
        print("-" * 70)
        for issue_type, line_num, line_content in issues[:10]:  # 每文件最多显示10个
            print(f"  行 {line_num}: {issue_type}")
            if len(line_content) > 80:
                line_content = line_content[:77] + "..."
            print(f"    {line_content}")
            total_issues += 1
        
        if len(issues) > 10:
            print(f"  ... 还有 {len(issues) - 10} 个问题")
            total_issues += len(issues) - 10
    
    print("\n" + "=" * 70)
    print(f"总计: {total_issues} 个问题需要修复")
    print(f"\n建议运行修复脚本: python scripts/fix_links.py")


if __name__ == '__main__':
    main()
