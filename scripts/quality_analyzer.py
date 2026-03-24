#!/usr/bin/env python3
"""
C_Lang知识库质量分析器
用于扫描TODO项、H1标题过多问题和元数据缺失
"""

import os
import re
import json
from datetime import datetime
from pathlib import Path
from collections import defaultdict

class QualityAnalyzer:
    def __init__(self, knowledge_path):
        self.knowledge_path = Path(knowledge_path)
        self.todos = []
        self.h1_issues = []
        self.metadata_issues = []
        self.stats = {
            'total_files': 0,
            'files_with_todos': 0,
            'total_todos': 0,
            'files_with_h1_issues': 0,
            'total_h1_excess': 0,
            'files_without_metadata': 0
        }
    
    def scan_all_files(self):
        """扫描所有markdown文件"""
        for md_file in self.knowledge_path.rglob("*.md"):
            self.stats['total_files'] += 1
            self._analyze_file(md_file)
    
    def _analyze_file(self, filepath):
        """分析单个文件"""
        try:
            content = filepath.read_text(encoding='utf-8')
            lines = content.split('\n')
        except Exception as e:
            print(f"Error reading {filepath}: {e}")
            return
        
        relative_path = filepath.relative_to(self.knowledge_path)
        
        # 分析TODO/FIXME/XXX
        todo_patterns = [
            (r'TODO[:\s]', 'TODO'),
            (r'FIXME[:\s]', 'FIXME'),
            (r'XXX[:\s]', 'XXX'),
            (r'HACK[:\s]', 'HACK'),
            (r'- \[ \] TODO', 'Checkbox TODO')
        ]
        
        file_todos = []
        for i, line in enumerate(lines, 1):
            for pattern, todo_type in todo_patterns:
                if re.search(pattern, line, re.IGNORECASE):
                    file_todos.append({
                        'line': i,
                        'content': line.strip(),
                        'type': todo_type
                    })
        
        if file_todos:
            self.stats['files_with_todos'] += 1
            self.stats['total_todos'] += len(file_todos)
            self.todos.append({
                'file': str(relative_path),
                'count': len(file_todos),
                'items': file_todos
            })
        
        # 分析H1标题数量
        h1_count = len(re.findall(r'^# [^#]', content, re.MULTILINE))
        if h1_count > 50:
            self.stats['files_with_h1_issues'] += 1
            self.stats['total_h1_excess'] += h1_count - 1  # 超过1个的都算问题
            self.h1_issues.append({
                'file': str(relative_path),
                'h1_count': h1_count,
                'severity': 'critical' if h1_count > 200 else 'high' if h1_count > 100 else 'medium'
            })
        
        # 分析元数据
        has_metadata = self._check_metadata(content)
        if not has_metadata:
            self.stats['files_without_metadata'] += 1
            self.metadata_issues.append(str(relative_path))
    
    def _check_metadata(self, content):
        """检查文件是否有标准元数据"""
        # 检查是否有元数据标识，如版本、最后更新、作者等
        metadata_patterns = [
            r'版本[:：]',
            r'Version[:\s]',
            r'最后更新[:：]',
            r'Last Updated[:\s]',
            r'作者[:：]',
            r'Author[:\s]',
            r'更新时间[:：]',
            r'更新日期[:：]'
        ]
        for pattern in metadata_patterns:
            if re.search(pattern, content, re.IGNORECASE):
                return True
        return False
    
    def generate_todo_backlog(self):
        """生成TODO跟踪文件内容"""
        lines = [
            "# TODO Backlog - C_Lang知识库质量优化",
            "",
            f"> **生成时间**: {datetime.now().strftime('%Y-%m-%d %H:%M:%S')}",
            f"> **统计**: 共发现 {self.stats['total_todos']} 个TODO项，分布在 {self.stats['files_with_todos']} 个文件中",
            "",
            "---",
            "",
            "## 📊 优先级分类",
            "",
            "### 🔴 高优先级 (需在2026-04-15前完成)",
            "",
        ]
        
        # 按优先级排序
        high_priority = [t for t in self.todos if t['count'] >= 10]
        medium_priority = [t for t in self.todos if 3 <= t['count'] < 10]
        low_priority = [t for t in self.todos if t['count'] < 3]
        
        for item in high_priority:
            lines.append(f"- [ ] **{item['file']}** ({item['count']}个TODO) - 截止日期: 2026-04-15")
            for todo in item['items'][:5]:  # 只显示前5个
                lines.append(f"  - 行{todo['line']}: `{todo['content'][:60]}...`")
            if len(item['items']) > 5:
                lines.append(f"  - ... 还有 {len(item['items']) - 5} 个")
            lines.append("")
        
        lines.extend([
            "### 🟡 中优先级 (需在2026-04-30前完成)",
            "",
        ])
        
        for item in medium_priority:
            lines.append(f"- [ ] **{item['file']}** ({item['count']}个TODO) - 截止日期: 2026-04-30")
        
        lines.extend([
            "",
            "### 🟢 低优先级 (需在2026-05-15前完成)",
            "",
        ])
        
        for item in low_priority:
            lines.append(f"- [ ] **{item['file']}** ({item['count']}个TODO) - 截止日期: 2026-05-15")
        
        lines.extend([
            "",
            "---",
            "",
            "## 📝 详细TODO清单",
            "",
        ])
        
        # 按文件列出所有TODO
        for item in sorted(self.todos, key=lambda x: x['count'], reverse=True):
            lines.append(f"### {item['file']}")
            lines.append("")
            for todo in item['items']:
                lines.append(f"- [ ] **行{todo['line']}** [{todo['type']}] {todo['content'][:100]}")
            lines.append("")
        
        return '\n'.join(lines)
    
    def generate_quality_report(self):
        """生成质量优化报告"""
        report = {
            'report_title': 'C_Lang知识库质量优化报告',
            'generated_at': datetime.now().strftime('%Y-%m-%d %H:%M:%S'),
            'executive_summary': {
                'total_files_scanned': self.stats['total_files'],
                'files_with_issues': len(set([t['file'] for t in self.todos] + [h['file'] for h in self.h1_issues])),
                'total_todos_found': self.stats['total_todos'],
                'files_with_h1_issues': self.stats['files_with_h1_issues'],
                'files_without_metadata': self.stats['files_without_metadata']
            },
            'todo_statistics': {
                'before_fix': self.stats['total_todos'],
                'after_fix': 0,  # 将在修复后更新
                'fix_rate': 0,
                'high_priority_files': len([t for t in self.todos if t['count'] >= 10]),
                'medium_priority_files': len([t for t in self.todos if 3 <= t['count'] < 10]),
                'low_priority_files': len([t for t in self.todos if t['count'] < 3])
            },
            'structure_optimization': {
                'files_restructured': [],
                'h1_issues_resolved': 0,
                'restructure_list': sorted(self.h1_issues, key=lambda x: x['h1_count'], reverse=True)
            },
            'metadata_standardization': {
                'files_updated': [],
                'template_applied': 0
            },
            'quality_score': {
                'before': self._calculate_quality_score(),
                'after': 0,  # 修复后更新
                'improvement': 0
            }
        }
        return report
    
    def _calculate_quality_score(self):
        """计算质量评分 (0-100)"""
        base_score = 100
        # 每个TODO扣1分，最多扣30分
        base_score -= min(self.stats['total_todos'] * 0.5, 30)
        # 每个H1问题文件扣2分，最多扣30分
        base_score -= min(self.stats['files_with_h1_issues'] * 2, 30)
        # 每个缺少元数据的文件扣0.1分，最多扣20分
        base_score -= min(self.stats['files_without_metadata'] * 0.1, 20)
        return max(0, round(base_score, 1))


