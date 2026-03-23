#!/usr/bin/env python3
"""最终冲刺 - 处理所有剩余孤立文档"""
import os
import json

# 读取孤立文档列表
with open('association_analysis_stats.json', 'r', encoding='utf-8') as f:
    stats = json.load(f)

isolated = [d for d in stats if d.get('internal_links', 0) == 0]

# 针对不同目录的关联模板
TEMPLATES = {
    'Video_Tutorials': '''
---

## 🔗 关联资源

### 对应核心知识
| 文档 | 关系 | 说明 |
|:-----|:-----|:-----|
| [核心知识体系](../../01_Core_Knowledge_System/README.md) | 知识基础 | C语言核心内容 |
| [视频教程目录](../README.md) | 上级目录 | 完整视频课程 |

### 学习路径
- 对应视频主题请参考核心知识体系相关章节
- 完整学习路径见 [全局索引](../../00_GLOBAL_INDEX.md)

''',
    'Zig': '''
---

## 🔗 Zig与C关联

### Zig与C互操作
| 文档 | 关系 | 说明 |
|:-----|:-----|:-----|
| [Zig学习路径](Zig_Learning_Paths.md) | 学习指南 | Zig学习路径 |
| [Rust互操作](../../03_System_Technology_Domains/16_Rust_Interoperability/README.md) | 对比参考 | 其他语言互操作 |
| [核心知识体系](../../01_Core_Knowledge_System/README.md) | C基础 | C语言核心概念 |

''',
    'System_Tech': '''
---

## 🔗 系统技术关联

### 核心技术
| 文档 | 关系 | 说明 |
|:-----|:-----|:-----|
| [系统技术总览](../README.md) | 上级目录 | 技术领域概览 |
| [并发编程](../../14_Concurrency_Parallelism/README.md) | 核心关联 | 并发基础 |
| [核心知识体系](../../01_Core_Knowledge_System/README.md) | 知识基础 | C语言核心 |

''',
    'default': '''
---

## 🔗 文档关联

### 核心关联
| 文档 | 关系类型 | 说明 |
|:-----|:---------|:-----|
| [核心知识体系](../../01_Core_Knowledge_System/README.md) | 知识基础 | C语言核心概念 |
| [全局索引](../../00_GLOBAL_INDEX.md) | 知识导航 | 完整知识图谱 |
| [学习路径](../../06_Thinking_Representation/06_Learning_Paths/README.md) | 学习指南 | 推荐学习路径 |

### 扩展阅读
- 根据主题参考相应核心知识模块
- 完整内容索引见 [全局索引](../../00_GLOBAL_INDEX.md)

'''
}

def get_template(filepath):
    """根据路径选择模板"""
    if 'Video_Tutorials' in filepath or 'Video_Scripts' in filepath:
        return TEMPLATES['Video_Tutorials']
    elif 'Zig' in filepath:
        return TEMPLATES['Zig']
    elif 'System_Technology_Domains' in filepath and '03_' in filepath:
        return TEMPLATES['System_Tech']
    else:
        return TEMPLATES['default']

def process_file(filepath):
    """处理单个文件"""
    full_path = os.path.join('knowledge', filepath)
    if not os.path.exists(full_path):
        return False, "文件不存在"
    
    try:
        with open(full_path, 'r', encoding='utf-8') as f:
            content = f.read()
        
        # 跳过已有链接的
        if '##' in content and ('🔗' in content or '关联' in content):
            return False, "已有关联"
        
        # 在文件开头添加
        template = get_template(filepath)
        new_content = template.strip() + '\n\n' + content
        
        with open(full_path, 'w', encoding='utf-8') as f:
            f.write(new_content)
        
        return True, "成功"
        
    except Exception as e:
        return False, str(e)

def main():
    print(f"处理 {len(isolated)} 个剩余孤立文档...")
    
    processed = 0
    skipped = 0
    errors = 0
    
    for doc in isolated:
        path = doc.get('path', '')
        success, msg = process_file(path)
        if success:
            processed += 1
            print(f"✅ {path}")
        elif "已有关联" in msg:
            skipped += 1
        else:
            errors += 1
            print(f"❌ {path}: {msg}")
    
    print(f"\n完成: 成功{processed}, 跳过{skipped}, 错误{errors}")

if __name__ == '__main__':
    main()
