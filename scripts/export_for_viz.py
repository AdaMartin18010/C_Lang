#!/usr/bin/env python3
"""
C_Lang知识库可视化数据导出脚本
从各种来源提取数据并转换为D3.js可用的JSON格式
"""

import json
import re
import os
from pathlib import Path
from collections import defaultdict
from typing import Dict, List, Set, Tuple, Any

# 配置
ROOT_DIR = Path(__file__).parent.parent
KNOWLEDGE_DIR = ROOT_DIR / "knowledge"
OUTPUT_DIR = ROOT_DIR / "web" / "visualization" / "data"

# 确保输出目录存在
OUTPUT_DIR.mkdir(parents=True, exist_ok=True)

# 定理类别颜色映射
CATEGORY_COLORS = {
    "操作语义": "#2ecc71",      # 绿色
    "指称语义": "#3498db",      # 蓝色
    "公理语义": "#f39c12",      # 橙色
    "分布式系统": "#1abc9c",    # 青色
    "桥梁定理": "#e74c3c",      # 红色
    "并发": "#9b59b6",          # 紫色
    "形式验证": "#34495e",      # 深灰
    "类型理论": "#e91e63",      # 粉红
    "其他": "#95a5a6"           # 灰色
}

# 知识主题颜色映射（9个主题）
TOPIC_COLORS = {
    "core": "#3498db",          # 核心知识 - 蓝
    "semantics": "#2ecc71",     # 形式语义 - 绿
    "system": "#e74c3c",        # 系统技术 - 红
    "industrial": "#f39c12",    # 工业场景 - 橙
    "standards": "#9b59b6",     # 标准规范 - 紫
    "deep": "#1abc9c",          # 深层结构 - 青
    "thinking": "#e91e63",      # 思维表示 - 粉
    "embedded": "#34495e",      # 嵌入式 - 深灰
    "other": "#95a5a6"          # 其他 - 灰
}

def load_json_file(path: Path) -> dict:
    """加载JSON文件"""
    try:
        with open(path, 'r', encoding='utf-8') as f:
            return json.load(f)
    except Exception as e:
        print(f"Error loading {path}: {e}")
        return {}

def extract_theorem_network() -> Tuple[List[dict], List[dict]]:
    """
    从定理网络数据中提取节点和边
    返回: (nodes, links)
    """
    theorems_file = ROOT_DIR / "scripts" / "theorem_network" / "theorems_enhanced.json"
    data = load_json_file(theorems_file)
    
    nodes = []
    links = []
    node_ids = set()
    
    # 关键路径上的定理
    critical_path = [
        "indirectCommit", "commitRuleCurrentTerm", "leaderCompleteness",
        "electionSafety", "voteTermConsistency", "voteUniqueness"
    ]
    
    for theorem_id, theorem in data.get("theorems", {}).items():
        node_id = theorem.get("name", "")
        if not node_id or node_id in node_ids:
            continue
        
        category = theorem.get("category", "其他")
        # 简化类别
        if category in ["操作语义"]:
            category = "操作语义"
        elif category in ["指称语义"]:
            category = "指称语义"
        elif category in ["公理语义"]:
            category = "公理语义"
        elif category in ["分布式系统"]:
            category = "分布式系统"
        elif "桥" in category:
            category = "桥梁定理"
        else:
            category = "其他"
        
        # 计算中心性（简化版）
        in_degree = len(theorem.get("used_by", []))
        out_degree = len(theorem.get("dependencies", []))
        centrality = in_degree + out_degree
        
        node = {
            "id": node_id,
            "name": theorem.get("name", ""),
            "category": category,
            "color": CATEGORY_COLORS.get(category, "#95a5a6"),
            "file_path": theorem.get("file_path", ""),
            "description": theorem.get("description", ""),
            "in_degree": in_degree,
            "out_degree": out_degree,
            "centrality": centrality,
            "is_critical": node_id in critical_path,
            "is_isolated": centrality == 0
        }
        nodes.append(node)
        node_ids.add(node_id)
    
    # 提取边
    for theorem_id, theorem in data.get("theorems", {}).items():
        source = theorem.get("name", "")
        for dep in theorem.get("dependencies", []):
            # 从依赖ID中提取名称
            dep_name = dep.split("::")[-1] if "::" in dep else dep
            if dep_name and dep_name in node_ids and source in node_ids:
                links.append({
                    "source": source,
                    "target": dep_name,
                    "type": "depends_on"
                })
    
    return nodes, links

