import re

with open('e:\\_src\\C_Lang\\knowledge\\README.md', 'r', encoding='utf-8') as f:
    content = f.read()

lines = content.split('\n')
print('Current file lines:', len(lines))

# Check if this is already modified or original
if '## 深入理解' in content:
    print('This is ORIGINAL file (has ## 深入理解)')
else:
    print('This is MODIFIED file (no ## 深入理解)')

# Find last fence
for i in range(len(lines)-1, -1, -1):
    if lines[i].strip().startswith('```'):
        print('Last fence at line', i+1, ':', repr(lines[i].strip()))
        break
