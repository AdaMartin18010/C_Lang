import os
import re

files = [
    r'knowledge\README.md',
    r'knowledge\01_Core_Knowledge_System\07_Modern_C\C2y_If_Declaration_Depth.md',
    r'knowledge\01_Core_Knowledge_System\09_Safety_Standards\MISRA_C_2023\tools\01_MISRA_Toolchain_Guide.md',
    r'knowledge\01_Core_Knowledge_System\09_Safety_Standards\MISRA_C_2025\tools\docs\DEVIATIONS.md',
    r'knowledge\02_Formal_Semantics_and_Physics\09_Physical_Machine_Layer\05_Modern_CPU_Architectures_2024.md',
    r'knowledge\02_Formal_Semantics_and_Physics\09_Physical_Machine_Layer\01_Digital_Logic_Foundations\02_Logic_Gates_Implementation.md',
    r'knowledge\02_Formal_Semantics_and_Physics\10_Lean4_Compilation\LEAN4_ECOSYSTEM.md',
    r'knowledge\02_Formal_Semantics_and_Physics\10_Lean4_Compilation\Z8_MetaM_Extended.md',
    r'knowledge\02_Formal_Semantics_and_Physics\13_Hardware_Description\04_HLS_with_C\03_HLS_Optimization.md',
    r'knowledge\02_Formal_Semantics_and_Physics\14_C_to_Hardware_Mapping\04_Functions_to_Modules.md',
    r'knowledge\02_Formal_Semantics_and_Physics\18_FPGA_Labs\04_Lab4_Pipelined_CPU.md',
    r'knowledge\02_Formal_Semantics_and_Physics\18_FPGA_Labs\05_Lab5_Cache_Controller.md',
    r'knowledge\02_Formal_Semantics_and_Physics\18_FPGA_Labs\06_Lab6_SoC_Integration.md',
    r'knowledge\04_Industrial_Scenarios\15_Industrial_Communication\01_Industrial_Communication_Protocols.md',
    r'knowledge\05_Deep_Structure_MetaPhysics\08_Debugging_Tools\03_Advanced_Debugging_Techniques.md',
    r'knowledge\07_Modern_Toolchain\04_Package_Management\01_Conan_Package_Manager.md',
    r'knowledge\08_Zig_C_Connection\01_Type_System_Mapping\03_Function_Pointer_Compatibility.md',
    r'knowledge\11_Machine_Learning_C\03_Embedded_AI_Deployment.md',
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
            else:
                in_fence = False
    if in_fence:
        print(f'=== {f} ===')
        print(f'Unclosed fence at line {fence_start}')
        # Show lines from fence_start to end
        start = fence_start - 1
        end = min(len(lines), fence_start + 12)
        for j in range(start, end):
            print(f'{j+1:4d}: {lines[j].rstrip()}')
        if end < len(lines):
            print('...')
            for j in range(len(lines)-3, len(lines)):
                print(f'{j+1:4d}: {lines[j].rstrip()}')
        print()
