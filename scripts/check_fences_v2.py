import os
import re

def check_file(filepath):
    with open(filepath, 'r', encoding='utf-8') as f:
        content = f.read()
    lines = content.split('\n')
    in_fence = False
    fence_start = 0
    for i, line in enumerate(lines):
        if re.match(r'^' + '`' * 3, line):
            if not in_fence:
                in_fence = True
                fence_start = i + 1
            elif line.strip() == '`' * 3:
                in_fence = False
    return in_fence, fence_start

issues = []
for root, dirs, files in os.walk('knowledge'):
    for name in files:
        if name.endswith('.md'):
            path = os.path.join(root, name)
            unclosed, line = check_file(path)
            if unclosed:
                issues.append((path, line))

print(f'Total files with unclosed fence: {len(issues)}')
for path, line in issues:
    print(f'{path}:{line}')
