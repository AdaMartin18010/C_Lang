#!/usr/bin/env python3
"""
批量修复内部链接脚本
在目录重命名后，扫描所有Markdown文件并更新相对链接

Usage:
    python scripts/fix_internal_links.py --rename old_name new_name
    python scripts/fix_internal_links.py --map rename_map.json
"""

import os
import re
import json
import argparse
from pathlib import Path

PROJECT_ROOT = Path(__file__).parent.parent.resolve()
KNOWLEDGE_DIR = PROJECT_ROOT / "knowledge"


def find_markdown_files() -> list[Path]:
    """Find all markdown files in knowledge directory."""
    return list(KNOWLEDGE_DIR.rglob("*.md"))


def replace_links_in_file(file_path: Path, rename_map: dict[str, str]) -> int:
    """
    Replace old directory names with new ones in markdown links.
    Returns number of replacements made.
    """
    try:
        with open(file_path, "r", encoding="utf-8", errors="ignore") as f:
            content = f.read()
    except Exception as e:
        print(f"Error reading {file_path}: {e}")
        return 0

    original = content
    # Match markdown links: [text](path) or [text](../../old/path/file.md)
    # We need to replace directory name segments in the path portion
    for old_name, new_name in rename_map.items():
        # Pattern: match the old directory name as a path segment
        # This handles cases like ../old_name/, ./old_name/, old_name/
        pattern = rf"(?<=/){re.escape(old_name)}(?=/|\.md)"
        content = re.sub(pattern, new_name, content)
        # Also handle cases where it's at the start of a relative path
        pattern2 = rf"(?<=\()\.?/?{re.escape(old_name)}(?=/|\.md)"
        content = re.sub(pattern2, lambda m: m.group(0).replace(old_name, new_name), content)

    if content != original:
        try:
            with open(file_path, "w", encoding="utf-8") as f:
                f.write(content)
            return 1
        except Exception as e:
            print(f"Error writing {file_path}: {e}")
            return 0
    return 0


def main():
    parser = argparse.ArgumentParser(description="Fix internal markdown links after directory renames")
    parser.add_argument("--map", type=str, help="JSON file with rename mapping {old: new}")
    parser.add_argument("--rename", nargs=2, metavar=("OLD", "NEW"), action="append",
                        help="Single rename pair")
    args = parser.parse_args()

    rename_map = {}
    if args.map:
        with open(args.map, "r", encoding="utf-8") as f:
            rename_map = json.load(f)
    if args.rename:
        for old, new in args.rename:
            rename_map[old] = new

    if not rename_map:
        print("No rename map provided. Use --map or --rename.")
        return

    print(f"Rename map: {rename_map}")
    print("Scanning markdown files...")

    files = find_markdown_files()
    modified = 0
    for fp in files:
        if replace_links_in_file(fp, rename_map):
            modified += 1

    print(f"Modified {modified} files")


if __name__ == "__main__":
    main()
