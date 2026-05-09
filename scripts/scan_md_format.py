#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""Scan Markdown files for common format issues."""

import os
import re
from pathlib import Path

def scan_file(filepath):
    issues = []
    try:
        with open(filepath, 'r', encoding='utf-8', errors='ignore') as f:
            content = f.read()
            lines = content.splitlines()
    except Exception as e:
        return [f"read_error: {e}"]

    # Multiple H1
    h1_count = len(re.findall(r'^# [^#]', content, re.MULTILINE))
    if h1_count > 1:
        issues.append(f"multi_h1({h1_count})")

    # Unclosed code fences
    fence_count = sum(1 for line in lines if line.startswith('```'))
    if fence_count % 2 != 0:
        issues.append(f"unclosed_fence({fence_count})")

    # Empty links
    empty_links = len(re.findall(r'\[[^\]]*\]\(\s*\)', content))
    if empty_links > 0:
        issues.append(f"empty_link({empty_links})")

    # Broken local links (simple check)
    link_pattern = re.compile(r'\[([^\]]*)\]\(([^)]+)\)')
    broken = 0
    for m in link_pattern.finditer(content):
        url = m.group(2)
        if url.startswith(('http://', 'https://', 'mailto:')):
            continue
        if url.startswith('#'):
            continue
        target = Path(filepath).parent / url
        if not target.exists():
            broken += 1
    if broken > 0:
        issues.append(f"broken_link({broken})")

    return issues

def main():
    root = Path('e:/_src/C_Lang/knowledge')
    results = []
    for md_file in list(root.rglob('*.md'))[:300]:
        rel = md_file.relative_to(Path('e:/_src/C_Lang'))
        issues = scan_file(md_file)
        if issues:
            results.append((str(rel), '; '.join(issues)))

    results.sort(key=lambda x: x[1], reverse=True)
    for rel, iss in results[:50]:
        print(f"{rel} :: {iss}")

if __name__ == '__main__':
    main()
