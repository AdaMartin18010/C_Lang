#!/usr/bin/env python3
"""大规模文档关联化工具 - 批量处理所有孤立文档"""
import os
import re
import glob
import json

# 读取关联分析结果，获取被引用最多的文档
with open('association_analysis_stats.json', 'r', encoding='utf-8') as f:
    stats = json.load(f)

# 核心关联目标（被引用最多的文档）
CORE_TARGETS = [
    ("01_Core_Knowledge_System/02_Core_Layer/02_Memory_Management.md", "内存管理"),
    ("01_Core_Knowledge_System/02_Core_Layer/01_Pointer_Depth.md", "指针深度"),
    ("03_System_Technology_Domains/14_Concurrency_Parallelism/README.md", "并发编程"),
    ("01_Core_Knowledge_System/01_Basic_Layer/02_Data_Type_System.md", "数据类型"),
    ("01_Core_Knowledge_System/02_Core_Layer/05_Arrays_Pointers.md", "数组与指针"),
    ("01_Core_Knowledge_System/05_Engineering_Layer/README.md", "软件工程"),
    ("02_Formal_Semantics_and_Physics/README.md", "形式语义"),
    ("03_System_Technology_Domains/README.md", "系统技术"),
    ("04_Industrial_Scenarios/README.md", "工业场景"),
    ("06_Thinking_Representation/README.md", "思维表征"),
]

def get_template_for_path(filepath):
    """根据文件路径生成针对性关联模板"""
    rel_path = filepath.replace('knowledge/', '').replace('\\', '/')
    
    # 计算相对路径层级
    depth = rel_path.count('/')
    prefix = '../' * depth if depth > 0 else './'
    
    links = []
    for target, name in CORE_TARGETS:
        # 计算相对路径
        target_parts = target.split('/')
        rel_parts = rel_path.split('/')
        
        # 找到共同前缀
        common = 0
        for i, (a, b) in enumerate(zip(rel_parts[:-1], target_parts)):
            if a == b:
                common += 1
            else:
                break
        
        # 构建相对路径
        up = len(rel_parts) - common - 1
        down = target_parts[common:]
        rel_target = '../' * up + '/'.join(down) if up > 0 else './' + '/'.join(down)
        
        links.append(f"| [{name}]({rel_target}) | 核心关联 | {name}基础 |")
    
    template = f'''
---

## 🔗 文档关联

### 核心关联
| 文档 | 关系类型 | 说明 |
|:-----|:---------|:-----|
{chr(10).join(links[:5])}

### 扩展阅读
| 文档 | 关系类型 | 说明 |
|:-----|:---------|:-----|
{chr(10).join(links[5:])}

'''
    return template

def process_file(filepath):
    """处理单个文件"""
    try:
        with open(filepath, 'r', encoding='utf-8') as f:
            content = f.read()
        
        # 跳过已有链接的文档
        if '##' in content and ('🔗' in content or '关联' in content or '文档关联' in content):
            return False, "已有关联"
        
        # 跳过非Markdown文件
        if not filepath.endswith('.md'):
            return False, "非Markdown"
        
        # 找到第一级标题后的位置
        lines = content.split('\n')
        insert_pos = -1
        
        for i, line in enumerate(lines):
            if line.startswith('# ') and i > 0:
                insert_pos = i + 1
                # 跳过空行
                while insert_pos < len(lines) and lines[insert_pos].strip() == '':
                    insert_pos += 1
                break
        
        if insert_pos == -1 or insert_pos >= len(lines):
            # 没有一级标题，在文件开头添加
            insert_pos = 0
        
        # 生成并插入模板
        template = get_template_for_path(filepath)
        new_lines = lines[:insert_pos] + template.strip().split('\n') + lines[insert_pos:]
        new_content = '\n'.join(new_lines)
        
        with open(filepath, 'w', encoding='utf-8') as f:
            f.write(new_content)
        
        return True, "成功"
        
    except Exception as e:
        return False, str(e)

def main():
    # 获取所有孤立文档（0链接的文档）
    isolated_docs = []
    for doc in stats.get('doc_stats', []):
        if doc.get('internal_links', 0) == 0:
            isolated_docs.append(doc['path'])
    
    print(f"发现 {len(isolated_docs)} 个孤立文档")
    
    # 批量处理
    processed = 0
    skipped = 0
    errors = 0
    
    # 优先处理关键目录
    priority_dirs = [
        'knowledge/01_Core_Knowledge_System',
        'knowledge/02_Formal_Semantics_and_Physics',
        'knowledge/03_System_Technology_Domains',
        'knowledge/04_Industrial_Scenarios',
        'knowledge/06_Thinking_Representation',
    ]
    
    # 先处理优先目录
    for doc_path in isolated_docs:
        full_path = os.path.join('knowledge', doc_path)
        if os.path.exists(full_path):
            success, msg = process_file(full_path)
            if success:
                processed += 1
                if processed % 50 == 0:
                    print(f"  已处理: {processed}")
            elif "已有关联" in msg:
                skipped += 1
            else:
                errors += 1
    
    print(f"\n批量处理完成:")
    print(f"  成功: {processed}")
    print(f"  跳过: {skipped}")
    print(f"  错误: {errors}")

if __name__ == '__main__':
    main()
