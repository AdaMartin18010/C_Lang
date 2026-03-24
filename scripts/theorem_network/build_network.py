#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
构建定理依赖网络并生成可视化
"""

import json
import random
from pathlib import Path
from collections import defaultdict
from dataclasses import dataclass, field
from typing import Dict, List, Set, Tuple

@dataclass
class NetworkNode:
    """网络节点"""
    id: str
    name: str
    category: str
    theorem_type: str
    description: str
    dependencies: List[str] = field(default_factory=list)
    used_by: List[str] = field(default_factory=list)
    depth: int = 0  # 在网络中的深度
    centrality: float = 0.0  # 中心性度量

@dataclass
class NetworkEdge:
    """网络边"""
    source: str
    target: str
    weight: float = 1.0
    edge_type: str = "depends_on"

class TheoremNetwork:
    """定理依赖网络"""
    
    # 248个核心定理的选择标准
    CORE_CATEGORIES = {
        '桥梁定理': ['BT-1', 'BT-2', 'BT-3', 'BT-4', 'BT-5'],
        '类型理论': ['MLTT', 'CoC', 'HoTT'],
        '操作语义': ['preservation', 'progress', 'safety', 'soundness'],
        '指称语义': ['continuity', 'fixedpoint', 'domain'],
        '公理语义': ['hoare', 'wp', 'vcg'],
        '分布式系统': ['raft', 'paxos', 'election', 'consensus'],
        '编译器': ['closure', 'tco', 'rc', 'ffi'],
    }
    
    def __init__(self, theorems_data: Dict):
        self.theorems = theorems_data.get('theorems', {})
        self.nodes: Dict[str, NetworkNode] = {}
        self.edges: List[NetworkEdge] = []
        self.category_colors = {
            '桥梁定理': '#e74c3c',
            '类型理论': '#3498db',
            '操作语义': '#2ecc71',
            '指称语义': '#9b59b6',
            '公理语义': '#f39c12',
            '分布式系统': '#1abc9c',
            '编译器理论': '#e91e63',
            '并发系统': '#ff5722',
            '类型系统': '#3f51b5',
            '共代数': '#795548',
            '博弈语义': '#607d8b',
            '线性逻辑': '#00bcd4',
            '范畴语义': '#8bc34a',
            '其他': '#9e9e9e',
        }
        
    def select_core_theorems(self, target_count: int = 248) -> List[str]:
        """选择核心定理"""
        selected = []
        
        # 优先选择桥梁定理及其辅助引理
        bridge_theorems = [k for k in self.theorems.keys() if 'BRIDGE::' in k or 'BT-' in k]
        selected.extend(bridge_theorems[:15])
        
        # 分布式系统核心定理
        distributed = [k for k in self.theorems.keys() 
                      if any(x in k.lower() for x in ['raft', 'election', 'safety', 'consensus', 'distributed'])
                      and k not in selected]
        selected.extend(distributed[:20])
        
        # 操作语义核心定理
        operational = [k for k in self.theorems.keys() 
                      if any(x in k.lower() for x in ['preservation', 'progress', 'safety', 'soundness', 'operational'])
                      and k not in selected]
        selected.extend(operational[:20])
        
        # 类型理论
        type_theory = [k for k in self.theorems.keys() 
                      if any(x in k.lower() for x in ['mltt', 'coc', 'type', 'typetheory'])
                      and k not in selected]
        selected.extend(type_theory[:25])
        
        # 公理语义
        axiomatic = [k for k in self.theorems.keys() 
                    if any(x in k.lower() for x in ['hoare', 'wp', 'axiomatic', 'verification'])
                    and k not in selected]
        selected.extend(axiomatic[:25])
        
        # 指称语义
        denotational = [k for k in self.theorems.keys() 
                       if any(x in k.lower() for x in ['denotational', 'continuity', 'fixedpoint', 'domain'])
                       and k not in selected]
        selected.extend(denotational[:20])
        
        # 编译器
        compiler = [k for k in self.theorems.keys() 
                   if any(x in k.lower() for x in ['closure', 'tco', 'rc', 'ffi', 'compiler'])
                   and k not in selected]
        selected.extend(compiler[:15])
        
        # 填充剩余
        remaining = [k for k in self.theorems.keys() if k not in selected]
        random.shuffle(remaining)
        needed = target_count - len(selected)
        selected.extend(remaining[:needed])
        
        return selected[:target_count]
    
    def build_network(self, core_theorem_ids: List[str]):
        """构建网络"""
        # 创建节点
        for th_id in core_theorem_ids:
            if th_id in self.theorems:
                th = self.theorems[th_id]
                self.nodes[th_id] = NetworkNode(
                    id=th_id,
                    name=th.get('name', th_id),
                    category=th.get('category', '其他'),
                    theorem_type=th.get('theorem_type', 'unknown'),
                    description=th.get('description', ''),
                    dependencies=th.get('dependencies', []),
                    used_by=th.get('used_by', [])
                )
        
        # 创建边
        for node_id, node in self.nodes.items():
            for dep in node.dependencies:
                # 查找依赖的完整ID
                dep_id = self._find_theorem_id(dep)
                if dep_id and dep_id in self.nodes:
                    self.edges.append(NetworkEdge(
                        source=node_id,
                        target=dep_id,
                        weight=1.0,
                        edge_type='depends_on'
                    ))
        
        # 计算深度和中心性
        self._compute_depths()
        self._compute_centrality()
    
    def _find_theorem_id(self, partial_id: str) -> str:
        """根据部分ID查找完整ID"""
        for key in self.nodes:
            if key.endswith(f"::{partial_id}") or key == f"BRIDGE::{partial_id}":
                return key
        return None
    
    def _compute_depths(self):
        """计算每个节点的深度（最长依赖链）"""
        def get_depth(node_id: str, visited: Set[str]) -> int:
            if node_id in visited:
                return 0
            if node_id not in self.nodes:
                return 0
            
            visited.add(node_id)
            node = self.nodes[node_id]
            
            if not node.dependencies:
                return 0
            
            max_dep_depth = 0
            for dep in node.dependencies:
                dep_id = self._find_theorem_id(dep)
                if dep_id and dep_id not in visited:
                    max_dep_depth = max(max_dep_depth, get_depth(dep_id, visited.copy()))
            
            return max_dep_depth + 1
        
        for node_id in self.nodes:
            self.nodes[node_id].depth = get_depth(node_id, set())
    
    def _compute_centrality(self):
        """计算简单的度中心性"""
        for node_id, node in self.nodes.items():
            in_degree = len([e for e in self.edges if e.target == node_id])
            out_degree = len([e for e in self.edges if e.source == node_id])
            node.centrality = in_degree + out_degree
    
    def identify_critical_path(self) -> List[str]:
        """识别关键路径（最长的依赖链）"""
        def find_longest_path(start: str, visited: Set[str]) -> List[str]:
            if start in visited or start not in self.nodes:
                return [start]
            
            visited.add(start)
            node = self.nodes[start]
            
            longest = [start]
            for dep in node.dependencies:
                dep_id = self._find_theorem_id(dep)
                if dep_id:
                    path = find_longest_path(dep_id, visited.copy())
                    if len(path) + 1 > len(longest):
                        longest = [start] + path
            
            return longest
        
        # 找到最长的路径
        longest = []
        for node_id in self.nodes:
            path = find_longest_path(node_id, set())
            if len(path) > len(longest):
                longest = path
        
        return longest
    
    def identify_isolated_nodes(self) -> List[str]:
        """识别孤立节点（无依赖也无被依赖）"""
        isolated = []
        for node_id, node in self.nodes.items():
            in_degree = len([e for e in self.edges if e.target == node_id])
            out_degree = len([e for e in self.edges if e.source == node_id])
            if in_degree == 0 and out_degree == 0:
                isolated.append(node_id)
        return isolated
    
    def get_statistics(self) -> Dict:
        """获取网络统计信息"""
        stats = {
            'total_nodes': len(self.nodes),
            'total_edges': len(self.edges),
            'by_category': defaultdict(int),
            'avg_dependencies': 0,
            'max_depth': 0,
            'critical_path_length': 0,
            'isolated_nodes': len(self.identify_isolated_nodes()),
            'most_central': [],
        }
        
        for node in self.nodes.values():
            stats['by_category'][node.category] += 1
            stats['max_depth'] = max(stats['max_depth'], node.depth)
        
        if self.nodes:
            total_deps = sum(len(n.dependencies) for n in self.nodes.values())
            stats['avg_dependencies'] = total_deps / len(self.nodes)
        
        critical_path = self.identify_critical_path()
        stats['critical_path_length'] = len(critical_path)
        
        # 最中心的节点
        central = sorted(self.nodes.values(), key=lambda n: n.centrality, reverse=True)[:10]
        stats['most_central'] = [(n.id, n.name, n.centrality) for n in central]
        
        return stats
    
    def generate_mermaid_full(self) -> str:
        """生成完整的Mermaid图"""
        lines = ['graph TD']
        
        # 添加节点定义
        for node_id, node in self.nodes.items():
            short_id = node_id.replace(':', '_').replace('.', '_')
            color = self.category_colors.get(node.category, '#9e9e9e')
            lines.append(f'    {short_id}["{node.name}"]')
            lines.append(f'    style {short_id} fill:{color}')
        
        lines.append('')
        
        # 添加边
        for edge in self.edges:
            source_short = edge.source.replace(':', '_').replace('.', '_')
            target_short = edge.target.replace(':', '_').replace('.', '_')
            lines.append(f'    {source_short} --> {target_short}')
        
        return '\n'.join(lines)
    
    def generate_mermaid_by_category(self, category: str) -> str:
        """生成按类别的Mermaid子图"""
        category_nodes = {k: v for k, v in self.nodes.items() if v.category == category}
        
        lines = [f'graph TD', f'    subgraph {category}']
        
        for node_id, node in category_nodes.items():
            short_id = node_id.replace(':', '_').replace('.', '_')
            color = self.category_colors.get(node.category, '#9e9c9e')
            lines.append(f'        {short_id}["{node.name}"]')
            lines.append(f'        style {short_id} fill:{color}')
        
        lines.append('    end')
        lines.append('')
        
        # 只添加类别内部的边
        for edge in self.edges:
            if edge.source in category_nodes and edge.target in category_nodes:
                source_short = edge.source.replace(':', '_').replace('.', '_')
                target_short = edge.target.replace(':', '_').replace('.', '_')
                lines.append(f'    {source_short} --> {target_short}')
        
        return '\n'.join(lines)
    
    def generate_mermaid_critical_path(self) -> str:
        """生成关键路径高亮的Mermaid图"""
        critical_path = self.identify_critical_path()
        critical_set = set(critical_path)
        
        lines = ['graph TD']
        
        # 添加所有节点
        for node_id, node in self.nodes.items():
            short_id = node_id.replace(':', '_').replace('.', '_')
            if node_id in critical_set:
                # 关键路径上的节点用红色高亮
                lines.append(f'    {short_id}["{node.name}"]')
                lines.append(f'    style {short_id} fill:#ff0000,stroke:#8b0000,stroke-width:4px')
            else:
                color = self.category_colors.get(node.category, '#9e9e9e')
                lines.append(f'    {short_id}["{node.name}"]')
                lines.append(f'    style {short_id} fill:{color}')
        
        lines.append('')
        
        # 添加边，关键路径上的边加粗
        for edge in self.edges:
            source_short = edge.source.replace(':', '_').replace('.', '_')
            target_short = edge.target.replace(':', '_').replace('.', '_')
            if edge.source in critical_set and edge.target in critical_set:
                lines.append(f'    {source_short} ==> {target_short}')
            else:
                lines.append(f'    {source_short} --> {target_short}')
        
        return '\n'.join(lines)
    
    def export_mermaid_files(self, output_dir: str):
        """导出所有Mermaid文件"""
        output_path = Path(output_dir)
        output_path.mkdir(parents=True, exist_ok=True)
        
        # 1. 完整网络
        mermaid_full = self.generate_mermaid_full()
        with open(output_path / 'theorem_network_full.mmd', 'w', encoding='utf-8') as f:
            f.write(mermaid_full)
        
        # 2. 关键路径
        mermaid_critical = self.generate_mermaid_critical_path()
        with open(output_path / 'theorem_network_critical_path.mmd', 'w', encoding='utf-8') as f:
            f.write(mermaid_critical)
        
        # 3. 各类别子图
        categories = set(node.category for node in self.nodes.values())
        for cat in categories:
            mermaid_cat = self.generate_mermaid_by_category(cat)
            safe_cat = cat.replace(' ', '_').replace('/', '_')
            with open(output_path / f'theorem_network_{safe_cat}.mmd', 'w', encoding='utf-8') as f:
                f.write(mermaid_cat)
        
        # 4. 统计摘要
        stats = self.get_statistics()
        summary = f"""# Theorem Network Statistics

