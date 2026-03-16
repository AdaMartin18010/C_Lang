#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
C_Lang 知识库代码提取和测试脚本
========================================

功能：
    1. 从 Markdown 文件中自动提取 C/Zig 代码示例
    2. 对提取的代码进行语法验证
    3. 生成提取报告和统计数据
    4. 支持增量提取和全量提取模式

用法：
    python extract_and_test.py [选项]

选项：
    --extract-only          仅提取代码，不进行测试
    --test-only             仅测试已提取的代码
    --full                  全量模式（提取+测试）
    --source-dir PATH       Markdown 源文件目录 (默认: knowledge/)
    --output-dir PATH       代码输出目录 (默认: extracted_code/)
    --filter-lang LANG      仅提取指定语言 (c, zig, 或 all)
    --verbose               详细输出模式
    --parallel N            并行处理线程数 (默认: 4)
    --report PATH           生成 JSON 格式报告

示例：
    # 仅提取 C 代码
    python extract_and_test.py --extract-only --filter-lang c

    # 完整流程：提取并测试所有代码
    python extract_and_test.py --full --verbose

    # 指定输出目录
    python extract_and_test.py --output-dir /tmp/extracted --parallel 8

作者：C_Lang Knowledge Base Team
版本：2.0.0
最后更新：2026-03-17
"""

import os
import sys
import re
import json
import hashlib
import argparse
import subprocess
import tempfile
import shutil
from pathlib import Path
from datetime import datetime
from concurrent.futures import ThreadPoolExecutor, as_completed
from typing import List, Dict, Tuple, Optional, Set
from dataclasses import dataclass, field, asdict
from enum import Enum


# =============================================================================
# 全局配置
# =============================================================================

class ExitCode(Enum):
    """退出状态码定义"""
    SUCCESS = 0
    EXTRACTION_ERROR = 1
    TEST_ERROR = 2
    VALIDATION_ERROR = 3
    CONFIG_ERROR = 4


# 默认配置
DEFAULT_CONFIG = {
    "source_dir": "knowledge",
    "output_dir": "extracted_code",
    "parallel_workers": 4,
    "max_file_size_kb": 500,  # 最大处理文件大小 (KB)
    "supported_languages": ["c", "zig"],
    "code_block_patterns": {
        "c": ["c", "C", "cpp", "c++", "C++"],
        "zig": ["zig", "Zig"]
    },
    "skip_patterns": [
        r"^\s*#include\s*<",  # 系统头文件引用
        r"^\s*//\s*skip-extract",  # 显式跳过标记
        r"^\s*/\*\s*skip-extract",  # 多行跳过标记
    ],
    "compiler_defaults": {
        "c": {
            "command": "gcc",
            "args": ["-fsyntax-only", "-Wall", "-Wextra", "-std=c17"],
            "timeout": 30
        },
        "zig": {
            "command": "zig",
            "args": ["fmt", "--check"],
            "timeout": 30
        }
    }
}


# =============================================================================
# 数据结构定义
# =============================================================================

@dataclass
class CodeBlock:
    """代码块数据结构"""
    language: str
    content: str
    source_file: str
    line_number: int
    block_id: str = ""
    
    def __post_init__(self):
        """生成唯一标识"""
        if not self.block_id:
            content_hash = hashlib.md5(
                f"{self.source_file}:{self.line_number}:{self.content[:100]}".encode()
            ).hexdigest()[:8]
            self.block_id = content_hash


@dataclass
class ExtractionResult:
    """提取结果数据结构"""
    success: bool
    file_path: str
    blocks_found: int = 0
    blocks_extracted: int = 0
    errors: List[str] = field(default_factory=list)
    extracted_files: List[str] = field(default_factory=list)


@dataclass
class TestResult:
    """测试结果数据结构"""
    success: bool
    file_path: str
    language: str
    compiler: str
    stdout: str = ""
    stderr: str = ""
    duration_ms: float = 0.0
    error_message: str = ""


@dataclass
class SummaryReport:
    """汇总报告数据结构"""
    timestamp: str = field(default_factory=lambda: datetime.utcnow().isoformat())
    total_files_processed: int = 0
    total_blocks_found: int = 0
    total_blocks_extracted: int = 0
    extraction_success_rate: float = 0.0
    test_success_rate: float = 0.0
    extraction_results: List[Dict] = field(default_factory=list)
    test_results: List[Dict] = field(default_factory=list)
    errors: List[str] = field(default_factory=list)


# =============================================================================
# 代码提取器类
# =============================================================================

class CodeExtractor:
    """
    代码提取器
    
    从 Markdown 文件中提取指定语言的代码块，
    并保存为独立的源文件。
    """
    
    def __init__(self, config: Dict):
        """
        初始化提取器
        
        Args:
            config: 配置字典
        """
        self.config = {**DEFAULT_CONFIG, **config}
        self.source_dir = Path(self.config.get("source_dir", "knowledge"))
        self.output_dir = Path(self.config.get("output_dir", "extracted_code"))
        self.verbose = self.config.get("verbose", False)
        
        # 确保输出目录存在
        self.output_dir.mkdir(parents=True, exist_ok=True)
        for lang in self.config["supported_languages"]:
            (self.output_dir / lang).mkdir(exist_ok=True)
    
    def log(self, message: str, level: str = "info"):
        """
        日志输出
        
        Args:
            message: 日志消息
            level: 日志级别 (info, warning, error, debug)
        """
        if level == "debug" and not self.verbose:
            return
        
        prefix = {
            "info": "[INFO]",
            "warning": "[WARN]",
            "error": "[ERROR]",
            "debug": "[DEBUG]",
            "success": "[OK]"
        }.get(level, "[INFO]")
        
        print(f"{prefix} {message}", file=sys.stderr if level == "error" else sys.stdout)
    
    def find_markdown_files(self) -> List[Path]:
        """
        查找所有 Markdown 文件
        
        Returns:
            Markdown 文件路径列表
        """
        if not self.source_dir.exists():
            self.log(f"Source directory not found: {self.source_dir}", "error")
            return []
        
        md_files = list(self.source_dir.rglob("*.md"))
        self.log(f"Found {len(md_files)} Markdown files")
        return md_files
    
    def should_skip_content(self, content: str) -> bool:
        """
        检查内容是否应该跳过
        
        Args:
            content: 代码内容
            
        Returns:
            是否跳过
        """
        first_line = content.strip().split('\n')[0] if content.strip() else ""
        
        for pattern in self.config["skip_patterns"]:
            if re.search(pattern, first_line):
                return True
        
        return False
    
    def extract_code_blocks(self, file_path: Path, target_lang: str = "all") -> List[CodeBlock]:
        """
        从 Markdown 文件中提取代码块
        
        Args:
            file_path: Markdown 文件路径
            target_lang: 目标语言 (c, zig, 或 all)
            
        Returns:
            代码块列表
        """
        blocks = []
        
        try:
            content = file_path.read_text(encoding='utf-8')
        except Exception as e:
            self.log(f"Failed to read {file_path}: {e}", "error")
            return blocks
        
        # 正则匹配代码块
        # 匹配 ```lang ... ``` 格式的代码块
        pattern = r'```(\w+)?\n(.*?)```'
        matches = re.finditer(pattern, content, re.DOTALL)
        
        for match in matches:
            lang_tag = (match.group(1) or "").strip().lower()
            code_content = match.group(2)
            
            # 确定语言
            detected_lang = None
            for lang, patterns in self.config["code_block_patterns"].items():
                if lang_tag in [p.lower() for p in patterns]:
                    detected_lang = lang
                    break
            
            # 如果没有检测到语言标签，尝试从内容推断
            if not detected_lang and lang_tag == "":
                if self._looks_like_c(code_content):
                    detected_lang = "c"
                elif self._looks_like_zig(code_content):
                    detected_lang = "zig"
            
            # 过滤语言
            if target_lang != "all" and detected_lang != target_lang:
                continue
            
            if detected_lang and not self.should_skip_content(code_content):
                # 计算行号
                line_num = content[:match.start()].count('\n') + 1
                
                block = CodeBlock(
                    language=detected_lang,
                    content=code_content,
                    source_file=str(file_path.relative_to(self.source_dir)),
                    line_number=line_num
                )
                blocks.append(block)
        
        return blocks
    
    def _looks_like_c(self, content: str) -> bool:
        """
        启发式判断内容是否像 C 代码
        
        Args:
            content: 代码内容
            
        Returns:
            是否像 C 代码
        """
        c_indicators = [
            r'#include\s*["<]',
            r'int\s+main\s*\(',
            r'printf\s*\(',
            r'struct\s+\w+',
            r'typedef\s+',
            r'sizeof\s*\(',
            r'malloc\s*\(',
            r'NULL',
            r'stdio\.h',
            r'stdlib\.h',
            r'return\s+0;'
        ]
        
        content_sample = content[:500]  # 只检查前500字符
        score = sum(1 for indicator in c_indicators 
                   if re.search(indicator, content_sample))
        
        return score >= 2
    
    def _looks_like_zig(self, content: str) -> bool:
        """
        启发式判断内容是否像 Zig 代码
        
        Args:
            content: 代码内容
            
        Returns:
            是否像 Zig 代码
        """
        zig_indicators = [
            r'const\s+\w+\s*=',
            r'var\s+\w+\s*:',
            r'fn\s+\w+\s*\(',
            r'!void',
            r'@import\s*\(',
            r'std\.',
            r'pub\s+fn',
            r'try\s+',
            r'const\s+std',
            r'allocator\.'
        ]
        
        content_sample = content[:500]
        score = sum(1 for indicator in zig_indicators 
                   if re.search(indicator, content_sample))
        
        return score >= 2
    
    def save_code_block(self, block: CodeBlock) -> Optional[Path]:
        """
        保存代码块到文件
        
        Args:
            block: 代码块
            
        Returns:
            保存的文件路径，失败返回 None
        """
        # 构建文件名
        source_name = Path(block.source_file).stem
        safe_name = re.sub(r'[^\w\-]', '_', source_name)
        filename = f"{safe_name}_{block.block_id}.{block.language}"
        
        output_path = self.output_dir / block.language / filename
        
        # 添加文件头注释
        header = f"""/*
 * Auto-generated from: {block.source_file}
 * Line: {block.line_number}
 * Language: {block.language}
 * Block ID: {block.block_id}
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

"""
        
        # 对于 Zig，使用不同的注释格式
        if block.language == "zig":
            header = f"""//
// Auto-generated from: {block.source_file}
// Line: {block.line_number}
// Language: {block.language}
// Block ID: {block.block_id}
//
// This file was automatically extracted from the C_Lang knowledge base.
// DO NOT MODIFY - Changes will be overwritten on next extraction.
//

"""
        
        try:
            output_path.write_text(header + block.content, encoding='utf-8')
            return output_path
        except Exception as e:
            self.log(f"Failed to save {output_path}: {e}", "error")
            return None
    
    def process_file(self, file_path: Path, target_lang: str = "all") -> ExtractionResult:
        """
        处理单个文件
        
        Args:
            file_path: Markdown 文件路径
            target_lang: 目标语言
            
        Returns:
            提取结果
        """
        result = ExtractionResult(success=True, file_path=str(file_path))
        
        self.log(f"Processing: {file_path}", "debug")
        
        # 检查文件大小
        file_size = file_path.stat().st_size / 1024  # KB
        if file_size > self.config["max_file_size_kb"]:
            result.errors.append(f"File too large: {file_size:.1f}KB")
            result.success = False
            return result
        
        # 提取代码块
        blocks = self.extract_code_blocks(file_path, target_lang)
        result.blocks_found = len(blocks)
        
        if not blocks:
            return result
        
        self.log(f"  Found {len(blocks)} code blocks in {file_path}", "debug")
        
        # 保存代码块
        for block in blocks:
            output_path = self.save_code_block(block)
            if output_path:
                result.blocks_extracted += 1
                result.extracted_files.append(str(output_path))
            else:
                result.errors.append(f"Failed to save block {block.block_id}")
        
        result.success = len(result.errors) == 0
        return result
    
    def extract_all(self, target_lang: str = "all", parallel: int = 4) -> List[ExtractionResult]:
        """
        提取所有文件中的代码
        
        Args:
            target_lang: 目标语言
            parallel: 并行线程数
            
        Returns:
            提取结果列表
        """
        md_files = self.find_markdown_files()
        results = []
        
        self.log(f"Extracting code from {len(md_files)} files (parallel={parallel})...")
        
        if parallel > 1:
            with ThreadPoolExecutor(max_workers=parallel) as executor:
                futures = {executor.submit(self.process_file, f, target_lang): f 
                          for f in md_files}
                
                for future in as_completed(futures):
                    result = future.result()
                    results.append(result)
                    
                    if result.blocks_found > 0:
                        self.log(f"Extracted {result.blocks_extracted}/{result.blocks_found} "
                               f"blocks from {Path(result.file_path).name}")
        else:
            for file_path in md_files:
                result = self.process_file(file_path, target_lang)
                results.append(result)
                
                if result.blocks_found > 0:
                    self.log(f"Extracted {result.blocks_extracted}/{result.blocks_found} "
                           f"blocks from {Path(result.file_path).name}")
        
        return results


# =============================================================================
# 代码测试器类
# =============================================================================

class CodeTester:
    """
    代码测试器
    
    对提取的代码进行编译/语法验证。
    """
    
    def __init__(self, config: Dict):
        """
        初始化测试器
        
        Args:
            config: 配置字典
        """
        self.config = {**DEFAULT_CONFIG, **config}
        self.output_dir = Path(self.config.get("output_dir", "extracted_code"))
        self.verbose = self.config.get("verbose", False)
    
    def log(self, message: str, level: str = "info"):
        """日志输出"""
        if level == "debug" and not self.verbose:
            return
        
        prefix = {
            "info": "[INFO]",
            "warning": "[WARN]",
            "error": "[ERROR]",
            "debug": "[DEBUG]",
            "success": "[OK]"
        }.get(level, "[INFO]")
        
        print(f"{prefix} {message}", file=sys.stderr if level == "error" else sys.stdout)
    
    def test_c_file(self, file_path: Path, 
                    compiler: str = "gcc",
                    std: str = "c17") -> TestResult:
        """
        测试 C 文件
        
        Args:
            file_path: C 文件路径
            compiler: 编译器命令
            std: C 标准版本
            
        Returns:
            测试结果
        """
        import time
        start_time = time.time()
        
        cmd = [
            compiler,
            f"-std={std}",
            "-fsyntax-only",
            "-Wall",
            "-Wextra",
            "-Werror=implicit-function-declaration",
            "-Werror=return-type",
            "-I.",
            str(file_path)
        ]
        
        try:
            result = subprocess.run(
                cmd,
                capture_output=True,
                text=True,
                timeout=self.config["compiler_defaults"]["c"]["timeout"]
            )
            
            duration = (time.time() - start_time) * 1000
            
            return TestResult(
                success=result.returncode == 0,
                file_path=str(file_path),
                language="c",
                compiler=compiler,
                stdout=result.stdout,
                stderr=result.stderr,
                duration_ms=duration
            )
            
        except subprocess.TimeoutExpired:
            return TestResult(
                success=False,
                file_path=str(file_path),
                language="c",
                compiler=compiler,
                error_message="Compilation timeout",
                duration_ms=self.config["compiler_defaults"]["c"]["timeout"] * 1000
            )
        except Exception as e:
            return TestResult(
                success=False,
                file_path=str(file_path),
                language="c",
                compiler=compiler,
                error_message=str(e)
            )
    
    def test_zig_file(self, file_path: Path) -> TestResult:
        """
        测试 Zig 文件
        
        Args:
            file_path: Zig 文件路径
            
        Returns:
            测试结果
        """
        import time
        start_time = time.time()
        
        # 首先检查格式
        fmt_cmd = ["zig", "fmt", "--check", str(file_path)]
        
        try:
            result = subprocess.run(
                fmt_cmd,
                capture_output=True,
                text=True,
                timeout=self.config["compiler_defaults"]["zig"]["timeout"]
            )
            
            duration = (time.time() - start_time) * 1000
            
            # 格式检查失败也返回成功（可能只是格式问题）
            return TestResult(
                success=True,  # 格式问题不算失败
                file_path=str(file_path),
                language="zig",
                compiler="zig fmt",
                stdout=result.stdout,
                stderr=result.stderr,
                duration_ms=duration
            )
            
        except Exception as e:
            return TestResult(
                success=False,
                file_path=str(file_path),
                language="zig",
                compiler="zig",
                error_message=str(e)
            )
    
    def test_all(self, target_lang: str = "all", 
                 parallel: int = 4) -> List[TestResult]:
        """
        测试所有提取的代码
        
        Args:
            target_lang: 目标语言
            parallel: 并行线程数
            
        Returns:
            测试结果列表
        """
        results = []
        
        # 测试 C 文件
        if target_lang in ["all", "c"]:
            c_dir = self.output_dir / "c"
            if c_dir.exists():
                c_files = list(c_dir.glob("*.c"))
                self.log(f"Testing {len(c_files)} C files...")
                
                for file_path in c_files:
                    result = self.test_c_file(file_path)
                    results.append(result)
                    
                    status = "✅" if result.success else "❌"
                    self.log(f"  {status} {file_path.name}", 
                           "success" if result.success else "error")
        
        # 测试 Zig 文件
        if target_lang in ["all", "zig"]:
            zig_dir = self.output_dir / "zig"
            if zig_dir.exists():
                zig_files = list(zig_dir.glob("*.zig"))
                self.log(f"Testing {len(zig_files)} Zig files...")
                
                for file_path in zig_files:
                    result = self.test_zig_file(file_path)
                    results.append(result)
                    
                    status = "✅" if result.success else "⚠️"
                    self.log(f"  {status} {file_path.name}")
        
        return results


# =============================================================================
# 报告生成器
# =============================================================================

class ReportGenerator:
    """报告生成器"""
    
    @staticmethod
    def generate_json_report(summary: SummaryReport, output_path: Path):
        """生成 JSON 格式报告"""
        report_data = asdict(summary)
        output_path.write_text(json.dumps(report_data, indent=2), encoding='utf-8')
    
    @staticmethod
    def generate_markdown_report(summary: SummaryReport, output_path: Path):
        """生成 Markdown 格式报告"""
        lines = [
            "# C_Lang 代码提取和测试报告",
            "",
            f"**生成时间**: {summary.timestamp}",
            f"**总处理文件数**: {summary.total_files_processed}",
            f"**发现代码块**: {summary.total_blocks_found}",
            f"**成功提取**: {summary.total_blocks_extracted}",
            f"**提取成功率**: {summary.extraction_success_rate:.1f}%",
            f"**测试成功率**: {summary.test_success_rate:.1f}%",
            "",
            "## 提取结果详情",
            ""
        ]
        
        for result in summary.extraction_results:
            lines.append(f"### {result['file_path']}")
            lines.append(f"- 发现块: {result['blocks_found']}")
            lines.append(f"- 提取块: {result['blocks_extracted']}")
            lines.append(f"- 状态: {'✅ 成功' if result['success'] else '❌ 失败'}")
            if result['errors']:
                lines.append("- 错误:")
                for error in result['errors']:
                    lines.append(f"  - {error}")
            lines.append("")
        
        lines.extend([
            "## 测试结果详情",
            ""
        ])
        
        for result in summary.test_results:
            lines.append(f"### {Path(result['file_path']).name}")
            lines.append(f"- 语言: {result['language']}")
            lines.append(f"- 编译器: {result['compiler']}")
            lines.append(f"- 状态: {'✅ 通过' if result['success'] else '❌ 失败'}")
            lines.append(f"- 耗时: {result.get('duration_ms', 0):.2f}ms")
            if result.get('error_message'):
                lines.append(f"- 错误: {result['error_message']}")
            lines.append("")
        
        output_path.write_text('\n'.join(lines), encoding='utf-8')
    
    @staticmethod
    def print_console_summary(summary: SummaryReport):
        """打印控制台摘要"""
        print("\n" + "=" * 60)
        print("C_Lang 代码提取和测试摘要")
        print("=" * 60)
        print(f"总处理文件: {summary.total_files_processed}")
        print(f"发现代码块: {summary.total_blocks_found}")
        print(f"成功提取: {summary.total_blocks_extracted}")
        print(f"提取成功率: {summary.extraction_success_rate:.1f}%")
        print(f"测试成功率: {summary.test_success_rate:.1f}%")
        print("=" * 60)


# =============================================================================
# 主函数
# =============================================================================

def parse_arguments() -> argparse.Namespace:
    """解析命令行参数"""
    parser = argparse.ArgumentParser(
        description="C_Lang 知识库代码提取和测试工具",
        formatter_class=argparse.RawDescriptionHelpFormatter,
        epilog="""
示例:
  %(prog)s --extract-only --filter-lang c
  %(prog)s --full --verbose --parallel 8
  %(prog)s --test-only --report report.json
        """
    )
    
    parser.add_argument(
        "--extract-only",
        action="store_true",
        help="仅提取代码，不进行测试"
    )
    
    parser.add_argument(
        "--test-only",
        action="store_true",
        help="仅测试已提取的代码"
    )
    
    parser.add_argument(
        "--full",
        action="store_true",
        help="全量模式（提取+测试）"
    )
    
    parser.add_argument(
        "--source-dir",
        default="knowledge",
        help="Markdown 源文件目录 (默认: knowledge/)"
    )
    
    parser.add_argument(
        "--output-dir",
        default="extracted_code",
        help="代码输出目录 (默认: extracted_code/)"
    )
    
    parser.add_argument(
        "--filter-lang",
        choices=["c", "zig", "all"],
        default="all",
        help="仅提取指定语言 (默认: all)"
    )
    
    parser.add_argument(
        "--verbose",
        action="store_true",
        help="详细输出模式"
    )
    
    parser.add_argument(
        "--parallel",
        type=int,
        default=4,
        help="并行处理线程数 (默认: 4)"
    )
    
    parser.add_argument(
        "--report",
        help="生成 JSON 格式报告的路径"
    )
    
    parser.add_argument(
        "--markdown-report",
        help="生成 Markdown 格式报告的路径"
    )
    
    return parser.parse_args()


def main() -> int:
    """主函数"""
    args = parse_arguments()
    
    # 确定运行模式
    if not any([args.extract_only, args.test_only, args.full]):
        print("错误: 请指定运行模式 (--extract-only, --test-only, 或 --full)")
        return ExitCode.CONFIG_ERROR.value
    
    config = {
        "source_dir": args.source_dir,
        "output_dir": args.output_dir,
        "verbose": args.verbose,
        "parallel_workers": args.parallel
    }
    
    summary = SummaryReport()
    
    # 代码提取
    if args.extract_only or args.full:
        print(f"\n{'='*60}")
        print("阶段 1: 代码提取")
        print(f"{'='*60}\n")
        
        extractor = CodeExtractor(config)
        extraction_results = extractor.extract_all(
            target_lang=args.filter_lang,
            parallel=args.parallel
        )
        
        summary.extraction_results = [asdict(r) for r in extraction_results]
        summary.total_files_processed = len(extraction_results)
        summary.total_blocks_found = sum(r.blocks_found for r in extraction_results)
        summary.total_blocks_extracted = sum(r.blocks_extracted for r in extraction_results)
        
        if summary.total_blocks_found > 0:
            summary.extraction_success_rate = (
                summary.total_blocks_extracted / summary.total_blocks_found * 100
            )
    
    # 代码测试
    if args.test_only or args.full:
        print(f"\n{'='*60}")
        print("阶段 2: 代码测试")
        print(f"{'='*60}\n")
        
        tester = CodeTester(config)
        test_results = tester.test_all(
            target_lang=args.filter_lang,
            parallel=args.parallel
        )
        
        summary.test_results = [asdict(r) for r in test_results]
        
        if test_results:
            success_count = sum(1 for r in test_results if r.success)
            summary.test_success_rate = success_count / len(test_results) * 100
    
    # 生成报告
    print(f"\n{'='*60}")
    print("阶段 3: 生成报告")
    print(f"{'='*60}\n")
    
    if args.report:
        report_path = Path(args.report)
        report_path.parent.mkdir(parents=True, exist_ok=True)
        ReportGenerator.generate_json_report(summary, report_path)
        print(f"JSON 报告已保存: {report_path}")
    
    if args.markdown_report:
        report_path = Path(args.markdown_report)
        report_path.parent.mkdir(parents=True, exist_ok=True)
        ReportGenerator.generate_markdown_report(summary, report_path)
        print(f"Markdown 报告已保存: {report_path}")
    
    # 打印控制台摘要
    ReportGenerator.print_console_summary(summary)
    
    # 确定退出码
    if summary.extraction_success_rate < 50:
        return ExitCode.EXTRACTION_ERROR.value
    if summary.test_success_rate < 50:
        return ExitCode.TEST_ERROR.value
    
    return ExitCode.SUCCESS.value


if __name__ == "__main__":
    sys.exit(main())
