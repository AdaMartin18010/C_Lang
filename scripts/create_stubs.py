import os
import re

link_re = re.compile(r'\[([^\]]+)\]\(([^)\s"]+)(?:\s+"[^"]*")?\)')

math_patterns = [
    re.compile(r'^[\s*]*$'),
    re.compile(r'^[.]{3,}$'),
    re.compile(r'^[A-Z]$'),
    re.compile(r'^[a-z]+$'),
    re.compile(r'^[0-9]+$'),
    re.compile(r'^[a-zA-Z0-9_]+$'),
    re.compile(r'^[a-zA-Z0-9_]+\s*[:=]'),
    re.compile(r'^[Δλ∀∃∧∨¬→↦⊢⊨∈∉∪∩⊆⊇⊂⊃∅∞]$'),
    re.compile(r'^[\w\s]+→[\w\s]+$'),
    re.compile(r'^\w+\s*\(.*\)$'),
    re.compile(r'^R\['),
    re.compile(r'^\$\{'),
    re.compile(r'^\{\{'),
    re.compile(r'^[a-zA-Z0-9_]+[;+\-*/<>!&|]+[a-zA-Z0-9_]+$'),
    re.compile(r'^[a-zA-Z0-9_]+[;+\-*/<>!&|]+\s+[a-zA-Z0-9_]+$'),
    re.compile(r'^[Δλ∀∃∧∨¬→↦⊢⊨∈∉∪∩⊆⊇⊂⊃∅∞].*'),
    re.compile(r'^M[₀₁₂₃₄₅₆₇₈₉].*'),
    re.compile(r'^λ.*'),
    re.compile(r'^[a-zA-Z0-9_]+\s*\(.*\)$'),
    re.compile(r'^[\"\'].*'),
    re.compile(r'^WP\['),
    re.compile(r"^'[^']+'$"),
    re.compile(r'^if\s+'),
    re.compile(r'^[a-zA-Z_][a-zA-Z0-9_]*->[a-zA-Z_]'),
    re.compile(r'^\w+\.\w+'),
    re.compile(r'^\.{3}[\w.]+'),
    re.compile(r'^\w+\s+then\s+\w+\s+else\s+\w+'),
]

def get_title_from_path(path):
    """Generate a title from the file path."""
    basename = os.path.basename(path)
    name = os.path.splitext(basename)[0]
    # Replace underscores and hyphens with spaces
    title = name.replace('_', ' ').replace('-', ' ')
    # Remove common prefixes
    for prefix in ['01_', '02_', '03_', '04_', '05_', '06_', '07_', '08_', '09_', '10_', '11_', '12_', '13_', '14_', '15_', '16_', '17_', '18_', '19_', '20_', '21_', '22_', '23_']:
        if title.startswith(prefix):
            title = title[3:]
            break
    # Capitalize words
    title = ' '.join(word.capitalize() for word in title.split())
    return title

def create_stub(target, sources):
    """Create a stub file for the missing target."""
    if target.endswith('.html'):
        return  # Skip HTML files
    
    # Ensure parent directory exists
    parent = os.path.dirname(target)
    if parent and not os.path.exists(parent):
        os.makedirs(parent, exist_ok=True)
    
    # For directories (without extension), create README.md
    if not os.path.splitext(target)[1]:
        target = os.path.join(target, 'README.md')
        if not os.path.exists(parent):
            os.makedirs(parent, exist_ok=True)
    
    if os.path.exists(target):
        return
    
    title = get_title_from_path(target)
    
    # Determine stub type
    if '10_Lean4_Compilation' in target:
        content = f"""# {title}

> **层级定位**: 02 Formal Semantics and Physics / 10 Lean4 Compilation
> **状态**: 📝 待补充

## 概述

本文档为 Lean4 编译项目治理/运营文件，目前为占位符。

## 待补充内容

- [ ] 详细内容

---

*最后更新: 2026-05-10*
"""
    elif '18_Robotics_Physical_Control' in target:
        content = f"""# {title}

> **层级定位**: 18 Robotics Physical Control
> **状态**: 📝 待补充

## 概述

本文档为机器人与物理控制模块的子文档，目前为占位符。

## 待补充内容

- [ ] 详细内容

---

*最后更新: 2026-05-10*
"""
    else:
        content = f"""# {title}

> **状态**: 📝 待补充 (Stub)

本文档目前为占位符，内容待后续补充。

## 待补充内容

- [ ] 详细技术内容

---

*最后更新: 2026-05-10*
"""
    
    with open(target, 'w', encoding='utf-8') as f:
        f.write(content)
    
    print(f'Created stub: {target}')

# Collect broken targets
target_sources = {}
for root, dirs, files in os.walk('knowledge'):
    for name in files:
        if name.endswith('.md'):
            path = os.path.join(root, name)
            with open(path, 'r', encoding='utf-8') as f:
                content = f.read()
            for m in link_re.finditer(content):
                text = m.group(1)
                link = m.group(2)
                if link.startswith('http://') or link.startswith('https://') or link.startswith('mailto:') or link.startswith('#'):
                    continue
                if '{{' in text or '{{' in link or '${' in text or '${}' in link:
                    continue
                if text in ('text', 'url', 'alt', 'path', '{{资源名称}}', '{{URL}}'):
                    continue
                if any(p.match(text.strip()) for p in math_patterns):
                    continue
                if link.startswith('/'):
                    target = os.path.normpath(os.path.join('knowledge', link.lstrip('/')))
                else:
                    target = os.path.normpath(os.path.join(root, link))
                target = target.split('#')[0]
                if target.endswith(('/', '\\')):
                    target_readme = target + 'README.md'
                    if os.path.exists(target_readme):
                        continue
                    target = target + 'README.md'
                if not os.path.exists(target):
                    target_sources.setdefault(target, []).append(path)

# Create stubs for all broken targets
created = 0
skipped = 0
for target, sources in target_sources.items():
    if target.endswith('.html'):
        skipped += 1
        continue
    try:
        create_stub(target, sources)
        created += 1
    except Exception as e:
        print(f'Error creating {target}: {e}')
        skipped += 1

print(f'\nCreated {created} stubs, skipped {skipped}')
