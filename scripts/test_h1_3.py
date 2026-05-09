import re

content = 'Line 1\n# Heading 1\nLine 3\n# Heading 2\n'

h1_re_single = re.compile(r'^# [^#]')
h1_re_multi = re.compile(r'^# [^#]', re.MULTILINE)

print(f'Without MULTILINE: {len(h1_re_single.findall(content))}')
print(f'With MULTILINE: {len(h1_re_multi.findall(content))}')
