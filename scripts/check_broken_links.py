import os
import re

# Match markdown links: [text](path) and [text](path "title")
link_re = re.compile(r'\[([^\]]+)\]\(([^)\s"]+)(?:\s+"[^"]*")?\)')

broken = []
total = 0

for root, dirs, files in os.walk('knowledge'):
    for name in files:
        if name.endswith('.md'):
            path = os.path.join(root, name)
            with open(path, 'r', encoding='utf-8') as f:
                content = f.read()
            
            for match in link_re.finditer(content):
                text = match.group(1)
                link = match.group(2)
                
                # Skip external links
                if link.startswith('http://') or link.startswith('https://') or link.startswith('mailto:') or link.startswith('#'):
                    continue
                
                # Skip anchor-only links
                if link.startswith('#'):
                    continue
                
                total += 1
                
                # Resolve relative path
                if link.startswith('/'):
                    target = os.path.normpath(os.path.join('knowledge', link.lstrip('/')))
                else:
                    target = os.path.normpath(os.path.join(root, link))
                
                # Remove anchor
                target = target.split('#')[0]
                
                if not os.path.exists(target):
                    broken.append((path, link, text))

print(f'Total internal links checked: {total}')
print(f'Broken links: {len(broken)}')

# Group by file
by_file = {}
for path, link, text in broken:
    by_file.setdefault(path, []).append((link, text))

for path, links in sorted(by_file.items()):
    print(f'\n{path}:')
    for link, text in links[:5]:
        print(f'  [{text}]({link})')
    if len(links) > 5:
        print(f'  ... and {len(links)-5} more')
