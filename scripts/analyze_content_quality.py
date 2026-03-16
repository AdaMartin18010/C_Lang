#!/usr/bin/env python3
"""
C_Lang Knowledge Base Content Quality Analyzer
分析 knowledge 目录下所有 Markdown 文件的内容质量
"""

import os
import re
from pathlib import Path
from collections import defaultdict
from dataclasses import dataclass, field
from typing import List, Dict, Set
import json

@dataclass
class FileAnalysis:
    """单个文件的分析结果"""
    path: str
    size: int
    size_kb: float
    
    # 结构检查
    has_concept_definition: bool = False
    has_property_description: bool = False
    has_example_code: bool = False
    has_anti_example: bool = False
    has_formal_description: bool = False
    has_diagram: bool = False
    
    # 内容统计
    line_count: int = 0
    code_block_count: int = 0
    code_block_lines: int = 0
    table_count: int = 0
    heading_count: int = 0
    
    # 内容质量评分 (0-100)
    structure_score: int = 0
    
    def get_missing_elements(self) -> List[str]:
        """返回缺少的结构要素"""
        missing = []
        if not self.has_concept_definition:
            missing.append("概念定义")
        if not self.has_property_description:
            missing.append("属性说明")
        if not self.has_example_code:
            missing.append("示例代码")
        if not self.has_anti_example:
            missing.append("反例/陷阱")
        if not self.has_formal_description:
            missing.append("形式化描述")
        if not self.has_diagram:
            missing.append("思维导图/图表")
        return missing


def check_concept_definition(content: str) -> bool:
    """检查是否有概念定义"""
    patterns = [
        r'##?\s*(?:什么是|定义|概念|概述|简介|Introduction|Definition|Concept)',
        r'(?:定义|概念)[：:]',
        r'(?i)(?:is defined as|definition|concept|what is)',
    ]
    for pattern in patterns:
        if re.search(pattern, content):
            return True
    return False


def check_property_description(content: str) -> bool:
    """检查是否有属性说明"""
    patterns = [
        r'##?\s*(?:属性|特性|特点|参数|返回值|Parameters|Returns|Properties|Features)',
        r'(?:属性|特性|参数)[：:]',
        r'(?i)(?:properties?|characteristics?|features?|attributes?)',
    ]
    for pattern in patterns:
        if re.search(pattern, content):
            return True
    return False


def check_example_code(content: str) -> bool:
    """检查是否有示例代码"""
    # 检查代码块
    code_block_pattern = r'```[\s\S]*?```'
    code_blocks = re.findall(code_block_pattern, content)
    if len(code_blocks) >= 1:
        # 进一步检查是否包含实际的代码（不只是语言声明）
        for block in code_blocks:
            lines = block.strip().split('\n')
            if len(lines) > 2:  # 至少包含语言声明和一行代码
                return True
    return False


def check_anti_example(content: str) -> bool:
    """检查是否有反例/陷阱/警告"""
    patterns = [
        r'##?\s*(?:陷阱|注意|警告|常见错误|反例|⚠️|❌|Warning|Caution|Common Mistakes|Anti-pattern|Pitfall)',
        r'(?:陷阱|注意|警告|错误|不要|避免)[：:]',
        r'(?i)(?:pitfall|warning|caution|danger|common mistake|do not|don\'t|avoid)',
        r'>\s*(?:⚠️|❌|警告|注意|Warning)',
    ]
    for pattern in patterns:
        if re.search(pattern, content):
            return True
    return False


def check_formal_description(content: str) -> bool:
    """检查是否有形式化描述"""
    patterns = [
        r'##?\s*(?:形式化|数学|定理|证明|Formal|Mathematical|Proof|Theorem|Semantics)',
        r'(?:形式化|数学描述|定理|证明)[：:]',
        r'(?i)(?:formal|mathematical|semantics?|axiomatic|denotational|operational)',
        r'```(?:math|latex|coq|isabelle)',
        r'\$\$.+?\$\$',  # LaTeX 块
        r'\\\[.+?\\\]',  # LaTeX 显示公式
    ]
    for pattern in patterns:
        if re.search(pattern, content):
            return True
    return False


