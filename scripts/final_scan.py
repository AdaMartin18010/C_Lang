import os
import re

fence_re = re.compile(r'^' + '`' * 3)
h1_re = re.compile(r'^# [^#]')

multi_h1 = 0
unclosed_fence = 0
both = 0
fence_files = []

for root, dirs, files in os.walk('knowledge'):
    for name in files:
        if name.endswith('.md'):
            path = os.path.join(root, name)
            with open(path, 'r', encoding='utf-8') as f:
                content = f.read()
            h1_count = len(h1_re.findall(content))
            lines = content.split('\n')
            in_fence = False
            for line in lines:
                if fence_re.match(line):
                    if not in_fence:
                        in_fence = True
                    elif line.strip() == '`' * 3:
                        in_fence = False
            has_multi_h1 = h1_count > 1
            has_unclosed = in_fence
            if has_multi_h1:
                multi_h1 += 1
            if has_unclosed:
                unclosed_fence += 1
                fence_files.append(path)
            if has_multi_h1 and has_unclosed:
                both += 1

print(f'Files with multi_h1: {multi_h1}')
print(f'Files with unclosed_fence: {unclosed_fence}')
print(f'Files with both: {both}')
if fence_files:
    print('\nUnclosed fence files:')
    for f in fence_files:
        print(f'  {f}')
