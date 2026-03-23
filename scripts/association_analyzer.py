#!/usr/bin/env python3
"""
知识库关联密度分析器
分析知识库的关联密度和网络特征
"""

import os
import re
import json
from pathlib import Path
from collections import defaultdict
from dataclasses import dataclass, asdict
from typing import List, Dict, Set, Tuple


@dataclass
class DocumentStats:
    """文档统计"""
    path: str
    total_lines: int
    external_links: int
    internal_links: int
    outgoing_links: List[str]
    isolation_score: float  # 0-1, 越高越孤立


class AssociationAnalyzer:
    """知识库关联分析器"""
    
    def __init__(self, knowledge_dir: str = "knowledge"):
        self.knowledge_dir = Path(knowledge_dir)
        self.doc_stats: List[DocumentStats] = []
        self.link_graph: Dict[str, Set[str]] = defaultdict(set)
        self.reverse_graph: Dict[str, Set[str]] = defaultdict(set)
        
    def analyze_all_documents(self):
        """分析所有文档"""
        print("🔍 分析所有文档的关联...")
        
        md_files = list(self.knowledge_dir.rglob("*.md"))
        total_files = len(md_files)
        
        for i, md_file in enumerate(md_files, 1):
            if i % 50 == 0:
                print(f"   已分析 {i}/{total_files}...")
                
            self._analyze_single_document(md_file)
            
    def _analyze_single_document(self, md_file: Path):
        """分析单个文档"""
        relative_path = str(md_file.relative_to(self.knowledge_dir))
        content = md_file.read_text(encoding='utf-8')
        lines = content.split('\n')
        
        # 提取链接
        pattern = r'\[([^\]]+)\]\(([^)]+)\)'
        links = re.findall(pattern, content)
        
        external_links = 0
        internal_links = 0
        outgoing = []
        
        for link_text, link_target in links:
            # 跳过外部链接
            if link_target.startswith(('http://', 'https://', 'mailto:')):
                external_links += 1
                continue
                
            # 跳过锚点
            target_file = link_target.split('#')[0]
            if not target_file:
                continue
                
            internal_links += 1
            
            # 解析目标路径
            source_dir = md_file.parent
            if target_file.startswith('/'):
                target_path = target_file[1:]
            else:
                try:
                    full_target = (source_dir / target_file).resolve()
                    target_path = str(full_target.relative_to(self.knowledge_dir.resolve()))
                except:
                    target_path = target_file
                    
            outgoing.append(target_path)
            self.link_graph[relative_path].add(target_path)
            self.reverse_graph[target_path].add(relative_path)
            
        # 计算孤立度
        isolation_score = 1.0 if internal_links == 0 else max(0, 1.0 - (internal_links / 10))
        
        stats = DocumentStats(
            path=relative_path,
            total_lines=len(lines),
            external_links=external_links,
            internal_links=internal_links,
            outgoing_links=outgoing,
            isolation_score=isolation_score
        )
        
        self.doc_stats.append(stats)
        
    def print_report(self):
        """打印分析报告"""
        print("\n" + "=" * 70)
        print("📊 知识库关联密度分析报告")
        print("=" * 70)
        
        total_docs = len(self.doc_stats)
        total_links = sum(s.internal_links for s in self.doc_stats)
        avg_links = total_links / total_docs if total_docs > 0 else 0
        
        # 孤立文档统计
        isolated_docs = [s for s in self.doc_stats if s.internal_links == 0]
        low_connected = [s for s in self.doc_stats if 1 <= s.internal_links < 3]
        well_connected = [s for s in self.doc_stats if s.internal_links >= 3]
        
        print(f"\n📁 文档统计:")
        print(f"   总文档数: {total_docs}")
        print(f"   总内部链接: {total_links}")
        print(f"   平均每个文档链接数: {avg_links:.2f}")
        
        print(f"\n🔗 关联健康度:")
        print(f"   完全孤立文档: {len(isolated_docs)} ({len(isolated_docs)/total_docs*100:.1f}%)")
        print(f"   低关联文档(1-2链接): {len(low_connected)} ({len(low_connected)/total_docs*100:.1f}%)")
        print(f"   良好关联文档(≥3链接): {len(well_connected)} ({len(well_connected)/total_docs*100:.1f}%)")
        
        # 网络密度
        possible_links = total_docs * (total_docs - 1)
        actual_unique_links = len(set(
            (src, dst) for src, dsts in self.link_graph.items() for dst in dsts
        ))
        network_density = actual_unique_links / possible_links if possible_links > 0 else 0
        
        print(f"\n🕸️ 网络特征:")
        print(f"   网络密度: {network_density:.4f} ({actual_unique_links}/{possible_links})")
        print(f"   唯一链接数: {actual_unique_links}")
        
        # 最孤立文档
        print(f"\n⚠️ 最孤立的10个文档:")
        most_isolated = sorted(self.doc_stats, key=lambda x: x.isolation_score, reverse=True)[:10]
        for i, stats in enumerate(most_isolated, 1):
            print(f"   {i}. {stats.path}")
            print(f"      行数: {stats.total_lines}, 内部链接: {stats.internal_links}")
            
        # 最连接文档
        print(f"\n✨ 连接最丰富的10个文档:")
        most_connected = sorted(self.doc_stats, key=lambda x: x.internal_links, reverse=True)[:10]
        for i, stats in enumerate(most_connected, 1):
            print(f"   {i}. {stats.path}")
            print(f"      内部链接: {stats.internal_links}")
            
        # 入度分析（被引用次数）
        in_degrees = defaultdict(int)
        for src, dsts in self.link_graph.items():
            for dst in dsts:
                in_degrees[dst] += 1
                
        print(f"\n📥 被引用最多的10个文档:")
        most_cited = sorted(in_degrees.items(), key=lambda x: x[1], reverse=True)[:10]
        for i, (path, count) in enumerate(most_cited, 1):
            print(f"   {i}. {path} (被引用{count}次)")
            
        print("\n" + "=" * 70)
        
    def export_data(self, output_prefix: str = "association_analysis"):
        """导出分析数据"""
        # 导出文档统计
        stats_data = [asdict(s) for s in self.doc_stats]
        with open(f"{output_prefix}_stats.json", 'w', encoding='utf-8') as f:
            json.dump(stats_data, f, indent=2, ensure_ascii=False)
            
        # 导出链接图
        graph_data = {k: list(v) for k, v in self.link_graph.items()}
        with open(f"{output_prefix}_graph.json", 'w', encoding='utf-8') as f:
            json.dump(graph_data, f, indent=2, ensure_ascii=False)
            
        print(f"\n📝 分析数据已导出:")
        print(f"   - {output_prefix}_stats.json")
        print(f"   - {output_prefix}_graph.json")
        
    def suggest_associations(self, min_similarity: int = 3) -> List[Tuple[str, str, str]]:
        """建议可能的关联"""
        print("\n💡 正在分析可能的关联...")
        suggestions = []
        
        # 基于目录结构建议关联
        dir_groups = defaultdict(list)
        for stats in self.doc_stats:
            dir_path = str(Path(stats.path).parent)
            dir_groups[dir_path].append(stats.path)
            
        # 同一目录下的文档应该有关联
        for dir_path, files in dir_groups.items():
            if len(files) > 1:
                for i, f1 in enumerate(files):
                    for f2 in files[i+1:]:
                        if f2 not in self.link_graph.get(f1, set()):
                            suggestions.append((f1, f2, "同目录文档"))
                            
        print(f"   找到 {len(suggestions)} 个可能的关联建议")
        return suggestions[:50]  # 返回前50个


def main():
    analyzer = AssociationAnalyzer()
    
    print("🔧 知识库关联密度分析器")
    print("=" * 70)
    
    # 分析所有文档
    analyzer.analyze_all_documents()
    
    # 打印报告
    analyzer.print_report()
    
    # 建议关联
    suggestions = analyzer.suggest_associations()
    
    # 导出数据
    analyzer.export_data()
    
    print("\n✅ 分析完成！")


if __name__ == "__main__":
    main()
