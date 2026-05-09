import os
import re

# For files directly under knowledge/SOME_DIR/, ../../ jumps out of knowledge/
# So ../../03_System_Technology_Domains/ should be ../03_System_Technology_Domains/

link_re = re.compile(r'\[([^\]]+)\]\(([^)\s"]+)\)')

fixed_files = 0
fixed_links = 0

for root, dirs, files in os.walk('knowledge'):
    for name in files:
        if not name.endswith('.md'):
            continue
        path = os.path.join(root, name)
        
        # Check if file is directly under knowledge/SOME_DIR/ (one level deep)
        # Path pattern: knowledge/DIR/file.md
        rel = os.path.relpath(path, 'knowledge')
        parts = rel.split(os.sep)
        if len(parts) != 2:  # Only fix files directly under knowledge/DIR/
            continue
        
        with open(path, 'r', encoding='utf-8') as f:
            content = f.read()
        
        changed = False
        new_content = content
        
        for m in link_re.finditer(content):
            link = m.group(2)
            if '../../03_System_Technology_Domains/' in link:
                new_link = link.replace('../../03_System_Technology_Domains/', '../03_System_Technology_Domains/')
                new_content = new_content.replace('](' + link + ')', '](' + new_link + ')')
                changed = True
                fixed_links += 1
        
        if changed:
            with open(path, 'w', encoding='utf-8') as f:
                f.write(new_content)
            fixed_files += 1
            print('Fixed: ' + path)

print(f'\nFixed {fixed_links} links in {fixed_files} files')
