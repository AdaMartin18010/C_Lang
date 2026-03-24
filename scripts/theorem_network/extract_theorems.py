#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
C_Lang知识库定理提取和依赖分析脚本
扫描所有.md和.lean文件，提取定理定义和依赖关系
"""

import os
import re
import json
from pathlib import Path
from collections import defaultdict
from dataclasses import dataclass, field, asdict
from typing import List, Dict, Set, Tuple, Optional

@dataclass
class Theorem:
    """定理数据结构"""
    name: str                           # 定理名称
    id: str                             # 定理ID（规范化）
    file_path: str                      # 文件路径
    line_number: int                    # 行号
    category: str                       # 类别（操作语义/指称语义/公理语义/分布式/并发等）
    theorem_type: str                   # 类型（theorem/lemma/def/structure等）
    description: str = ""               # 描述
    dependencies: List[str] = field(default_factory=list)  # 依赖的定理
    used_by: List[str] = field(default_factory=list)       # 被哪些定理使用
    lean_code: str = ""                 # Lean代码（如果是.lean文件）
    
class TheoremExtractor:
    """定理提取器"""
    
    # 定理命名模式
    THEOREM_PATTERNS = [
        # BT系列桥梁定理
        r'(?i)定理\s+(BT-\d+[A-Z]?)',  # 定理 BT-1, BT-1-A等
        r'(?i)(BT-\d+[A-Z]?)',          # BT-1, BT-2等
        
        # 类型理论定理
        r'(?i)定理\s+(MLTT-\d+)',       # MLTT系列
        r'(?i)定理\s+(CoC-\d+)',        # CoC系列
        r'(?i)(MLTT-[A-Z]+-\d+)',       # MLTT-XX-00格式
        r'(?i)(CoC-[A-Z]+-\d+)',        # CoC-XX-00格式
        
        # 语义相关
        r'(?i)定理\s+( preservation|progress|safety|soundness|completeness|determinism)',
        r'(?i)(preservation|progress|safety|soundness|completeness|determinism)\s*定理',
        
        # 分布式系统
        r'(?i)定理\s+(electionSafety|logMatching|leaderCompleteness|stateMachineSafety)',
        r'(?i)(electionSafety|logMatching|leaderCompleteness|stateMachineSafety)',
        
        # 编译器相关
        r'(?i)定理\s+(CC-\d+|TCO-\d+|RC-\d+|FFI-\d+|SIMD-\d+)',
        r'(?i)(CC-\d+|TCO-\d+|RC-\d+|FFI-\d+|SIMD-\d+)',
        
        # 类型保持等
        r'类型保持(?:定理)?',
        r'进展(?:定理)?',
        r'安全性(?:定理)?',
        r'健全性(?:定理)?',
        r'完备性(?:定理)?',
        r'选举安全性',
        r'日志匹配性',
        r'领导者完备性',
        r'状态机安全性',
        
        # 引理
        r'(?i)引理\s+([A-Z]+-\d+-[A-Z])',  # 引理 BT-1-A
        r'(?i)(Lemma\s+[A-Z]+-\d+-[A-Z])', # Lemma BT-1-A
        
        # 定义
        r'(?i)定义\s+([\d.]+)',         # 定义 1.1, 1.1.1等
    ]
    
    # Lean代码中的定理模式
    LEAN_THEOREM_PATTERNS = [
        r'theorem\s+(\w+)',
        r'lemma\s+(\w+)',
        r'def\s+(\w+)',
        r'structure\s+(\w+)',
        r'inductive\s+(\w+)',
    ]
    
    # 类别映射
    CATEGORY_MAP = {
        'OperationalSemantics': '操作语义',
        'DenotationalSemantics': '指称语义', 
        'HoareLogic': '公理语义',
        'DistributedConsensus': '分布式系统',
        'Raft': '分布式系统',
        'Paxos': '分布式系统',
        'Concurrency': '并发系统',
        'GameSemantics': '博弈语义',
        'Coalgebraic': '共代数方法',
        'LinearLogic': '线性逻辑',
        'TypeTheory': '类型理论',
        'Compiler': '编译器',
        'ClosureConversion': '编译器',
        'TCO': '编译器',
        'FFI': 'FFI边界',
        'SIMD': 'SIMD优化',
        'MetaM': '元编程',
        'Categorical': '范畴语义',
        'MLTT': '类型理论',
        'CoC': '类型理论',
        'HoTT': '同伦类型论',
    }
    
    def __init__(self, root_dir: str):
        self.root_dir = Path(root_dir)
        self.theorems: Dict[str, Theorem] = {}
        self.file_categories: Dict[str, str] = {}
        
    def determine_category(self, file_path: str) -> str:
        """根据文件路径确定定理类别"""
        path_lower = file_path.lower()
        
        # 检查路径中的关键词
        if 'operational' in path_lower:
            return '操作语义'
        elif 'denotational' in path_lower:
            return '指称语义'
        elif 'hoare' in path_lower or 'axiomatic' in path_lower:
            return '公理语义'
        elif 'raft' in path_lower or 'distributed' in path_lower:
            return '分布式系统'
        elif 'bridge' in path_lower:
            return '桥梁定理'
        elif 'mltt' in path_lower or 'coc' in path_lower:
            return '类型理论'
        elif 'closure' in path_lower or 'compiler' in path_lower:
            return '编译器理论'
        elif 'game' in path_lower:
            return '博弈语义'
        elif 'coalgebraic' in path_lower:
            return '共代数'
        elif 'linear' in path_lower:
            return '线性逻辑'
        elif 'hott' in path_lower:
            return '同伦类型论'
        elif 'categorical' in path_lower:
            return '范畴语义'
        elif 'type' in path_lower:
            return '类型系统'
        else:
            return '其他'
    
    def extract_theorems_from_lean(self, file_path: Path) -> List[Theorem]:
        """从Lean文件中提取定理"""
        theorems = []
        category = self.determine_category(str(file_path))
        
        try:
            with open(file_path, 'r', encoding='utf-8') as f:
                content = f.read()
                lines = content.split('\n')
        except Exception as e:
            print(f"Error reading {file_path}: {e}")
            return theorems
        
        for line_num, line in enumerate(lines, 1):
            for pattern in self.LEAN_THEOREM_PATTERNS:
                matches = re.finditer(pattern, line)
                for match in matches:
                    name = match.group(1)
                    theorem_type = pattern.split()[0]
                    
                    # 生成唯一ID
                    rel_path = str(file_path.relative_to(self.root_dir))
                    theorem_id = f"{rel_path}::{name}"
                    
                    theorem = Theorem(
                        name=name,
                        id=theorem_id,
                        file_path=rel_path,
                        line_number=line_num,
                        category=category,
                        theorem_type=theorem_type,
                        description=f"{theorem_type} {name} from Lean file"
                    )
                    theorems.append(theorem)
                    self.theorems[theorem_id] = theorem
        
        return theorems
    
    def extract_theorems_from_md(self, file_path: Path) -> List[Theorem]:
        """从Markdown文件中提取定理"""
        theorems = []
        category = self.determine_category(str(file_path))
        
        try:
            with open(file_path, 'r', encoding='utf-8') as f:
                content = f.read()
                lines = content.split('\n')
        except Exception as e:
            print(f"Error reading {file_path}: {e}")
            return theorems
        
        # 提取BT系列定理
        bt_pattern = r'(?i)[#【\s]*(?:定理|Theorem)\s+(BT-\d+[A-Z]?)[#】\s]*'
        for line_num, line in enumerate(lines, 1):
            matches = re.finditer(bt_pattern, line)
            for match in matches:
                name = match.group(1)
                theorem_id = f"BRIDGE::{name}"
                
                # 查找描述（接下来的几行）
                description = ""
                for i in range(line_num, min(line_num + 10, len(lines))):
                    desc_line = lines[i].strip()
                    if desc_line and not desc_line.startswith('#'):
                        description = desc_line[:200]
                        break
                
                theorem = Theorem(
                    name=name,
                    id=theorem_id,
                    file_path=str(file_path.relative_to(self.root_dir)),
                    line_number=line_num,
                    category='桥梁定理',
                    theorem_type='theorem',
                    description=description
                )
                theorems.append(theorem)
                self.theorems[theorem_id] = theorem
        
        # 提取其他模式
        other_patterns = [
            (r'(?i)[#【\s]*引理\s+([A-Z]+-\d+-[A-Z])', 'lemma', '引理'),
            (r'(?i)定理\s+(MLTT-[A-Z]+-\d+)', 'theorem', '类型理论'),
            (r'(?i)定理\s+(CoC-[A-Z]+-\d+)', 'theorem', '类型理论'),
            (r'(?i)(类型保持(?:定理)?)', 'theorem', '类型系统'),
            (r'(?i)(进展(?:定理)?)', 'theorem', '类型系统'),
            (r'(?i)(安全性(?:定理)?)', 'theorem', '类型系统'),
            (r'(?i)(electionSafety|选举安全性)', 'theorem', '分布式系统'),
            (r'(?i)(logMatching|日志匹配性)', 'theorem', '分布式系统'),
            (r'(?i)(leaderCompleteness|领导者完备性)', 'theorem', '分布式系统'),
            (r'(?i)(stateMachineSafety|状态机安全性)', 'theorem', '分布式系统'),
        ]
        
        for line_num, line in enumerate(lines, 1):
            for pattern, th_type, th_category in other_patterns:
                matches = re.finditer(pattern, line)
                for match in matches:
                    name = match.group(1)
                    theorem_id = f"MD::{file_path.stem}::{name}"
                    
                    if theorem_id not in self.theorems:
                        theorem = Theorem(
                            name=name,
                            id=theorem_id,
                            file_path=str(file_path.relative_to(self.root_dir)),
                            line_number=line_num,
                            category=th_category,
                            theorem_type=th_type,
                            description=f"{th_type} {name}"
                        )
                        theorems.append(theorem)
                        self.theorems[theorem_id] = theorem
        
        return theorems
    
    def extract_dependencies(self, file_path: Path) -> Dict[str, List[str]]:
        """从文件中提取依赖关系"""
        deps = defaultdict(list)
        
        try:
            with open(file_path, 'r', encoding='utf-8') as f:
                content = f.read()
        except Exception as e:
            return deps
        
        # 在Markdown中查找"由...定理"、"根据...引理"等模式
        dep_patterns = [
            r'由(?:引理|定理|lemma|theorem)\s+([A-Z]+-\d+[A-Z]?)',
            r'根据(?:引理|定理|lemma|theorem)\s+([A-Z]+-\d+[A-Z]?)',
            r'(?:应用|apply|使用)\s+(?:引理|定理|lemma|theorem)?\s*([A-Z]+-\d+[A-Z]?)',
            r'依赖于\s+([A-Z]+-\d+[A-Z]?)',
            r'see\s+(?:theorem|lemma)\s+([A-Z]+-\d+[A-Z]?)',
            r'（见\s*([A-Z]+-\d+[A-Z]?)）',
        ]
        
        for pattern in dep_patterns:
            for match in re.finditer(pattern, content):
                ref = match.group(1)
                # 这里需要更精确地关联到具体的定理
                
        return deps
    
    def scan_directory(self, pattern: str) -> List[Path]:
        """扫描目录中的文件"""
        files = []
        for path in self.root_dir.rglob(pattern):
            if path.is_file():
                files.append(path)
        return files
    
    def extract_all_theorems(self):
        """提取所有定理"""
        # 扫描Lean文件
        lean_files = self.scan_directory('*.lean')
        print(f"Found {len(lean_files)} Lean files")
        
        for lean_file in lean_files:
            self.extract_theorems_from_lean(lean_file)
        
        # 扫描Markdown文件（限制在相关目录）
        md_dirs = [
            'knowledge/02_Formal_Semantics_and_Physics',
            'knowledge/03_System_Technology_Domains',
            'knowledge/05_Deep_Structure_MetaPhysics',
        ]
        
        md_files = []
        for md_dir in md_dirs:
            dir_path = self.root_dir / md_dir
            if dir_path.exists():
                md_files.extend(dir_path.rglob('*.md'))
        
        print(f"Found {len(md_files)} Markdown files")
        
        for md_file in md_files:
            self.extract_theorems_from_md(md_file)
        
        # 特别处理BRIDGE_THEOREMS_DEEPENED.md
        bridge_file = self.root_dir / 'knowledge/02_Formal_Semantics_and_Physics/10_Lean4_Compilation/BRIDGE_THEOREMS_DEEPENED.md'
        if bridge_file.exists():
            self.extract_bridge_theorems(bridge_file)
    
    def extract_bridge_theorems(self, file_path: Path):
        """专门提取桥梁定理"""
        try:
            with open(file_path, 'r', encoding='utf-8') as f:
                content = f.read()
        except Exception as e:
            print(f"Error reading {file_path}: {e}")
            return
        
        # BT-1 到 BT-5 主定理
        main_theorems = [
            ('BT-1', '操作语义到指称语义的构造'),
            ('BT-2', '指称语义到操作语义的提取'),
            ('BT-3', '指称语义到公理语义的转换'),
            ('BT-4', '公理语义到指称语义的转换'),
            ('BT-5', '操作语义与公理语义的直接对应'),
        ]
        
        for th_id, desc in main_theorems:
            theorem_id = f"BRIDGE::{th_id}"
            if theorem_id not in self.theorems:
                self.theorems[theorem_id] = Theorem(
                    name=th_id,
                    id=theorem_id,
                    file_path='knowledge/02_Formal_Semantics_and_Physics/10_Lean4_Compilation/BRIDGE_THEOREMS_DEEPENED.md',
                    line_number=0,
                    category='桥梁定理',
                    theorem_type='theorem',
                    description=desc
                )
        
        # 辅助引理
        lemmas = [
            ('BT-1-A', '极限存在性'),
            ('BT-1-B', '单调性保持'),
            ('BT-1-C', '连续性'),
            ('BT-2-A', '提取良定义性'),
            ('BT-2-B', '求值上下文分解'),
            ('BT-2-C', '正规化'),
            ('BT-3-A', 'wp的单调性'),
            ('BT-3-B', 'wp的合取性'),
            ('BT-3-C', 'wp的连续性'),
            ('BT-4-A', '关系的确定性'),
            ('BT-4-B', '全函数性'),
            ('BT-4-C', '一致性保持'),
            ('BT-5-A', '路径存在性'),
            ('BT-5-B', '不变式保持'),
            ('BT-5-C', '终止性'),
        ]
        
        for lemma_id, desc in lemmas:
            theorem_id = f"BRIDGE::{lemma_id}"
            if theorem_id not in self.theorems:
                self.theorems[theorem_id] = Theorem(
                    name=lemma_id,
                    id=theorem_id,
                    file_path='knowledge/02_Formal_Semantics_and_Physics/10_Lean4_Compilation/BRIDGE_THEOREMS_DEEPENED.md',
                    line_number=0,
                    category='桥梁定理',
                    theorem_type='lemma',
                    description=desc
                )
    
    def build_dependency_graph(self):
        """构建依赖图"""
        # 手动定义已知的依赖关系
        
        # BT-1依赖
        if 'BRIDGE::BT-1' in self.theorems:
            bt1 = self.theorems['BRIDGE::BT-1']
            bt1.dependencies = ['BT-1-A', 'BT-1-B', 'BT-1-C']
        
        # BT-2依赖
        if 'BRIDGE::BT-2' in self.theorems:
            bt2 = self.theorems['BRIDGE::BT-2']
            bt2.dependencies = ['BT-2-A', 'BT-2-B', 'BT-2-C']
        
        # BT-3依赖
        if 'BRIDGE::BT-3' in self.theorems:
            bt3 = self.theorems['BRIDGE::BT-3']
            bt3.dependencies = ['BT-3-A', 'BT-3-B', 'BT-3-C']
        
        # BT-4依赖
        if 'BRIDGE::BT-4' in self.theorems:
            bt4 = self.theorems['BRIDGE::BT-4']
            bt4.dependencies = ['BT-4-A', 'BT-4-B', 'BT-4-C']
        
        # BT-5依赖
        if 'BRIDGE::BT-5' in self.theorems:
            bt5 = self.theorems['BRIDGE::BT-5']
            bt5.dependencies = ['BT-5-A', 'BT-5-B', 'BT-5-C']
        
        # 构建反向依赖（used_by）
        for th_id, theorem in self.theorems.items():
            for dep in theorem.dependencies:
                dep_id = None
                # 尝试找到依赖的完整ID
                for key in self.theorems:
                    if key.endswith(f"::{dep}") or key == f"BRIDGE::{dep}":
                        dep_id = key
                        break
                
                if dep_id and dep_id in self.theorems:
                    if th_id not in self.theorems[dep_id].used_by:
                        self.theorems[dep_id].used_by.append(th_id)
    
    def add_manual_theorems(self):
        """添加手动收集的定理"""
        
        # 操作语义核心定理
        operational_theorems = [
            ('preservation', '类型保持定理', '操作语义'),
            ('progress', '进展定理', '操作语义'),
            ('safety', '安全性定理', '操作语义'),
            ('soundness', '健全性定理', '操作语义'),
            ('determinism', '确定性定理', '操作语义'),
            ('normalization', '规范化定理', '操作语义'),
            ('substitution', '替换引理', '操作语义'),
            ('weakening', '弱化引理', '操作语义'),
        ]
        
        # Raft分布式定理
        raft_theorems = [
            ('electionSafety', '选举安全性', '分布式系统'),
            ('voteUniqueness', '投票唯一性', '分布式系统'),
            ('voteTermConsistency', '投票任期一致性', '分布式系统'),
            ('logMatchingLemma', '日志匹配性引理', '分布式系统'),
            ('logPrefixConsistency', '日志前缀一致性', '分布式系统'),
            ('leaderCompleteness', '领导者完备性', '分布式系统'),
            ('commitRuleCurrentTerm', '提交规则当前任期', '分布式系统'),
            ('indirectCommit', '间接提交', '分布式系统'),
            ('stateMachineSafety', '状态机安全性', '分布式系统'),
            ('committedLogPrefix', '已提交日志前缀', '分布式系统'),
            ('termMonotonicity', '任期单调性', '分布式系统'),
            ('systemTermMonotonicity', '系统任期单调性', '分布式系统'),
            ('leaderLogMonotonicity', 'Leader日志单调性', '分布式系统'),
            ('raftSafety', 'Raft安全性综合', '分布式系统'),
        ]
        
        # Hoare逻辑定理
        hoare_theorems = [
            ('assignRule', '赋值规则', '公理语义'),
            ('seqRule', '序列规则', '公理语义'),
            ('ifRule', '条件规则', '公理语义'),
            ('whileRule', '循环规则', '公理语义'),
            ('consequenceRule', '推论规则', '公理语义'),
            ('skipRule', '跳过规则', '公理语义'),
            ('verifySwap', '交换验证', '公理语义'),
            ('verifyIncr', '递增验证', '公理语义'),
            ('verifyAbs', '绝对值验证', '公理语义'),
            ('verifyDecr', '递减验证', '公理语义'),
            ('verifyFactorial', '阶乘验证', '公理语义'),
            ('verifySum', '求和验证', '公理语义'),
            ('verifyGCD', 'GCD验证', '公理语义'),
        ]
        
        # 编译器相关定理
        compiler_theorems = [
            ('closure_conversion_correctness', '闭包转换正确性', '编译器理论'),
            ('tco_correctness', '尾调用优化正确性', '编译器理论'),
            ('rc_correctness', '引用计数正确性', '编译器理论'),
            ('ffi_boundary_safety', 'FFI边界安全性', '编译器理论'),
            ('simd_vectorization_correctness', 'SIMD向量化正确性', '编译器理论'),
        ]
        
        # 添加所有手动定理
        all_manual = (
            operational_theorems + raft_theorems + 
            hoare_theorems + compiler_theorems
        )
        
        for name, desc, category in all_manual:
            th_id = f"MANUAL::{name}"
            if th_id not in self.theorems:
                self.theorems[th_id] = Theorem(
                    name=name,
                    id=th_id,
                    file_path='various',
                    line_number=0,
                    category=category,
                    theorem_type='theorem',
                    description=desc
                )
    
    def get_statistics(self) -> Dict:
        """获取统计信息"""
        stats = {
            'total_theorems': len(self.theorems),
            'by_category': defaultdict(int),
            'by_type': defaultdict(int),
            'by_file': defaultdict(int),
        }
        
        for th in self.theorems.values():
            stats['by_category'][th.category] += 1
            stats['by_type'][th.theorem_type] += 1
            stats['by_file'][th.file_path] += 1
        
        return stats
    
    def export_to_json(self, output_path: str):
        """导出到JSON"""
        data = {
            'theorems': {k: asdict(v) for k, v in self.theorems.items()},
            'statistics': dict(self.get_statistics())
        }
        
        with open(output_path, 'w', encoding='utf-8') as f:
            json.dump(data, f, ensure_ascii=False, indent=2)
        
        print(f"Exported to {output_path}")


def main():
    root_dir = 'e:/_src/C_Lang'
    extractor = TheoremExtractor(root_dir)
    
    print("Starting theorem extraction...")
    extractor.extract_all_theorems()
    print(f"Extracted {len(extractor.theorems)} theorems")
    
    print("Adding manual theorems...")
    extractor.add_manual_theorems()
    print(f"Total theorems after adding manual: {len(extractor.theorems)}")
    
    print("Building dependency graph...")
    extractor.build_dependency_graph()
    
    # 统计信息
    stats = extractor.get_statistics()
    print(f"\nStatistics:")
    print(f"  Total theorems: {stats['total_theorems']}")
    print(f"  By category:")
    for cat, count in sorted(stats['by_category'].items(), key=lambda x: -x[1]):
        print(f"    {cat}: {count}")
    
    # 导出
    extractor.export_to_json('scripts/theorem_network/theorems_raw.json')
    
    return extractor


if __name__ == '__main__':
    main()
