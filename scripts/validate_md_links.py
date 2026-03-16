#!/usr/bin/env python3
"""
验证 knowledge 目录下所有 Markdown 文件的内部链接有效性
"""

import os
import re
from pathlib import Path
from collections import defaultdict


def extract_links(content):
    """从 Markdown 内容中提取所有链接 [text](path)"""
    # 匹配 [text](path) 格式的链接
    pattern = r'\[([^\]]+)\]\(([^)]+)\)'
    return re.findall(pattern, content)


def is_internal_link(link_path):
    """判断是否为内部链接（指向 knowledge/ 目录下的相对路径）"""
    # 排除外部链接（http://, https://, ftp://, mailto: 等）
    if link_path.startswith(('http://', 'https://', 'ftp://', 'mailto:', 'tel:', 'javascript:')):
        return False
    # 排除锚点链接（仅 # 开头）
    if link_path.startswith('#'):
        return False
    # 排除空链接
    if not link_path.strip():
        return False
    return True


def resolve_link(base_dir, link_path):
    """解析相对链接为绝对路径"""
    # 移除链接中的锚点部分（#section）
    if '#' in link_path:
        link_path = link_path.split('#')[0]
    
    # 如果链接为空（只有锚点的情况）
    if not link_path:
        return None
    
    # 解析相对路径
    target_path = (base_dir / link_path).resolve()
    return target_path


def validate_links(knowledge_dir):
    """验证 knowledge 目录下所有 Markdown 文件的内部链接"""
    knowledge_path = Path(knowledge_dir).resolve()
    
    # 存储失效链接: {源文件: [(链接文本, 链接路径, 错误信息), ...]}
    broken_links = defaultdict(list)
    
    # 统计信息
    stats = {
        'total_files': 0,
        'total_links': 0,
        'internal_links': 0,
        'broken_links': 0
    }
    
    # 遍历所有 .md 文件
    for md_file in knowledge_path.rglob('*.md'):
        stats['total_files'] += 1
        
        try:
            with open(md_file, 'r', encoding='utf-8') as f:
                content = f.read()
        except Exception as e:
            print(f"⚠️  无法读取文件: {md_file} - {e}")
            continue
        
        # 提取所有链接
        links = extract_links(content)
        
        for link_text, link_path in links:
            stats['total_links'] += 1
            
            # 检查是否为内部链接
            if not is_internal_link(link_path):
                continue
            
            stats['internal_links'] += 1
            
            # 获取文件所在目录作为基础路径
            base_dir = md_file.parent
            
            # 解析链接路径
            target_path = resolve_link(base_dir, link_path)
            
            if target_path is None:
                continue  # 只有锚点的链接
            
            # 检查文件是否存在
            if not target_path.exists():
                stats['broken_links'] += 1
                rel_source = md_file.relative_to(knowledge_path)
                broken_links[str(rel_source)].append({
                    'text': link_text,
                    'path': link_path,
                    'resolved': str(target_path.relative_to(knowledge_path.parent)) if target_path.is_relative_to(knowledge_path.parent) else str(target_path),
                    'error': '文件不存在'
                })
            elif not target_path.is_file():
                stats['broken_links'] += 1
                rel_source = md_file.relative_to(knowledge_path)
                broken_links[str(rel_source)].append({
                    'text': link_text,
                    'path': link_path,
                    'resolved': str(target_path.relative_to(knowledge_path.parent)) if target_path.is_relative_to(knowledge_path.parent) else str(target_path),
                    'error': '路径不是文件'
                })
    
    return broken_links, stats


def main():
    knowledge_dir = r'E:\_src\C_Lang\knowledge'
    
    print("=" * 80)
    print("🔍 Markdown 内部链接验证工具")
    print("=" * 80)
    print(f"📁 扫描目录: {knowledge_dir}")
    print()
    
    broken_links, stats = validate_links(knowledge_dir)
    
    # 打印统计信息
    print("-" * 80)
    print("📊 统计信息:")
    print(f"   扫描文件数: {stats['total_files']}")
    print(f"   总链接数: {stats['total_links']}")
    print(f"   内部链接数: {stats['internal_links']}")
    print(f"   失效链接数: {stats['broken_links']}")
    print("-" * 80)
    print()
    
    # 打印失效链接详情
    if broken_links:
        print("❌ 发现失效链接:")
        print("=" * 80)
        
        for source_file, links in sorted(broken_links.items()):
            print(f"\n📄 文件: knowledge/{source_file}")
            for link in links:
                print(f"   🔗 链接文本: [{link['text']}]")
                print(f"      链接路径: ({link['path']})")
                print(f"      解析路径: {link['resolved']}")
                print(f"      错误原因: {link['error']}")
        
        print("\n" + "=" * 80)
        print(f"⚠️  共发现 {stats['broken_links']} 个失效链接，分布在 {len(broken_links)} 个文件中")
        print("=" * 80)
        return 1
    else:
        print("✅ 所有内部链接均有效！")
        print("=" * 80)
        return 0


if __name__ == '__main__':
    exit(main())
