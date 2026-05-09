import os, re
from collections import defaultdict

# Pattern to find links pointing to 01_Core_Knowledge_System with exactly 3 ../
pattern = re.compile(r'\[([^\]]+)\]\(../../../01_Core_Knowledge_System/([^)]+)\)')
files = defaultdict(list)

for root, dirs, filenames in os.walk('knowledge'):
    for fn in filenames:
        if fn.endswith('.md'):
            path = os.path.join(root, fn)
            with open(path, 'r', encoding='utf-8') as f:
                content = f.read()
            for m in pattern.finditer(content):
                files[path].append(m.group(0))

print(f'Files with ../../../01_Core_Knowledge_System/ links: {len(files)}')
for p in sorted(files.keys())[:15]:
    rel = os.path.relpath(p, '.')
    print(f'  {rel} ({len(files[p])} links)')
