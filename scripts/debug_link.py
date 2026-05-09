import os
import re

link_re = re.compile(r'\[([^\]]+)\]\(([^)\s"]+)(?:\s+"[^"]*")?\)')
path = 'knowledge/20_Industrial_Case_Studies/23_Cache_System.md'
with open(path, 'r', encoding='utf-8') as f:
    content = f.read()
for m in link_re.finditer(content):
    text = m.group(1)
    link = m.group(2)
    if 'In_Memory' in link or '11_In_Memory' in link:
        print(repr(text), '->', repr(link))
        target = os.path.normpath(os.path.join(os.path.dirname(path), link))
        target = target.split('#')[0]
        print('  resolved:', target)
        print('  ends with slash:', target.endswith('/'))
        print('  ends with backslash:', target.endswith('\\'))
        if target.endswith('/') or target.endswith('\\'):
            print('  readme exists:', os.path.exists(target + 'README.md'))
        print('  target exists:', os.path.exists(target))