def main():
    analyzer = QualityAnalyzer('knowledge')
    print("🔍 正在扫描知识库文件...")
    analyzer.scan_all_files()
    
    print(f"✅ 扫描完成: {analyzer.stats['total_files']} 个文件")
    print(f"📋 发现 {analyzer.stats['total_todos']} 个TODO项")
    print(f"⚠️ 发现 {analyzer.stats['files_with_h1_issues']} 个H1标题过多文件")
    
    # 生成TODO_BACKLOG.md
    todo_content = analyzer.generate_todo_backlog()
    output_dir = Path('knowledge/00_VERSION_TRACKING')
    output_dir.mkdir(parents=True, exist_ok=True)
    (output_dir / 'TODO_BACKLOG.md').write_text(todo_content, encoding='utf-8')
    print(f"📝 已生成: {output_dir / 'TODO_BACKLOG.md'}")
    
    # 生成报告
    report = analyzer.generate_quality_report()
    
    # 保存为JSON
    (output_dir / 'quality_report.json').write_text(
        json.dumps(report, indent=2, ensure_ascii=False),
        encoding='utf-8'
    )
    
    # 生成Markdown报告
    report_md = generate_markdown_report(report)
    (output_dir / 'QUALITY_OPTIMIZATION_REPORT.md').write_text(report_md, encoding='utf-8')
    print(f"📝 已生成: {output_dir / 'QUALITY_OPTIMIZATION_REPORT.md'}")
    
    return analyzer, report


