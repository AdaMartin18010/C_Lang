import re

with open('knowledge/06_Thinking_Representation/09_Bloom_Taxonomy/05_Analysis_Frameworks.md', 'r', encoding='utf-8') as f:
    lines = f.readlines()

fence_re = re.compile(r'^' + '`' * 3)
for i, line in enumerate(lines):
    if fence_re.match(line):
        print(f'{i+1}: {repr(line[:20])}')