def check_diagram(content: str) -> bool:
    """检查是否有思维导图/图表"""
    patterns = [
        r'##?\s*(?:思维导图|图表|可视化|关系图|Diagram|Mind Map|Visualization|Graph)',
        r'```(?:mermaid|graphviz|plantuml|dot)',
        r'(?i)(?:mind.?map|diagram|flowchart|graph|visualization)',
        r'!\[.*?\]\(.+?\)',  # 图片链接
        r'```\s*mermaid\s*\n\s*(?:graph|flowchart|mindmap|classDiagram|sequenceDiagram)',
    ]
    for pattern in patterns:
        if re.search(pattern, content):
            return True
    return False


def analyze_file(filepath: str) -> FileAnalysis:
    """分析单个Markdown文件"""
    try:
        with open(filepath, 'r', encoding='utf-8', errors='ignore') as f:
            content = f.read()
    except Exception as e:
        print(f"Error reading {filepath}: {e}")
        return None
    
    size = os.path.getsize(filepath)
    size_kb = round(size / 1024, 2)
    lines = content.split('\n')
    line_count = len(lines)
    
    # 统计代码块
    code_blocks = re.findall(r'```[\s\S]*?```', content)
    code_block_count = len(code_blocks)
    code_block_lines = sum(len(block.split('\n')) for block in code_blocks)
    
    # 统计表格
    table_count = len(re.findall(r'\|.*\|.*\|', content))
    
    # 统计标题
    heading_count = len(re.findall(r'^#{1,6}\s', content, re.MULTILINE))
    
    # 结构检查
    has_concept = check_concept_definition(content)
    has_property = check_property_description(content)
    has_example = check_example_code(content)
    has_anti = check_anti_example(content)
    has_formal = check_formal_description(content)
    has_diagram = check_diagram(content)
    
    # 计算结构评分
    structure_score = 0
    if has_concept:
        structure_score += 15
    if has_property:
        structure_score += 15
    if has_example:
        structure_score += 25
    if has_anti:
        structure_score += 15
    if has_formal:
        structure_score += 15
    if has_diagram:
        structure_score += 15
    
    # 文件大小评分（太大或太小都扣分）
    if size_kb < 1:
        structure_score = min(structure_score, 30)  # 小于1KB的文件最高30分
    elif size_kb < 3:
        structure_score = min(structure_score, 50)  # 小于3KB的文件最高50分
    
    return FileAnalysis(
        path=filepath,
        size=size,
        size_kb=size_kb,
        has_concept_definition=has_concept,
        has_property_description=has_property,
        has_example_code=has_example,
        has_anti_example=has_anti,
        has_formal_description=has_formal,
        has_diagram=has_diagram,
        line_count=line_count,
        code_block_count=code_block_count,
        code_block_lines=code_block_lines,
        table_count=table_count,
        heading_count=heading_count,
        structure_score=structure_score
    )


def analyze_directory(base_dir: str) -> List[FileAnalysis]:
    """分析目录下所有Markdown文件"""
    results = []
    md_files = list(Path(base_dir).rglob('*.md'))
    
    print(f"Found {len(md_files)} Markdown files to analyze...")
    
    for i, filepath in enumerate(md_files, 1):
        if i % 50 == 0:
            print(f"  Analyzed {i}/{len(md_files)} files...")
        
        result = analyze_file(str(filepath))
        if result:
            results.append(result)
    
    return results


