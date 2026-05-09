import os, re, json

base = "e:\\_src\\C_Lang"

with open(os.path.join(base, 'md_files_list.txt'), 'r', encoding='utf-8') as f:
    files = [line.strip() for line in f if line.strip()]

log = []
needs_rewrite = []

# Pre-collect all md files for link checking
all_md_files = set()
for root, dirs, files_walk in os.walk(os.path.join(base, 'knowledge')):
    for fname in files_walk:
        if fname.endswith('.md'):
            all_md_files.add(os.path.normpath(os.path.join(root, fname)))

def count_problems(r):
    cnt = 0
    if r.get('unclosed_fence'):
        cnt += 1
    if r.get('h1_count', 0) > 2:
        cnt += r['h1_count'] - 2
    cnt += len(r.get('broken_links', []))
    if r.get('has_trailing_template'):
        cnt += 1
    return cnt

def fix_file(fp):
    changes = []
    with open(fp, 'r', encoding='utf-8') as f:
        content = f.read()
    
    original = content
    lines = content.split('\n')
    
    # === Detect problems ===
    
    # 1. unclosed fence (correct logic)
    fence_lines = []
    in_fence = False
    for i, line in enumerate(lines):
        stripped = line.strip()
        if re.match(r'^```\s*$', stripped):
            in_fence = not in_fence
            fence_lines.append((i, 'toggle'))
        elif re.match(r'^```\S', stripped):
            in_fence = True
            fence_lines.append((i, 'start'))
    unclosed = in_fence
    
    # 2. multi_h1
    h1_lines = [i for i, line in enumerate(lines) if re.match(r'^# ', line)]
    
    # 3. broken links
    link_pattern = re.compile(r'\[([^\]]*)\]\(([^)]+)\)')
    broken_links = []
    for m in link_pattern.finditer(content):
        url = m.group(2)
        if not url.startswith(('http://', 'https://', 'mailto:', '#')):
            target = os.path.normpath(os.path.join(os.path.dirname(fp), url))
            if not os.path.exists(target):
                broken_links.append({'match': m.group(0), 'text': m.group(1), 'url': url, 'target': target})
    
    # 4. trailing template
    has_template = False
    if '## 深入理解' in content or '## 深入探讨' in content:
        if re.search(r'核心原理|实践应用|最佳实践|应用场景\d', content):
            has_template = True
    
    problem_count = (1 if unclosed else 0) + max(0, len(h1_lines)-2) + len(broken_links) + (1 if has_template else 0)
    
    if problem_count > 20:
        needs_rewrite.append({'file': fp, 'problems': problem_count})
        return None
    
    # === Fix 4: trailing template ===
    if has_template:
        # Try to find and remove trailing template section
        # Match from "## 深入理解" or "## 深入探讨" to end, if it contains template patterns
        template_patterns = [
            r'## 深入理解\s*\n(?:.*?核心原理.*?\n.*?实践应用.*?\n.*?最佳实践.*?\n.*?)+',
            r'## 深入探讨\s*\n(?:.*?核心概念.*?\n.*?实践应用.*?\n.*?学习建议.*?\n.*?)+',
            r'## 深入理解\s*\n.*?核心原理.*?实践应用.*?最佳实践.*?',
            r'## 深入探讨\s*\n.*?核心概念.*?实践应用.*?学习建议.*?',
        ]
        removed = False
        for pat in template_patterns:
            m = re.search(pat, content, re.DOTALL)
            if m:
                # Check if the matched section is mostly template/empty
                section = m.group(0)
                if len(section) < 3000 and ('应用场景1' in section or '应用场景2' in section or '应用场景3' in section or '深入探讨技术原理' in section):
                    content = content[:m.start()] + content[m.end():]
                    # Clean up trailing whitespace/newlines
                    content = re.sub(r'\n{3,}$', '\n\n', content)
                    changes.append('remove_trailing_template')
                    removed = True
                    break
        
        if not removed:
            # Fallback: remove from "## 深入理解" or "## 深入探讨" to end if it's near the end
            for marker in ['## 深入理解', '## 深入探讨']:
                idx = content.rfind(marker)
                if idx != -1:
                    section = content[idx:]
                    if '应用场景1' in section or '应用场景2' in section or '应用场景3' in section:
                        content = content[:idx]
                        content = re.sub(r'\n{3,}$', '\n\n', content)
                        changes.append('remove_trailing_template')
                        removed = True
                        break
    
    # Re-split lines after template removal
    lines = content.split('\n')
    
    # === Fix 1: unclosed fence ===
    if unclosed:
        # Recompute fence state on modified content
        in_fence = False
        for i, line in enumerate(lines):
            stripped = line.strip()
            if re.match(r'^```\s*$', stripped):
                in_fence = not in_fence
            elif re.match(r'^```\S', stripped):
                in_fence = True
        
        if in_fence:
            # Find last fence start (language marker or bare toggle that left us IN)
            last_fence_start = -1
            temp_in = False
            for i, line in enumerate(lines):
                stripped = line.strip()
                if re.match(r'^```\s*$', stripped):
                    temp_in = not temp_in
                    last_fence_start = i if temp_in else last_fence_start
                elif re.match(r'^```\S', stripped):
                    temp_in = True
                    last_fence_start = i
            
            # Add closing fence before trailing metadata or at end
            if last_fence_start != -1:
                # Check if there's trailing metadata block after last_fence_start
                # Insert ``` after the content, before any trailing --- or metadata
                insert_pos = len(lines)
                # Look backwards from end for a good insertion point
                for i in range(len(lines)-1, last_fence_start, -1):
                    stripped = lines[i].strip()
                    if stripped and not stripped.startswith('>') and stripped != '---':
                        insert_pos = i + 1
                        break
                lines.insert(insert_pos, '```')
                content = '\n'.join(lines)
                changes.append('fix_unclosed_fence')
    
    # === Fix 2: multi_h1 ===
    if len(h1_lines) > 2:
        # Need to recompute h1 lines on current content
        lines = content.split('\n')
        h1_indices = [i for i, line in enumerate(lines) if re.match(r'^# ', line)]
        # Keep first H1, downgrade rest
        downgrade_count = 0
        for idx in h1_indices[1:]:
            lines[idx] = '##' + lines[idx][1:]
            downgrade_count += 1
        content = '\n'.join(lines)
        changes.append('downgrade_h1_x' + str(downgrade_count))
    
    # === Fix 3: broken links ===
    if broken_links:
        # Recompute broken links on current content
        link_pattern = re.compile(r'\[([^\]]*)\]\(([^)]+)\)')
        broken_links = []
        for m in link_pattern.finditer(content):
            url = m.group(2)
            if not url.startswith(('http://', 'https://', 'mailto:', '#')):
                target = os.path.normpath(os.path.join(os.path.dirname(fp), url))
                if not os.path.exists(target):
                    broken_links.append({'match': m.group(0), 'text': m.group(1), 'url': url})
        
        fixed_count = 0
        for bl in broken_links:
            old = bl['match']
            new = '[' + bl['text'] + ' [链接失效]](' + bl['url'] + ')'
            content = content.replace(old, new, 1)
            fixed_count += 1
        if fixed_count:
            changes.append('fix_broken_links_x' + str(fixed_count))
    
    if content != original:
        with open(fp, 'w', encoding='utf-8') as f:
            f.write(content)
    
    return changes

for fp in files:
    try:
        changes = fix_file(fp)
        if changes is not None and changes:
            rel = os.path.relpath(fp, base)
            log.append({'file': rel, 'changes': changes})
    except Exception as e:
        rel = os.path.relpath(fp, base)
        log.append({'file': rel, 'error': str(e)})

print('Fixed files: ' + str(len([x for x in log if 'changes' in x])))
print('Needs rewrite: ' + str(len(needs_rewrite)))

with open(os.path.join(base, 'fix_log.json'), 'w', encoding='utf-8') as f:
    json.dump({'fixed': log, 'needs_rewrite': needs_rewrite}, f, ensure_ascii=False, indent=2)

print('\n--- Fixed files ---')
for entry in log[:50]:
    print(entry['file'] + ': ' + ', '.join(entry.get('changes', [])))
if len(log) > 50:
    print('... and ' + str(len(log)-50) + ' more')

print('\n--- Needs rewrite ---')
for entry in needs_rewrite[:20]:
    print(entry['file'] + ' (' + str(entry['problems']) + ' problems)')
if len(needs_rewrite) > 20:
    print('... and ' + str(len(needs_rewrite)-20) + ' more')
