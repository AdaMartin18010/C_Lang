import os, re, json

base = "e:\\_src\\C_Lang"

with open(os.path.join(base, 'md_files_list.txt'), 'r', encoding='utf-8') as f:
    files = [line.strip() for line in f if line.strip()]

results = {
    'still_unclosed': [],
    'still_multi_h1': [],
    'still_broken_links': [],
    'still_template': [],
    'ok': []
}

for fp in files:
    try:
        with open(fp, 'r', encoding='utf-8') as f:
            content = f.read()
    except Exception as e:
        continue
    
    lines = content.split('\n')
    
    # unclosed fence
    in_fence = False
    for line in lines:
        stripped = line.strip()
        if re.match(r'^```\s*$', stripped):
            in_fence = not in_fence
        elif re.match(r'^```\S', stripped):
            in_fence = True
    
    # multi h1
    outside = set()
    in_fence = False
    for i, line in enumerate(lines):
        stripped = line.strip()
        if re.match(r'^```\s*$', stripped):
            in_fence = not in_fence
        elif re.match(r'^```\S', stripped):
            in_fence = True
        if not in_fence and re.match(r'^# ', line):
            outside.add(i)
    
    # broken links
    link_pattern = re.compile(r'\[([^\]]*)\]\(([^)]+)\)')
    broken = []
    for m in link_pattern.finditer(content):
        url = m.group(2)
        if not url.startswith(('http://', 'https://', 'mailto:', '#')):
            target = os.path.normpath(os.path.join(os.path.dirname(fp), url))
            if not os.path.exists(target):
                broken.append(url)
    
    # template
    has_template = False
    if '## 深入理解' in content or '## 深入探讨' in content:
        if re.search(r'核心原理|实践应用|最佳实践|应用场景\d', content):
            has_template = True
    
    rel = os.path.relpath(fp, base)
    issues = []
    if in_fence:
        issues.append('unclosed')
    if len(outside) > 2:
        issues.append('multi_h1(' + str(len(outside)) + ')')
    if broken:
        issues.append('broken_links(' + str(len(broken)) + ')')
    if has_template:
        issues.append('template')
    
    if issues:
        entry = rel + ': ' + ', '.join(issues)
        if in_fence:
            results['still_unclosed'].append(entry)
        if len(outside) > 2:
            results['still_multi_h1'].append(entry)
        if broken:
            results['still_broken_links'].append(entry)
        if has_template:
            results['still_template'].append(entry)
    else:
        results['ok'].append(rel)

print('Files still with issues:')
print('  Unclosed fence:', len(results['still_unclosed']))
print('  Multi H1:', len(results['still_multi_h1']))
print('  Broken links:', len(results['still_broken_links']))
print('  Template:', len(results['still_template']))
print('  OK:', len(results['ok']))

print('\n--- Still unclosed fence (first 20) ---')
for e in results['still_unclosed'][:20]:
    print('  ' + e)
if len(results['still_unclosed']) > 20:
    print('  ... and', len(results['still_unclosed'])-20, 'more')

print('\n--- Still template (first 20) ---')
for e in results['still_template'][:20]:
    print('  ' + e)
if len(results['still_template']) > 20:
    print('  ... and', len(results['still_template'])-20, 'more')

# Save to file
with open(os.path.join(base, 'verify_results.json'), 'w', encoding='utf-8') as f:
    json.dump(results, f, ensure_ascii=False, indent=2)
