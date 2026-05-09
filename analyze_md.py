import os, re, json

base = "e:\\_src\\C_Lang"
with open(os.path.join(base, 'md_files_list.txt'), 'r', encoding='utf-8') as f:
    files = [line.strip() for line in f if line.strip()]

results = []
for fp in files:
    try:
        with open(fp, 'r', encoding='utf-8') as f:
            content = f.read()
    except Exception as e:
        results.append({'file': fp, 'error': str(e)})
        continue
    
    lines = content.split('\n')
    
    # 1. unclosed_fence
    fence_count = sum(1 for line in lines if line.strip().startswith('```'))
    unclosed = fence_count % 2 == 1
    
    # 2. multi_h1
    h1_count = sum(1 for line in lines if re.match(r'^# ', line))
    
    # 3. broken_link (relative only)
    link_pattern = re.compile(r'\[([^\]]*)\]\(([^)]+)\)')
    broken_links = []
    for m in link_pattern.finditer(content):
        url = m.group(2)
        if not url.startswith(('http://', 'https://', 'mailto:', '#')):
            target = os.path.normpath(os.path.join(os.path.dirname(fp), url))
            if not os.path.exists(target):
                broken_links.append({'text': m.group(1), 'url': url, 'target': target})
    
    # 4. trailing template filler
    has_trailing_template = False
    if '## 深入理解' in content or '## 深入探讨' in content:
        if re.search(r'核心原理|实践应用|最佳实践|应用场景\d', content):
            has_trailing_template = True
    
    results.append({
        'file': fp,
        'lines': len(lines),
        'unclosed_fence': unclosed,
        'fence_count': fence_count,
        'h1_count': h1_count,
        'broken_links': broken_links,
        'has_trailing_template': has_trailing_template
    })

with open(os.path.join(base, 'analysis_results.json'), 'w', encoding='utf-8') as f:
    json.dump(results, f, ensure_ascii=False, indent=2)

unclosed_files = [r for r in results if r.get('unclosed_fence')]
multi_h1_files = [r for r in results if r.get('h1_count', 0) > 2]
broken_link_files = [r for r in results if r.get('broken_links')]
template_files = [r for r in results if r.get('has_trailing_template')]

print(f'Total files: {len(results)}')
print(f'Unclosed fence: {len(unclosed_files)}')
print(f'Multi H1 (>2): {len(multi_h1_files)}')
print(f'Broken links: {len(broken_link_files)}')
print(f'Trailing template: {len(template_files)}')
print()

print('--- Unclosed fence files ---')
for r in unclosed_files[:10]:
    rel = os.path.relpath(r['file'], base)
    print(f'  {rel} (fences: {r["fence_count"]})')
if len(unclosed_files) > 10:
    print(f'  ... and {len(unclosed_files)-10} more')

print('--- Multi H1 files ---')
for r in multi_h1_files[:10]:
    rel = os.path.relpath(r['file'], base)
    print(f'  {rel} (H1 count: {r["h1_count"]})')
if len(multi_h1_files) > 10:
    print(f'  ... and {len(multi_h1_files)-10} more')

print('--- Broken link files ---')
for r in broken_link_files[:10]:
    rel = os.path.relpath(r['file'], base)
    print(f'  {rel} ({len(r["broken_links"])} broken)')
if len(broken_link_files) > 10:
    print(f'  ... and {len(broken_link_files)-10} more')

print('--- Trailing template files ---')
for r in template_files[:10]:
    rel = os.path.relpath(r['file'], base)
    print(f'  {rel}')
if len(template_files) > 10:
    print(f'  ... and {len(template_files)-10} more')