def generate_markdown_report(report):
    """生成Markdown格式的报告"""
    summary = report['executive_summary']
    todo_stats = report['todo_statistics']
    structure = report['structure_optimization']
    
    lines = [
        "# QUALITY_OPTIMIZATION_REPORT.md",
        "",
        f"> **生成时间**: {report['generated_at']}",
        "> **报告状态**: 修复前基线",
        "",
        "---",
        "",
        "## 📊 执行摘要",
        "",
        f"| 指标 | 数值 |",
        f"|------|------|",
        f"| 扫描文件总数 | {summary['total_files_scanned']} |",
        f"| 存在问题的文件 | {summary['files_with_issues']} |",
        f"| 发现TODO总数 | {summary['total_todos_found']} |",
        f"| H1标题过多文件 | {summary['files_with_h1_issues']} |",
        f"| 缺少元数据文件 | {summary['files_without_metadata']} |",
        "",
        "---",
        "",
        "## 📋 TODO统计",
        "",
        "### 修复前后对比",
        "",
        f"| 状态 | TODO数量 | 占比 |",
        f"|------|----------|------|",
        f"| 🔴 修复前 | {todo_stats['before_fix']} | 100% |",
        f"| 🟡 修复后 | {todo_stats['after_fix']} | {todo_stats['fix_rate']}% |",
        f"| 🟢 修复数量 | {todo_stats['before_fix'] - todo_stats['after_fix']} | {todo_stats['fix_rate']}% |",
        "",
        "### 优先级分布",
        "",
        f"- **高优先级** (≥10个TODO): {todo_stats['high_priority_files']} 个文件",
        f"- **中优先级** (3-9个TODO): {todo_stats['medium_priority_files']} 个文件",
        f"- **低优先级** (1-2个TODO): {todo_stats['low_priority_files']} 个文件",
        "",
        "---",
        "",
        "## 🏗️ 结构优化清单",
        "",
        f"### H1标题过多文档 (>50个H1) - 共{len(structure['restructure_list'])}个",
        "",
        "| 文件路径 | H1数量 | 严重程度 | 优化状态 |",
        "|----------|--------|----------|----------|",
    ]
    
    for item in structure['restructure_list'][:30]:  # 显示前30个
        status = "✅ 已修复" if item['file'] in structure['files_restructured'] else "⏳ 待修复"
        lines.append(f"| {item['file']} | {item['h1_count']} | {item['severity']} | {status} |")
    
    if len(structure['restructure_list']) > 30:
        lines.append(f"| ... 还有 {len(structure['restructure_list']) - 30} 个文件 | - | - | - |")
    
    lines.extend([
        "",
        "### 优化策略",
        "",
        "1. **H1 → H2/H3转换**: 将除主标题外的所有H1转换为H2或H3",
        "2. **层级重组**: 建立清晰的文档层级结构",
        "3. **目录更新**: 同步更新文档目录链接",
        "",
        "---",
        "",
        "## 📈 质量评分变化",
        "",
        f"| 阶段 | 评分 | 变化 |",
        f"|------|------|------|",
        f"| 修复前 | {report['quality_score']['before']} | - |",
        f"| 修复后 | {report['quality_score']['after']} | +{report['quality_score']['improvement']} |",
        "",
        "### 评分标准",
        "",
        "- 基础分: 100分",
        "- 每个TODO扣0.5分 (上限30分)",
        "- 每个H1问题文件扣2分 (上限30分)",
        "- 每个缺少元数据文件扣0.1分 (上限20分)",
        "",
        "---",
        "",
        "## 📝 元数据标准化",
        "",
        f"### 标准化模板",
        "",
        "```markdown",
        "---",
        "版本: v1.0.0",
        "最后更新: 2026-03-25",
        "作者: C_Lang Team",
        "分类: Core Knowledge",
        "难度: L3 进阶",
        "预计学习时间: 2小时",
        "---",
        "```",
        "",
        f"### 待标准化文件数: {report['executive_summary']['files_without_metadata']}",
        "",
        "---",
        "",
        "## 🎯 后续行动计划",
        "",
        "1. **短期 (1-2周)**:",
        "   - [ ] 修复所有高优先级TODO",
        "   - [ ] 重组最严重的前10个H1过多文档",
        "",
        "2. **中期 (1个月)**:",
        "   - [ ] 修复50%以上的TODO",
        "   - [ ] 完成所有H1过多文档的重组",
        "   - [ ] 为关键文档添加元数据",
        "",
        "3. **长期 (3个月)**:",
        "   - [ ] 完成所有TODO清理",
        "   - [ ] 建立文档质量检查自动化",
        "   - [ ] 质量评分达到90分以上",
        "",
        "---",
        "",
        "*报告由质量优化工具自动生成*",
    ])
    
    return '\n'.join(lines)


if __name__ == '__main__':
    main()
