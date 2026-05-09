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

def get_fence_state(lines):
    """Return final fence state (True=IN, False=OUT) and list of fence transitions."""
    in_fence = False
    transitions = []
    for i, line in enumerate(lines):
        stripped = line.strip()
        if re.match(r'^```\s*$', stripped):
            in_fence = not in_fence
            transitions.append((i, 'toggle', in_fence))
        elif re.match(r'^```\S', stripped):
            in_fence = True
            transitions.append((i, 'start', in_fence))
    return in_fence, transitions

def fix_file(fp):
    changes = []
    with open(fp, 'r', encoding='utf-8') as f:
        content = f.read()
    
    original = content
    lines = content.split('\n')
    
    # === Detect problems ===
    
    # 1. unclosed fence
    unclosed, transitions = get_fence_state(lines)
    
    # 2. multi_h1 (only outside fences)
    outside_lines = get_lines_outside_fences(lines)
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
    template_marker = None
    for marker in ['## 深入理解', '## 深入探讨']:
        idx = content.rfind(marker)
        if idx != -1:
            section = content[idx:]
            if '应用场景1' in section or '应用场景2' in section or '应用场景3' in section:
                has_template = True
                template_marker = marker
                break
            elif re.search(r'核心原理|实践应用|最佳实践', section) and len(section) < 3000:
                has_template = True
                template_marker = marker
                break
    
    problem_count = (1 if unclosed else 0) + max(0, len(h1_lines)-2) + len(broken_links) + (1 if has_template else 0)
    
    if problem_count > 20:
        needs_rewrite.append({'file': fp, 'problems': problem_count})
        return None
    
    # === Fix 4: trailing template ===
    if has_template and template_marker:
        # Find the line number of template marker
        marker_line = -1
        for i, line in enumerate(lines):
            if line.strip().startswith(template_marker):
                marker_line = i
                break
        
        if marker_line != -1:
            # Check if marker is inside a fence
            in_fence_at_marker = False
            for i in range(marker_line):
                stripped = lines[i].strip()
                if re.match(r'^```\s*$', stripped):
                    in_fence_at_marker = not in_fence_at_marker
                elif re.match(r'^```\S', stripped):
                    in_fence_at_marker = True
            
            if in_fence_at_marker:
                # Marker is inside a fence. Find the fence start.
                fence_start = -1
                temp_in = False
                for i in range(marker_line):
                    stripped = lines[i].strip()
                    if re.match(r'^```\s*$', stripped):
                        temp_in = not temp_in
                        if temp_in:
                            fence_start = i
                    elif re.match(r'^```\S', stripped):
                        temp_in = True
                        fence_start = i
                
                if fence_start != -1:
                    # Delete from fence_start to end
                    new_lines = lines[:fence_start]
                    new_lines.append('```')
                    lines = new_lines
                    content = '\n'.join(lines)
                    content = re.sub(r'\n{3,}$', '\n\n', content)
                    changes.append('remove_trailing_template_and_close_fence')
            else:
                # Marker is outside fence. Delete from marker to end.
                lines = lines[:marker_line]
                content = '\n'.join(lines)
                content = re.sub(r'\n{3,}$', '\n\n', content)
                changes.append('remove_trailing_template')
    
    # Re-split lines after template removal
    lines = content.split('\n')
    
    # === Fix 1: unclosed fence ===
    unclosed_after, transitions_after = get_fence_state(lines)
    if unclosed_after:
        # Find the last fence start
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
            # Check if this looks like a template fence
            after_content = '\n'.join(lines[last_fence_start:])
            is_template_fence = (
                len(after_content) < 200 and 
                re.search(r'应用场景|最佳实践|核心原理|学习建议|深入探讨|理解基础概念|掌握核心机制', after_content)
            )
            
            # Also check if the fence has no real content (just empty lines and short text)
            fence_lines = [l.strip() for l in lines[last_fence_start:] if l.strip()]
            is_empty_fence = len(fence_lines) <= 2 and fence_lines[0] == '```'
            
            if is_template_fence or is_empty_fence:
                # Remove the fence and everything after
                lines = lines[:last_fence_start]
                content = '\n'.join(lines)
                content = re.sub(r'\n{3,}$', '\n\n', content)
                changes.append('remove_template_fence')
            else:
                # Add closing fence
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
        if downgrade_count:
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

print('\n--- Fixed files (first 50) ---')
for entry in log[:50]:
    print(entry['file'] + ': ' + ', '.join(entry.get('changes', [])))
if len(log) > 50:
    print('... and ' + str(len(log)-50) + ' more')

print('\n--- Needs rewrite ---')
for entry in needs_rewrite[:20]:
    print(entry['file'] + ' (' + str(entry['problems']) + ' problems)')
if len(needs_rewrite) > 20:
    print('... and ' + str(len(needs_rewrite)-20) + ' more')
