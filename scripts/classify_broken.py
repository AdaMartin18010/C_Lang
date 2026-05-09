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
                    target_sources.setdefault(target, []).append((path, link, text))

# Categorize
lean4 = {}
robotics = {}
path_errors = {}
real_missing = {}

for target, sources in target_sources.items():
    if '10_Lean4_Compilation' in target:
        lean4[target] = sources
    elif '18_Robotics_Physical_Control' in target:
        robotics[target] = sources
    elif 'knowledge\\01_System_Programming' in target or 'knowledge\\01_Core_Knowledge_System' in target and '03_System_Technology_Domains' in target:
        path_errors[target] = sources
    else:
        real_missing[target] = sources

print(f'=== Lean4 governance docs: {len(lean4)} ===')
for t in sorted(lean4):
    print(f'  {t}')
print()

print(f'=== Robotics subdocs: {len(robotics)} ===')
for t in sorted(robotics)[:10]:
    print(f'  {t}')
if len(robotics) > 10:
    print(f'  ... and {len(robotics)-10} more')
print()

print(f'=== Path errors: {len(path_errors)} ===')
for t, srcs in sorted(path_errors.items())[:20]:
    print(f'  {t}')
    for s, l, txt in srcs[:2]:
        print(f'    from: {s} link={l}')
print()

print(f'=== Real missing files: {len(real_missing)} ===')
for t in sorted(real_missing)[:30]:
    print(f'  {t}')
if len(real_missing) > 30:
    print(f'  ... and {len(real_missing)-30} more')
