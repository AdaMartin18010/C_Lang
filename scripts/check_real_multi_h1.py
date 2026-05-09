import os
import re

h1_re = re.compile(r'^# [^#]', re.MULTILINE)

real_multi_h1 = []
for root, dirs, files in os.walk('knowledge'):
    for name in files:
        if name.endswith('.md'):
            path = os.path.join(root, name)
            with open(path, 'r', encoding='utf-8') as f:
                content = f.read()
            lines = content.split('\n')
            h1_lines = []
            for i, line in enumerate(lines):
                if line.startswith('# ') and not line.startswith('## '):
                    h1_lines.append((i+1, line))
            if len(h1_lines) > 1:
                # Check if most H1s are comment separators (start with # === or # -)
                comment_separators = sum(1 for _, line in h1_lines if line.startswith('# =') or line.startswith('# -') or line.startswith('# *') or line.startswith('# <') or line.startswith('# H'))
                real_h1s = len(h1_lines) - comment_separators
                if real_h1s > 1:
                    real_multi_h1.append((path, len(h1_lines), real_h1s))

print(f'Files with real multi_h1 (>1 non-comment H1): {len(real_multi_h1)}')
for path, total, real in sorted(real_multi_h1, key=lambda x: -x[2])[:20]:
    print(f'  {real}/{total}: {path}')
