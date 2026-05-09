import os, re, json

base = "e:\\_src\\C_Lang"

with open(os.path.join(base, 'md_files_list.txt'), 'r', encoding='utf-8') as f:
    files = [line.strip() for line in f if line.strip()]

log = []
needs_rewrite = []

def get_lines_outside_fences(lines):
    """Return a set of line indices that are OUTSIDE code fences."""
    in_fence = False
    outside = set()
    for i, line in enumerate(lines):
        stripped = line.strip()
        if re.match(r'^```\s*$', stripped):
            in_fence = not in_fence
        elif re.match(r'^```\S', stripped):
            in_fence = True
        if not in_fence:
            outside.add(i)
    return outside

def fix_file(fp):
    changes = []
    with open(fp, 'r', encoding='utf-8') as f:
        content = f.read()
    
    original = content
    lines = content.split('\n')
    
    # === Detect problems ===
    
    # 1. unclosed fence (correct logic)
    in_fence = False
    for i, line in enumerate(lines):
        stripped = line.strip()
        if re.match(r'^```\s*$', stripped):
            in_fence = not in_fence
        elif re.match(r'^```\S', stripped):
            in_fence = True
    unclosed = in_fence
    
    # 2. multi_h1 (only outside fences)
    outside_lines = get_lines_outside_fences(lines[:])
    h1_lines = [i for i in outside_lines if re.match(r'^# ', lines[i])]
    
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
        # Find template section boundaries
        # Try to find from last occurrence of template marker to end
        removed = False
        for marker in ['## 深入理解', '## 深入探讨']:
            idx = content.rfind(marker)
            if idx != -1:
                section = content[idx:]
                # Check if it's a template section
                is_template = False
                if '应用场景1' in section or '应用场景2' in section or '应用场景3' in section:
                    is_template = True
                elif '核心原理' in section and '实践应用' in section and len(section) < 3000:
                    is_template = True
                elif '深入探讨技术原理' in section and len(section) < 3000:
                    is_template = True
                
                if is_template:
                    # Before deleting, check if there's an open fence just before the marker
                    before = content[:idx]
                    after = content[idx:]
                    
                    # Check if we are inside a fence at the marker position
                    # We need to find the fence state just before idx
                    before_lines = before.split('\n')
                    in_fence_before = False
                    for line in before_lines:
                        stripped = line.strip()
                        if re.match(r'^```\s*$', stripped):
                            in_fence_before = not in_fence_before
                        elif re.match(r'^```\S', stripped):
                            in_fence_before = True
                    
                    if in_fence_before:
                        # The marker is inside a code block. We should find the start of this fence
                        # and close it, or delete the whole fence content including the start
                        # For safety, find the last fence start and close it properly
                        fence_start_line = -1
                        temp_in = False
                        for i, line in enumerate(before_lines):
                            stripped = line.strip()
                            if re.match(r'^```\s*$', stripped):
                                temp_in = not temp_in
                                if temp_in:
                                    fence_start_line = i
                            elif re.match(r'^```\S', stripped):
                                temp_in = True
                                fence_start_line = i
                        
                        if fence_start_line != -1:
                            # Close the fence before the marker, then delete the marker section
                            # Reconstruct: lines up to fence_start_line, then close fence, skip template
                            new_lines = before_lines[:fence_start_line]
                            new_lines.append('```')
                            content = '\n'.join(new_lines)
                            # Clean up trailing whitespace
                            content = re.sub(r'\n{3,}$', '\n\n', content)
                            changes.append('remove_trailing_template_and_close_fence')
                            removed = True
                            break
                    else:
                        # Not in fence, just delete the section
                        content = before
                        content = re.sub(r'\n{3,}$', '\n\n', content)
                        changes.append('remove_trailing_template')
                        removed = True
                        break
        
        if not removed:
            # Fallback: try regex-based removal
            template_patterns = [
                r'## 深入理解\s*\n(?:.*?核心原理.*?\n.*?实践应用.*?\n.*?最佳实践.*?\n.*?)+',
                r'## 深入探讨\s*\n(?:.*?核心概念.*?\n.*?实践应用.*?\n.*?学习建议.*?\n.*?)+',
            ]
            for pat in template_patterns:
                m = re.search(pat, content, re.DOTALL)
                if m:
                    section = m.group(0)
                    if len(section) < 3000:
                        content = content[:m.start()] + content[m.end():]
                        content = re.sub(r'\n{3,}$', '\n\n', content)
                        changes.append('remove_trailing_template_regex')
                        removed = True
                        break
    
    # Re-split lines after template removal
    lines = content.split('\n')
    
    # === Fix 1: unclosed fence ===
    if unclosed:
        in_fence = False
        for i, line in enumerate(lines):
            stripped = line.strip()
            if re.match(r'^```\s*$', stripped):
                in_fence = not in_fence
            elif re.match(r'^```\S', stripped):
                in_fence = True
        
        if in_fence:
            # Find last fence start
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
            
            if last_fence_start != -1:
                # Check if content after last_fence_start looks like template residue
                after_start = '\n'.join(lines[last_fence_start:])
                if len(after_start) < 200 and re.search(r'应用场景|最佳实践|核心原理|学习建议|深入探讨', after_start):
                    # This is likely a template fence, remove it and everything after
                    content = '\n'.join(lines[:last_fence_start])
                    content = re.sub(r'\n{3,}$', '\n\n', content)
                    changes.append('remove_trailing_template_fence')
                else:
                    # Legitimate unclosed fence, add closing
                    insert_pos = len(lines)
                    for i in range(len(lines)-1, last_fence_start, -1):
                        stripped = lines[i].strip()
                        if stripped and not stripped.startswith('>') and stripped != '---':
                            insert_pos = i + 1
                            break
                    lines.insert(insert_pos, '```')
                    content = '\n'.join(lines)
                    changes.append('fix_unclosed_fence')
    
    # === Fix 2: multi_h1 (only outside fences) ===
    if len(h1_lines) > 2:
        lines = content.split('\n')
        outside_lines = get_lines_outside_fences(lines)
        h1_indices = [i for i in outside_lines if re.match(r'^# ', lines[i])]
        downgrade_count = 0
        for idx in h1_indices[1:]:
            lines[idx] = '##' + lines[idx][1:]
            downgrade_count += 1
        content = '\n'.join(lines)
        changes.append('downgrade_h1_x' + str(downgrade_count))
    
    # === Fix 3: broken links ===
    if broken_links:
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
        import traceback
        rel = os.path.relpath(fp, base)
        log.append({'file': rel, 'error': str(e), 'traceback': traceback.format_exc()})

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
