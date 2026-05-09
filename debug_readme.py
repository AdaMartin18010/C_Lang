import re, subprocess

result = subprocess.run(['git', 'show', 'HEAD:knowledge/README.md'], 
                       capture_output=True, text=True, encoding='utf-8', cwd='e:\\_src\\C_Lang')
content = result.stdout
lines = content.split('\n')

# Find template marker
marker = '## 深入理解'
idx = content.rfind(marker)
print('Template marker at char index:', idx)

before = content[:idx]
before_lines = before.split('\n')
print('before_lines count:', len(before_lines))

# Check fence state before marker
in_fence_before = False
for line in before_lines:
    stripped = line.strip()
    if re.match(r'^```\s*$', stripped):
        in_fence_before = not in_fence_before
    elif re.match(r'^```\S', stripped):
        in_fence_before = True

print('in_fence_before:', in_fence_before)

if in_fence_before:
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
    if fence_start_line != -1:
        print('Content at fence_start_line:', repr(before_lines[fence_start_line]))
        # Show context
        for j in range(max(0, fence_start_line-2), min(len(before_lines), fence_start_line+3)):
            print('  Line', j+1, ':', repr(before_lines[j]))