def extract_lean_ecosystem() -> Tuple[List[dict], List[dict]]:
    """
    从Lean4生态系统文档中提取层次结构
    返回: (nodes, links)
    """
    # 基于LEAN4_ECOSYSTEM.md构建层次结构
    ecosystem = {
        "name": "Lean 4 Ecosystem",
        "type": "root",
        "children": [
            {
                "name": "语言核心",
                "type": "category",
                "children": [
                    {"name": "Lean 4 内核", "type": "component", "size": 100, "description": "自举内核实现"},
                    {"name": "类型检查器", "type": "component", "size": 80, "description": "依赖类型检查"},
                    {"name": "编译器前端", "type": "component", "size": 90, "description": "Parser/Elaborator"},
                    {"name": "编译器后端", "type": "component", "size": 85, "description": "IR → C/LLVM"},
                    {"name": "运行时系统", "type": "component", "size": 75, "description": "引用计数 + 任务调度"}
                ]
            },
            {
                "name": "数学图书馆",
                "type": "category",
                "children": [
                    {"name": "Mathlib4", "type": "library", "size": 4200, "description": "420万+行代码，18万+定理"},
                    {"name": "Algebra", "type": "math", "size": 800, "description": "群论、环论、域论"},
                    {"name": "Analysis", "type": "math", "size": 600, "description": "实分析、复分析"},
                    {"name": "Topology", "type": "math", "size": 500, "description": "点集拓扑、代数拓扑"},
                    {"name": "NumberTheory", "type": "math", "size": 400, "description": "解析数论、代数数论"}
                ]
            },
            {
                "name": "核心组件",
                "type": "category",
                "children": [
                    {"name": "Std4", "type": "library", "size": 300, "description": "标准库"},
                    {"name": "Aesop", "type": "tool", "size": 150, "description": "自动化证明策略"},
                    {"name": "Lake", "type": "tool", "size": 120, "description": "构建工具"},
                    {"name": "LeanInk", "type": "tool", "size": 80, "description": "文档生成"},
                    {"name": "mathport", "type": "tool", "size": 100, "description": "Lean 3→4迁移工具"}
                ]
            },
            {
                "name": "领域框架",
                "type": "category",
                "children": [
                    {"name": "SciLean", "type": "framework", "size": 200, "description": "科学计算"},
                    {"name": "LeanCopilot", "type": "framework", "size": 180, "description": "AI辅助证明"},
                    {"name": "Polyrith", "type": "framework", "size": 100, "description": "算术求解器"},
                    {"name": "SMTLean", "type": "framework", "size": 120, "description": "SMT集成"},
                    {"name": "Duper", "type": "framework", "size": 90, "description": "超归结定理证明器"}
                ]
            },
            {
                "name": "形式化项目",
                "type": "category",
                "children": [
                    {"name": "Liquid Tensor Experiment", "type": "project", "size": 300, "description": "Liquid Tensor猜想"},
                    {"name": "Prime Number Theorem", "type": "project", "size": 200, "description": "素数定理形式化"},
                    {"name": "Sphere Eversion", "type": "project", "size": 250, "description": "球面外翻"},
                    {"name": "FLT Formalization", "type": "project", "size": 400, "description": "费马大定理"}
                ]
            }
        ]
    }
    
    nodes = []
    links = []
    
    def process_node(node, parent=None, depth=0):
        node_id = node["name"].replace(" ", "_").replace("-", "_")
        n = {
            "id": node_id,
            "name": node["name"],
            "type": node.get("type", "unknown"),
            "size": node.get("size", 50),
            "description": node.get("description", ""),
            "depth": depth
        }
        nodes.append(n)
        
        if parent:
            links.append({
                "source": parent,
                "target": node_id,
                "type": "contains"
            })
        
        for child in node.get("children", []):
            process_node(child, node_id, depth + 1)
    
    process_node(ecosystem)
    return nodes, links

