with open('knowledge/README.md', 'r', encoding='utf-8') as f:
    lines = f.readlines()

for i in range(1669, len(lines)):
    if lines[i].startswith('`' * 3):
        print(f'{i+1}: {repr(lines[i])}')
