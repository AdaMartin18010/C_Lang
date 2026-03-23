#!/usr/bin/env python3
"""
C语言代码示例验证脚本
自动提取Markdown文件中的C代码块并验证编译
"""

import os
import re
import sys
import subprocess
import tempfile
import json
from pathlib import Path
from dataclasses import dataclass
from typing import List, Tuple, Optional
from concurrent.futures import ThreadPoolExecutor, as_completed
import argparse


@dataclass
class ValidationResult:
    """验证结果"""
    file_path: str
    code_block_index: int
    line_number: int
    success: bool
    output: str
    error: str
    duration_ms: float


class CCodeValidator:
    """C代码验证器"""
    
    def __init__(self, knowledge_dir: str = "knowledge", 
                 compiler: str = "gcc",
                 compiler_flags: List[str] = None):
        self.knowledge_dir = Path(knowledge_dir)
        self.compiler = compiler
        self.compiler_flags = compiler_flags or [
            "-Wall", "-Wextra", "-Wpedantic",
            "-std=c17", "-O0", "-c"
        ]
        self.results: List[ValidationResult] = []
        self.stats = {
            "total": 0,
            "success": 0,
            "failed": 0,
            "skipped": 0
        }
    
    def extract_code_blocks(self, md_file: Path) -> List[Tuple[int, str, str]]:
        """
        从Markdown文件提取C代码块
        返回: [(行号, 代码类型, 代码内容), ...]
        """
        content = md_file.read_text(encoding='utf-8')
        
        # 匹配 ```c 或 ```c 代码块
        pattern = r'```c\n(.*?)```'
        matches = []
        
        for match in re.finditer(pattern, content, re.DOTALL):
            code = match.group(1).strip()
            
            # 计算行号
            line_num = content[:match.start()].count('\n') + 1
            
            # 过滤掉伪代码、片段和不完整的示例
            if self._is_valid_c_code(code):
                matches.append((line_num, code))
        
        return matches
    
    def _is_valid_c_code(self, code: str) -> bool:
        """判断是否为可编译的C代码"""
        # 必须包含完整的函数定义或main函数
        has_function = re.search(r'\b\w+\s+\w+\s*\([^)]*\)\s*\{', code)
        has_main = 'int main' in code or 'void main' in code
        
        # 排除明显的伪代码标记
        pseudo_markers = ['XXX', 'TODO', 'FIXME', '...', '/* ... */']
        has_pseudo = any(marker in code for marker in pseudo_markers)
        
        # 代码长度检查(太短的可能是片段)
        if len(code) < 50:
            return False
        
        return (has_function or has_main) and not has_pseudo
    
    def compile_code(self, code: str, timeout: int = 10) -> Tuple[bool, str, str, float]:
        """
        编译C代码
        返回: (成功, 标准输出, 标准错误, 耗时毫秒)
        """
        import time
        
        start_time = time.time()
        
        # 创建临时文件
        with tempfile.NamedTemporaryFile(mode='w', suffix='.c', 
                                         delete=False) as f:
            f.write(code)
            temp_source = f.name
        
        temp_object = temp_source.replace('.c', '.o')
        
        try:
            cmd = [self.compiler] + self.compiler_flags + \
                  [temp_source, '-o', temp_object]
            
            result = subprocess.run(
                cmd,
                capture_output=True,
                text=True,
                timeout=timeout
            )
            
            duration = (time.time() - start_time) * 1000
            
            success = result.returncode == 0
            
            # 清理警告中的临时文件路径
            stdout = self._clean_output(result.stdout, temp_source)
            stderr = self._clean_output(result.stderr, temp_source)
            
            return success, stdout, stderr, duration
            
        except subprocess.TimeoutExpired:
            return False, "", "编译超时", (time.time() - start_time) * 1000
        except Exception as e:
            return False, "", str(e), (time.time() - start_time) * 1000
        finally:
            # 清理临时文件
            for f in [temp_source, temp_object]:
                if os.path.exists(f):
                    os.unlink(f)
    
    def _clean_output(self, output: str, temp_path: str) -> str:
        """清理输出中的临时文件路径"""
        return output.replace(temp_path, '<source>')
    
    def validate_file(self, md_file: Path) -> List[ValidationResult]:
        """验证单个Markdown文件"""
        results = []
        code_blocks = self.extract_code_blocks(md_file)
        
        print(f"  找到 {len(code_blocks)} 个代码块")
        
        for idx, (line_num, code) in enumerate(code_blocks):
            self.stats["total"] += 1
            
            # 检查是否为特殊标记的示例
            if '@compile: skip' in code or '@skip-validation' in code:
                self.stats["skipped"] += 1
                continue
            
            success, stdout, stderr, duration = self.compile_code(code)
            
            if success:
                self.stats["success"] += 1
                print(f"    ✓ 代码块 {idx+1} (行{line_num})")
            else:
                self.stats["failed"] += 1
                print(f"    ✗ 代码块 {idx+1} (行{line_num})")
                if stderr:
                    # 只显示第一行错误
                    first_error = stderr.split('\n')[0][:100]
                    print(f"      错误: {first_error}")
            
            results.append(ValidationResult(
                file_path=str(md_file),
                code_block_index=idx,
                line_number=line_num,
                success=success,
                output=stdout,
                error=stderr,
                duration_ms=duration
            ))
        
        return results
    
    def validate_all(self, pattern: str = "**/*.md", 
                     max_workers: int = 4) -> List[ValidationResult]:
        """验证所有匹配的文件"""
        md_files = list(self.knowledge_dir.glob(pattern))
        
        print(f"找到 {len(md_files)} 个Markdown文件")
        print("=" * 60)
        
        # 串行处理以更好显示进度
        for md_file in md_files:
            print(f"\n处理: {md_file}")
            try:
                results = self.validate_file(md_file)
                self.results.extend(results)
            except Exception as e:
                print(f"  错误: {e}")
        
        return self.results
    
    def print_report(self):
        """打印验证报告"""
        print("\n" + "=" * 60)
        print("验证报告")
        print("=" * 60)
        
        total = self.stats["total"]
        success = self.stats["success"]
        failed = self.stats["failed"]
        skipped = self.stats["skipped"]
        
        print(f"总代码块: {total}")
        print(f"成功:     {success} ({100*success/total:.1f}%)")
        print(f"失败:     {failed} ({100*failed/total:.1f}%)")
        print(f"跳过:     {skipped}")
        
        if failed > 0:
            print("\n失败详情:")
            for r in self.results:
                if not r.success:
                    print(f"\n  {r.file_path}:{r.line_number}")
                    if r.error:
                        print(f"  错误: {r.error[:200]}")
        
        print("\n" + "=" * 60)
    
    def export_json(self, output_file: str):
        """导出结果为JSON"""
        data = {
            "stats": self.stats,
            "results": [
                {
                    "file": r.file_path,
                    "line": r.line_number,
                    "success": r.success,
                    "error": r.error if not r.success else None
                }
                for r in self.results
            ]
        }
        
        with open(output_file, 'w') as f:
            json.dump(data, f, indent=2)
        
        print(f"\n结果已导出到: {output_file}")


def main():
    parser = argparse.ArgumentParser(description='验证C语言代码示例')
    parser.add_argument('--dir', default='knowledge', 
                       help='知识库目录')
    parser.add_argument('--pattern', default='**/*.md',
                       help='文件匹配模式')
    parser.add_argument('--compiler', default='gcc',
                       help='C编译器')
    parser.add_argument('--output', '-o', default='validation_report.json',
                       help='输出报告文件')
    parser.add_argument('--strict', action='store_true',
                       help='将警告视为错误')
    
    args = parser.parse_args()
    
    flags = ["-Wall", "-Wextra", "-Wpedantic", "-std=c17", "-O0", "-c"]
    if args.strict:
        flags.append("-Werror")
    
    validator = CCodeValidator(
        knowledge_dir=args.dir,
        compiler=args.compiler,
        compiler_flags=flags
    )
    
    validator.validate_all(pattern=args.pattern)
    validator.print_report()
    validator.export_json(args.output)
    
    # 如果有失败，返回非零退出码
    if validator.stats["failed"] > 0:
        sys.exit(1)


if __name__ == "__main__":
    main()