def extract_knowledge_graph() -> Tuple[List[dict], List[dict]]:
    """
    从关联分析数据中提取知识图谱
    返回: (nodes, links) - Top 200核心文档
    """
    stats_file = ROOT_DIR / "association_analysis_stats.json"
    data = load_json_file(stats_file)
    
    nodes = []
    links = []
    
    # 获取前200个文档
    sorted_docs = sorted(data, key=lambda x: x.get("internal_links", 0) + x.get("external_links", 0), reverse=True)[:200]
    
    # 文档路径到主题的映射
    def get_topic(path: str) -> str:
        path_lower = path.lower()
        if "01_core" in path_lower:
            return "core"
        elif "02_formal" in path_lower:
            return "semantics"
        elif "03_system" in path_lower:
            return "system"
        elif "04_industrial" in path_lower:
            return "industrial"
        elif "05_deep" in path_lower:
            return "deep"
        elif "06_thinking" in path_lower:
            return "thinking"
        elif "embedded" in path_lower or "bare_metal" in path_lower:
            return "embedded"
        elif "standard" in path_lower or "misra" in path_lower:
            return "standards"
        else:
            return "other"
    
    # 创建节点
    doc_ids = set()
    for doc in sorted_docs:
        path = doc.get("path", "")
        topic = get_topic(path)
        node_id = path.replace("\\", "/").replace(".md", "")
        doc_ids.add(node_id)
        
        # 计算PageRank风格的得分（简化版）
        pagerank = doc.get("internal_links", 0) * 0.7 + doc.get("external_links", 0) * 0.3
        
        nodes.append({
            "id": node_id,
            "name": Path(path).stem,
            "path": path,
            "topic": topic,
            "topic_color": TOPIC_COLORS.get(topic, "#95a5a6"),
            "size": pagerank * 3 + 5,  # 节点大小
            "pagerank": round(pagerank, 2),
            "internal_links": doc.get("internal_links", 0),
            "external_links": doc.get("external_links", 0),
            "line_count": doc.get("total_lines", 0)
        })
    
    # 创建边（基于outgoing_links）
    for doc in sorted_docs:
        source = doc.get("path", "").replace("\\", "/").replace(".md", "")
        for target_raw in doc.get("outgoing_links", [])[:10]:  # 限制每个节点的出边
            target = target_raw.replace("\\", "/").replace(".md", "")
            # 只保留在Top 200中的链接
            target_base = target.split("#")[0]  # 移除锚点
            if target_base in doc_ids and source != target_base:
                links.append({
                    "source": source,
                    "target": target_base,
                    "type": "references"
                })
    
    return nodes, links

def generate_mathlib_coverage() -> dict:
    """生成Mathlib4数学领域覆盖数据"""
    return {
        "name": "Mathlib4 Coverage",
        "children": [
            {
                "name": "代数",
                "value": 800,
                "children": [
                    {"name": "群论", "value": 200},
                    {"name": "环论", "value": 180},
                    {"name": "域论", "value": 150},
                    {"name": "模论", "value": 120},
                    {"name": "同调代数", "value": 150}
                ]
            },
            {
                "name": "分析",
                "value": 600,
                "children": [
                    {"name": "实分析", "value": 150},
                    {"name": "复分析", "value": 120},
                    {"name": "泛函分析", "value": 130},
                    {"name": "测度论", "value": 100},
                    {"name": "微分方程", "value": 100}
                ]
            },
            {
                "name": "拓扑",
                "value": 500,
                "children": [
                    {"name": "点集拓扑", "value": 150},
                    {"name": "代数拓扑", "value": 120},
                    {"name": "微分拓扑", "value": 100},
                    {"name": "代数几何", "value": 130}
                ]
            },
            {
                "name": "数论",
                "value": 400,
                "children": [
                    {"name": "解析数论", "value": 120},
                    {"name": "代数数论", "value": 150},
                    {"name": "丢番图方程", "value": 80},
                    {"name": "算术几何", "value": 50}
                ]
            },
            {
                "name": "线性代数",
                "value": 450,
                "children": [
                    {"name": "矩阵论", "value": 150},
                    {"name": "张量代数", "value": 100},
                    {"name": "表示论", "value": 120},
                    {"name": "多线性代数", "value": 80}
                ]
            },
            {
                "name": "组合",
                "value": 300,
                "children": [
                    {"name": "图论", "value": 120},
                    {"name": "枚举组合", "value": 80},
                    {"name": "极值组合", "value": 60},
                    {"name": "设计理论", "value": 40}
                ]
            },
            {
                "name": "几何",
                "value": 350,
                "children": [
                    {"name": "欧氏几何", "value": 80},
                    {"name": "微分几何", "value": 120},
                    {"name": "黎曼几何", "value": 100},
                    {"name": "辛几何", "value": 50}
                ]
            },
            {
                "name": "逻辑",
                "value": 250,
                "children": [
                    {"name": "模型论", "value": 80},
                    {"name": "证明论", "value": 70},
                    {"name": "递归论", "value": 50},
                    {"name": "集合论", "value": 50}
                ]
            }
        ]
    }

