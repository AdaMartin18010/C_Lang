#!/usr/bin/env python3
"""
C_Lang 知识库链接修复脚本
用于自动修复常见的断链问题

使用方法:
    python scripts/fix_links.py [knowledge_directory]
"""

import os
import sys

# 修复规则
FIX_RULES = [
    # 路径缩写修复
    ('01_Core/', '01_Core_Knowledge_System/'),
    ('02_Formal/', '02_Formal_Semantics_and_Physics/'),
    ('03_System/', '03_System_Technology_Domains/'),
    ('04_Industrial/', '04_Industrial_Scenarios/'),
    ('05_Standards/', '05_C_Language_Standards/'),
    ('06_Thinking/', '06_Thinking_Representation/'),
    
    # 相对路径修复
    ('./01_Core/', './01_Core_Knowledge_System/'),
    ('./02_Formal/', './02_Formal_Semantics_and_Physics/'),
    ('./03_System/', './03_System_Technology_Domains/'),
    ('./04_Industrial/', './04_Industrial_Scenarios/'),
    ('./06_Thinking/', './06_Thinking_Representation/'),
    
    ('../01_Core/', '../01_Core_Knowledge_System/'),
    ('../02_Formal/', '../02_Formal_Semantics_and_Physics/'),
    ('../03_System/', '../03_System_Technology_Domains/'),
    ('../04_Industrial/', '../04_Industrial_Scenarios/'),
    ('../06_Thinking/', '../06_Thinking_Representation/'),
    
    ('../../01_Core/', '../../01_Core_Knowledge_System/'),
    ('../../03_System/', '../../03_System_Technology_Domains/'),
    ('../../04_Industrial/', '../../04_Industrial_Scenarios/'),
    ('../../06_Thinking/', '../../06_Thinking_Representation/'),
    
    # 大小写修复
    ('/README.md)', '/readme.md)'),
    ('/README.md]', '/readme.md]'),
    ('/README.md#', '/readme.md#'),
    ('/README.md ', '/readme.md '),
    
    # Zig目录修复
    ('Zig/', 'zig/'),
    ('./Zig/', './zig/'),
    ('../Zig/', '../zig/'),
]


def fix_file(filepath):
    """修复单个文件"""
    try:
        with open(filepath, 'r', encoding='utf-8') as f:
            content = f.read()
        
        original_content = content
        fixes = 0
        
        for old, new in FIX_RULES:
            if old in content:
                count = content.count(old)
                content = content.replace(old, new)
                fixes += count
        
        if fixes > 0:
            with open(filepath, 'w', encoding='utf-8') as f:
                f.write(content)
            return fixes
        
        return 0
    except Exception as e:
        print(f"  错误: {e}")
        return -1


def main():
    """主函数"""
    if len(sys.argv) > 1:
        knowledge_dir = sys.argv[1]
    else:
        knowledge_dir = 'knowledge'
    
    if not os.path.exists(knowledge_dir):
        print(f"错误: 目录不存在: {knowledge_dir}")
        sys.exit(1)
    
    print(f"正在修复目录: {knowledge_dir}")
    print("=" * 70)
    
    md_files = []
    for root, dirs, files in os.walk(knowledge_dir):
        for file in files:
            if file.endswith('.md'):
                md_files.append(os.path.join(root, file))
    
    print(f"找到 {len(md_files)} 个Markdown文件\n")
    
    total_fixes = 0
    fixed_files = 0
    
    for filepath in md_files:
        fixes = fix_file(filepath)
        if fixes > 0:
            rel_path = os.path.relpath(filepath, knowledge_dir)
            print(f"已修复 {rel_path}: {fixes} 处")
            total_fixes += fixes
            fixed_files += 1
    
    print("\n" + "=" * 70)
    print(f"修复完成: {fixed_files} 个文件，共 {total_fixes} 处断链")


if __name__ == '__main__':
    main()
