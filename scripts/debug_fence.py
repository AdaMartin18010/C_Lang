with open('e:/_src/C_Lang/knowledge/12_Practice_Exercises/10_Debugging_Puzzles.md', 'r', encoding='utf-8', errors='ignore') as f:
    lines = f.readlines()

s = '```'
print(f'fence string: len={len(s)}, repr={repr(s)}')
line = lines[118]
print(f'line 119: {repr(line[:20])}')
print(f'startswith: {line.startswith(s)}')
print(f'line[0]==s[0]: {line[0] == s[0]}')
print(f'ord(line[0]): {ord(line[0])}')
print(f'ord(s[0]): {ord(s[0])}')
