#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
增强定理依赖网络，添加手动定义的依赖关系
"""

import json
from pathlib import Path
from collections import defaultdict

def load_theorems():
    """加载定理数据"""
    with open('scripts/theorem_network/theorems_raw.json', 'r', encoding='utf-8') as f:
        return json.load(f)

def define_dependency_chains():
    """定义已知的定理依赖链"""
    
    chains = {
        # 桥梁定理依赖链
        '桥梁定理': {
            'BT-1': ['BT-1-A', 'BT-1-B', 'BT-1-C'],
            'BT-2': ['BT-2-A', 'BT-2-B', 'BT-2-C'],
            'BT-3': ['BT-3-A', 'BT-3-B', 'BT-3-C'],
            'BT-4': ['BT-4-A', 'BT-4-B', 'BT-4-C'],
            'BT-5': ['BT-5-A', 'BT-5-B', 'BT-5-C'],
            # 跨桥梁依赖
            'BT-2': ['BT-1'],  # BT-2依赖于BT-1的构造
            'BT-3': ['BT-1', 'BT-2'],  # BT-3依赖于前两个
            'BT-4': ['BT-2', 'BT-3'],
            'BT-5': ['BT-1', 'BT-3'],
        },
        
        # 操作语义核心定理链
        '操作语义': {
            'preservation': ['substitution', 'weakening'],
            'progress': ['type_of_value'],
            'safety': ['preservation', 'progress'],
            'soundness': ['safety', 'preservation'],
            'normalization': ['progress', 'preservation'],
            'determinism': ['value_irreducible'],
        },
        
        # 类型理论定理链
        '类型理论': {
            'MLTT-FORM-00': [],
            'MLTT-INTRO-00': ['MLTT-FORM-00'],
            'MLTT-ELIM-00': ['MLTT-FORM-00', 'MLTT-INTRO-00'],
            'MLTT-COMP-00': ['MLTT-ELIM-00'],
            'MLTT-SUBST-00': ['MLTT-COMP-00'],
            'MLTT-WEAK-00': ['MLTT-SUBST-00'],
            'CoC-VAR-00': [],
            'CoC-ABS-00': ['CoC-VAR-00'],
            'CoC-APP-00': ['CoC-ABS-00'],
            'CoC-CONV-00': ['CoC-APP-00'],
        },
        
        # 公理语义定理链
        '公理语义': {
            'assignRule': [],
            'seqRule': ['assignRule'],
            'ifRule': ['assignRule'],
            'whileRule': ['assignRule'],
            'consequenceRule': ['assignRule'],
            'verifySwap': ['seqRule', 'assignRule'],
            'verifyIncr': ['assignRule'],
            'verifyAbs': ['ifRule', 'assignRule'],
            'verifyDecr': ['whileRule', 'assignRule'],
            'verifyFactorial': ['whileRule', 'seqRule', 'assignRule'],
            'verifySum': ['whileRule', 'seqRule'],
            'verifyGCD': ['whileRule', 'ifRule'],
        },
        
        # 分布式系统定理链
        '分布式系统': {
            'voteUniqueness': [],
            'voteTermConsistency': ['voteUniqueness'],
            'electionSafety': ['voteUniqueness', 'voteTermConsistency'],
            'logMatchingLemma': [],
            'logPrefixConsistency': ['logMatchingLemma'],
            'leaderCompleteness': ['electionSafety', 'logMatchingLemma'],
            'commitRuleCurrentTerm': ['leaderCompleteness'],
            'indirectCommit': ['commitRuleCurrentTerm'],
            'stateMachineSafety': ['leaderCompleteness', 'logMatchingLemma', 'logPrefixConsistency'],
            'committedLogPrefix': ['stateMachineSafety', 'logPrefixConsistency'],
            'termMonotonicity': [],
            'systemTermMonotonicity': ['termMonotonicity'],
            'leaderLogMonotonicity': ['leaderCompleteness'],
            'raftSafety': ['electionSafety', 'stateMachineSafety', 'leaderCompleteness'],
        },
        
        # 编译器定理链
        '编译器理论': {
            'closure_conversion_correctness': ['preservation'],
            'tco_correctness': ['preservation'],
            'rc_correctness': ['preservation'],
            'ffi_boundary_safety': ['preservation', 'safety'],
            'simd_vectorization_correctness': ['preservation'],
        },
        
        # 指称语义定理链
        '指称语义': {
            'continuity': [],
            'fixedpoint': ['continuity'],
            'domain': ['continuity', 'fixedpoint'],
            'meaning': ['domain', 'fixedpoint'],
        },
    }
    
    return chains

def build_enhanced_dependencies(theorems_data):
    """构建增强的依赖关系"""
    theorems = theorems_data.get('theorems', {})
    chains = define_dependency_chains()
    
    # 创建名称到ID的映射
    name_to_id = {}
    for th_id, th in theorems.items():
        name = th.get('name', '')
        if name:
            if name not in name_to_id:
                name_to_id[name] = []
            name_to_id[name].append(th_id)
    
    # 添加依赖关系
    for category, deps in chains.items():
        for th_name, dependencies in deps.items():
            # 查找定理ID
            th_ids = name_to_id.get(th_name, [])
            for th_id in th_ids:
                if th_id in theorems:
                    current_deps = theorems[th_id].get('dependencies', [])
                    for dep_name in dependencies:
                        if dep_name not in current_deps:
                            current_deps.append(dep_name)
                    theorems[th_id]['dependencies'] = current_deps
    
    # 构建used_by关系
    for th_id, th in theorems.items():
        th['used_by'] = []
    
    for th_id, th in theorems.items():
        for dep in th.get('dependencies', []):
            # 查找依赖的定理
            for other_id, other_th in theorems.items():
                if other_th.get('name') == dep:
                    if th_id not in other_th.get('used_by', []):
                        if 'used_by' not in other_th:
                            other_th['used_by'] = []
                        other_th['used_by'].append(th_id)
    
    return theorems_data

def select_248_core_theorems(theorems_data):
    """精心选择248个核心定理"""
    theorems = theorems_data.get('theorems', {})
    
    # 定义优先级
    priorities = {
        'BRIDGE::BT-1': 100,
        'BRIDGE::BT-2': 100,
        'BRIDGE::BT-3': 100,
        'BRIDGE::BT-4': 100,
        'BRIDGE::BT-5': 100,
        'BRIDGE::BT-1-A': 90,
        'BRIDGE::BT-1-B': 90,
        'BRIDGE::BT-1-C': 90,
        'BRIDGE::BT-2-A': 90,
        'BRIDGE::BT-2-B': 90,
        'BRIDGE::BT-2-C': 90,
        'BRIDGE::BT-3-A': 90,
        'BRIDGE::BT-3-B': 90,
        'BRIDGE::BT-3-C': 90,
        'BRIDGE::BT-4-A': 90,
        'BRIDGE::BT-4-B': 90,
        'BRIDGE::BT-4-C': 90,
        'BRIDGE::BT-5-A': 90,
        'BRIDGE::BT-5-B': 90,
        'BRIDGE::BT-5-C': 90,
    }
    
    # 为每个定理分配优先级
    scored = []
    for th_id, th in theorems.items():
        score = priorities.get(th_id, 0)
        
        # 根据类别加分
        category = th.get('category', '')
        if category == '桥梁定理':
            score += 80
        elif category == '分布式系统':
            score += 70
        elif category in ['操作语义', '指称语义', '公理语义']:
            score += 65
        elif category == '类型理论':
            score += 60
        elif category == '编译器理论':
            score += 50
        
        # 根据类型加分
        th_type = th.get('theorem_type', '')
        if th_type == 'theorem':
            score += 10
        elif th_type == 'lemma':
            score += 5
        
        # 根据依赖数量加分（被依赖越多越重要）
        used_by = th.get('used_by', [])
        score += len(used_by) * 3
        
        scored.append((th_id, score))
    
    # 按分数排序
    scored.sort(key=lambda x: -x[1])
    
    # 选择前248个
    selected = [th_id for th_id, _ in scored[:248]]
    
    return selected

def generate_complete_network(theorems_data, selected_ids):
    """生成完整的网络数据"""
    theorems = theorems_data.get('theorems', {})
    
    nodes = []
    edges = []
    
    for th_id in selected_ids:
        if th_id in theorems:
            th = theorems[th_id]
            nodes.append({
                'id': th_id,
                'name': th.get('name', th_id),
                'category': th.get('category', '其他'),
                'type': th.get('theorem_type', 'unknown'),
                'description': th.get('description', '')[:100],
            })
            
            # 添加边
            for dep in th.get('dependencies', []):
                # 查找依赖的ID
                for other_id in selected_ids:
                    if other_id in theorems:
                        if theorems[other_id].get('name') == dep:
                            edges.append({
                                'source': th_id,
                                'target': other_id,
                                'type': 'depends_on'
                            })
    
    return {'nodes': nodes, 'edges': edges}

def main():
    print("Loading theorems...")
    data = load_theorems()
    
    print("Building enhanced dependencies...")
    data = build_enhanced_dependencies(data)
    
    print("Selecting 248 core theorems...")
    selected = select_248_core_theorems(data)
    print(f"Selected {len(selected)} theorems")
    
    print("Generating complete network...")
    network = generate_complete_network(data, selected)
    print(f"Network has {len(network['nodes'])} nodes and {len(network['edges'])} edges")
    
    # 保存
    with open('scripts/theorem_network/network_248.json', 'w', encoding='utf-8') as f:
        json.dump(network, f, ensure_ascii=False, indent=2)
    
    # 更新原始数据
    with open('scripts/theorem_network/theorems_enhanced.json', 'w', encoding='utf-8') as f:
        json.dump(data, f, ensure_ascii=False, indent=2)
    
    print("Done!")
    return network

if __name__ == '__main__':
    main()
