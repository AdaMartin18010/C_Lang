import os, re

link_re = re.compile(r'\[([^\]]+)\]\(([^)\s"]+)(?:\s+"[^"]*")?\)')

files_modified = 0
fixes = 0

for root, dirs, files in os.walk('knowledge'):
    for name in files:
        if not name.endswith('.md'):
            continue
        path = os.path.join(root, name)
        with open(path, 'r', encoding='utf-8') as f:
            content = f.read()
        
        original = content
        
        for match in link_re.finditer(content):
            text = match.group(1)
            link = match.group(2)
            
            if link.startswith('http') or link.startswith('mailto:') or link.startswith('#'):
                continue
            
            old_link = link
            new_link = link
            
            # Fix case issue first
            if 'readme.md' in new_link:
                test_link = new_link.replace('readme.md', 'README.md')
                if not link.startswith('/'):
                    test_target = os.path.normpath(os.path.join(root, test_link))
                    if os.path.exists(test_target.split('#')[0]):
                        new_link = test_link
            
            # Check if still broken
            if link.startswith('/'):
                target = os.path.normpath(os.path.join('knowledge', link.lstrip('/')))
            else:
                target = os.path.normpath(os.path.join(root, new_link))
            target = target.split('#')[0]
            
            if os.path.exists(target):
                continue
            
            # Try to find target by searching knowledge/
            # Handle directory links (ending with /)
            search_link = new_link
            if search_link.endswith('/'):
                search_candidates = [search_link + 'README.md', search_link + 'readme.md']
            else:
                search_candidates = [search_link]
            
            corrected = False
            for cand in search_candidates:
                parts = cand.replace('\\', '/').split('/')
                # Try finding under knowledge/
                for i, part in enumerate(parts):
                    if part in ('knowledge', '01_Core_Knowledge_System', '02_Formal_Semantics_and_Physics',
                                '03_System_Technology_Domains', '04_Industrial_Scenarios',
                                '05_Deep_Structure_MetaPhysics', '06_Thinking_Representation',
                                '07_Modern_Toolchain', '08_Zig_C_Connection', '09_Data_Structures_Algorithms',
                                '10_WebAssembly_C', '11_Machine_Learning_C', '12_Practice_Exercises',
                                '13_Interactive_Tools', '14_Video_Tutorials', '15_Memory_Safe_Languages',
                                '16_Embedded_AI_Agents', '17_Ada_SPARK', '18_Formal_Verification_Tools',
                                '19_Real_Time_Systems', '20_Industrial_Case_Studies', '21_Computation_Theory',
                                '22_Distributed_Systems', '23_Embedded_Linux', '24_Complete_Chain',
                                'Zig', '00_VERSION_TRACKING', '00_GLOBAL_INDEX.md'):
                        suffix = '/'.join(parts[i:])
                        correct_target = os.path.normpath(os.path.join('knowledge', suffix))
                        if os.path.exists(correct_target):
                            correct_rel = os.path.relpath(correct_target, root).replace('\\', '/')
                            correct_rel = correct_rel.replace('readme.md', 'README.md')
                            if correct_rel != old_link:
                                new_link = correct_rel
                                corrected = True
                            break
                if corrected:
                    break
            
            # Special: handle links like 06_Learning_Paths/readme.md when file is in 06_Thinking_Representation/
            if not corrected:
                for subdir in ['06_Learning_Paths', '05_Concept_Mappings', '01_Decision_Trees',
                               '02_Comparison_Matrices', '03_Mind_Maps', '04_Case_Studies',
                               '07_Knowledge_Graph', '08_Index', '09_Bloom_Taxonomy',
                               '09_State_Machines', '10_Concept_Maps']:
                    if subdir in new_link:
                        parts = new_link.replace('\\', '/').split('/')
                        try:
                            idx = parts.index(subdir)
                            suffix = '/'.join(parts[idx:])
                            correct_target = os.path.normpath(os.path.join('knowledge/06_Thinking_Representation', suffix))
                            if os.path.exists(correct_target):
                                correct_rel = os.path.relpath(correct_target, root).replace('\\', '/')
                                correct_rel = correct_rel.replace('readme.md', 'README.md')
                                if correct_rel != old_link:
                                    new_link = correct_rel
                                    corrected = True
                                break
                        except ValueError:
                            pass
            
            if new_link != old_link:
                old_pattern = f'[{text}]({old_link})'
                new_pattern = f'[{text}]({new_link})'
                content = content.replace(old_pattern, new_pattern)
                fixes += 1
        
        if content != original:
            with open(path, 'w', encoding='utf-8') as f:
                f.write(content)
            files_modified += 1

print(f'Files modified: {files_modified}')
print(f'Links fixed: {fixes}')
