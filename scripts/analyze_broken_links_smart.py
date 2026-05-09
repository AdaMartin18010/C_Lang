import os, re
from collections import defaultdict

link_re = re.compile(r'\[([^\]]+)\]\(([^)\s"]+)(?:\s+"[^"]*")?\)')

# Common target patterns we know exist in the repo
known_targets = {
    '01_Core_Knowledge_System/02_Core_Layer/02_Memory_Management.md',
    '01_Core_Knowledge_System/02_Core_Layer/01_Pointer_Depth.md',
    '01_Core_Knowledge_System/01_Basic_Layer/02_Data_Type_System.md',
    '01_Core_Knowledge_System/02_Core_Layer/05_Arrays_Pointers.md',
    '03_System_Technology_Domains/14_Concurrency_Parallelism/readme.md',
    '01_Core_Knowledge_System/readme.md',
    '00_GLOBAL_INDEX.md',
}

broken_by_reason = defaultdict(list)
fixable = []  # (file_path, old_link, new_link, link_text)

for root, dirs, files in os.walk('knowledge'):
    for name in files:
        if not name.endswith('.md'):
            continue
        path = os.path.join(root, name)
        with open(path, 'r', encoding='utf-8') as f:
            content = f.read()
        
        for match in link_re.finditer(content):
            text = match.group(1)
            link = match.group(2)
            
            if link.startswith('http') or link.startswith('mailto:') or link.startswith('#'):
                continue
            
            # Resolve relative path
            if link.startswith('/'):
                target = os.path.normpath(os.path.join('knowledge', link.lstrip('/')))
            else:
                target = os.path.normpath(os.path.join(root, link))
            
            target_no_anchor = target.split('#')[0]
            
            if os.path.exists(target_no_anchor):
                continue
            
            # This link is broken. Analyze why.
            # Check if it's a case issue (readme.md vs README.md)
            if 'readme.md' in link:
                alt = target_no_anchor.replace('readme.md', 'README.md')
                if os.path.exists(alt):
                    broken_by_reason['case: readme.md'].append((path, link, text))
                    new_link = link.replace('readme.md', 'README.md')
                    fixable.append((path, link, new_link, text))
                    continue
            
            # Check if target exists under knowledge/ but path has wrong depth
            if '01_Core_Knowledge_System' in link:
                # Try to find the target under knowledge/
                parts = link.split('/')
                # Find where 01_Core_Knowledge_System starts
                try:
                    idx = parts.index('01_Core_Knowledge_System')
                    suffix = '/'.join(parts[idx:])
                    correct_target = os.path.normpath(os.path.join('knowledge', suffix))
                    if os.path.exists(correct_target):
                        # Target exists. Compute correct relative path from file.
                        correct_rel = os.path.relpath(correct_target, root).replace('\\', '/')
                        if correct_rel != link:
                            broken_by_reason['wrong_depth: 01_Core_Knowledge_System'].append((path, link, text))
                            fixable.append((path, link, correct_rel, text))
                            continue
                except ValueError:
                    pass
            
            # Check if target exists under knowledge/ for other patterns
            if '03_System_Technology_Domains' in link:
                parts = link.split('/')
                try:
                    idx = parts.index('03_System_Technology_Domains')
                    suffix = '/'.join(parts[idx:])
                    correct_target = os.path.normpath(os.path.join('knowledge', suffix))
                    if os.path.exists(correct_target):
                        correct_rel = os.path.relpath(correct_target, root).replace('\\', '/')
                        if correct_rel != link:
                            broken_by_reason['wrong_depth: 03_System_Technology_Domains'].append((path, link, text))
                            fixable.append((path, link, correct_rel, text))
                            continue
                except ValueError:
                    pass
            
            # Check 00_GLOBAL_INDEX.md
            if '00_GLOBAL_INDEX.md' in link:
                correct_target = 'knowledge/00_GLOBAL_INDEX.md'
                if os.path.exists(correct_target):
                    correct_rel = os.path.relpath(correct_target, root).replace('\\', '/')
                    if correct_rel != link:
                        broken_by_reason['wrong_depth: 00_GLOBAL_INDEX.md'].append((path, link, text))
                        fixable.append((path, link, correct_rel, text))
                        continue
            
            # Unknown / target truly missing
            broken_by_reason['target_missing'].append((path, link, text))

print('=== Broken Link Analysis ===')
total = sum(len(v) for v in broken_by_reason.values())
print(f'Total broken links analyzed: {total}')
print(f'Fixable links: {len(fixable)}')
print()

for reason, items in sorted(broken_by_reason.items(), key=lambda x: -len(x[1])):
    print(f'{reason}: {len(items)}')

print()
print('=== Sample fixable links ===')
for path, old, new, text in fixable[:10]:
    rel = os.path.relpath(path, '.')
    print(f'{rel}')
    print(f'  [{text}]({old}) -> [{text}]({new})')
