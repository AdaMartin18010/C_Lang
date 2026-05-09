import re

for fp in [
    'e:\\_src\\C_Lang\\knowledge\\01_Core_Knowledge_System\\07_Modern_C\\C2y_If_Declaration_Depth.md',
    'e:\\_src\\C_Lang\\knowledge\\02_Formal_Semantics_and_Physics\\09_Physical_Machine_Layer\\05_Modern_CPU_Architectures_2024.md'
]:
    with open(fp, 'r', encoding='utf-8') as f:
        lines = f.readlines()
    
    in_fence = False
    issues = []
    for i, line in enumerate(lines):
        stripped = line.strip()
        if re.match(r'^```\s*$', stripped):
            # Bare fence - toggles
            in_fence = not in_fence
            issues.append((i+1, in_fence, stripped, 'toggle'))
        elif re.match(r'^```\S', stripped):
            # Language fence - always starts
            in_fence = True
            issues.append((i+1, in_fence, stripped, 'start'))
    
    print('=== ' + fp + ' ===')
    print('Final state: ' + ('IN fence' if in_fence else 'OUT of fence'))
    print('Last 10 transitions:')
    for ln, state, txt, kind in issues[-10:]:
        print('  Line ' + str(ln) + ': ' + txt + ' [' + kind + '] -> ' + ('IN' if state else 'OUT'))
    print()
