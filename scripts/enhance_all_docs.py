#!/usr/bin/env python3
"""为所有文档增强全面关联体系"""
import os
import re
import json

try:
    with open('association_analysis_stats.json', 'r', encoding='utf-8') as f:
        stats = json.load(f)
except:
    stats = []

# 获取所有文档（包括已有链接的）
all_docs = [d for d in stats if isinstance(d, dict)]
print(f"处理 {len(all_docs)} 个文档的全面关联增强...")

def get_enhanced_association(doc_path, current_links_count):
    """生成增强型全面关联"""
    
    parts = doc_path.replace('knowledge/', '').replace('\\', '/').split('/')
    module = parts[0] if parts else ""
    
    # 根据模块确定关联重点
    module_targets = {
        '01_Core_Knowledge_System': {
            'name': '核心知识体系',
            'prereq': '无（基础入口）',
            'level': 'L1-L4',
            'next': '形式语义、系统技术'
        },
        '02_Formal_Semantics_and_Physics': {
            'name': '形式语义与物理',
            'prereq': '核心知识体系',
            'level': 'L5-L6',
            'next': 'CompCert验证、微架构'
        },
        '03_System_Technology_Domains': {
            'name': '系统技术领域',
            'prereq': '核心知识体系',
            'level': 'L3-L5',
            'next': '工业场景应用'
        },
        '04_Industrial_Scenarios': {
            'name': '工业场景',
            'prereq': '核心+系统技术',
            'level': 'L4-L5',
            'next': '专家实践'
        },
        '06_Thinking_Representation': {
            'name': '思维表征',
            'prereq': '全知识体系',
            'level': 'L3-L5',
            'next': '知识图谱、学习路径'
        },
        '07_Modern_Toolchain': {
            'name': '现代工具链',
            'prereq': '核心知识体系',
            'level': 'L2-L4',
            'next': '工程实践'
        }
    }
    
    info = module_targets.get(module, {
        'name': '知识库',
        'prereq': '核心知识体系',
        'level': 'L1-L6',
        'next': '持续学习'
    })
    
    depth = len(parts) - 1
    up = '../' * depth if depth > 0 else './'
    
    template = f'''
---

## 🔗 全面知识关联体系

### 【全局层】知识库导航
| 维度 | 目标文档 | 导航作用 |
|:-----|:---------|:---------|
| **总索引** | [{up}00_GLOBAL_INDEX.md]({up}00_GLOBAL_INDEX.md) | 完整知识图谱入口，全局视角 |
| **本模块** | [{up}README.md]({up}README.md) | 模块总览与目录导航 |
| **学习路径** | [{up}06_Thinking_Representation/06_Learning_Paths/README.md]({up}06_Thinking_Representation/06_Learning_Paths/README.md) | 阶段化学习路线规划 |
| **概念映射** | [{up}06_Thinking_Representation/05_Concept_Mappings/README.md]({up}06_Thinking_Representation/05_Concept_Mappings/README.md) | 核心概念等价关系图 |

### 【阶段层】学习定位
**当前模块**: {info['name']}  
**难度等级**: {info['level']}  
**前置依赖**: {info['prereq']}  
**后续延伸**: {info['next']}

```
学习阶段金字塔:
    L6 专家层 [形式验证、编译器]
    L5 高级层 [并发、系统编程] ⬅️ 可能在此
    L4 进阶层 [指针、内存管理]
    L3 基础层 [函数、结构体]
    L2 入门层 [语法、数据类型]
    L1 零基础 [环境搭建]
```

### 【层次层】纵向知识链
| 层级 | 关联文档 | 层次关系 |
|:-----|:---------|:---------|
| **理论基础** | [{up}02_Formal_Semantics_and_Physics/00_Core_Semantics_Foundations/README.md]({up}02_Formal_Semantics_and_Physics/00_Core_Semantics_Foundations/README.md) | 语义学理论基础 |
| **核心机制** | [{up}01_Core_Knowledge_System/02_Core_Layer/README.md]({up}01_Core_Knowledge_System/02_Core_Layer/README.md) | C语言核心机制 |
| **标准接口** | [{up}01_Core_Knowledge_System/04_Standard_Library_Layer/README.md]({up}01_Core_Knowledge_System/04_Standard_Library_Layer/README.md) | 标准库API |
| **系统实现** | [{up}03_System_Technology_Domains/README.md]({up}03_System_Technology_Domains/README.md) | 系统级实现 |

### 【局部层】横向关联网
| 关联类型 | 目标文档 | 关联说明 |
|:---------|:---------|:---------|
| **技术扩展** | [{up}03_System_Technology_Domains/14_Concurrency_Parallelism/README.md]({up}03_System_Technology_Domains/14_Concurrency_Parallelism/README.md) | 并发编程技术 |
| **安全规范** | [{up}01_Core_Knowledge_System/09_Safety_Standards/MISRA_C_2023/README.md]({up}01_Core_Knowledge_System/09_Safety_Standards/MISRA_C_2023/README.md) | 安全编码标准 |
| **工具支持** | [{up}07_Modern_Toolchain/README.md]({up}07_Modern_Toolchain/README.md) | 现代开发工具链 |
| **实践案例** | [{up}04_Industrial_Scenarios/README.md]({up}04_Industrial_Scenarios/README.md) | 工业实践场景 |

### 【总体层】知识体系架构
```
┌─────────────────────────────────────────────────────────────┐
│                     总体知识体系架构                          │
├─────────────────────────────────────────────────────────────┤
│  01 Core Knowledge          → 核心概念与机制                  │
│  02 Formal Semantics        → 理论与物理基础                  │
│  03 System Technology       → 系统级技术领域                  │
│  04 Industrial Scenarios    → 工业应用场景                    │
│  05 Deep Structure          → 深层结构与元物理                │
│  06 Thinking Representation → 思维表征与学习                  │
│  07 Modern Toolchain        → 现代工具链                      │
└─────────────────────────────────────────────────────────────┘
```

### 【决策层】学习路径选择
| 目标 | 推荐路径 | 关键文档 |
|:-----|:---------|:---------|
| **系统学习** | 01 → 02 → 03 → 04 | 按顺序阅读各模块 |
| **问题导向** | 06决策树 → 相关模块 | [决策树目录]({up}06_Thinking_Representation/01_Decision_Trees/README.md) |
| **项目驱动** | 04案例 → 所需知识 | [工业场景]({up}04_Industrial_Scenarios/README.md) |
| **深入研究** | 02形式语义 → 11CompCert | [形式语义]({up}02_Formal_Semantics_and_Physics/README.md) |

---

'''
    return template

