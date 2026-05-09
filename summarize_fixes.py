import json
with open('e:\\_src\\C_Lang\\fix_log.json', 'r', encoding='utf-8') as f:
    data = json.load(f)

print('Total fixed files:', len(data['fixed']))
print('Total needs rewrite:', len(data['needs_rewrite']))

fix_counts = {}
for entry in data['fixed']:
    for change in entry.get('changes', []):
        base_type = change.split('_x')[0]
        fix_counts[base_type] = fix_counts.get(base_type, 0) + 1

print('\nFix type counts:')
for k, v in sorted(fix_counts.items(), key=lambda x: -x[1]):
    print('  ' + k + ': ' + str(v))

print('\nNeeds rewrite files:')
for entry in data['needs_rewrite']:
    print('  ' + entry['file'] + ': ' + str(entry['problems']) + ' problems')
