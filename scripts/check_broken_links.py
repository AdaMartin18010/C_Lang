import os
import re

# Match markdown links: [text](path) and [text](path "title")
link_re = re.compile(r'\[([^\]]+)\]\(([^)\s"]+)(?:\s+"[^"]*")?\)')

# Patterns that indicate a link is actually a mathematical/formal notation,
# not a real markdown link
math_patterns = [
    re.compile(r'^[\s*]*$'),  # empty or just stars
    re.compile(r'^[.]{3,}$'),  # ellipsis like ...
    re.compile(r'^[A-Z]$'),  # single uppercase letter like S, Q, P
    re.compile(r'^[a-z]+$'),  # single lowercase word like 'skip', 'idx', 'opc'
    re.compile(r'^[0-9]+$'),  # just numbers
    re.compile(r'^[a-zA-Z0-9_]+$'),  # alphanumeric identifiers: S1, Q2, x0
    re.compile(r'^[a-zA-Z0-9_]+\s*[:=]'),  # assignment-like: x := e, y':C
    re.compile(r'^[Δλ∀∃∧∨¬→↦⊢⊨∈∉∪∩⊆⊇⊂⊃∅∞]$'),  # math symbols
    re.compile(r'^[\w\s]+→[\w\s]+$'),  # arrow notation: C → D
    re.compile(r'^\w+\s*\(.*\)$'),  # function-like: sm->current, data.function
    re.compile(r'^R\['),  # register notation: R[A+1..A+B-1]
    re.compile(r'^\$\{'),  # template variable: ${...}
    re.compile(r'^\{\{'),  # template: {{...}}
    re.compile(r'^[a-zA-Z0-9_]+[;+\-*/<>!&|]+[a-zA-Z0-9_]+$'),  # expressions with operators: S1; S2
    re.compile(r'^[a-zA-Z0-9_]+[;+\-*/<>!&|]+\s+[a-zA-Z0-9_]+$'),  # expressions with space: S1; S2
    re.compile(r'^[Δλ∀∃∧∨¬→↦⊢⊨∈∉∪∩⊆⊇⊂⊃∅∞].*'),  # math symbols at start
    re.compile(r'^M[₀₁₂₃₄₅₆₇₈₉].*'),  # subscripted M: M₁ M₂
    re.compile(r'^λ.*'),  # lambda expressions
    re.compile(r'^[a-zA-Z0-9_]+\s*\(.*\)$'),  # function-like with args
    re.compile(r'^["\'].*'),  # regex patterns starting with quotes
    re.compile(r'^WP\['),  # WP[...] notation
    re.compile(r"^'[^']+'$"),  # quoted strings: 'mov_rax_imm32'
    re.compile(r'^if\s+'),  # conditional: if B then S1 else S2
    re.compile(r'^[a-zA-Z_][a-zA-Z0-9_]*->[a-zA-Z_]'),  # pointer access: sm->current
    re.compile(r'^\w+\.\w+'),  # dotted access: data.function
    re.compile(r'^\.\.\.[\w.]+'),  # spread/ellipsis prefix: ...data.args
    re.compile(r'^\w+\s+then\s+\w+\s+else\s+\w+'),  # if-then-else fragments
]

broken = []
total = 0

for root, dirs, files in os.walk('knowledge'):
    for name in files:
        if name.endswith('.md'):
            path = os.path.join(root, name)
            with open(path, 'r', encoding='utf-8') as f:
                content = f.read()
            
            for match in link_re.finditer(content):
                text = match.group(1)
                link = match.group(2)
                
                # Skip external links and non-markdown internal links
                if link.startswith('http://') or link.startswith('https://') or link.startswith('mailto:') or link.startswith('#'):
                    continue
                # Skip non-markdown file links (html, pdf, images, etc.)
                link_lower = link.split('#')[0].lower()
                if any(link_lower.endswith(ext) for ext in ['.html', '.htm', '.pdf', '.png', '.jpg', '.jpeg', '.gif', '.svg', '.mp4', '.webm']):
                    continue
                
                # Skip anchor-only links
                if link.startswith('#'):
                    continue
                
                # Skip template placeholders
                if '{{' in text or '{{' in link or '${' in text or '${' in link:
                    continue
                if text in ('text', 'url', 'alt', 'path', '{{资源名称}}', '{{URL}}'):
                    continue
                
                # Skip mathematical/formal notation links
                is_math = False
                for pattern in math_patterns:
                    if pattern.match(text.strip()):
                        is_math = True
                        break
                if is_math:
                    continue
                
                total += 1
                
                # Remove anchor
                link_no_anchor = link.split('#')[0]
                
                # Check if it's a directory link (trailing / before normpath strips it)
                is_dir_link = link_no_anchor.endswith('/')
                
                # Resolve relative path
                if link.startswith('/'):
                    target = os.path.normpath(os.path.join('knowledge', link_no_anchor.lstrip('/')))
                else:
                    target = os.path.normpath(os.path.join(root, link_no_anchor))
                
                # For directory links, check if README.md exists
                if is_dir_link:
                    target_readme = os.path.join(target, 'README.md')
                    if os.path.exists(target_readme):
                        continue
                
                if not os.path.exists(target):
                    broken.append((path, link, text))

print(f'Total internal links checked: {total}')
print(f'Broken links: {len(broken)}')

# Group by file
by_file = {}
for path, link, text in broken:
    by_file.setdefault(path, []).append((link, text))

for path, links in sorted(by_file.items()):
    print(f'\n{path}:')
    for link, text in links[:5]:
        print(f'  [{text}]({link})')
    if len(links) > 5:
        print(f'  ... and {len(links)-5} more')
