import os
import re

link_re = re.compile(r'\[([^\]]+)\]\(([^)\s"]+)\)')

target_dirs = set()
for root, dirs, files in os.walk('knowledge'):
    for name in files:
        if name.endswith('.md'):
            path = os.path.join(root, name)
            with open(path, 'r', encoding='utf-8') as f:
                content = f.read()
            for m in link_re.finditer(content):
                link = m.group(2)
                if link.endswith('readme.md'):
                    target_dir = os.path.normpath(os.path.join(root, link))
                    target_dir = os.path.dirname(target_dir)
                    target_dirs.add(target_dir)

has_readme = 0
has_README = 0
has_both = 0
has_neither = 0
readme_only_dirs = []

for d in target_dirs:
    if not os.path.isdir(d):
        has_neither += 1
        continue
    entries = os.listdir(d)
    lower_entries = [e.lower() for e in entries]
    has_r = 'readme.md' in lower_entries
    
    if has_r:
        actual_names = [e for e in entries if e.lower() == 'readme.md']
        if actual_names[0] == 'README.md':
            has_README += 1
        elif actual_names[0] == 'readme.md':
            has_readme += 1
            readme_only_dirs.append(d)
        else:
            has_both += 1
    else:
        has_neither += 1

print(f'Target dirs with readme.md links: {len(target_dirs)}')
print(f'  Has README.md (upper): {has_README}')
print(f'  Has readme.md (lower only): {has_readme}')
print(f'  Has other case: {has_both}')
print(f'  Dir does not exist: {has_neither}')
print()
print('Dirs with only readme.md (lower):')
for d in sorted(readme_only_dirs)[:30]:
    print(' ', d)
if len(readme_only_dirs) > 30:
    print(f'  ... and {len(readme_only_dirs)-30} more')