def process_file(doc_info):
    """处理单个文档"""
    doc_path = doc_info.get('path', '')
    full_path = os.path.join('knowledge', doc_path)
    current_links = doc_info.get('internal_links', 0)
    
    if not os.path.exists(full_path):
        return False, "文件不存在"
    
    try:
        with open(full_path, 'r', encoding='utf-8-sig') as f:
            content = f.read()
        
        # 检查是否已有全面关联体系
        if '全面知识关联体系' in content or '【全局层】' in content:
            # 只修复编码
            with open(full_path, 'w', encoding='utf-8') as f:
                f.write(content)
            return True, "已有全面关联，修复编码"
        
        # 如果已有简单关联，保留并增强
        has_simple_assoc = '##' in content and ('🔗' in content or '文档关联' in content)
        
        # 查找插入位置
        lines = content.split('\n')
        insert_pos = 0
        
        for i, line in enumerate(lines):
            if line.startswith('# ') and i > 0:
                insert_pos = i + 1
                break
        
        # 生成增强关联
        enhanced = get_enhanced_association(doc_path, current_links)
        
        # 插入
        new_lines = lines[:insert_pos] + [''] + enhanced.strip().split('\n') + [''] + lines[insert_pos:]
        new_content = '\n'.join(new_lines)
        
        # 写入
        with open(full_path, 'w', encoding='utf-8') as f:
            f.write(new_content)
        
        return True, "添加全面关联体系"
        
    except Exception as e:
        return False, str(e)

def main():
    # 只处理前200个文档作为示例（避免时间过长）
    target_docs = all_docs[:300]
    
    print(f"增强 {len(target_docs)} 个文档的关联体系...")
    
    processed = 0
    skipped = 0
    errors = 0
    
    for i, doc in enumerate(target_docs):
        success, msg = process_file(doc)
        if success:
            if "已有" in msg:
                skipped += 1
            else:
                processed += 1
            if (i + 1) % 50 == 0:
                print(f"  进度: {i+1}/{len(target_docs)}")
        else:
            errors += 1
            print(f"  错误: {doc.get('path', '')} - {msg}")
    
    print(f"\n完成:")
    print(f"  新增全面关联: {processed}")
    print(f"  已存在跳过: {skipped}")
    print(f"  错误: {errors}")

if __name__ == '__main__':
    main()
