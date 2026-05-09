import os
import re
import shutil

link_re = re.compile(r'\[([^\]]+)\]\(([^)\s"]+)\)')

# Step 1: Find all directories that have readme.md (lowercase) as the actual file
target_dirs = {}
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
                    if os.path.isdir(target_dir):
                        entries = os.listdir(target_dir)
                        readme_entries = [e for e in entries if e.lower() == 'readme.md']
                        if readme_entries and readme_entries[0] == 'readme.md':
                            target_dirs[target_dir] = True

print(f'Found {len(target_dirs)} dirs with lowercase readme.md to rename')

# Step 2: Rename readme.md -> README.md in those directories
for d in target_dirs:
    old_path = os.path.join(d, 'readme.md')
    new_path = os.path.join(d, 'README.md')
    # Windows is case-insensitive, so we need a two-step rename
    temp_path = os.path.join(d, '_temp_readme_.md')
    shutil.move(old_path, temp_path)
    shutil.move(temp_path, new_path)
    print(f'Renamed: {old_path} -> {new_path}')

# Step 3: Fix all links pointing to readme.md -> README.md
fixed_count = 0
files_fixed = set()
for root, dirs, files in os.walk('knowledge'):
    for name in files:
        if name.endswith('.md'):
            path = os.path.join(root, name)
            with open(path, 'r', encoding='utf-8') as f:
                content = f.read()
            
            if 'readme.md' not in content:
                continue
            
            changed = False
            new_content = content
            for m in link_re.finditer(content):
                link = m.group(2)
                if link.endswith('readme.md'):
                    target_dir = os.path.normpath(os.path.join(root, link))
                    target_dir = os.path.dirname(target_dir)
                    if target_dir in target_dirs:
                        old_link = link
                        new_link = link[:-9] + 'README.md'
                        # Use replace carefully to avoid partial matches
                        new_content = new_content.replace('](' + old_link + ')', '](' + new_link + ')')
                        changed = True
                        fixed_count += 1
            
            if changed:
                with open(path, 'w', encoding='utf-8') as f:
                    f.write(new_content)
                files_fixed.add(path)

print(f'\nFixed {fixed_count} links in {len(files_fixed)} files')
