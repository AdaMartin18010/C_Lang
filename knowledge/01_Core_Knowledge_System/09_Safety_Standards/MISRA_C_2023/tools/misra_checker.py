#!/usr/bin/env python3
"""
MISRA C:2023 合规性检查工具
简化版静态分析工具，用于基础MISRA规则检查
"""

import re
import sys
import argparse
from pathlib import Path
from typing import List, Tuple

class MISRAViolation:
    """MISRA违规记录"""
    def __init__(self, rule: str, line: int, message: str, severity: str = "Required"):
        self.rule = rule
        self.line = line
        self.message = message
        self.severity = severity
    
    def __str__(self):
        return f"[{self.severity}] Rule {self.rule} (Line {self.line}): {self.message}"

class MISRAChecker:
    """MISRA C:2023 规则检查器"""
    
    def __init__(self):
        self.violations: List[MISRAViolation] = []
        self.lines: List[str] = []
        
    def check_file(self, filepath: Path) -> List[MISRAViolation]:
        """检查单个文件"""
        self.violations = []
        
        try:
            content = filepath.read_text(encoding='utf-8')
            self.lines = content.split('\n')
        except Exception as e:
            print(f"Error reading {filepath}: {e}")
            return []
        
        # 运行所有检查
        self._check_variable_initialization()
        self._check_goto()
        
        return self.violations
    
    def _check_variable_initialization(self):
        """Rule 14.1: 检查变量初始化"""
        var_pattern = re.compile(r'^(\s*)(int|char|float|double|bool|uint\d+_t|int\d+_t)\s+([a-zA-Z_][a-zA-Z0-9_]*)\s*;')
        
        for i, line in enumerate(self.lines, 1):
            match = var_pattern.match(line)
            if match:
                self.violations.append(MISRAViolation(
                    "14.1", i,
                    f"Variable should be initialized at declaration",
                    "Required"
                ))
    
    def _check_goto(self):
        """Rule 12.6: 检查goto使用"""
        for i, line in enumerate(self.lines, 1):
            if re.search(r'\bgoto\b', line):
                self.violations.append(MISRAViolation(
                    "12.6", i,
                    "Use of goto - ensure only forward jumps for cleanup",
                    "Required"
                ))

def main():
    parser = argparse.ArgumentParser(description='MISRA C:2023 Compliance Checker')
    parser.add_argument('files', nargs='+', help='C source files to check')
    
    args = parser.parse_args()
    
    checker = MISRAChecker()
    all_violations = []
    
    print("="*70)
    print("MISRA C:2023 Compliance Checker")
    print("="*70)
    
    for filepath in args.files:
        path = Path(filepath)
        if not path.exists():
            print(f"File not found: {filepath}")
            continue
        
        print(f"\nChecking: {path}")
        print("-"*70)
        
        violations = checker.check_file(path)
        
        if violations:
            for v in violations:
                print(v)
            print(f"\nTotal violations: {len(violations)}")
        else:
            print("No violations found")
        
        all_violations.extend([(str(path), v) for v in violations])
    
    # 汇总
    print("\n" + "="*70)
    print("Summary")
    print("="*70)
    
    if all_violations:
        print(f"Total: {len(all_violations)} violations in {len(args.files)} file(s)")
        return 1
    else:
        print("No MISRA violations found!")
        return 0

if __name__ == '__main__':
    sys.exit(main())
