#!/usr/bin/env python3
"""
C_Lang 全面链接检查脚本
检查所有 Markdown 文件中的内部链接有效性

Usage:
    python scripts/comprehensive_link_check.py
    python scripts/comprehensive_link_check.py --fix
    python scripts/comprehensive_link_check.py --report link_report.json
"""

import os
import re
import sys
import json
import argparse
from pathlib import Path
from datetime import datetime
from dataclasses import dataclass, field, asdict
from typing import List, Set, Tuple
from urllib.parse import urlparse

PROJECT_ROOT = Path(__file__).parent.parent.resolve()
KNOWLEDGE_DIR = PROJECT_ROOT / "knowledge"
EXAMPLES_DIR = PROJECT_ROOT / "examples"
README_PATH = PROJECT_ROOT / "README.md"

# Patterns for markdown links
MD_LINK_PATTERN = re.compile(r'\[([^\]]*)\]\(([^)]+)\)')
MD_REF_PATTERN = re.compile(r'\[([^\]]+)\]:\s*(\S+)')


@dataclass
class LinkIssue:
    file: str
    line: int
    link_text: str
    link_target: str
    issue_type: str  # 'broken', 'external', 'empty', 'anchor_missing'
    suggestion: str = ""


def resolve_link(source_file: Path, link_target: str) -> Tuple[bool, Path]:
    """Resolve a relative link target against the source file's directory."""
    if link_target.startswith("/"):
        target = PROJECT_ROOT / link_target.lstrip("/")
    elif link_target.startswith("http://") or link_target.startswith("https://"):
        return True, Path(link_target)  # External link
    else:
        # Relative path
        target = source_file.parent / link_target

    # Remove anchor
    target_str = str(target)
    if "#" in target_str:
        target_str = target_str.split("#")[0]
        target = Path(target_str)

    return target.exists(), target


def check_anchor(file_path: Path, anchor: str) -> bool:
    """Check if an anchor exists in a markdown file."""
    if not file_path.exists():
        return False
    try:
        content = file_path.read_text(encoding="utf-8", errors="ignore")
        anchor = anchor.lower().replace(" ", "-")
        # Check markdown headers
        header_pattern = re.compile(r'^#{1,6}\s+(.+)$', re.MULTILINE)
        for match in header_pattern.finditer(content):
            header_text = match.group(1).strip().lower()
            header_anchor = re.sub(r'[^\w\s-]', '', header_text).replace(" ", "-")
            if header_anchor == anchor:
                return True
        # Check HTML anchors
        anchor_pattern = re.compile(r'<a[^>]+name=["\']([^"\']+)["\']')
        for match in anchor_pattern.finditer(content):
            if match.group(1).lower() == anchor:
                return True
        return False
    except Exception:
        return False


def scan_file(file_path: Path) -> List[LinkIssue]:
    """Scan a single markdown file for link issues."""
    issues = []
    try:
        content = file_path.read_text(encoding="utf-8", errors="ignore")
    except Exception as e:
        return [LinkIssue(
            file=str(file_path.relative_to(PROJECT_ROOT)),
            line=0,
            link_text="",
            link_target="",
            issue_type="read_error",
            suggestion=str(e)
        )]

    lines = content.splitlines()
    rel_path = str(file_path.relative_to(PROJECT_ROOT))

    # Find all markdown links
    for line_num, line in enumerate(lines, 1):
        for match in MD_LINK_PATTERN.finditer(line):
            link_text = match.group(1)
            link_target = match.group(2)

            # Skip external links (optional: could check HTTP status)
            if link_target.startswith(("http://", "https://", "mailto:", "tel:")):
                continue

            # Skip anchor-only links
            if link_target.startswith("#"):
                anchor = link_target[1:]
                if not check_anchor(file_path, anchor):
                    issues.append(LinkIssue(
                        file=rel_path,
                        line=line_num,
                        link_text=link_text,
                        link_target=link_target,
                        issue_type="anchor_missing",
                        suggestion=f"Anchor '#{anchor}' not found in {rel_path}"
                    ))
                continue

            # Empty link
            if not link_target.strip():
                issues.append(LinkIssue(
                    file=rel_path,
                    line=line_num,
                    link_text=link_text,
                    link_target="",
                    issue_type="empty",
                    suggestion="Empty link target"
                ))
                continue

            # Split target and anchor
            target_path = link_target
            anchor = ""
            if "#" in link_target:
                target_path, anchor = link_target.split("#", 1)

            exists, resolved = resolve_link(file_path, target_path)

            if not exists:
                # Try with .md extension
                if not target_path.endswith(".md"):
                    exists_md, resolved_md = resolve_link(file_path, target_path + ".md")
                    if exists_md:
                        exists = True
                        resolved = resolved_md
                        issues.append(LinkIssue(
                            file=rel_path,
                            line=line_num,
                            link_text=link_text,
                            link_target=link_target,
                            issue_type="missing_extension",
                            suggestion=f"Target exists but missing .md extension: {target_path}.md"
                        ))
                        continue

                issues.append(LinkIssue(
                    file=rel_path,
                    line=line_num,
                    link_text=link_text,
                    link_target=link_target,
                    issue_type="broken",
                    suggestion=f"Target not found: {resolved}"
                ))
            else:
                # Check anchor if present
                if anchor and str(resolved).endswith(".md"):
                    if not check_anchor(resolved, anchor):
                        issues.append(LinkIssue(
                            file=rel_path,
                            line=line_num,
                            link_text=link_text,
                            link_target=link_target,
                            issue_type="anchor_missing",
                            suggestion=f"Anchor '#{anchor}' not found in {resolved}"
                        ))

    return issues