def main():
    """主函数：导出所有可视化数据"""
    print("=" * 60)
    print("C_Lang知识库可视化数据导出")
    print("=" * 60)
    
    # 1. 导出定理网络数据
    print("\n[1/4] 导出定理网络数据...")
    theorem_nodes, theorem_links = extract_theorem_network()
    theorem_data = {
        "nodes": theorem_nodes,
        "links": theorem_links,
        "stats": {
            "total_nodes": len(theorem_nodes),
            "total_links": len(theorem_links),
            "categories": list(set(n["category"] for n in theorem_nodes))
        }
    }
    with open(OUTPUT_DIR / "theorem_network.json", "w", encoding="utf-8") as f:
        json.dump(theorem_data, f, ensure_ascii=False, indent=2)
    print(f"  ✓ 定理网络: {len(theorem_nodes)} 节点, {len(theorem_links)} 边")
    
    # 2. 导出Lean生态系统数据
    print("\n[2/4] 导出Lean生态系统数据...")
    lean_nodes, lean_links = extract_lean_ecosystem()
    lean_data = {
        "nodes": lean_nodes,
        "links": lean_links,
        "stats": {
            "total_nodes": len(lean_nodes),
            "total_links": len(lean_links)
        },
        "mathlib_coverage": generate_mathlib_coverage()
    }
    with open(OUTPUT_DIR / "lean_ecosystem.json", "w", encoding="utf-8") as f:
        json.dump(lean_data, f, ensure_ascii=False, indent=2)
    print(f"  ✓ Lean生态: {len(lean_nodes)} 节点, {len(lean_links)} 边")
    
    # 3. 导出知识图谱数据
    print("\n[3/4] 导出知识图谱数据...")
    kg_nodes, kg_links = extract_knowledge_graph()
    kg_data = {
        "nodes": kg_nodes,
        "links": kg_links,
        "stats": {
            "total_nodes": len(kg_nodes),
            "total_links": len(kg_links),
            "topics": list(set(n["topic"] for n in kg_nodes))
        }
    }
    with open(OUTPUT_DIR / "knowledge_graph.json", "w", encoding="utf-8") as f:
        json.dump(kg_data, f, ensure_ascii=False, indent=2)
    print(f"  ✓ 知识图谱: {len(kg_nodes)} 节点, {len(kg_links)} 边")
    
    # 4. 导出统计摘要
    print("\n[4/4] 导出统计摘要...")
    summary = {
        "export_time": "2026-03-25",
        "datasets": {
            "theorem_network": {
                "nodes": len(theorem_nodes),
                "links": len(theorem_links),
                "description": "248个定理的依赖关系网络"
            },
            "lean_ecosystem": {
                "nodes": len(lean_nodes),
                "links": len(lean_links),
                "description": "Lean 4生态系统层次结构"
            },
            "knowledge_graph": {
                "nodes": len(kg_nodes),
                "links": len(kg_links),
                "description": "Top 200核心文档的知识图谱"
            }
        }
    }
    with open(OUTPUT_DIR / "summary.json", "w", encoding="utf-8") as f:
        json.dump(summary, f, ensure_ascii=False, indent=2)
    
    print("\n" + "=" * 60)
    print("数据导出完成！")
    print(f"输出目录: {OUTPUT_DIR}")
    print("=" * 60)

if __name__ == "__main__":
    main()
