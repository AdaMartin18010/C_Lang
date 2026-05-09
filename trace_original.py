import re, subprocess

# Get original content from git
result = subprocess.run(['git', 'show', 'HEAD:knowledge/README.md'], 
                       capture_output=True, text=True, encoding='utf-8', cwd='e:\\_src\\C_Lang')
lines = result.stdout.split('\n')

in_fence = False
for i, line in enumerate(lines):
    stripped = line.strip()
    if re.match(r'^```\s*$', stripped):
        old = in_fence
        in_fence = not in_fence
        print('Line', i+1, ':', 'OUT' if not old else 'IN', '->', 'OUT' if not in_fence else 'IN', '|', repr(stripped))
    elif re.match(r'^```\S', stripped):
        old = in_fence
        in_fence = True
        print('Line', i+1, ':', 'OUT' if not old else 'IN', '-> IN |', repr(stripped))

print('Final:', 'IN' if in_fence else 'OUT')
