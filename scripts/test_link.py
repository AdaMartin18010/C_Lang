import os, re

link_re = re.compile(r'\[([^\]]+)\]\(([^)\s"]+)(?:\s+"[^"]*")?\)')

root = r'knowledge\02_Formal_Semantics_and_Physics\10_ISA_Machine_Code\08_RISCV_CPU_Complete'
path = r'knowledge\02_Formal_Semantics_and_Physics\10_ISA_Machine_Code\08_RISCV_CPU_Complete\01_RV32I_Single_Cycle.md'

with open(path, 'r', encoding='utf-8') as f:
    content = f.read()

for match in link_re.finditer(content):
    text = match.group(1)
    link = match.group(2)
    if '01_Core_Knowledge_System' in link:
        target = os.path.normpath(os.path.join(root, link))
        exists = os.path.exists(target)
        print(f'[{text}]({link})')
        print(f'  -> {target}')
        print(f'  exists: {exists}')
