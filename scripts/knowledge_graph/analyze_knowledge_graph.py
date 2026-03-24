#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
C_Lang Knowledge Graph Analyzer

Analyzes internal link dependencies across the knowledge base and generates:
- Document relationship matrices
- Hub document identification (PageRank-style)
- Isolated document detection
- Mermaid visualization diagrams
- Comprehensive knowledge graph report
"""

import json
import re
import os
from pathlib import Path
from collections import defaultdict, Counter
from typing import Dict, List, Set, Tuple, Any
import math

# Configuration
KNOWLEDGE_DIR = Path("knowledge")
OUTPUT_DIR = Path("scripts/knowledge_graph/output")
GRAPH_FILE = Path("association_analysis_graph.json")
STATS_FILE = Path("association_analysis_stats.json")

# Topic patterns for classification
TOPIC_PATTERNS = {
    "core_knowledge": ["01_Core_Knowledge_System", "01_Core"],
    "formal_semantics": ["02_Formal_Semantics_and_Physics", "02_Formal"],
    "system_tech": ["03_System_Technology_Domains", "03_System"],
    "industrial": ["04_Industrial_Scenarios", "04_Industrial"],
    "standards": ["05_C_Language_Standards", "05_Deep_Structure_MetaPhysics"],
    "thinking": ["06_Thinking_Representation", "06_Thinking"],
    "toolchain": ["07_Modern_Toolchain"],
    "embedded": ["06_Embedded_Bare_Metal"],
    "global": ["00_"]
}


def classify_document(doc_path: str) -> str:
    """Classify document into topic category."""
    for topic, patterns in TOPIC_PATTERNS.items():
        for pattern in patterns:
            if pattern in doc_path:
                return topic
    return "other"


def normalize_path(path: str) -> str:
    """Normalize path for consistent comparison."""
    # Remove 'knowledge\' or 'knowledge/' prefix
    path = re.sub(r'^knowledge[/\\]', '', path)
    path = path.replace('\\', '/')
    return path.lower()


def extract_filename(path: str) -> str:
    """Extract clean filename for display."""
    path = normalize_path(path)
    parts = path.split('/')
    # Get last meaningful part
    for part in reversed(parts):
        if part and part != 'readme.md':
            return part.replace('.md', '').replace('_', ' ')
    return parts[-1].replace('.md', '').replace('_', ' ') if parts else 'unknown'


def load_graph_data() -> Tuple[Dict, List]:
    """Load association graph and stats data."""
    with open(GRAPH_FILE, 'r', encoding='utf-8') as f:
        graph = json.load(f)
    with open(STATS_FILE, 'r', encoding='utf-8') as f:
        stats = json.load(f)
    return graph, stats


def build_document_index(graph: Dict) -> Dict[str, Dict]:
    """Build comprehensive document index with metadata."""
    docs = {}
    
    for doc_path, outgoing in graph.items():
        normalized = normalize_path(doc_path)
        topic = classify_document(doc_path)
        
        docs[normalized] = {
            'original_path': doc_path,
            'normalized_path': normalized,
            'topic': topic,
            'outgoing_links': set(normalize_path(link) for link in outgoing),
            'incoming_links': set(),
            'outgoing_count': len(outgoing),
            'incoming_count': 0
        }
    
    # Calculate incoming links
    for doc_path, data in docs.items():
        for linked_doc in data['outgoing_links']:
            if linked_doc in docs:
                docs[linked_doc]['incoming_links'].add(doc_path)
    
    # Update incoming counts
    for doc_path, data in docs.items():
        data['incoming_count'] = len(data['incoming_links'])
        data['total_connections'] = data['incoming_count'] + data['outgoing_count']
    
    return docs


def calculate_pagerank(docs: Dict, damping: float = 0.85, iterations: int = 100) -> Dict[str, float]:
    """Calculate PageRank scores for documents."""
    n = len(docs)
    if n == 0:
        return {}
    
    # Initialize scores
    scores = {doc: 1.0 / n for doc in docs}
    
    for _ in range(iterations):
        new_scores = {}
        for doc in docs:
            score = (1 - damping) / n
            
            # Sum contributions from incoming links
            for incoming in docs[doc]['incoming_links']:
                if incoming in docs and docs[incoming]['outgoing_count'] > 0:
                    score += damping * scores[incoming] / docs[incoming]['outgoing_count']
            
            new_scores[doc] = score
        
        scores = new_scores
    
    return scores


def identify_hubs(docs: Dict, pagerank: Dict, top_n: int = 50) -> List[Dict]:
    """Identify top hub documents."""
    hub_scores = []
    
    for doc, data in docs.items():
        # Combined hub score: weighted sum of incoming, outgoing, and pagerank
        score = (
            0.4 * data['incoming_count'] +
            0.2 * data['outgoing_count'] +
            0.4 * pagerank.get(doc, 0) * 1000  # Scale pagerank
        )
        
        hub_scores.append({
            'path': doc,
            'original_path': data['original_path'],
            'topic': data['topic'],
            'incoming': data['incoming_count'],
            'outgoing': data['outgoing_count'],
            'pagerank': pagerank.get(doc, 0),
            'hub_score': score
        })
    
    hub_scores.sort(key=lambda x: x['hub_score'], reverse=True)
    return hub_scores[:top_n]


def identify_isolated_docs(docs: Dict) -> Tuple[List[str], List[str], List[str]]:
    """Identify isolated, sink, and source documents."""
    isolated = []
    sinks = []  # Only incoming, no outgoing
    sources = []  # Only outgoing, no incoming
    
    for doc, data in docs.items():
        if data['incoming_count'] == 0 and data['outgoing_count'] == 0:
            isolated.append(doc)
        elif data['incoming_count'] > 0 and data['outgoing_count'] == 0:
            sinks.append(doc)
        elif data['incoming_count'] == 0 and data['outgoing_count'] > 0:
            sources.append(doc)
    
    return isolated, sinks, sources


def analyze_topic_clusters(docs: Dict) -> Dict[str, Dict]:
    """Analyze document clusters by topic."""
    clusters = defaultdict(lambda: {'docs': [], 'internal_links': 0, 'external_links': 0})
    
    for doc, data in docs.items():
        topic = data['topic']
        clusters[topic]['docs'].append(doc)
        
        for linked in data['outgoing_links']:
            if linked in docs:
                if docs[linked]['topic'] == topic:
                    clusters[topic]['internal_links'] += 1
                else:
                    clusters[topic]['external_links'] += 1
    
    # Calculate statistics
    for topic, cluster in clusters.items():
        cluster['doc_count'] = len(cluster['docs'])
        cluster['density'] = (
            cluster['internal_links'] / (cluster['doc_count'] * (cluster['doc_count'] - 1))
            if cluster['doc_count'] > 1 else 0
        )
        cluster['cohesion'] = (
            cluster['internal_links'] / (cluster['internal_links'] + cluster['external_links'])
            if (cluster['internal_links'] + cluster['external_links']) > 0 else 0
        )
    
    return dict(clusters)


def generate_mermaid_graph(docs: Dict, hub_docs: List[Dict], title: str, 
                           max_nodes: int = 50, topic_filter: str = None) -> str:
    """Generate Mermaid graph diagram."""
    lines = [f"---", f"title: {title}", f"---", ""]
    lines.append("graph TB")
    
    # Select nodes
    if topic_filter:
        selected = [d for d in hub_docs if classify_document(d['original_path']) == topic_filter][:max_nodes]
    else:
        selected = hub_docs[:max_nodes]
    
    selected_paths = {d['path'] for d in selected}
    
    # Topic color mapping
    topic_colors = {
        'core_knowledge': '#e1f5fe',
        'formal_semantics': '#f3e5f5',
        'system_tech': '#e8f5e9',
        'industrial': '#fff3e0',
        'standards': '#fce4ec',
        'thinking': '#f1f8e9',
        'toolchain': '#e0f2f1',
        'embedded': '#fff8e1',
        'global': '#f5f5f5',
        'other': '#fafafa'
    }
    
    # Add nodes
    node_ids = {}
    for i, hub in enumerate(selected):
        node_id = f"N{i}"
        node_ids[hub['path']] = node_id
        label = extract_filename(hub['path'])
        # Truncate long labels
        if len(label) > 30:
            label = label[:27] + "..."
        color = topic_colors.get(hub['topic'], '#fafafa')
        lines.append(f"    {node_id}[\"{label}\"]:::topic_{hub['topic']}")
    
    # Add edges (limit to avoid clutter)
    edge_count = 0
    max_edges = min(150, len(selected) * 3)
    
    for hub in selected:
        source_id = node_ids.get(hub['path'])
        if not source_id:
            continue
            
        doc_data = docs.get(hub['path'])
        if not doc_data:
            continue
        
        for linked in list(doc_data['outgoing_links'])[:5]:  # Limit outgoing per node
            if linked in node_ids and edge_count < max_edges:
                target_id = node_ids[linked]
                lines.append(f"    {source_id} --> {target_id}")
                edge_count += 1
    
    # Add styles
    lines.append("")
    lines.append("    classDef topic_core_knowledge fill:#e3f2fd,stroke:#1976d2,stroke-width:2px")
    lines.append("    classDef topic_formal_semantics fill:#f3e5f5,stroke:#7b1fa2,stroke-width:2px")
    lines.append("    classDef topic_system_tech fill:#e8f5e9,stroke:#388e3c,stroke-width:2px")
    lines.append("    classDef topic_industrial fill:#fff3e0,stroke:#f57c00,stroke-width:2px")
    lines.append("    classDef topic_standards fill:#fce4ec,stroke:#c2185b,stroke-width:2px")
    lines.append("    classDef topic_thinking fill:#f1f8e9,stroke:#689f38,stroke-width:2px")
    lines.append("    classDef topic_toolchain fill:#e0f2f1,stroke:#00796b,stroke-width:2px")
    lines.append("    classDef topic_embedded fill:#fff8e1,stroke:#ffa000,stroke-width:2px")
    lines.append("    classDef topic_global fill:#f5f5f5,stroke:#616161,stroke-width:2px")
    lines.append("    classDef topic_other fill:#fafafa,stroke:#9e9e9e,stroke-width:1px")
    
    return "\n".join(lines)


def generate_topic_sunburst(clusters: Dict) -> str:
    """Generate Mermaid pie chart for topic distribution."""
    lines = ["---", "title: Knowledge Base Topic Distribution", "---", ""]
    lines.append("pie showData")
    lines.append("    title Topic Distribution by Document Count")
    
    topic_names = {
        'core_knowledge': 'Core Knowledge',
        'formal_semantics': 'Formal Semantics',
        'system_tech': 'System Technology',
        'industrial': 'Industrial Scenarios',
        'standards': 'Standards & Metaphysics',
        'thinking': 'Thinking & Representation',
        'toolchain': 'Modern Toolchain',
        'embedded': 'Embedded Systems',
        'global': 'Global Index',
        'other': 'Other'
    }
    
    for topic, cluster in sorted(clusters.items(), key=lambda x: x[1]['doc_count'], reverse=True):
        name = topic_names.get(topic, topic)
        lines.append(f'    "{name}" : {cluster["doc_count"]}')
    
    return "\n".join(lines)


def identify_broken_links(docs: Dict) -> List[Tuple[str, str]]:
    """Identify links pointing to non-existent documents."""
    broken = []
    all_paths = set(docs.keys())
    
    for doc, data in docs.items():
        for link in data['outgoing_links']:
            if link not in all_paths and not link.startswith('http'):
                broken.append((doc, link))
    
    return broken


def suggest_new_links(docs: Dict, top_n: int = 50) -> List[Dict]:
    """Suggest potential new links based on topic similarity."""
    suggestions = []
    
    for doc, data in docs.items():
        topic = data['topic']
        
        # Find documents in same topic that aren't linked
        for other_doc, other_data in docs.items():
            if doc != other_doc and other_data['topic'] == topic:
                if other_doc not in data['outgoing_links'] and doc not in other_data['outgoing_links']:
                    # Calculate suggestion score based on mutual connections
                    common_out = len(data['outgoing_links'] & other_data['outgoing_links'])
                    common_in = len(data['incoming_links'] & other_data['incoming_links'])
                    score = common_out + common_in
                    
                    if score > 0:
                        suggestions.append({
                            'from': doc,
                            'to': other_doc,
                            'topic': topic,
                            'common_connections': score,
                            'reason': f"Share {score} common connections in {topic}"
                        })
    
    suggestions.sort(key=lambda x: x['common_connections'], reverse=True)
    return suggestions[:top_n]


def generate_report(docs: Dict, stats: List, pagerank: Dict, hubs: List,
                   isolated: List, sinks: List, sources: List, 
                   clusters: Dict, broken_links: List, suggestions: List) -> str:
    """Generate comprehensive markdown report."""
    
    report = []
    report.append("# C_Lang Knowledge Graph Analysis Report")
    report.append(f"\n**Generated**: 2026-03-25")
    report.append(f"\n**Knowledge Base**: C_Lang Deep Knowledge System")
    report.append("")
    
    # Executive Summary
    report.append("## Executive Summary\n")
    report.append(f"- **Total Documents**: {len(docs)}")
    report.append(f"- **Total Internal Links**: {sum(d['outgoing_count'] for d in docs.values())}")
    report.append(f"- **Graph Density**: {len(docs) > 1 and sum(d['outgoing_count'] for d in docs.values()) / (len(docs) * (len(docs) - 1)):.4f}")
    report.append(f"- **Core Hub Documents**: {len(hubs)}")
    report.append(f"- **Isolated Documents**: {len(isolated)}")
    report.append(f"- **Broken Links**: {len(broken_links)}")
    report.append("")
    
    # Graph Statistics
    report.append("## Graph Statistics\n")
    report.append("### Overall Metrics\n")
    report.append("| Metric | Value |")
    report.append("|--------|-------|")
    report.append(f"| Nodes (Documents) | {len(docs)} |")
    report.append(f"| Edges (Links) | {sum(d['outgoing_count'] for d in docs.values())} |")
    report.append(f"| Avg Out-Degree | {sum(d['outgoing_count'] for d in docs.values()) / len(docs):.2f} |")
    report.append(f"| Avg In-Degree | {sum(d['incoming_count'] for d in docs.values()) / len(docs):.2f} |")
    report.append(f"| Max Out-Degree | {max(d['outgoing_count'] for d in docs.values())} |")
    report.append(f"| Max In-Degree | {max(d['incoming_count'] for d in docs.values())} |")
    report.append("")
    
    # Topic Clusters
    report.append("## Topic Cluster Analysis\n")
    report.append("| Topic | Documents | Internal Links | External Links | Cohesion |")
    report.append("|-------|-----------|----------------|----------------|----------|")
    
    topic_names = {
        'core_knowledge': 'Core Knowledge',
        'formal_semantics': 'Formal Semantics',
        'system_tech': 'System Technology',
        'industrial': 'Industrial',
        'standards': 'Standards & Metaphysics',
        'thinking': 'Thinking & Representation',
        'toolchain': 'Toolchain',
        'embedded': 'Embedded',
        'global': 'Global',
        'other': 'Other'
    }
    
    for topic, cluster in sorted(clusters.items(), key=lambda x: x[1]['doc_count'], reverse=True):
        name = topic_names.get(topic, topic)
        report.append(f"| {name} | {cluster['doc_count']} | {cluster['internal_links']} | {cluster['external_links']} | {cluster['cohesion']:.2f} |")
    report.append("")
    
    # Core Hub Documents (PageRank)
    report.append("## Core Hub Documents (Top 30)\n")
    report.append("Ranked by hub score combining incoming links, outgoing links, and PageRank.\n")
    report.append("| Rank | Document | Topic | In | Out | PageRank | Hub Score |")
    report.append("|------|----------|-------|-----|-----|----------|-----------|")
    
    for i, hub in enumerate(hubs[:30], 1):
        short_path = hub['path'][:50] + "..." if len(hub['path']) > 50 else hub['path']
        topic = topic_names.get(hub['topic'], hub['topic'])
        report.append(f"| {i} | `{short_path}` | {topic} | {hub['incoming']} | {hub['outgoing']} | {hub['pagerank']:.6f} | {hub['hub_score']:.2f} |")
    report.append("")
    
    # Isolated Documents Analysis
    report.append("## Document Connectivity Analysis\n")
    report.append(f"- **Isolated Documents** (no links): {len(isolated)}")
    report.append(f"- **Sink Documents** (only incoming): {len(sinks)}")
    report.append(f"- **Source Documents** (only outgoing): {len(sources)}\n")
    
    if isolated:
        report.append("### Isolated Documents (Need Attention)\n")
        for doc in isolated[:20]:
            report.append(f"- `{doc}`")
        if len(isolated) > 20:
            report.append(f"- ... and {len(isolated) - 20} more")
        report.append("")
    
    # Visualization Diagrams
    report.append("## Knowledge Graph Visualizations\n")
    
    # Overall graph
    report.append("### Overall Knowledge Graph (Top 50 Core Documents)\n")
    report.append("```mermaid")
    report.append(generate_mermaid_graph(docs, hubs, "Overall Knowledge Graph", 50))
    report.append("```\n")
    
    # Formal Semantics subgraph
    report.append("### Formal Semantics Subgraph\n")
    report.append("```mermaid")
    report.append(generate_mermaid_graph(docs, hubs, "Formal Semantics Knowledge Graph", 40, "formal_semantics"))
    report.append("```\n")
    
    # Industrial subgraph
    report.append("### Industrial Applications Subgraph\n")
    report.append("```mermaid")
    report.append(generate_mermaid_graph(docs, hubs, "Industrial Applications Graph", 40, "industrial"))
    report.append("```\n")
    
    # Topic Distribution
    report.append("### Topic Distribution\n")
    report.append("```mermaid")
    report.append(generate_topic_sunburst(clusters))
    report.append("```\n")
    
    # Improvement Recommendations
    report.append("## Improvement Recommendations\n")
    
    # Broken links
    if broken_links:
        report.append(f"### Broken Links ({len(broken_links)} found)\n")
        report.append("| Source | Broken Link |")
        report.append("|--------|-------------|")
        for source, target in broken_links[:30]:
            short_source = source[:40] + "..." if len(source) > 40 else source
            short_target = target[:40] + "..." if len(target) > 40 else target
            report.append(f"| `{short_source}` | `{short_target}` |")
        if len(broken_links) > 30:
            report.append(f"| ... | ... ({len(broken_links) - 30} more) |")
        report.append("")
    
    # New link suggestions
    if suggestions:
        report.append(f"### Suggested New Links ({len(suggestions)} recommendations)\n")
        report.append("Based on shared connections within topics:\n")
        report.append("| From | To | Reason |")
        report.append("|------|-----|--------|")
        for sugg in suggestions[:20]:
            from_short = sugg['from'][:35] + "..." if len(sugg['from']) > 35 else sugg['from']
            to_short = sugg['to'][:35] + "..." if len(sugg['to']) > 35 else sugg['to']
            report.append(f"| `{from_short}` | `{to_short}` | {sugg['reason']} |")
        report.append("")
    
    # Action items
    report.append("### Priority Action Items\n")
    report.append("1. **Connect Isolated Documents**: Link isolated documents to relevant hub documents")
    report.append("2. **Fix Broken Links**: Repair or remove broken internal links")
    report.append("3. **Cross-Topic Bridging**: Add links between related topics (e.g., Core Knowledge → Industrial)")
    report.append("4. **Hub Strengthening**: Enhance connectivity to top 10 hub documents")
    report.append("5. **Standard Library Integration**: Link more documents to standard library references")
    report.append("")
    
    # Footer
    report.append("---\n")
    report.append("*Report generated by C_Lang Knowledge Graph Analyzer*")
    
    return "\n".join(report)


def main():
    """Main execution function."""
    print("=" * 60)
    print("C_Lang Knowledge Graph Analyzer")
    print("=" * 60)
    
    # Load data
    print("\n[1/7] Loading graph data...")
    graph, stats = load_graph_data()
    print(f"      Loaded {len(graph)} documents from graph")
    
    # Build index
    print("\n[2/7] Building document index...")
    docs = build_document_index(graph)
    print(f"      Indexed {len(docs)} documents")
    
    # Calculate PageRank
    print("\n[3/7] Calculating PageRank scores...")
    pagerank = calculate_pagerank(docs)
    print(f"      Calculated scores for {len(pagerank)} documents")
    
    # Identify hubs
    print("\n[4/7] Identifying hub documents...")
    hubs = identify_hubs(docs, pagerank, 100)
    print(f"      Found top {len(hubs)} hub documents")
    
    # Identify isolated docs
    print("\n[5/7] Analyzing document connectivity...")
    isolated, sinks, sources = identify_isolated_docs(docs)
    print(f"      Isolated: {len(isolated)}, Sinks: {len(sinks)}, Sources: {len(sources)}")
    
    # Topic clusters
    print("\n[6/7] Analyzing topic clusters...")
    clusters = analyze_topic_clusters(docs)
    print(f"      Found {len(clusters)} topic clusters")
    
    # Find broken links and suggestions
    print("\n[7/7] Finding improvement opportunities...")
    broken_links = identify_broken_links(docs)
    suggestions = suggest_new_links(docs, 50)
    print(f"      Found {len(broken_links)} broken links, {len(suggestions)} suggestions")
    
    # Generate report
    print("\n[FINAL] Generating report...")
    report = generate_report(docs, stats, pagerank, hubs, isolated, sinks, sources, 
                           clusters, broken_links, suggestions)
    
    # Write report
    output_path = KNOWLEDGE_DIR / "00_VERSION_TRACKING" / "KNOWLEDGE_GRAPH.md"
    with open(output_path, 'w', encoding='utf-8') as f:
        f.write(report)
    print(f"      Report written to: {output_path}")
    
    # Write diagram files separately
    diagrams_dir = KNOWLEDGE_DIR / "00_VERSION_TRACKING" / "diagrams"
    diagrams_dir.mkdir(exist_ok=True)
    
    # Overall graph
    with open(diagrams_dir / "overall_graph.mmd", 'w', encoding='utf-8') as f:
        f.write(generate_mermaid_graph(docs, hubs, "Overall Knowledge Graph", 50))
    
    # Formal semantics graph
    with open(diagrams_dir / "formal_semantics_graph.mmd", 'w', encoding='utf-8') as f:
        f.write(generate_mermaid_graph(docs, hubs, "Formal Semantics Graph", 40, "formal_semantics"))
    
    # Industrial graph
    with open(diagrams_dir / "industrial_graph.mmd", 'w', encoding='utf-8') as f:
        f.write(generate_mermaid_graph(docs, hubs, "Industrial Applications Graph", 40, "industrial"))
    
    # Topic distribution
    with open(diagrams_dir / "topic_distribution.mmd", 'w', encoding='utf-8') as f:
        f.write(generate_topic_sunburst(clusters))
    
    print(f"      Diagrams written to: {diagrams_dir}")
    
    # Summary
    print("\n" + "=" * 60)
    print("Analysis Complete!")
    print("=" * 60)
    print(f"\nDocuments Analyzed: {len(docs)}")
    print(f"Hub Documents: {len(hubs)}")
    print(f"Topic Clusters: {len(clusters)}")
    print(f"Broken Links: {len(broken_links)}")
    print(f"\nOutputs:")
    print(f"  - Report: {output_path}")
    print(f"  - Diagrams: {diagrams_dir}")


if __name__ == "__main__":
    main()
