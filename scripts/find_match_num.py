import re

with open('knowledge/01_Core_Knowledge_System/07_Modern_C/C2y_If_Declaration_Depth.md', 'r', encoding='utf-8') as f:
    lines = f.readlines()

fence_re = re.compile(r'^' + '`' * 3)
targets = {2608, 2628, 2634, 2664, 2670, 2784, 2794, 2821, 2841, 2844, 2850, 2883}

for idx, (i, line) in enumerate([(i, lines[i]) for i in range(len(lines)) if fence_re.match(lines[i])]):
    if i+1 in targets:
        print(f'Match {idx+1}: line {i+1}: {line.strip()}')
