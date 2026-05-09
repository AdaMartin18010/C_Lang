import re

with open('knowledge/01_Core_Knowledge_System/07_Modern_C/C2y_If_Declaration_Depth.md', 'r', encoding='utf-8') as f:
    lines = f.readlines()
line = lines[2669]
print(repr(line))
print(line.startswith('`' * 3))
print(re.match('^' + '`' * 3, line))

# Check all lines between 2664 and 2784
for i in range(2663, 2784):
    if re.match('^' + '`' * 3, lines[i]):
        print(f'Match at {i+1}: {repr(lines[i][:20])}')
