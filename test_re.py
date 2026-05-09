import re

with open('e:\\_src\\C_Lang\\knowledge\\README.md', 'r', encoding='utf-8') as f:
    lines = f.readlines()

stripped = lines[355].strip()
print('from file:')
print('  repr:', ascii(stripped))
print('  len:', len(stripped))
print('  bare:', bool(re.match(r'^```\s*$', stripped)))
print('  lang:', bool(re.match(r'^```\S', stripped)))

s = '```'
print('literal:')
print('  repr:', ascii(s))
print('  len:', len(s))
print('  bare:', bool(re.match(r'^```\s*$', s)))
print('  lang:', bool(re.match(r'^```\S', s)))

# Maybe the file has different characters
for ch in stripped:
    print('  ord:', ord(ch), 'name:', hex(ord(ch)))
