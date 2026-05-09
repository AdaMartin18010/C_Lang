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
        if stripped.startswith('```'):
            in_fence = not in_fence
            issues.append((i+1, in_fence, stripped))
    
    print('=== ' + fp + ' ===')
    print('Final state: ' + ('IN fence' if in_fence else 'OUT of fence'))
    if issues:
        print('Last 10 fence transitions:')
        for ln, state, txt in issues[-10:]:
            print('  Line ' + str(ln) + ': ' + txt + ' -> ' + ('IN' if state else 'OUT'))
    print()
