import re

with open('knowledge/01_Core_Knowledge_System/07_Modern_C/C2y_If_Declaration_Depth.md', 'r', encoding='utf-8') as f:
    lines = f.readlines()

fence_re = re.compile(r'^' + '`' * 3)
matches = [(i+1, line.strip()) for i, line in enumerate(lines) if fence_re.match(line)]

in_fence = False
for idx, (line_num, line) in enumerate(matches):
    old = in_fence
    in_fence = not in_fence
    if 77 <= idx <= 89:
        print(f'{idx+1:3d}: line {line_num:5d}: {old}->{in_fence}  {line}')

print(f'Final: {in_fence}')
print(f'Total: {len(matches)}')
