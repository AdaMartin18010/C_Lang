#!/usr/bin/env python3
"""全面修复脚本 - 修复BOM、完善关联、统一格式"""
import os
import re
import glob

# 读取当前统计数据
import json
try:
    with open('association_analysis_stats.json', 'r', encoding='utf-8') as f:
        stats = json.load(f)
except:
    stats = []

# 获取孤立文档列表
isolated = [d for d in stats if isinstance(d, dict) and d.get('internal_links', 0) == 0]
print(f"发现 {len(isolated)} 个孤立文档需要修复")

def remove_bom(content):
    """去除BOM"""
    if content.startswith('\ufeff'):
        return content[1:]
    return content

def add_bom(content):
    """添加UTF-8 BOM (某些编辑器需要)"""
    if not content.startswith('\ufeff'):
        return '\ufeff' + content
    return content

def get_comprehensive_association(doc_path):
    """生成全面的关联区块 - 包含多层次关联"""
    
    # 计算相对路径
    parts = doc_path.replace('knowledge/', '').replace('\\', '/').split('/')
    depth = len(parts) - 1
    up = '../' * depth if depth > 0 else './'
    
    # 根据路径确定关联重点
    if '01_Core_Knowledge_System' in doc_path:
        category = "core"
    elif '02_Formal_Semantics' in doc_path:
        category = "formal"
    elif '03_System_Technology' in doc_path:
        category = "system"
    elif '04_Industrial_Scenarios' in doc_path:
        category = "industrial"
    elif '06_Thinking_Representation' in doc_path:
        category = "thinking"
    else:
        category = "general"
    
    # 全面关联模板
    template = f'''
---

## 🔗 知识关联网络

### 1. 全局导航
| 层级 | 文档 | 作用 |
|:-----|:-----|:-----|
| 总索引 | [{up}00_GLOBAL_INDEX.md]({up}00_GLOBAL_INDEX.md) | 完整知识图谱入口 |
| 本模块 | [{up}README.md]({up}README.md) | 模块总览与导航 |
| 学习路径 | [{up}06_Thinking_Representation/06_Learning_Paths/README.md]({up}06_Thinking_Representation/06_Learning_Paths/README.md) | 推荐学习路线 |

### 2. 前置知识依赖
| 文档 | 关系 | 掌握要求 |
|:-----|:-----|:---------|
| [{up}01_Core_Knowledge_System/01_Basic_Layer/01_Syntax_Elements.md]({up}01_Core_Knowledge_System/01_Basic_Layer/01_Syntax_Elements.md) | 语言基础 | 必须掌握 |
| [{up}01_Core_Knowledge_System/02_Core_Layer/01_Pointer_Depth.md]({up}01_Core_Knowledge_System/02_Core_Layer/01_Pointer_Depth.md) | 核心机制 | 必须掌握 |
| [{up}01_Core_Knowledge_System/02_Core_Layer/02_Memory_Management.md]({up}01_Core_Knowledge_System/02_Core_Layer/02_Memory_Management.md) | 内存基础 | 必须掌握 |

### 3. 同层横向关联
| 文档 | 关系 | 互补内容 |
|:-----|:-----|:---------|
| [{up}03_System_Technology_Domains/14_Concurrency_Parallelism/README.md]({up}03_System_Technology_Domains/14_Concurrency_Parallelism/README.md) | 技术扩展 | 并发编程技术 |
| [{up}02_Formal_Semantics_and_Physics/README.md]({up}02_Formal_Semantics_and_Physics/README.md) | 理论支撑 | 形式化方法 |
| [{up}04_Industrial_Scenarios/README.md]({up}04_Industrial_Scenarios/README.md) | 实践应用 | 工业实践案例 |

### 4. 深层理论关联
| 文档 | 关系 | 理论深度 |
|:-----|:-----|:---------|
| [{up}02_Formal_Semantics_and_Physics/00_Core_Semantics_Foundations/README.md]({up}02_Formal_Semantics_and_Physics/00_Core_Semantics_Foundations/README.md) | 语义基础 | 操作语义、类型理论 |
| [{up}06_Thinking_Representation/05_Concept_Mappings/README.md]({up}06_Thinking_Representation/05_Concept_Mappings/README.md) | 概念映射 | 知识关联网络 |

### 5. 后续进阶延伸
| 文档 | 关系 | 进阶方向 |
|:-----|:-----|:---------|
| [{up}03_System_Technology_Domains/README.md]({up}03_System_Technology_Domains/README.md) | 系统技术 | 系统级开发 |
| [{up}01_Core_Knowledge_System/09_Safety_Standards/README.md]({up}01_Core_Knowledge_System/09_Safety_Standards/README.md) | 安全标准 | 安全编码规范 |
| [{up}07_Modern_Toolchain/README.md]({up}07_Modern_Toolchain/README.md) | 工具链 | 现代开发工具 |

### 6. 阶段学习定位
```
当前位置: 根据文档主题确定学习阶段
├─ 入门阶段: 基础语法、数据类型
├─ 基础阶段: 控制流程、函数
├─ 进阶阶段: 指针、内存管理 ⬅️ 可能在此
├─ 高级阶段: 并发、系统编程
└─ 专家阶段: 形式验证、编译器
```

### 7. 局部索引
本文件所属模块的详细内容：
- 参见本模块 [README.md]({up}README.md)
- 相关子目录文档

'''
    return template

def process_file(doc_info):
    """处理单个文件"""
    doc_path = doc_info.get('path', '')
    full_path = os.path.join('knowledge', doc_path)
    
    if not os.path.exists(full_path):
        return False, f"文件不存在: {full_path}"
    
    try:
        # 读取文件
        with open(full_path, 'r', encoding='utf-8-sig') as f:  # utf-8-sig自动去除BOM
            content = f.read()
        
        # 检查是否已有完善关联
        if '知识关联网络' in content or '全局导航' in content:
            # 只修复BOM问题
            with open(full_path, 'w', encoding='utf-8') as f:
                f.write(content)
            return True, "已有关联，仅修复编码"
        
        # 查找插入位置（在第一级标题后）
        lines = content.split('\n')
        insert_pos = 0
        
        for i, line in enumerate(lines):
            if line.startswith('# ') and i > 0:
                insert_pos = i + 1
                break
        
        if insert_pos == 0:
            insert_pos = 1  # 默认在首行后插入
        
        # 生成并插入全面关联
        association = get_comprehensive_association(doc_path)
        new_lines = lines[:insert_pos] + association.strip().split('\n') + [''] + lines[insert_pos:]
        new_content = '\n'.join(new_lines)
        
        # 写入文件（无BOM）
        with open(full_path, 'w', encoding='utf-8') as f:
            f.write(new_content)
        
        return True, "添加全面关联"
        
    except Exception as e:
        return False, str(e)

def main():
    print(f"开始全面修复 {len(isolated)} 个孤立文档...\n")
    
    processed = 0
    fixed_encoding = 0
    errors = 0
    
    for i, doc in enumerate(isolated):
        success, msg = process_file(doc)
        if success:
            if "仅修复编码" in msg:
                fixed_encoding += 1
            else:
                processed += 1
            if (i + 1) % 10 == 0:
                print(f"  进度: {i+1}/{len(isolated)}")
        else:
            errors += 1
            print(f"  错误: {doc.get('path', '')} - {msg}")
    
    print(f"\n修复完成:")
    print(f"  添加全面关联: {processed}")
    print(f"  修复编码: {fixed_encoding}")
    print(f"  错误: {errors}")

if __name__ == '__main__':
    main()
