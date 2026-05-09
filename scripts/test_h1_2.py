import re

h1_re = re.compile(r'^# [^#]')

with open('knowledge/02_Formal_Semantics_and_Physics/10_Lean4_Compilation/LEAN4_ECOSYSTEM.md', 'r', encoding='utf-8') as f:
    content = f.read()

matches = h1_re.findall(content)
print(f'H1 matches: {len(matches)}')
for m in matches[:10]:
    print(f'  {repr(m)}')