def generate_report(results: List[FileAnalysis], output_path: str):
    """生成分析报告"""
    
    # 按大小分组
    size_groups = {
        '< 5KB': [],
        '5-10KB': [],
        '10-20KB': [],
        '> 20KB': []
    }
    
    for r in results:
        if r.size_kb < 5:
            size_groups['< 5KB'].append(r)
        elif r.size_kb < 10:
            size_groups['5-10KB'].append(r)
        elif r.size_kb < 20:
            size_groups['10-20KB'].append(r)
        else:
            size_groups['> 20KB'].append(r)
    
    # 找出内容最薄弱的30个文件（按结构评分和大小综合排序）
    # 评分越低、文件越小，越薄弱
    weak_files = sorted(results, key=lambda x: (x.structure_score, x.size_kb))[:30]
    
    # 统计结构要素覆盖率
    total = len(results)
    structure_coverage = {
        '概念定义': sum(1 for r in results if r.has_concept_definition),
        '属性说明': sum(1 for r in results if r.has_property_description),
        '示例代码': sum(1 for r in results if r.has_example_code),
        '反例/陷阱': sum(1 for r in results if r.has_anti_example),
        '形式化描述': sum(1 for r in results if r.has_formal_description),
        '思维导图/图表': sum(1 for r in results if r.has_diagram),
    }
    
    # 按模块统计平均评分
    module_scores = defaultdict(list)
    for r in results:
        # 提取模块名（第一个目录层级）
        rel_path = os.path.relpath(r.path, 'knowledge')
        parts = rel_path.split(os.sep)
        if len(parts) > 0:
            module = parts[0]
            module_scores[module].append(r.structure_score)
    
    module_avg_scores = {
        module: round(sum(scores)/len(scores), 1) 
        for module, scores in module_scores.items()
    }
    
    # 找出薄弱模块（平均评分低于40分的）
    weak_modules = sorted(
        [(m, s) for m, s in module_avg_scores.items() if s < 40],
        key=lambda x: x[1]
    )
    
    # 生成报告内容
    report_lines = []
    report_lines.append("# C_Lang Knowledge Base 内容质量分析报告")
    report_lines.append("")
    report_lines.append(f"**分析时间**: 2026-03-16")
    report_lines.append(f"**分析文件总数**: {total} 个 Markdown 文件")
    report_lines.append("")
    
    # 统计摘要
    report_lines.append("## 一、统计摘要")
    report_lines.append("")
    report_lines.append("### 1.1 文件大小分布")
    report_lines.append("")
    report_lines.append("| 大小范围 | 文件数量 | 占比 |")
    report_lines.append("|----------|----------|------|")
    for group_name, files in size_groups.items():
        percentage = round(len(files) / total * 100, 1)
        report_lines.append(f"| {group_name} | {len(files)} | {percentage}% |")
    report_lines.append("")
    
    # 结构要素覆盖率
    report_lines.append("### 1.2 内容结构要素覆盖率")
    report_lines.append("")
    report_lines.append("| 结构要素 | 覆盖文件数 | 覆盖率 |")
    report_lines.append("|----------|------------|--------|")
    for element, count in structure_coverage.items():
        percentage = round(count / total * 100, 1)
        report_lines.append(f"| {element} | {count} | {percentage}% |")
    report_lines.append("")
    
    # 评分分布
    score_distribution = {
        '优秀 (80-100)': sum(1 for r in results if 80 <= r.structure_score <= 100),
        '良好 (60-79)': sum(1 for r in results if 60 <= r.structure_score < 80),
        '一般 (40-59)': sum(1 for r in results if 40 <= r.structure_score < 60),
        '薄弱 (20-39)': sum(1 for r in results if 20 <= r.structure_score < 40),
        '需重点加强 (0-19)': sum(1 for r in results if 0 <= r.structure_score < 20),
    }
    
    report_lines.append("### 1.3 内容质量评分分布")
    report_lines.append("")
    report_lines.append("| 评分等级 | 文件数量 | 占比 |")
    report_lines.append("|----------|----------|------|")
    for level, count in score_distribution.items():
        percentage = round(count / total * 100, 1)
        report_lines.append(f"| {level} | {count} | {percentage}% |")
    report_lines.append("")
    
    # 模块平均评分
    report_lines.append("### 1.4 各模块平均评分")
    report_lines.append("")
    report_lines.append("| 模块 | 文件数 | 平均评分 |")
    report_lines.append("|------|--------|----------|")
    for module in sorted(module_avg_scores.keys()):
        score = module_avg_scores[module]
        count = len(module_scores[module])
        report_lines.append(f"| {module} | {count} | {score} |")
    report_lines.append("")
    
    # 最薄弱的30个文件
    report_lines.append("## 二、内容最薄弱的30个文件")
    report_lines.append("")
    report_lines.append("以下文件按结构完整度评分从低到高排序：")
    report_lines.append("")
    report_lines.append("| 排名 | 文件路径 | 大小 | 评分 | 缺少的要素 |")
    report_lines.append("|------|----------|------|------|------------|")
    
    for i, f in enumerate(weak_files, 1):
        rel_path = os.path.relpath(f.path, 'knowledge')
        missing = ', '.join(f.get_missing_elements()) if f.get_missing_elements() else '无'
        report_lines.append(f"| {i} | `{rel_path}` | {f.size_kb}KB | {f.structure_score} | {missing} |")
    
    report_lines.append("")
    
    # 详细薄弱文件分析
    report_lines.append("### 2.1 薄弱文件详细分析")
    report_lines.append("")
    
    for i, f in enumerate(weak_files, 1):
        rel_path = os.path.relpath(f.path, 'knowledge')
        report_lines.append(f"#### {i}. `{rel_path}`")
        report_lines.append("")
        report_lines.append(f"- **文件大小**: {f.size_kb} KB ({f.line_count} 行)")
        report_lines.append(f"- **结构评分**: {f.structure_score}/100")
        report_lines.append(f"- **代码块**: {f.code_block_count} 个 ({f.code_block_lines} 行)")
        report_lines.append(f"- **表格**: {f.table_count} 个")
        report_lines.append(f"- **标题**: {f.heading_count} 个")
        report_lines.append("")
        report_lines.append("**结构要素检查**:")
        report_lines.append("")
        report_lines.append(f"- [x] 概念定义" if f.has_concept_definition else f"- [ ] 概念定义")
        report_lines.append(f"- [x] 属性说明" if f.has_property_description else f"- [ ] 属性说明")
        report_lines.append(f"- [x] 示例代码" if f.has_example_code else f"- [ ] 示例代码")
        report_lines.append(f"- [x] 反例/陷阱" if f.has_anti_example else f"- [ ] 反例/陷阱")
        report_lines.append(f"- [x] 形式化描述" if f.has_formal_description else f"- [ ] 形式化描述")
        report_lines.append(f"- [x] 思维导图/图表" if f.has_diagram else f"- [ ] 思维导图/图表")
        report_lines.append("")
        
        missing = f.get_missing_elements()
        if missing:
            report_lines.append(f"**建议补充**: {', '.join(missing)}")
        report_lines.append("")
    
    # 需要优先加强的模块
    report_lines.append("## 三、需要优先加强的模块建议")
    report_lines.append("")
    
    if weak_modules:
        report_lines.append("### 3.1 薄弱模块（平均评分 < 40）")
        report_lines.append("")
        for module, score in weak_modules:
            count = len(module_scores[module])
            report_lines.append(f"- **{module}**: 平均评分 {score} 分（{count} 个文件）")
        report_lines.append("")
    
    # 生成模块详细建议
    report_lines.append("### 3.2 模块加强建议")
    report_lines.append("")
    
    # 小文件最多的模块
    small_file_modules = defaultdict(int)
    for r in results:
        if r.size_kb < 3:
            rel_path = os.path.relpath(r.path, 'knowledge')
            parts = rel_path.split(os.sep)
            if len(parts) > 0:
                small_file_modules[parts[0]] += 1
    
    report_lines.append("#### 小文件集中模块（< 3KB）")
    report_lines.append("")
    for module, count in sorted(small_file_modules.items(), key=lambda x: -x[1])[:10]:
        report_lines.append(f"- **{module}**: {count} 个小文件")
    report_lines.append("")
    
    # 缺少示例代码的模块
    no_example_modules = defaultdict(int)
    for r in results:
        if not r.has_example_code:
            rel_path = os.path.relpath(r.path, 'knowledge')
            parts = rel_path.split(os.sep)
            if len(parts) > 0:
                no_example_modules[parts[0]] += 1
    
    report_lines.append("#### 缺少示例代码的模块")
    report_lines.append("")
    for module, count in sorted(no_example_modules.items(), key=lambda x: -x[1])[:10]:
        report_lines.append(f"- **{module}**: {count} 个文件无示例代码")
    report_lines.append("")
    
    # 综合建议
    report_lines.append("### 3.3 综合改进建议")
    report_lines.append("")
    report_lines.append("1. **优先补充示例代码**: 示例代码是最核心的学习要素，应优先为缺少代码的文件补充")
    report_lines.append("2. **增加反例/陷阱说明**: 帮助学习者避免常见错误")
    report_lines.append("3. **添加概念定义**: 确保每个主题都有清晰的定义部分")
    report_lines.append("4. **丰富可视化内容**: 添加思维导图、流程图等帮助理解")
    report_lines.append("5. **扩充小文件内容**: 小于 3KB 的文件内容过于简略，需要重点扩充")
    report_lines.append("")
    
    # 写入报告
    with open(output_path, 'w', encoding='utf-8') as f:
        f.write('\n'.join(report_lines))
    
    print(f"\nReport saved to: {output_path}")
    
    # 同时生成 JSON 数据供进一步分析
    json_data = {
        'summary': {
            'total_files': total,
            'size_distribution': {k: len(v) for k, v in size_groups.items()},
            'structure_coverage': structure_coverage,
            'score_distribution': score_distribution,
            'module_avg_scores': module_avg_scores
        },
        'weak_files': [
            {
                'path': os.path.relpath(f.path, 'knowledge'),
                'size_kb': f.size_kb,
                'structure_score': f.structure_score,
                'missing_elements': f.get_missing_elements(),
                'line_count': f.line_count,
                'code_block_count': f.code_block_count,
                'heading_count': f.heading_count
            }
            for f in weak_files
        ]
    }
    
    json_path = output_path.replace('.md', '.json')
    with open(json_path, 'w', encoding='utf-8') as f:
        json.dump(json_data, f, indent=2, ensure_ascii=False)
    
    print(f"JSON data saved to: {json_path}")


