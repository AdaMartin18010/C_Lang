#!/usr/bin/env python3
"""批量文档关联化工具 - 自动为README添加关联链接"""
import os
import re
import glob

# 标准关联模板
ASSOCIATION_TEMPLATE = '''
---

## 🔗 文档关联

### 前置知识
| 文档 | 关系类型 | 说明 |
|:-----|:---------|:-----|
| [C语言基础](../01_Core_Knowledge_System/01_Basic_Layer/01_Syntax_Elements.md) | 基础依赖 | 语法基础 |
| [核心知识体系](../01_Core_Knowledge_System/README.md) | 知识基础 | C语言核心概念 |

### 横向关联
| 文档 | 关系类型 | 说明 |
|:-----|:---------|:-----|
| [形式语义](../02_Formal_Semantics_and_Physics/README.md) | 理论支撑 | 形式化方法 |
| [系统技术](../03_System_Technology_Domains/README.md) | 技术应用 | 系统级开发 |

### 后续延伸
| 文档 | 关系类型 | 说明 |
|:-----|:---------|:-----|
| [工业场景](../04_Industrial_Scenarios/README.md) | 实际应用 | 工业实践 |
| [安全标准](../01_Core_Knowledge_System/09_Safety_Standards/README.md) | 安全规范 | 安全编码 |

'''

def add_association_to_file(filepath):
    """为单个文件添加关联链接"""
    try:
        with open(filepath, 'r', encoding='utf-8') as f:
            content = f.read()
        
        # 检查是否已有 🔗 标记
        if '##' in content and ('🔗' in content or '关联' in content):
            return False, "已有关联链接"
        
        # 在第一级标题后插入关联块
        lines = content.split('\n')
        insert_pos = 0
        
        for i, line in enumerate(lines):
            if line.startswith('# ') and i > 0:
                insert_pos = i + 1
                break
        
        if insert_pos == 0:
            return False, "未找到插入位置"
        
        # 插入关联块
        new_lines = lines[:insert_pos] + ASSOCIATION_TEMPLATE.strip().split('\n') + lines[insert_pos:]
        new_content = '\n'.join(new_lines)
        
        with open(filepath, 'w', encoding='utf-8') as f:
            f.write(new_content)
        
        return True, "成功添加"
        
    except Exception as e:
        return False, str(e)

def main():
    # 处理关键模块的README
    targets = [
        'knowledge/04_Industrial_Scenarios/*/README.md',
        'knowledge/05_Deep_Structure_MetaPhysics/*/README.md',
        'knowledge/06_Thinking_Representation/*/README.md',
        'knowledge/07_Modern_Toolchain/*/README.md',
    ]
    
    processed = 0
    skipped = 0
    errors = 0
    
    for pattern in targets:
        files = glob.glob(pattern)
        for filepath in files:
            success, msg = add_association_to_file(filepath)
            if success:
                processed += 1
                print(f"✅ {filepath}: {msg}")
            elif "已有关联" in msg:
                skipped += 1
            else:
                errors += 1
                print(f"❌ {filepath}: {msg}")
    
    print(f"\n处理完成: 成功{processed}, 跳过{skipped}, 错误{errors}")

if __name__ == '__main__':
    main()
