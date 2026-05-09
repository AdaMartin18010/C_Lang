import re

files = [
    r'knowledge\README.md',
    r'knowledge\02_Formal_Semantics_and_Physics\10_Lean4_Compilation\Z8_MetaM_Extended.md',
    r'knowledge\02_Formal_Semantics_and_Physics\13_Hardware_Description\04_HLS_with_C\02_C_to_Hardware_Pragma.md',
    r'knowledge\02_Formal_Semantics_and_Physics\13_Hardware_Description\04_HLS_with_C\03_HLS_Optimization.md',
    r'knowledge\06_Thinking_Representation\09_Bloom_Taxonomy\05_Analysis_Frameworks.md',
    r'knowledge\06_Thinking_Representation\10_Concept_Maps\README.md',
]

fence_re = re.compile(r'^' + '`' * 3)

for f in files:
    with open(f, 'r', encoding='utf-8') as fh:
        lines = fh.readlines()
    in_fence = False
    fence_start = 0
    for i, line in enumerate(lines):
        if fence_re.match(line):
            if not in_fence:
                in_fence = True
                fence_start = i + 1
            elif line.strip() == '`' * 3:
                in_fence = False
    if in_fence:
        print(f'=== {f} ===')
        print(f'Unclosed fence at line {fence_start}')
        start = max(0, fence_start - 3)
        end = min(len(lines), fence_start + 12)
        for j in range(start, end):
            print(f'{j+1:4d}: {lines[j].rstrip()}')
        if end < len(lines):
            print('...')
            for j in range(len(lines)-3, len(lines)):
                print(f'{j+1:4d}: {lines[j].rstrip()}')
        print()
