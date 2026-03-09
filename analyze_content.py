import os
import re

# 遍历所有md文件
results = []
for root, dirs, files in os.walk('knowledge'):
    for file in files:
        if file.endswith('.md'):
            path = os.path.join(root, file)
            with open(path, 'r', encoding='utf-8', errors='ignore') as f:
                lines = f.readlines()
            
            total = len(lines)
            non_empty = len([l for l in lines if l.strip()])
            
            # 检测代码行（实质内容）
            code_lines = 0
            text_lines = 0
            in_code = False
            for line in lines:
                stripped = line.strip()
                if stripped.startswith('```'):
                    in_code = not in_code
                    continue
                if in_code and stripped:
                    code_lines += 1
                elif not in_code and stripped and not stripped.startswith(('#', '|', '-', '*', '!', '[', '>', '---')):
                    text_lines += 1
            
            content_lines = code_lines + text_lines
            ratio = (content_lines / non_empty * 100) if non_empty else 0
            
            results.append({
                'file': path.replace('knowledge/', ''),
                'total': total,
                'non_empty': non_empty,
                'content': content_lines,
                'code': code_lines,
                'ratio': round(ratio, 1)
            })

# 按实质内容比例排序
results.sort(key=lambda x: x['ratio'])

print('=== 实质内容比例最低的文件（可能内容不足）===')
print(f"{'文件路径':<60} {'总行':>6} {'非空':>6} {'实质':>6} {'比例':>6}")
print('-' * 90)
for r in results[:50]:
    print(f"{r['file']:<60} {r['total']:>6} {r['non_empty']:>6} {r['content']:>6} {r['ratio']:>5.1f}%")
