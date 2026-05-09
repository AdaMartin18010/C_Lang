import os, re

link_re = re.compile(r'\[([^\]]+)\]\(([^)\s"]+)(?:\s+"[^"]*")?\)')

fixes_applied = 0
files_modified = set()

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
            
            old_link = link
            new_link = link
            
            # Fix 1: case issue (readme.md -> README.md)
            if 'readme.md' in new_link:
                # Check if target with README.md exists
                test_link = new_link.replace('readme.md', 'README.md')
                if link.startswith('/'):
                    test_target = os.path.normpath(os.path.join('knowledge', test_link.lstrip('/')))
                else:
                    test_target = os.path.normpath(os.path.join(root, test_link))
                test_target = test_target.split('#')[0]
                if os.path.exists(test_target):
                    new_link = test_link
            
            # Fix 2: wrong depth for known targets
            # Check if the current link is broken
            if link.startswith('/'):
                target = os.path.normpath(os.path.join('knowledge', link.lstrip('/')))
            else:
                target = os.path.normpath(os.path.join(root, link))
            target = target.split('#')[0]
            
            if os.path.exists(target):
                # Link is fine (or was fixed by case correction)
                continue
            
            # Try to find correct path by stripping leading ../ and matching under knowledge/
            corrected = False
            for known_prefix in ['01_Core_Knowledge_System', '03_System_Technology_Domains', '02_Formal_Semantics_and_Physics', '04_Industrial_Scenarios', '05_Deep_Structure_MetaPhysics', '06_Thinking_Representation', '07_Modern_Toolchain', '08_Zig_C_Connection', '09_Data_Structures_Algorithms']:
                if known_prefix in new_link:
                    parts = new_link.split('/')
                    try:
                        idx = parts.index(known_prefix)
                        suffix = '/'.join(parts[idx:])
                        correct_target = os.path.normpath(os.path.join('knowledge', suffix))
                        if os.path.exists(correct_target):
                            correct_rel = os.path.relpath(correct_target, root).replace('\\', '/')
                            # Also fix case in the correct_rel
                            correct_rel = correct_rel.replace('readme.md', 'README.md')
                            if correct_rel != new_link:
                                new_link = correct_rel
                                corrected = True
                            break
                    except ValueError:
                        pass
            
            # Also check for 00_GLOBAL_INDEX.md and other root-level knowledge files
            if not corrected:
                for root_file in ['00_GLOBAL_INDEX.md', 'README.md']:
                    if root_file in new_link:
                        correct_target = os.path.normpath(os.path.join('knowledge', root_file))
                        if os.path.exists(correct_target):
                            correct_rel = os.path.relpath(correct_target, root).replace('\\', '/')
                            correct_rel = correct_rel.replace('readme.md', 'README.md')
                            if correct_rel != new_link:
                                new_link = correct_rel
                                corrected = True
                            break
            
            if new_link != old_link:
                # Replace in content
                old_pattern = f'[{text}]({old_link})'
                new_pattern = f'[{text}]({new_link})'
                content = content.replace(old_pattern, new_pattern)
        
        if content != original:
            with open(path, 'w', encoding='utf-8') as f:
                f.write(content)
            files_modified.add(path)
            # Count fixes
            fixes_applied += original.count('](') - content.count('](')  # rough count

print(f'Files modified: {len(files_modified)}')
print(f'Links fixed (approx): {fixes_applied}')
