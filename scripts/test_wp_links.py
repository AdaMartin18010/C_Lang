import re

link_re = re.compile(r'\[([^\]]+)\]\(([^)\s"]+)(?:\s+"[^"]*")?\)')

math_patterns = [
    re.compile(r'^[\s*]*$'),
    re.compile(r'^[.]{3,}$'),
    re.compile(r'^[A-Z]$'),
    re.compile(r'^[a-z]+$'),
    re.compile(r'^[0-9]+$'),
    re.compile(r'^[a-zA-Z0-9_]+\s*[:=]'),
    re.compile(r'^[Δλ∀∃∧∨¬→↦⊢⊨∈∉∪∩⊆⊇⊂⊃∅∞]$'),
    re.compile(r'^[\w\s]+→[\w\s]+$'),
    re.compile(r'^\w+\s*\(.*\)$'),
    re.compile(r'^R\['),
    re.compile(r'^\$\{'),
    re.compile(r'^\{\{'),
    re.compile(r'^[a-zA-Z0-9_]+[;+\-*/<>!&|]+[a-zA-Z0-9_]+$'),
    re.compile(r"'^[^']+'$"),
    re.compile(r'^if\s+'),
    re.compile(r'^[a-zA-Z_][a-zA-Z0-9_]*->[a-zA-Z_]'),
    re.compile(r'^\w+\.\w+'),
    re.compile(r'^\.{3}[\w.]+'),
    re.compile(r'^\w+\s+then\s+\w+\s+else\s+\w+'),
    re.compile(r'^[a-zA-Z0-9_]+[;+\-*/<>!&|]+\s+[a-zA-Z0-9_]+$'),
    re.compile(r'^[Δλ∀∃∧∨¬→↦⊢⊨∈∉∪∩⊆⊇⊂⊃∅∞].*'),
    re.compile(r'^M[₀₁₂₃₄₅₆₇₈₉].*'),
    re.compile(r'^λ.*'),
    re.compile(r'^[a-zA-Z0-9_]+\s*\(.*\)$'),
    re.compile(r'^["\'].*'),
    re.compile(r'^WP\['),
]

with open('knowledge/05_Deep_Structure_MetaPhysics/04_Formal_Verification_Energy/01_WP_Energy_Landscape.md', 'r', encoding='utf-8') as f:
    content = f.read()

for m in link_re.finditer(content):
    text = m.group(1)
    link = m.group(2)
    if 'S1' in text or 'S2' in text or 'WP' in link:
        is_math = any(p.match(text.strip()) for p in math_patterns)
        print(repr(text), '->', repr(link), 'math=' + str(is_math))
