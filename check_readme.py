#!/usr/bin/env python3
"""
Knowledge 目录 README.md 完整性检查脚本

功能：
- 遍历指定目录下的所有子目录
- 检查每个目录是否包含 README.md 文件
- 生成详细的统计报告

作者：Kimi Code CLI
日期：2026-03-16
"""

import os
from datetime import datetime
from collections import defaultdict


def check_readme_completeness(knowledge_dir):
    """
    检查 knowledge 目录下所有子目录的 README.md 完整性
    
    参数:
        knowledge_dir: 要检查的根目录路径
        
    返回:
        dict: 包含详细检查结果的字典
    """
    
    # 存储所有目录信息的列表
    all_dirs_info = []
    
    # 使用 os.walk 遍历目录树
    # os.walk 生成 (root, dirs, files) 三元组
    for root, dirs, files in os.walk(knowledge_dir):
        for d in dirs:
            dir_path = os.path.join(root, d)
            readme_path = os.path.join(dir_path, 'README.md')
            has_readme = os.path.exists(readme_path)
            
            # 计算相对路径和目录深度
            rel_path = os.path.relpath(dir_path, knowledge_dir)
            depth = rel_path.count(os.sep)
            
            # 收集 README 文件信息（如果存在）
            readme_info = {}
            if has_readme:
                stat = os.stat(readme_path)
                readme_info = {
                    'size_bytes': stat.st_size,
                    'modified_time': datetime.fromtimestamp(
                        stat.st_mtime
                    ).strftime('%Y-%m-%d %H:%M:%S')
                }
            
            all_dirs_info.append({
                'absolute_path': dir_path,
                'relative_path': rel_path,
                'depth': depth,
                'has_readme': has_readme,
                'readme_info': readme_info
            })
    
    # 分类统计
    has_readme_list = [d for d in all_dirs_info if d['has_readme']]
    missing_readme_list = [d for d in all_dirs_info if not d['has_readme']]
    
    # 按深度统计
    depth_stats = defaultdict(lambda: {'total': 0, 'has_readme': 0, 'missing_readme': 0})
    for d in all_dirs_info:
        depth_stats[d['depth']]['total'] += 1
        if d['has_readme']:
            depth_stats[d['depth']]['has_readme'] += 1
        else:
            depth_stats[d['depth']]['missing_readme'] += 1
    
    # 计算 README 文件大小统计
    if has_readme_list:
        readme_sizes = [d['readme_info'].get('size_bytes', 0) for d in has_readme_list]
        total_readme_size = sum(readme_sizes)
        avg_readme_size = total_readme_size / len(has_readme_list)
        min_readme_size = min(readme_sizes)
        max_readme_size = max(readme_sizes)
    else:
        total_readme_size = avg_readme_size = min_readme_size = max_readme_size = 0
    
    return {
        'total_dirs': len(all_dirs_info),
        'has_readme_count': len(has_readme_list),
        'missing_readme_count': len(missing_readme_list),
        'coverage_percent': (len(has_readme_list) / len(all_dirs_info) * 100) 
                           if all_dirs_info else 0,
        'all_dirs_info': all_dirs_info,
        'has_readme_list': has_readme_list,
        'missing_readme_list': missing_readme_list,
        'depth_stats': dict(depth_stats),
        'total_readme_size': total_readme_size,
        'avg_readme_size': avg_readme_size,
        'min_readme_size': min_readme_size,
        'max_readme_size': max_readme_size
    }


def print_report(results):
    """打印格式化的检查报告"""
    
    print('=' * 70)
    print('KNOWLEDGE 目录 README.md 完整性检查报告')
    print('=' * 70)
    print()
    
    print('[一、总体统计]')
    print('-' * 70)
    print(f"检查根目录: {results['knowledge_dir']}")
    print(f"检查时间: {datetime.now().strftime('%Y-%m-%d %H:%M:%S')}")
    print()
    print(f"总目录数:          {results['total_dirs']}")
    print(f"有 README.md:      {results['has_readme_count']}")
    print(f"缺少 README.md:    {results['missing_readme_count']}")
    print(f"覆盖率:            {results['coverage_percent']:.2f}%")
    print()
    
    print('[二、README 文件统计]')
    print('-' * 70)
    print(f"README 文件总大小: {results['total_readme_size']:,} bytes "
          f"({results['total_readme_size']/1024:.2f} KB)")
    print(f"README 平均大小:   {results['avg_readme_size']:.2f} bytes "
          f"({results['avg_readme_size']/1024:.2f} KB)")
    print(f"最小 README 大小:  {results['min_readme_size']:,} bytes")
    print(f"最大 README 大小:  {results['max_readme_size']:,} bytes")
    print()
    
    print('[三、目录层级分布]')
    print('-' * 70)
    for depth in sorted(results['depth_stats'].keys()):
        stats = results['depth_stats'][depth]
        print(f"深度 {depth}: {stats['total']} 个目录 "
              f"(有README: {stats['has_readme']}, 缺README: {stats['missing_readme']})")
    print()
    
    print('[四、目录结构示例（前20个）]')
    print('-' * 70)
    for i, d in enumerate(results['all_dirs_info'][:20]):
        status = 'Y' if d['has_readme'] else 'N'
        indent = '  ' * d['depth']
        print(f"[{status}] {indent}{d['relative_path']}")
    
    remaining = results['total_dirs'] - 20
    if remaining > 0:
        print(f"... 还有 {remaining} 个目录 ...")
    print()
    
    print('[五、缺少 README.md 的目录列表]')
    print('-' * 70)
    if results['missing_readme_list']:
        for d in results['missing_readme_list']:
            print(f"  - {d['relative_path']}")
    else:
        print("所有目录都有 README.md 文件！")
    print()
    
    print('=' * 70)
    print('检查完成')
    print('=' * 70)


def main():
    """主函数"""
    knowledge_dir = r'E:\_src\C_Lang\knowledge'
    
    # 执行检查
    results = check_readme_completeness(knowledge_dir)
    results['knowledge_dir'] = knowledge_dir
    
    # 打印报告
    print_report(results)
    
    return results


if __name__ == '__main__':
    main()
