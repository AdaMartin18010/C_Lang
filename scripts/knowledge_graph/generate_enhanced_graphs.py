#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Enhanced Knowledge Graph Visualizations

Generates detailed Mermaid diagrams for specific knowledge domains.
"""

import json
import re
from pathlib import Path
from collections import defaultdict
from typing import Dict, List, Set, Tuple

GRAPH_FILE = Path("association_analysis_graph.json")
OUTPUT_DIR = Path("knowledge/00_VERSION_TRACKING/diagrams")


def normalize_path(path: str) -> str:
    path = re.sub(r'^knowledge[/\\]', '', path)
    return path.replace('\\', '/').lower()


def get_topic(doc_path: str) -> str:
    path = doc_path.lower()
    if '01_core' in path or 'core_knowledge' in path:
        return 'core'
    elif '02_formal' in path or 'formal_semantics' in path:
        return 'formal'
    elif '03_system' in path or 'system_technology' in path:
        return 'system'
    elif '04_industrial' in path:
        return 'industrial'
    elif '05_deep' in path or 'standards' in path:
        return 'standards'
    elif '06_thinking' in path or 'thinking_representation' in path:
        return 'thinking'
    elif '07_toolchain' in path or 'modern_toolchain' in path:
        return 'toolchain'
    return 'other'


def load_and_build_graph():
    with open(GRAPH_FILE, 'r', encoding='utf-8') as f:
        graph = json.load(f)
    
    docs = {}
    for doc_path, outgoing in graph.items():
        normalized = normalize_path(doc_path)
        docs[normalized] = {
            'original': doc_path,
            'outgoing': set(normalize_path(link) for link in outgoing),
            'incoming': set(),
            'topic': get_topic(doc_path)
        }
    
    # Build incoming links
    for doc, data in docs.items():
        for linked in data['outgoing']:
            if linked in docs:
                docs[linked]['incoming'].add(doc)
    
    return docs


def get_topic_docs(docs: Dict, topic: str, min_connections: int = 2) -> List[Tuple[str, Dict]]:
    """Get documents for a specific topic, sorted by connectivity."""
    topic_docs = [(d, data) for d, data in docs.items() 
                  if data['topic'] == topic]
    
    # Sort by total connections
    topic_docs.sort(key=lambda x: len(x[1]['incoming']) + len(x[1]['outgoing']), reverse=True)
    return topic_docs


def generate_detailed_topic_graph(docs: Dict, topic: str, title: str, 
                                   max_nodes: int = 30, max_edges: int = 80) -> str:
    """Generate detailed topic-specific graph."""
    topic_docs = get_topic_docs(docs, topic)
    
    if not topic_docs:
        return f"%% No documents found for topic: {topic}"
    
    # Select top nodes
    selected = topic_docs[:max_nodes]
    selected_paths = {d[0] for d in selected}
    
    lines = [f"---", f"title: {title}", f"---", ""]
    lines.append("graph LR")
    lines.append("")
    
    # Topic-specific colors
    colors = {
        'core': ('#e3f2fd', '#1976d2'),
        'formal': ('#f3e5f5', '#7b1fa2'),
        'system': ('#e8f5e9', '#388e3c'),
        'industrial': ('#fff3e0', '#f57c00'),
        'standards': ('#fce4ec', '#c2185b'),
        'thinking': ('#f1f8e9', '#689f38'),
        'toolchain': ('#e0f2f1', '#00796b'),
        'other': ('#fafafa', '#616161')
    }
    
    # Add subgraphs by subcategory
    subcategories = defaultdict(list)
    for path, data in selected:
        # Extract subcategory from path
        parts = path.split('/')
        if len(parts) >= 2:
            subcat = parts[1] if len(parts) > 1 else 'general'
        else:
            subcat = 'general'
        subcategories[subcat].append((path, data))
    
    node_map = {}
    node_id = 0
    
    # Generate nodes within subgraphs
    for subcat, nodes in sorted(subcategories.items()):
        clean_subcat = subcat.replace('_', ' ').title()
        lines.append(f"    subgraph {clean_subcat}")
        
        for path, data in nodes:
            node_map[path] = f"N{node_id}"
            label = Path(path).stem.replace('_', ' ').title()
            if len(label) > 25:
                label = label[:22] + "..."
            lines.append(f"        {node_map[path]}[{label}]")
            node_id += 1
        
        lines.append("    end")
        lines.append("")
    
    # Add edges
    edge_count = 0
    for path, data in selected:
        source_id = node_map.get(path)
        if not source_id:
            continue
        
        for linked in list(data['outgoing'])[:3]:  # Limit outgoing
            if linked in node_map and edge_count < max_edges:
                target_id = node_map[linked]
                lines.append(f"    {source_id} --> {target_id}")
                edge_count += 1
    
    # Add cross-topic connections
    cross_edges = 0
    for path, data in selected:
        source_id = node_map.get(path)
        if not source_id:
            continue
        
        for linked in data['outgoing']:
            if linked in docs and docs[linked]['topic'] != topic and cross_edges < 10:
                if linked not in node_map:
                    # Add external node
                    node_map[linked] = f"E{cross_edges}"
                    ext_label = Path(linked).stem[:20]
                    ext_topic = docs[linked]['topic']
                    bg, stroke = colors.get(ext_topic, ('#eeeeee', '#999999'))
                    lines.append(f"    {node_map[linked]}((\"{ext_label}\"))")
                    lines.append(f"    style {node_map[linked]} fill:{bg},stroke:{stroke},stroke-dasharray: 5 5")
                
                lines.append(f"    {source_id} -.-> {node_map[linked]}")
                cross_edges += 1
    
    lines.append("")
    bg, stroke = colors.get(topic, ('#fafafa', '#616161'))
    lines.append(f"    classDef topicDefault fill:{bg},stroke:{stroke},stroke-width:2px")
    lines.append(f"    class {','.join(node_map.values())} topicDefault")
    
    return "\n".join(lines)


def generate_core_knowledge_flow() -> str:
    """Generate learning path flow diagram for core knowledge."""
    lines = ["---", "title: Core Knowledge Learning Path", "---", ""]
    lines.append("flowchart TD")
    lines.append("")
    
    # Define learning path
    levels = [
        ("Basic Layer", ["Syntax Elements", "Data Types", "Operators", "Control Flow"]),
        ("Core Layer", ["Pointers", "Memory Management", "Arrays", "Functions"]),
        ("Construction", ["Structures", "Preprocessor", "Modularization"]),
        ("Standard Library", ["I/O", "String", "Math", "Threads"]),
        ("Advanced", ["Undefined Behavior", "Portability", "Extensions"])
    ]
    
    node_map = {}
    node_id = 0
    
    for level_name, topics in levels:
        lines.append(f"    subgraph {level_name}")
        for topic in topics:
            node_map[topic] = f"N{node_id}"
            lines.append(f"        {node_map[topic]}[{topic}]")
            node_id += 1
        lines.append("    end")
        lines.append("")
    
    # Add progression arrows
    prev_topics = None
    for level_name, topics in levels:
        if prev_topics:
            # Connect last of previous to first of current
            lines.append(f"    {node_map[prev_topics[-1]]} --> {node_map[topics[0]]}")
        prev_topics = topics
    
    lines.append("")
    lines.append("    classDef basic fill:#e8f5e9,stroke:#4caf50,stroke-width:2px")
    lines.append("    classDef core fill:#fff3e0,stroke:#ff9800,stroke-width:2px")
    lines.append("    classDef advanced fill:#fce4ec,stroke:#e91e63,stroke-width:2px")
    
    return "\n".join(lines)


def generate_theorem_dependency_graph() -> str:
    """Generate theorem dependency visualization."""
    # This would parse theorem references from documents
    lines = ["---", "title: Theorem Dependency Network", "---", ""]
    lines.append("graph TD")
    lines.append("")
    lines.append("    %% Foundational Theorems")
    lines.append("    T1[\"Substitution Lemma\"]:::foundational")
    lines.append("    T2[\"Weakening Lemma\"]:::foundational")
    lines.append("    T3[\"Progress Theorem\"]:::foundational")
    lines.append("")
    lines.append("    %% Type System Theorems")
    lines.append("    T4[\"Type Preservation\"]:::typesystem")
    lines.append("    T5[\"Type Safety\"]:::typesystem")
    lines.append("")
    lines.append("    %% Memory Safety Theorems")
    lines.append("    T6[\"Memory Safety\"]:::memory")
    lines.append("    T7[\"No Use-After-Free\"]:::memory")
    lines.append("")
    lines.append("    T1 --> T4")
    lines.append("    T2 --> T4")
    lines.append("    T3 --> T5")
    lines.append("    T4 --> T5")
    lines.append("    T5 --> T6")
    lines.append("    T6 --> T7")
    lines.append("")
    lines.append("    classDef foundational fill:#e3f2fd,stroke:#1976d2")
    lines.append("    classDef typesystem fill:#f3e5f5,stroke:#7b1fa2")
    lines.append("    classDef memory fill:#e8f5e9,stroke:#388e3c")
    
    return "\n".join(lines)


def generate_system_architecture_diagram() -> str:
    """Generate system technology architecture diagram."""
    lines = ["---", "title: System Technology Domain Architecture", "---", ""]
    lines.append("graph TB")
    lines.append("")
    lines.append("    subgraph Core_Systems")
    lines.append("        S1[OS Kernel]")
    lines.append("        S2[Concurrency]")
    lines.append("        S3[Memory Management]")
    lines.append("    end")
    lines.append("")
    lines.append("    subgraph Network_&_I/O")
    lines.append("        N1[Socket Programming]")
    lines.append("        N2[RDMA]")
    lines.append("        N3[DPDK]")
    lines.append("    end")
    lines.append("")
    lines.append("    subgraph Security")
    lines.append("        SEC1[Cryptography]")
    lines.append("        SEC2[Secure Boot]")
    lines.append("        SEC3[Exploit Mitigation]")
    lines.append("    end")
    lines.append("")
    lines.append("    subgraph Specialized")
    lines.append("        SP1[GPU/CUDA]")
    lines.append("        SP2[eBPF]")
    lines.append("        SP3[Virtualization]")
    lines.append("    end")
    lines.append("")
    lines.append("    S1 --> N1")
    lines.append("    S2 --> N2")
    lines.append("    S1 --> SEC1")
    lines.append("    S2 --> SEC3")
    lines.append("    N1 --> SEC1")
    lines.append("    S1 --> SP3")
    lines.append("    S3 --> SP1")
    
    return "\n".join(lines)


def main():
    print("Generating enhanced knowledge graph visualizations...")
    
    docs = load_and_build_graph()
    print(f"Loaded {len(docs)} documents")
    
    OUTPUT_DIR.mkdir(parents=True, exist_ok=True)
    
    # Generate topic-specific detailed graphs
    graphs = [
        ('core_knowledge_detailed.mmd', 'core', 'Core Knowledge System - Detailed'),
        ('formal_semantics_detailed.mmd', 'formal', 'Formal Semantics & Physics - Detailed'),
        ('system_tech_detailed.mmd', 'system', 'System Technology Domains - Detailed'),
        ('industrial_detailed.mmd', 'industrial', 'Industrial Scenarios - Detailed'),
        ('thinking_representation.mmd', 'thinking', 'Thinking & Representation'),
    ]
    
    for filename, topic, title in graphs:
        content = generate_detailed_topic_graph(docs, topic, title, 35, 100)
        with open(OUTPUT_DIR / filename, 'w', encoding='utf-8') as f:
            f.write(content)
        print(f"  Generated: {filename}")
    
    # Generate specialized diagrams
    with open(OUTPUT_DIR / 'learning_path_flow.mmd', 'w', encoding='utf-8') as f:
        f.write(generate_core_knowledge_flow())
    print(f"  Generated: learning_path_flow.mmd")
    
    with open(OUTPUT_DIR / 'theorem_dependency.mmd', 'w', encoding='utf-8') as f:
        f.write(generate_theorem_dependency_graph())
    print(f"  Generated: theorem_dependency.mmd")
    
    with open(OUTPUT_DIR / 'system_architecture.mmd', 'w', encoding='utf-8') as f:
        f.write(generate_system_architecture_diagram())
    print(f"  Generated: system_architecture.mmd")
    
    print(f"\nAll diagrams saved to: {OUTPUT_DIR}")


if __name__ == "__main__":
    main()
