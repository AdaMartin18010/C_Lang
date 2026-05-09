#!/usr/bin/env python3
"""
C_Lang Knowledge Base Health Check
Comprehensive quality scanner for the knowledge base.
"""

import os
import re
import sys

KNOWLEDGE_DIR = 'knowledge'

class HealthChecker:
    def __init__(self):
        self.issues = []
        self.stats = {
            'total_files': 0,
            'total_lines': 0,
            'total_links': 0,
        }

    def add_issue(self, category, file, message, severity='warning'):
        self.issues.append({
            'category': category,
            'file': file,
            'message': message,
            'severity': severity,
        })

    def check_broken_links(self):
        link_re = re.compile(r'\[([^\]]+)\]\(([^)\s"]+)(?:\s+"[^"]*")?\)')
        
        math_patterns = [
            re.compile(r'^[\s*]*$'),
            re.compile(r'^[.]{3,}$'),
            re.compile(r'^[A-Z]$'),
            re.compile(r'^[a-z]+$'),
            re.compile(r'^[0-9]+$'),
            re.compile(r'^[a-zA-Z0-9_]+$'),
            re.compile(r'^[a-zA-Z0-9_]+\s*[:=]'),
            re.compile(r'^[Δλ∀∃∧∨¬→↦⊢⊨∈∉∪∩⊆⊇⊂⊃∅∞]$'),
            re.compile(r'^[\w\s]+→[\w\s]+$'),
            re.compile(r'^\w+\s*\(.*\)$'),
            re.compile(r'^R\['),
            re.compile(r'^\$\{'),
            re.compile(r'^\{\{'),
            re.compile(r'^[a-zA-Z0-9_]+[;+\-*/<>!&|]+[a-zA-Z0-9_]+$'),
            re.compile(r'^[a-zA-Z0-9_]+[;+\-*/<>!&|]+\s+[a-zA-Z0-9_]+$'),
            re.compile(r'^[Δλ∀∃∧∨¬→↦⊢⊨∈∉∪∩⊆⊇⊂⊃∅∞].*'),
            re.compile(r'^M[₀₁₂₃₄₅₆₇₈₉].*'),
            re.compile(r'^λ.*'),
            re.compile(r'^[a-zA-Z0-9_]+\s*\(.*\)$'),
            re.compile(r'^[\"\'].*'),
            re.compile(r'^WP\['),
            re.compile(r"^'[^']+'$"),
            re.compile(r'^if\s+'),
            re.compile(r'^[a-zA-Z_][a-zA-Z0-9_]*->[a-zA-Z_]'),
            re.compile(r'^\w+\.\w+'),
            re.compile(r'^\.{3}[\w.]+'),
            re.compile(r'^\w+\s+then\s+\w+\s+else\s+\w+'),
        ]

        broken_count = 0
        for root, dirs, files in os.walk(KNOWLEDGE_DIR):
            for name in files:
                if not name.endswith('.md'):
                    continue
                path = os.path.join(root, name)
                with open(path, 'r', encoding='utf-8') as f:
                    content = f.read()
                
                for m in link_re.finditer(content):
                    text = m.group(1)
                    link = m.group(2)
                    
                    if link.startswith('http://') or link.startswith('https://') or link.startswith('mailto:') or link.startswith('#'):
                        continue
                    link_lower = link.split('#')[0].lower()
                    if any(link_lower.endswith(ext) for ext in ['.html', '.htm', '.pdf', '.png', '.jpg', '.jpeg', '.gif', '.svg', '.mp4', '.webm']):
                        continue
                    if link.startswith('#'):
                        continue
                    if '{{' in text or '{{' in link or '${' in text or '${' in link:
                        continue
                    if text in ('text', 'url', 'alt', 'path', '{{资源名称}}', '{{URL}}'):
                        continue
                    if any(p.match(text.strip()) for p in math_patterns):
                        continue
                    
                    self.stats['total_links'] += 1
                    
                    link_no_anchor = link.split('#')[0]
                    is_dir_link = link_no_anchor.endswith('/')
                    
                    if link.startswith('/'):
                        target = os.path.normpath(os.path.join(KNOWLEDGE_DIR, link_no_anchor.lstrip('/')))
                    else:
                        target = os.path.normpath(os.path.join(root, link_no_anchor))
                    
                    if is_dir_link:
                        target_readme = os.path.join(target, 'README.md')
                        if os.path.exists(target_readme):
                            continue
                    
                    if not os.path.exists(target):
                        broken_count += 1
                        self.add_issue('broken_link', path, f'[{text}]({link})', 'error')
        
        return broken_count

    def check_unclosed_fences(self):
        unclosed_count = 0
        for root, dirs, files in os.walk(KNOWLEDGE_DIR):
            for name in files:
                if not name.endswith('.md'):
                    continue
                path = os.path.join(root, name)
                with open(path, 'r', encoding='utf-8') as f:
                    content = f.read()
                
                in_fence = False
                for line in content.split('\n'):
                    if line.startswith('```') or line.startswith('~~~'):
                        in_fence = not in_fence
                
                if in_fence:
                    unclosed_count += 1
                    self.add_issue('unclosed_fence', path, 'File ends inside a code fence', 'error')
        
        return unclosed_count

    def check_readme_case(self):
        readme_link_count = 0
        for root, dirs, files in os.walk(KNOWLEDGE_DIR):
            for name in files:
                if not name.endswith('.md'):
                    continue
                path = os.path.join(root, name)
                with open(path, 'r', encoding='utf-8') as f:
                    content = f.read()
                
                for m in re.finditer(r'\]\([^)]*readme\.md[^)]*\)', content):
                    readme_link_count += 1
                    self.add_issue('readme_case', path, f'Contains lowercase readme.md link: {m.group(0)}', 'warning')
        
        return readme_link_count

    def check_tiny_files(self):
        tiny_count = 0
        for root, dirs, files in os.walk(KNOWLEDGE_DIR):
            for name in files:
                if not name.endswith('.md'):
                    continue
                path = os.path.join(root, name)
                size = os.path.getsize(path)
                if size < 200:
                    tiny_count += 1
                    self.add_issue('tiny_file', path, f'File is only {size} bytes (likely a stub)', 'info')
        
        return tiny_count

    def check_trailing_whitespace(self):
        tws_count = 0
        for root, dirs, files in os.walk(KNOWLEDGE_DIR):
            for name in files:
                if not name.endswith('.md'):
                    continue
                path = os.path.join(root, name)
                with open(path, 'r', encoding='utf-8') as f:
                    lines = f.readlines()
                
                for i, line in enumerate(lines, 1):
                    if line.rstrip('\n').endswith(' '):
                        tws_count += 1
                        self.add_issue('trailing_whitespace', path, f'Line {i} has trailing spaces', 'info')
                        break  # Only report once per file
        
        return tws_count

    def run(self):
        print('=== C_Lang Knowledge Base Health Check ===\n')
        
        broken = self.check_broken_links()
        fences = self.check_unclosed_fences()
        readme = self.check_readme_case()
        tiny = self.check_tiny_files()
        tws = self.check_trailing_whitespace()
        
        errors = [i for i in self.issues if i['severity'] == 'error']
        warnings = [i for i in self.issues if i['severity'] == 'warning']
        infos = [i for i in self.issues if i['severity'] == 'info']
        
        print(f'Summary:')
        print(f'  Broken links: {broken}')
        print(f'  Unclosed fences: {fences}')
        print(f'  Lowercase readme.md links: {readme}')
        print(f'  Tiny files (< 200 bytes): {tiny}')
        print(f'  Files with trailing whitespace: {tws}')
        print(f'  Total issues: {len(errors)} errors, {len(warnings)} warnings, {len(infos)} info')
        
        if errors:
            print(f'\n❌ Errors ({len(errors)}):')
            for issue in errors[:20]:
                print(f"  [{issue['category']}] {issue['file']}: {issue['message']}")
            if len(errors) > 20:
                print(f'  ... and {len(errors) - 20} more')
        
        if warnings:
            print(f'\n⚠️  Warnings ({len(warnings)}):')
            for issue in warnings[:10]:
                print(f"  [{issue['category']}] {issue['file']}: {issue['message']}")
            if len(warnings) > 10:
                print(f'  ... and {len(warnings) - 10} more')
        
        if not errors and not warnings:
            print('\n✅ All clear! No errors or warnings.')
        
        return 1 if errors else 0

if __name__ == '__main__':
    sys.exit(HealthChecker().run())