## Overview
- Total Nodes: {stats['total_nodes']}
- Total Edges: {stats['total_edges']}
- Max Depth: {stats['max_depth']}
- Critical Path Length: {stats['critical_path_length']}
- Isolated Nodes: {stats['isolated_nodes']}
- Average Dependencies: {stats['avg_dependencies']:.2f}

## By Category
"""
        for cat, count in sorted(stats['by_category'].items(), key=lambda x: -x[1]):
            summary += f"- {cat}: {count}\n"
        
        summary += "\n## Most Central Theorems\n"
        for node_id, name, centrality in stats['most_central'][:5]:
            summary += f"- {name} (centrality: {centrality})\n"
        
        with open(output_path / 'network_statistics.md', 'w', encoding='utf-8') as f:
            f.write(summary)
        
        print(f"Exported Mermaid files to {output_dir}")


def main():
    # 加载提取的定理数据
    with open('scripts/theorem_network/theorems_raw.json', 'r', encoding='utf-8') as f:
        data = json.load(f)
    
    network = TheoremNetwork(data)
    
    # 选择核心定理
    core_ids = network.select_core_theorems(248)
    print(f"Selected {len(core_ids)} core theorems")
    
    # 构建网络
    network.build_network(core_ids)
    print(f"Network built with {len(network.nodes)} nodes and {len(network.edges)} edges")
    
    # 统计
    stats = network.get_statistics()
    print(f"\nNetwork Statistics:")
    print(f"  Total nodes: {stats['total_nodes']}")
    print(f"  Total edges: {stats['total_edges']}")
    print(f"  Max depth: {stats['max_depth']}")
    print(f"  Critical path length: {stats['critical_path_length']}")
    print(f"  Isolated nodes: {stats['isolated_nodes']}")
    print(f"\nBy category:")
    for cat, count in sorted(stats['by_category'].items(), key=lambda x: -x[1]):
        print(f"  {cat}: {count}")
    
    # 导出
    network.export_mermaid_files('knowledge/02_Formal_Semantics_and_Physics/diagrams')
    
    return network


if __name__ == '__main__':
    main()
