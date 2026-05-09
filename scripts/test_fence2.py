import re

with open('knowledge/01_Core_Knowledge_System/07_Modern_C/C2y_If_Declaration_Depth.md', 'r', encoding='utf-8') as f:
    lines = f.readlines()

for i in [2607, 2627, 2633, 2663, 2669, 2783, 2793, 2820, 2840, 2843, 2849, 2882]:
    line = lines[i]
    print(f'{i+1}: {repr(line[:10])}  starts_with_```={line.startswith("`" * 3)}')
