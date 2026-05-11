#!/usr/bin/env python3
"""
C_Lang 项目统计脚本
自动生成项目统计数据，用于更新 README.md 和 PROJECT_STATUS.md

Usage:
    python scripts/generate_stats.py
"""

import os
import json
from pathlib import Path
from datetime import datetime

PROJECT_ROOT = Path(__file__).parent.parent.resolve()
KNOWLEDGE_DIR = PROJECT_ROOT / "knowledge"
EXAMPLES_DIR = PROJECT_ROOT / "examples"
EXTRACTED_DIR = PROJECT_ROOT / "extracted_code"


def count_files_and_lines(directory: Path, pattern: str = "*") -> tuple[int, int]:
    """Count files matching pattern and total lines."""
    total_files = 0
    total_lines = 0
    for path in directory.rglob(pattern):
        if path.is_file():
            total_files += 1
            try:
                with open(path, "r", encoding="utf-8", errors="ignore") as f:
                    total_lines += sum(1 for _ in f)
            except Exception:
                pass
    return total_files, total_lines


def count_by_subdirectory(directory: Path, pattern: str = "*.md") -> dict[str, int]:
    """Count files by immediate subdirectory."""
    counts = {}
    for subdir in sorted(directory.iterdir()):
        if subdir.is_dir():
            files = list(subdir.rglob(pattern))
            counts[subdir.name] = len(files)
    return counts


def count_code_examples() -> dict[str, int]:
    """Count extracted code files by language."""
    result = {}
    if EXTRACTED_DIR.exists():
        for lang_dir in EXTRACTED_DIR.iterdir():
            if lang_dir.is_dir():
                result[lang_dir.name] = len(list(lang_dir.rglob("*")))
    return result


def main():
    print("=" * 60)
    print("C_Lang 项目统计报告")
    print(f"生成时间: {datetime.now().isoformat()}")
    print("=" * 60)

    # Markdown files
    md_files, md_lines = count_files_and_lines(KNOWLEDGE_DIR, "*.md")
    print(f"\n📄 Markdown 文档")
    print(f"   文件数: {md_files}")
    print(f"   总行数: {md_lines:,}")

    # C code files
    c_files, c_lines = count_files_and_lines(EXTRACTED_DIR, "*.c")
    print(f"\n💻 C 代码文件")
    print(f"   文件数: {c_files}")
    print(f"   总行数: {c_lines:,}")

    # Zig code files
    zig_files, zig_lines = count_files_and_lines(EXTRACTED_DIR, "*.zig")
    print(f"\n⚡ Zig 代码文件")
    print(f"   文件数: {zig_files}")
    print(f"   总行数: {zig_lines:,}")

    # Examples
    ex_c, _ = count_files_and_lines(EXAMPLES_DIR, "*.c")
    ex_py, _ = count_files_and_lines(EXAMPLES_DIR, "*.py")
    print(f"\n🔧 示例代码")
    print(f"   C 示例: {ex_c}")
    print(f"   Python 示例: {ex_py}")

    # Top-level directory breakdown
    print(f"\n📂 知识目录分布 (Markdown)")
    subdir_counts = count_by_subdirectory(KNOWLEDGE_DIR, "*.md")
    for name, count in sorted(subdir_counts.items(), key=lambda x: -x[1])[:15]:
        print(f"   {name:45s} {count:4d} files")

    # Save JSON report
    report = {
        "generated_at": datetime.now().isoformat(),
        "markdown": {"files": md_files, "lines": md_lines},
        "c_code": {"files": c_files, "lines": c_lines},
        "zig_code": {"files": zig_files, "lines": zig_lines},
        "examples": {"c": ex_c, "python": ex_py},
        "top_directories": subdir_counts,
    }

    report_path = PROJECT_ROOT / "project_stats.json"
    with open(report_path, "w", encoding="utf-8") as f:
        json.dump(report, f, indent=2, ensure_ascii=False)

    print(f"\n✅ 报告已保存: {report_path}")


if __name__ == "__main__":
    main()
