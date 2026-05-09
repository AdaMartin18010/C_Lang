import re

with open('knowledge/README.md', 'r', encoding='utf-8') as f:
    lines = f.readlines()

print(f'Original lines: {len(lines)}')

# Apply fix
if not lines[-1].endswith('\n'):
    lines[-1] += '\n'
lines.append('\n```\n')

print(f'After fix: {len(lines)}')

# Verify
fence_re = re.compile(r'^' + '`' * 3)
in_fence = False
for i, line in enumerate(lines):
    if fence_re.match(line):
        old = in_fence
        if not in_fence:
            in_fence = True
        elif line.strip() == '`' * 3:
            in_fence = False
        print(f'{i+1}: {old}->{in_fence}  {repr(line[:10])}')

print(f'Final in_fence: {in_fence}')
