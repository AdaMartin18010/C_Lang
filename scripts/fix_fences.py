import os
import re

def fix_file(filepath):
    with open(filepath, 'r', encoding='utf-8') as f:
        lines = f.readlines()
    
    original = list(lines)
    fence_re = re.compile(r'^' + '`' * 3)
    in_fence = False
    
    for i, line in enumerate(lines):
        if fence_re.match(line):
            if not in_fence:
                in_fence = True
            elif line.strip() == '`' * 3:
                in_fence = False
    
    if not in_fence:
        return False  # No issue
    
    # File-specific fixes
    rel_path = os.path.relpath(filepath)
    
    if rel_path == r'knowledge\README.md':
        # Add closing fence at end
        if not lines[-1].endswith('\n'):
            lines[-1] += '\n'
        lines.append('\n```\n')
        print(f'Fixed {rel_path}: added closing fence at end')
    
    elif rel_path == r'knowledge\02_Formal_Semantics_and_Physics\10_Lean4_Compilation\Z8_MetaM_Extended.md':
        # Line 150 has inline backtick fence at end
        # Fix by removing the inline fence and adding proper fence on next line
        if r'\n```' in lines[149]:
            lines[149] = lines[149].replace(r'\n```', '')
            lines.insert(150, '```\n')
            print(f'Fixed {rel_path}: fixed inline fence on line 150')
    
    elif rel_path == r'knowledge\02_Formal_Semantics_and_Physics\13_Hardware_Description\04_HLS_with_C\02_C_to_Hardware_Pragma.md':
        # Add closing fence before the footer
        for i in range(len(lines) - 1, -1, -1):
            if lines[i].strip() and not lines[i].strip().startswith('>'):
                # Insert after last non-empty non-footer line
                lines.insert(i + 1, '\n```\n')
                break
        print(f'Fixed {rel_path}: added closing fence')
    
    elif rel_path == r'knowledge\02_Formal_Semantics_and_Physics\13_Hardware_Description\04_HLS_with_C\03_HLS_Optimization.md':
        # Add closing fence before the footer
        for i in range(len(lines) - 1, -1, -1):
            if lines[i].strip() and not lines[i].strip().startswith('>'):
                lines.insert(i + 1, '\n```\n')
                break
        print(f'Fixed {rel_path}: added closing fence')
    
    elif rel_path == r'knowledge\06_Thinking_Representation\09_Bloom_Taxonomy\05_Analysis_Frameworks.md':
        # Line 628 has an orphan closing fence that should be removed
        # The actual code block is closed by line 602
        for i, line in enumerate(lines):
            if i + 1 == 628 and line.strip() == '`' * 3:
                lines[i] = '\n'
                print(f'Fixed {rel_path}: removed orphan fence on line 628')
                break
    
    elif rel_path == r'knowledge\06_Thinking_Representation\10_Concept_Maps\README.md':
        # Line 210 has orphan opening fence
        for i, line in enumerate(lines):
            if i + 1 == 210 and line.strip() == '`' * 3:
                lines[i] = '\n'
                print(f'Fixed {rel_path}: removed orphan fence on line 210')
                break
    
    # Verify fix
    in_fence = False
    for line in lines:
        if fence_re.match(line):
            if not in_fence:
                in_fence = True
            elif line.strip() == '`' * 3:
                in_fence = False
    
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
