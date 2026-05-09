import os

# Fix remaining path errors by correcting source links
fixes = [
    ('knowledge/03_System_Technology_Domains/16_High_Performance_Computing/02_OpenMP_Parallelism.md', '../../14_Concurrency_Parallelism/', '../../03_System_Technology_Domains/14_Concurrency_Parallelism/'),
    ('knowledge/03_System_Technology_Domains/18_Systems_Performance/03_Lock_Free_Programming.md', '../../09_High_Performance_Log/', '../../03_System_Technology_Domains/09_High_Performance_Log/'),
    ('knowledge/03_System_Technology_Domains/18_Systems_Performance/03_Lock_Free_Programming.md', '../../11_In_Memory_Database/', '../../03_System_Technology_Domains/11_In_Memory_Database/'),
    ('knowledge/03_System_Technology_Domains/19_Systems_Security/02_Binary_Exploitation.md', '../../06_Security_Boot/', '../../03_System_Technology_Domains/06_Security_Boot/'),
    ('knowledge/03_System_Technology_Domains/19_Systems_Security/02_Binary_Exploitation.md', '../../07_Hardware_Security/', '../../03_System_Technology_Domains/07_Hardware_Security/'),
    ('knowledge/03_System_Technology_Domains/23_Storage_Systems/02_SSD_Optimization.md', '../../18_Database_Internals/', '../../03_System_Technology_Domains/18_Database_Internals/'),
    ('knowledge/03_System_Technology_Domains/23_Storage_Systems/02_SSD_Optimization.md', '../../15_Industrial_Communication/', '../../03_System_Technology_Domains/15_Industrial_Communication/'),
    ('knowledge/09_Data_Structures_Algorithms/09_Distributed_Algorithms/01_Distributed_Consensus.md', '../../14_Concurrency_Parallelism/', '../../03_System_Technology_Domains/14_Concurrency_Parallelism/'),
    ('knowledge/09_Data_Structures_Algorithms/09_Distributed_Algorithms/01_Distributed_Consensus.md', '../../12_Blockchain_Crypto/', '../../04_Industrial_Scenarios/12_Blockchain_Crypto/'),
    ('knowledge/20_Industrial_Case_Studies/23_Cache_System.md', '../../11_In_Memory_Database/', '../../03_System_Technology_Domains/11_In_Memory_Database/'),
    ('knowledge/20_Industrial_Case_Studies/23_Cache_System.md', '../../16_OS_Kernel/', '../../03_System_Technology_Domains/16_OS_Kernel/'),
]

for path, old, new in fixes:
    if os.path.exists(path):
        with open(path, 'r', encoding='utf-8') as f:
            content = f.read()
        if old in content:
            content = content.replace(old, new)
            with open(path, 'w', encoding='utf-8') as f:
                f.write(content)
            print('Fixed: ' + path)
        else:
            print('Old not found: ' + path)
    else:
        print('File not found: ' + path)

# Create missing directories
dirs_to_create = [
    'knowledge/03_System_Technology_Domains/06_Security_Boot',
    'knowledge/03_System_Technology_Domains/07_Hardware_Security',
    'knowledge/03_System_Technology_Domains/15_Industrial_Communication',
    'knowledge/02_Formal_Semantics_and_Physics/13_Hardware_Description/code',
    'knowledge/06_Thinking_Representation/09_State_Machines/code',
]

for d in dirs_to_create:
    readme = os.path.join(d, 'README.md')
    if not os.path.exists(readme):
        os.makedirs(d, exist_ok=True)
        title = os.path.basename(d).replace('_', ' ').title()
        with open(readme, 'w', encoding='utf-8') as f:
            f.write('# ' + title + '\n\n> **状态**: 📝 待补充 (Stub)\n\n本文档目前为占位符。\n\n---\n\n*最后更新: 2026-05-10*\n')
        print('Created: ' + readme)
