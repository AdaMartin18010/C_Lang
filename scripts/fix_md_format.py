#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""Batch fix common Markdown format issues."""

import re
from pathlib import Path

def remove_trailing_template(content):
    """Remove trailing template filler content."""
    pattern = r'\n## 深入理解\n\n### 核心原理\n\n深入探讨技术原理和实现细节。\n\n### 实践应用\n\n- 应用场景1\n- 应用场景2\n- 应用场景3\n\n### 最佳实践\n\n1\. 理解基础概念\n2\. 掌握核心机制\n3\. 应用到实际项目\n\n---\n\n> \*\*最后更新\*\*: [^<\n]+\n> \*\*维护者\*\*: [^<\n]+'
    match = re.search(pattern, content)
    if match:
        return content[:match.start()]
    return content

def fix_multi_h1(content):
    """Downgrade excess H1 to H2."""
    lines = content.splitlines()
    h1_count = 0
    result = []
    for line in lines:
        if line.startswith('# ') and not line.startswith('## '):
            h1_count += 1
            if h1_count > 1:
                result.append('#' + line)
                continue
        result.append(line)
    return '\n'.join(result)

def process_file(filepath):
    try:
        with open(filepath, 'r', encoding='utf-8', errors='ignore') as f:
            content = f.read()
    except Exception:
        return None, None

    original = content
    changed = []

    # Remove trailing template
    content = remove_trailing_template(content)
    if content != original:
        changed.append("removed_trailing_template")

    # Fix multi H1 for files with >2 H1
    h1_count = len(re.findall(r'^# [^#]', content, re.MULTILINE))
    if h1_count > 2:
        content = fix_multi_h1(content)
        if content != original:
            changed.append("fixed_multi_h1")

    if changed:
        with open(filepath, 'w', encoding='utf-8') as f:
            f.write(content)
        return str(filepath), changed
    return None, None

def main():
    root = Path('e:/_src/C_Lang/knowledge')
    fixed = []
    for md_file in list(root.rglob('*.md'))[:300]:
        path, changes = process_file(md_file)
        if path:
            rel = Path(path).relative_to(Path('e:/_src/C_Lang'))
            fixed.append(f"{rel}: {', '.join(changes)}")

    for line in fixed[:50]:
        print(line)
    print(f"\nTotal fixed: {len(fixed)}")

if __name__ == '__main__':
    main()
