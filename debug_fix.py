import re, subprocess

result = subprocess.run(['git', 'show', 'HEAD:knowledge/README.md'], 
                       capture_output=True, text=True, encoding='utf-8', cwd='e:\\_src\\C_Lang')
content = result.stdout
lines = content.split('\n')

# Simulate remove_trailing_template_and_close_fence
marker = '## 深入理解'
idx = content.rfind(marker)
before = content[:idx]
before_lines = before.split('\n')

in_fence_before = False
for line in before_lines:
    stripped = line.strip()
    if re.match(r'^```\s*$', stripped):
        in_fence_before = not in_fence_before
    elif re.match(r'^```\S', stripped):
        in_fence_before = True

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

print('fence_start_line:', fence_start_line)
print('in_fence_before:', in_fence_before)

new_lines = before_lines[:fence_start_line]
new_lines.append('```')
content = '\n'.join(new_lines)
content = re.sub(r'\n{3,}$', '\n\n', content)

# Now simulate fix_unclosed_fence
lines = content.split('\n')
in_fence = False
for i, line in enumerate(lines):
    stripped = line.strip()
    if re.match(r'^```\s*$', stripped):
        in_fence = not in_fence
    elif re.match(r'^```\S', stripped):
        in_fence = True

print('After template removal, in_fence:', in_fence)

if in_fence:
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
    
    print('last_fence_start:', last_fence_start)
    if last_fence_start != -1:
        after_start = '\n'.join(lines[last_fence_start:])
        print('after_start length:', len(after_start))
        print('matches template?', bool(re.search(r'应用场景|最佳实践|核心原理|学习建议|深入探讨', after_start)))
        if len(after_start) < 200 and re.search(r'应用场景|最佳实践|核心原理|学习建议|深入探讨', after_start):
            print('Would: remove_trailing_template_fence')
        else:
            print('Would: fix_unclosed_fence')
            insert_pos = len(lines)
            for i in range(len(lines)-1, last_fence_start, -1):
                stripped = lines[i].strip()
                if stripped and not stripped.startswith('>') and stripped != '---':
                    insert_pos = i + 1
                    break
            print('insert_pos:', insert_pos)
            print('insert line content:', repr(lines[insert_pos-1]) if insert_pos > 0 else 'N/A')
