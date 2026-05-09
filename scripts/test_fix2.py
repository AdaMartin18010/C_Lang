import re

with open('knowledge/README.md', 'r', encoding='utf-8') as f:
    lines = f.readlines()

if not lines[-1].endswith('\n'):
    lines[-1] += '\n'
lines.append('\n```\n')

print(f'Line 1684: {repr(lines[1683])}')
print(f'Strip: {repr(lines[1683].strip())}')
print(f'Equals ```: {lines[1683].strip() == "`" * 3}')
