import os
import re

files_to_update = [
    r'knowledge\00_VERSION_TRACKING\README.md',
    r'knowledge\05_C_Language_Standards\C23_C2Y_ROADMAP.md',
    r'knowledge\01_Core_Knowledge_System\07_Modern_C\C2y_Defer_Depth.md',
    r'knowledge\01_Core_Knowledge_System\09_Safety_Standards\MISRA_C_2025\MISRA_C_2025_Comprehensive_Guide.md',
    r'knowledge\01_Core_Knowledge_System\07_Modern_C\C2y_Preview\01_Defer_Keyword.md',
    r'knowledge\01_Core_Knowledge_System\07_Modern_C\C2y_Preview\02_Elvis_Operator.md',
    r'knowledge\01_Core_Knowledge_System\07_Modern_C\C2y_Preview\04_Counter_Macro.md',
    r'knowledge\01_Core_Knowledge_System\07_Modern_C\C2y_Preview\05_Typeof_Return.md',
    r'knowledge\01_Core_Knowledge_System\07_Modern_C\C2y_Preview\README.md',
    r'knowledge\00_VERSION_TRACKING\LLVM_22_C23_Update.md',
    r'knowledge\00_VERSION_TRACKING\LLVM_21_C23_Update.md',
    r'knowledge\02_Formal_Semantics_and_Physics\10_Lean4_Compilation\Z8_MetaM_Extended.md',
    r'knowledge\02_Formal_Semantics_and_Physics\13_Hardware_Description\04_HLS_with_C\02_C_to_Hardware_Pragma.md',
    r'knowledge\02_Formal_Semantics_and_Physics\13_Hardware_Description\04_HLS_with_C\03_HLS_Optimization.md',
    r'knowledge\06_Thinking_Representation\09_Bloom_Taxonomy\05_Analysis_Frameworks.md',
    r'knowledge\06_Thinking_Representation\10_Concept_Maps\README.md',
    r'knowledge\README.md',
]

# Patterns to match "last updated" lines and capture the date
patterns = [
    r'(最后更新[\s*:]+)\d{4}-\d{2}-\d{2}',
    r'(Last Updated[\s*:]+)\d{4}-\d{2}-\d{2}',
    r'(最后更新日期[\s*:]+)\d{4}-\d{2}-\d{2}',
]

updated = 0
for filepath in files_to_update:
    if not os.path.exists(filepath):
        print(f'Skip (not found): {filepath}')
        continue
    
    with open(filepath, 'r', encoding='utf-8') as f:
        content = f.read()
    
    original = content
    # Only replace the LAST occurrence of each pattern
    for pattern in patterns:
        matches = list(re.finditer(pattern, content, re.IGNORECASE))
        if matches:
            last_match = matches[-1]
            # Replace the date part
            prefix = last_match.group(1)
            start = last_match.start()
            end = last_match.end()
            content = content[:start] + prefix + '2026-05-10' + content[end:]
    
    if content != original:
        with open(filepath, 'w', encoding='utf-8') as f:
            f.write(content)
        print(f'Updated: {filepath}')
        updated += 1
    else:
        print(f'No date found: {filepath}')

print(f'\nTotal updated: {updated}/{len(files_to_update)}')