def main():
    base_dir = 'knowledge'
    output_path = 'knowledge\content_quality_report.md'
    
    print("=" * 60)
    print("C_Lang Knowledge Base Content Quality Analyzer")
    print("=" * 60)
    print()
    
    results = analyze_directory(base_dir)
    
    print(f"\nAnalysis complete. Analyzed {len(results)} files.")
    
    generate_report(results, output_path)
    
    # 打印简要统计
    print("\n" + "=" * 60)
    print("简要统计")
    print("=" * 60)
    
    size_groups = {
        '< 5KB': sum(1 for r in results if r.size_kb < 5),
        '5-10KB': sum(1 for r in results if 5 <= r.size_kb < 10),
        '10-20KB': sum(1 for r in results if 10 <= r.size_kb < 20),
        '> 20KB': sum(1 for r in results if r.size_kb >= 20)
    }
    
    print("\n文件大小分布:")
    for group, count in size_groups.items():
        print(f"  {group}: {count} 个文件")
    
    avg_score = sum(r.structure_score for r in results) / len(results)
    print(f"\n平均结构评分: {avg_score:.1f}/100")
    
    weak_count = sum(1 for r in results if r.structure_score < 40)
    print(f"薄弱文件数量 (< 40分): {weak_count} 个")


if __name__ == '__main__':
    main()
