#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Markdown 目录生成工具
为知识库中的 Markdown 文件添加 Table of Contents
"""

import os
import re
import sys
from pathlib import Path

def extract_headers(content):
    """提取 Markdown 标题，返回 (级别, 标题文本, 锚点) 的列表"""
    headers = []
    # 跳过 YAML frontmatter
    if content.startswith('---'):
        end = content.find('---', 3)
        if end != -1:
            content = content[end+3:]
    
    # 匹配标题行
    pattern = r'^(#{1,6})\s+(.+)$'
    for match in re.finditer(pattern, content, re.MULTILINE):
        level = len(match.group(1))
        title = match.group(2).strip()
        # 生成锚点
        anchor = generate_anchor(title)
        headers.append((level, title, anchor))
    return headers

def generate_anchor(title):
    """根据标题生成 GitHub 风格的锚点"""
    # 移除代码标记
    anchor = re.sub(r'`([^`]+)`', r'\1', title)
    # 移除链接
    anchor = re.sub(r'\[([^\]]+)\]\([^\)]+\)', r'\1', anchor)
    # 移除特殊字符，保留中文、字母、数字、空格和连字符
    anchor = re.sub(r"[^\u4e00-\u9fa5a-zA-Z0-9\s\-]", '', anchor)
    # 替换空格为连字符
    anchor = re.sub(r'\s+', '-', anchor.strip())
    # 转换为小写（仅对英文部分）
    anchor = anchor.lower()
    # 移除连续的连字符
    anchor = re.sub(r'-+', '-', anchor)
    anchor = anchor.strip('-')
    return anchor if anchor else 'heading'

def generate_toc(headers, min_level=2, max_level=4):
    """生成目录 Markdown 文本"""
    if not headers:
        return ""
    
    # 过滤级别
    filtered = [(l, t, a) for l, t, a in headers if min_level <= l <= max_level]
    if not filtered:
        return ""
    
    # 计算缩进基数
    min_in_filtered = min(l for l, _, _ in filtered)
    
    toc_lines = ["## 📑 目录", ""]
    
    for level, title, anchor in filtered:
        indent = "  " * (level - min_in_filtered)
        toc_lines.append(f"{indent}- [{title}](#{anchor})")
    
    toc_lines.append("")
    return "\n".join(toc_lines)

def find_insert_position(content):
    """找到插入目录的最佳位置"""
    lines = content.split('\n')
    
    # 寻找第一个二级标题或概要表格之后的位置
    in_frontmatter = False
    frontmatter_end = 0
    found_first_heading = False
    
    for i, line in enumerate(lines):
        # 跳过 frontmatter
        if line.strip() == '---':
            if not in_frontmatter:
                in_frontmatter = True
            else:
                in_frontmatter = False
                frontmatter_end = i + 1
            continue
        
        if in_frontmatter:
            continue
            
        # 检查是否是二级标题
        if line.startswith('## ') and not found_first_heading:
            # 如果找到二级标题，在它之前插入
            if '概要' in line or '目录' in line:
                # 跳过概要部分，找到下一个标题
                continue
            return i
    
    # 默认在第一个空行后或 frontmatter 后插入
    for i, line in enumerate(lines[frontmatter_end:], frontmatter_end):
        if line.strip() == '' and i > frontmatter_end:
            return i + 1
    
    return frontmatter_end + 2

def add_toc_to_file(filepath, dry_run=False, force=False):
    """为单个文件添加目录"""
    try:
        with open(filepath, 'r', encoding='utf-8') as f:
            content = f.read()
    except Exception as e:
        print(f"❌ 读取失败: {filepath} - {e}")
        return False
    
    # 检查是否已有目录
    if not force and ('## 📑 目录' in content or '## 目录' in content or '## Table of Contents' in content):
        return False  # 跳过已有目录的文件
    
    headers = extract_headers(content)
    if len(headers) < 5:  # 标题太少，不需要目录
        return False
    
    toc = generate_toc(headers)
    if not toc:
        return False
    
    # 找到插入位置
    lines = content.split('\n')
    insert_pos = find_insert_position(content)
    
    # 插入目录
    new_lines = lines[:insert_pos] + ['', '---', ''] + toc.split('\n') + ['', '---', ''] + lines[insert_pos:]
    new_content = '\n'.join(new_lines)
    
    if not dry_run:
        # 备份原文件
        backup_path = str(filepath) + '.bak'
        with open(backup_path, 'w', encoding='utf-8') as f:
            f.write(content)
        
        with open(filepath, 'w', encoding='utf-8') as f:
            f.write(new_content)
    
    return True

def process_directory(directory, dry_run=False, force=False, min_size_kb=10):
    """处理整个目录"""
    count = 0
    skipped = 0
    errors = 0
    
    for root, dirs, files in os.walk(directory):
        # 跳过隐藏目录和特定目录
        dirs[:] = [d for d in dirs if not d.startswith('.') and d not in ['__pycache__', 'node_modules']]
        
        for file in files:
            if not file.endswith('.md'):
                continue
                
            filepath = os.path.join(root, file)
            
            # 检查文件大小
            size_kb = os.path.getsize(filepath) / 1024
            if size_kb < min_size_kb:
                skipped += 1
                continue
            
            try:
                if add_toc_to_file(filepath, dry_run, force):
                    action = "[预览]" if dry_run else "✅ 已添加"
                    print(f"{action}: {filepath} ({size_kb:.1f} KB)")
                    count += 1
            except Exception as e:
                print(f"❌ 错误: {filepath} - {e}")
                errors += 1
    
    return count, skipped, errors

def main():
    """主函数"""
    import argparse
    
    parser = argparse.ArgumentParser(description='为 Markdown 文件添加目录')
    parser.add_argument('path', nargs='?', default='knowledge', help='目标路径 (默认: knowledge)')
    parser.add_argument('--dry-run', '-n', action='store_true', help='预览模式，不实际修改文件')
    parser.add_argument('--force', '-f', action='store_true', help='强制更新已有目录的文件')
    parser.add_argument('--min-size', '-s', type=int, default=10, help='最小文件大小(KB)，小于此值的文件将被跳过')
    parser.add_argument('--restore', '-r', action='store_true', help='从备份恢复文件')
    
    args = parser.parse_args()
    
    if args.restore:
        # 恢复备份
        restored = 0
        for root, dirs, files in os.walk(args.path):
            for file in files:
                if file.endswith('.md.bak'):
                    bak_path = os.path.join(root, file)
                    orig_path = bak_path[:-4]
                    os.replace(bak_path, orig_path)
                    print(f"🔄 已恢复: {orig_path}")
                    restored += 1
        print(f"\n共恢复 {restored} 个文件")
        return
    
    print(f"{'='*60}")
    print(f"📚 Markdown 目录生成工具")
    print(f"{'='*60}")
    print(f"目标路径: {args.path}")
    print(f"最小文件大小: {args.min_size} KB")
    print(f"模式: {'预览' if args.dry_run else '实际修改'}")
    print(f"{'='*60}\n")
    
    count, skipped, errors = process_directory(args.path, args.dry_run, args.force, args.min_size)
    
    print(f"\n{'='*60}")
    print(f"📊 处理结果")
    print(f"{'='*60}")
    print(f"✅ 已处理: {count}")
    print(f"⏭️  跳过(太小): {skipped}")
    print(f"❌ 错误: {errors}")
    print(f"{'='*60}")
    
    if not args.dry_run and count > 0:
        print(f"\n💡 提示: 如需恢复，请运行: python {sys.argv[0]} --restore")

if __name__ == '__main__':
    main()
