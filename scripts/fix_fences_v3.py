import os
import re

def check_unclosed(lines):
    fence_re = re.compile(r'^' + '`' * 3)
    in_fence = False
    fence_start = 0
    for i, line in enumerate(lines):
        if fence_re.match(line):
            if not in_fence:
                in_fence = True
                fence_start = i + 1
            elif line.strip() == '`' * 3:
                in_fence = False
    return in_fence, fence_start

def fix_file(filepath):
    with open(filepath, 'r', encoding='utf-8') as f:
        lines = f.readlines()
    
    in_fence, fence_start = check_unclosed(lines)
    
    if not in_fence:
        return False
    
    rel_path = os.path.relpath(filepath)
    
    if rel_path == r'knowledge\README.md':
        if not lines[-1].endswith('\n'):
            lines[-1] += '\n'
        lines.append('```\n')
        print(f'Fixed {rel_path}: added closing fence at end')
    
    elif rel_path == r'knowledge\02_Formal_Semantics_and_Physics\10_Lean4_Compilation\Z8_MetaM_Extended.md':
        if r'\n```' in lines[149]:
            lines[149] = lines[149].replace(r'\n```', '')
            lines.insert(150, '```\n')
            print(f'Fixed {rel_path}: fixed inline fence on line 150')
    
    elif rel_path == r'knowledge\02_Formal_Semantics_and_Physics\13_Hardware_Description\04_HLS_with_C\02_C_to_Hardware_Pragma.md':
        if not lines[-1].endswith('\n'):
            lines[-1] += '\n'
        lines.append('```\n')
        print(f'Fixed {rel_path}: added closing fence at end')
    
    elif rel_path == r'knowledge\02_Formal_Semantics_and_Physics\13_Hardware_Description\04_HLS_with_C\03_HLS_Optimization.md':
        if not lines[-1].endswith('\n'):
            lines[-1] += '\n'
        lines.append('```\n')
        print(f'Fixed {rel_path}: added closing fence at end')
    
    elif rel_path == r'knowledge\06_Thinking_Representation\09_Bloom_Taxonomy\05_Analysis_Frameworks.md':
        for i, line in enumerate(lines):
            if i + 1 == 628 and line.strip() == '`' * 3:
                lines[i] = '\n'
                print(f'Fixed {rel_path}: removed orphan fence on line 628')
                break
    
    elif rel_path == r'knowledge\06_Thinking_Representation\10_Concept_Maps\README.md':
        for i, line in enumerate(lines):
            if i + 1 == 210 and line.strip() == '`' * 3:
                lines[i] = '\n'
                print(f'Fixed {rel_path}: removed orphan fence on line 210')
                break
    
    # Verify fix using modified lines
    in_fence, _ = check_unclosed(lines)
    if in_fence:
        print(f'  WARNING: {rel_path} still has unclosed fence!')
        return False
    
    with open(filepath, 'w', encoding='utf-8') as f:
        f.writelines(lines)
    
    return True

files = [
    r'knowledge\README.md',
    r'knowledge\02_Formal_Semantics_and_Physics\10_Lean4_Compilation\Z8_MetaM_Extended.md',
    r'knowledge\02_Formal_Semantics_and_Physics\13_Hardware_Description\04_HLS_with_C\02_C_to_Hardware_Pragma.md',
    r'knowledge\02_Formal_Semantics_and_Physics\13_Hardware_Description\04_HLS_with_C\03_HLS_Optimization.md',
    r'knowledge\06_Thinking_Representation\09_Bloom_Taxonomy\05_Analysis_Frameworks.md',
    r'knowledge\06_Thinking_Representation\10_Concept_Maps\README.md',
]

fixed = 0
for f in files:
    if fix_file(f):
        fixed += 1

print(f'\nTotal fixed: {fixed}/{len(files)}')
