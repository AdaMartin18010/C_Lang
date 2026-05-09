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

broken_targets = set()
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
                    broken_targets.add(('DIR', target))
                elif not os.path.exists(target):
                    broken_targets.add(('FILE', target))

print(f'Unique broken targets: {len(broken_targets)}')
dirs = [t for k, t in broken_targets if k == 'DIR']
files = [t for k, t in broken_targets if k == 'FILE']
print(f'Directories without README: {len(dirs)}')
print(f'Missing files: {len(files)}')
print()
print('Directories:')
for d in sorted(dirs):
    print(' ', d)
print()
print('Files (sample):')
for f in sorted(files)[:50]:
    print(' ', f)
if len(files) > 50:
    print(f'  ... and {len(files)-50} more')
