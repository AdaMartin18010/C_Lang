import os

files = [
    r'knowledge\01_Core_Knowledge_System\07_Modern_C\C2y_If_Declaration_Depth.md',
    r'knowledge\01_Core_Knowledge_System\09_Safety_Standards\MISRA_C_2023\tools\01_MISRA_Toolchain_Guide.md',
    r'knowledge\01_Core_Knowledge_System\09_Safety_Standards\MISRA_C_2025\tools\docs\DEVIATIONS.md',
    r'knowledge\02_Formal_Semantics_and_Physics\09_Physical_Machine_Layer\05_Modern_CPU_Architectures_2024.md',
    r'knowledge\02_Formal_Semantics_and_Physics\09_Physical_Machine_Layer\01_Digital_Logic_Foundations\02_Logic_Gates_Implementation.md',
    r'knowledge\02_Formal_Semantics_and_Physics\10_Lean4_Compilation\LEAN4_ECOSYSTEM.md',
    r'knowledge\02_Formal_Semantics_and_Physics\13_Hardware_Description\04_HLS_with_C\02_C_to_Hardware_Pragma.md',
    r'knowledge\02_Formal_Semantics_and_Physics\14_C_to_Hardware_Mapping\04_Functions_to_Modules.md',
    r'knowledge\02_Formal_Semantics_and_Physics\18_FPGA_Labs\04_Lab4_Pipelined_CPU.md',
    r'knowledge\02_Formal_Semantics_and_Physics\18_FPGA_Labs\05_Lab5_Cache_Controller.md',
    r'knowledge\02_Formal_Semantics_and_Physics\18_FPGA_Labs\06_Lab6_SoC_Integration.md',
    r'knowledge\04_Industrial_Scenarios\15_Industrial_Communication\01_Industrial_Communication_Protocols.md',
    r'knowledge\05_Deep_Structure_MetaPhysics\08_Debugging_Tools\03_Advanced_Debugging_Techniques.md',
    r'knowledge\06_Thinking_Representation\09_Bloom_Taxonomy\05_Analysis_Frameworks.md',
    r'knowledge\06_Thinking_Representation\10_Concept_Maps\README.md',
    r'knowledge\07_Modern_Toolchain\04_Package_Management\01_Conan_Package_Manager.md',
    r'knowledge\08_Zig_C_Connection\01_Type_System_Mapping\03_Function_Pointer_Compatibility.md',
    r'knowledge\11_Machine_Learning_C\03_Embedded_AI_Deployment.md',
]

fixed = 0
for path in files:
    with open(path, 'r', encoding='utf-8') as f:
        content = f.read()
    
    lines = content.split('\n')
    in_fence = False
    for line in lines:
        if line.startswith('```'):
            in_fence = not in_fence
    
    if in_fence:
        # Add closing fence at the end
        if not content.endswith('\n'):
            content += '\n'
        content += '```\n'
        with open(path, 'w', encoding='utf-8') as f:
            f.write(content)
        fixed += 1
        print(f'Fixed: {path}')

print(f'Total fixed: {fixed}')
