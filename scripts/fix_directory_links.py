import os, re

link_re = re.compile(r'\[([^\]]+)\]\(([^)\s"]+)(?:\s+"[^"]*")?\)')

files_modified = 0
fixes = 0

for root, dirs, files in os.walk('knowledge'):
    for name in files:
        if not name.endswith('.md'):
            continue
        path = os.path.join(root, name)
        with open(path, 'r', encoding='utf-8') as f:
            content = f.read()
        
        original = content
        
        for match in link_re.finditer(content):
            text = match.group(1)
            link = match.group(2)
            
            if link.startswith('http') or link.startswith('mailto:') or link.startswith('#'):
                continue
            
            if not link.endswith('/'):
                continue
            
            old_link = link
            new_link = link
            
            # Try adding README.md
            test_link = new_link + 'README.md'
            if not link.startswith('/'):
                test_target = os.path.normpath(os.path.join(root, test_link))
                if os.path.exists(test_target):
                    new_link = test_link
            
            if new_link == old_link:
                # Try readme.md
                test_link = new_link + 'readme.md'
                if not link.startswith('/'):
                    test_target = os.path.normpath(os.path.join(root, test_link))
                    if os.path.exists(test_target):
                        new_link = test_link
            
            if new_link != old_link:
                old_pattern = f'[{text}]({old_link})'
                new_pattern = f'[{text}]({new_link})'
                content = content.replace(old_pattern, new_pattern)
                fixes += 1
        
        if content != original:
            with open(path, 'w', encoding='utf-8') as f:
                f.write(content)
            files_modified += 1

print(f'Files modified: {files_modified}')
print(f'Directory links fixed: {fixes}')
