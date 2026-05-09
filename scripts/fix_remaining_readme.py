import os
import re

link_re = re.compile(r'\[([^\]]+)\]\(([^)\s"]+)\)')

def get_actual_case(dirpath, filename):
    """Check if a file with the given name exists in dirpath (case-insensitive match),
    and return the actual case-sensitive filename."""
    if not os.path.isdir(dirpath):
        return None
    try:
        entries = os.listdir(dirpath)
    except OSError:
        return None
    for entry in entries:
        if entry.lower() == filename.lower():
            return entry
    return None

fixed_count = 0
files_fixed = 0

for root, dirs, files in os.walk('knowledge'):
    for name in files:
        if name.endswith('.md'):
            path = os.path.join(root, name)
            with open(path, 'r', encoding='utf-8') as f:
                content = f.read()
            
            # Quick check
            if 'readme.md' not in content:
                continue
            
            changed = False
            new_content = content
            
            for m in link_re.finditer(content):
                link = m.group(2)
                # Only fix lowercase readme.md links
                if not link.endswith('readme.md'):
                    continue
                
                # Resolve the directory of the target
                target_dir = os.path.normpath(os.path.join(root, link))
                target_dir = os.path.dirname(target_dir)
                
                actual = get_actual_case(target_dir, 'README.md')
                if actual and actual != 'readme.md':
                    old_link = link
                    new_link = link[:-9] + actual  # replace readme.md with actual case
                    new_content = new_content.replace('](' + old_link + ')', '](' + new_link + ')')
                    changed = True
                    fixed_count += 1
            
            if changed:
                with open(path, 'w', encoding='utf-8') as f:
                    f.write(new_content)
                files_fixed += 1
                print('Fixed: ' + path)

print(f'\nFixed {fixed_count} links in {files_fixed} files')
