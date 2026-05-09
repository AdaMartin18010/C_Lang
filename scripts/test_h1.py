import re

h1_re = re.compile(r'^# [^#]')

with open('knowledge/README.md', 'r', encoding='utf-8') as f:
    content = f.read()

matches = h1_re.findall(content)
print(f'H1 matches in README.md: {len(matches)}')
for m in matches[:5]:
    print(f'  {repr(m)}')