def main():
    parser = argparse.ArgumentParser(description="Comprehensive link checker for C_Lang")
    parser.add_argument("--report", type=str, help="Write JSON report to file")
    parser.add_argument("--fix", action="store_true", help="Attempt to fix issues (missing .md extensions)")
    parser.add_argument("--verbose", "-v", action="store_true", help="Verbose output")
    args = parser.parse_args()

    print("=" * 60)
    print("C_Lang 全面链接检查")
    print(f"时间: {datetime.now().isoformat()}")
    print("=" * 60)

    # Collect all markdown files
    md_files = []
    for root_dir in [KNOWLEDGE_DIR, EXAMPLES_DIR, PROJECT_ROOT]:
        if root_dir.exists():
            for md_file in root_dir.rglob("*.md"):
                md_files.append(md_file)

    print(f"\n扫描 {len(md_files)} 个 Markdown 文件...")

    all_issues: List[LinkIssue] = []
    broken_count = 0
    anchor_count = 0
    empty_count = 0
    ext_count = 0

    for i, md_file in enumerate(md_files, 1):
        if args.verbose:
            print(f"  [{i}/{len(md_files)}] {md_file.relative_to(PROJECT_ROOT)}")
        issues = scan_file(md_file)
        all_issues.extend(issues)
        for issue in issues:
            if issue.issue_type == "broken":
                broken_count += 1
            elif issue.issue_type == "anchor_missing":
                anchor_count += 1
            elif issue.issue_type == "empty":
                empty_count += 1
            elif issue.issue_type == "missing_extension":
                ext_count += 1

    # Summary
    print("\n" + "=" * 60)
    print("检查结果汇总")
    print("=" * 60)
    print(f"  扫描文件数:    {len(md_files)}")
    print(f"  问题总数:      {len(all_issues)}")
    print(f"  断链:          {broken_count}")
    print(f"  锚点缺失:      {anchor_count}")
    print(f"  空链接:        {empty_count}")
    print(f"  缺少扩展名:    {ext_count}")

    # Group by file
    if all_issues:
        print("\n  问题详情 (按文件):")
        file_groups = {}
        for issue in all_issues:
            file_groups.setdefault(issue.file, []).append(issue)

        for file_path, issues in sorted(file_groups.items()):
            print(f"\n    📄 {file_path} ({len(issues)} issues)")
            for issue in issues:
                icon = "🔴" if issue.issue_type == "broken" else "🟡"
                print(f"      {icon} L{issue.line}: [{issue.link_text}] -> {issue.link_target}")
                if args.verbose and issue.suggestion:
                    print(f"         💡 {issue.suggestion}")

    # Write report
    report = {
        "generated_at": datetime.now().isoformat(),
        "total_files": len(md_files),
        "total_issues": len(all_issues),
        "breakdown": {
            "broken": broken_count,
            "anchor_missing": anchor_count,
            "empty": empty_count,
            "missing_extension": ext_count,
        },
        "issues": [asdict(i) for i in all_issues],
    }

    if args.report:
        report_path = Path(args.report)
        report_path.write_text(json.dumps(report, indent=2, ensure_ascii=False), encoding="utf-8")
        print(f"\n  报告已保存: {report_path}")

    if broken_count > 0 or empty_count > 0:
        print(f"\n  ⚠️  发现 {broken_count + empty_count} 个严重问题")
        sys.exit(1)
    else:
        print(f"\n  ✅ 无严重链接问题")
        sys.exit(0)


if __name__ == "__main__":
    main()
