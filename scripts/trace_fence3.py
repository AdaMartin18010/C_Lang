import re

with open('knowledge/06_Thinking_Representation/09_Bloom_Taxonomy/05_Analysis_Frameworks.md', 'r', encoding='utf-8') as f:
    lines = f.readlines()

fence_re = re.compile(r'^' + '`' * 3)
in_fence = False
for i, line in enumerate(lines):
    if fence_re.match(line):
        old = in_fence
        if not in_fence:
            in_fence = True
        elif line.strip() == '`' * 3:
            in_fence = False
        print(f'{i+1:5d}: {old}->{in_fence}  {repr(line[:15])}')

print(f'Final: {in_fence}')
