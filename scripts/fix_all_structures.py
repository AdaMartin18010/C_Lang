#!/usr/bin/env python3
"""
文档结构批量修复工具
修复所有H1标题过多的文档
"""

import re
import json
from pathlib import Path
from datetime import datetime

def fix_document_structure(filepath):
    """修复文档的H1结构问题"""
    content = Path(filepath).read_text(encoding='utf-8')
    lines = content.split('\n')
    
    main_title = None
    fixed_lines = []
    in_code_block = False
    code_fence = ''
    
    for i, line in enumerate(lines):
        # 检测代码块
        fence_match = re.match(r'^(```|~~~)(\w*)$', line)
        if fence_match:
            if not in_code_block:
                in_code_block = True
                code_fence = fence_match.group(1)
            elif line.strip().startswith(code_fence):
                in_code_block = False
                code_fence = ''
            fixed_lines.append(line)
            continue
        
        # 在代码块内不处理
        if in_code_block:
            fixed_lines.append(line)
            continue
        
        # 检测H1标题
        h1_match = re.match(r'^(# )(.+)$', line)
        if h1_match:
            title = h1_match.group(2)
            
            # 第一个H1是主标题，保留
            if main_title is None:
                main_title = title
                fixed_lines.append(line)
                continue
            
            # 根据内容智能判断层级
            # 数字开头的主要章节 → H2
            if re.match(r'^\d+\.', title):
                fixed_lines.append(f"## {title}")
            # FAQ问题 → H3
            elif title.startswith('Q') and ':' in title:
                fixed_lines.append(f"### {title}")
            # 特定关键词 → H2
            elif any(kw in title for kw in ['概述', '简介', '介绍', '基础', '总结', '附录', 
                                               '深入理解', '核心原理', '实践应用', '最佳实践',
                                               'Overview', 'Introduction', 'Guide', 'Tutorial',
                                               'Summary', 'Appendix', 'Reference', 'Conclusion']):
                fixed_lines.append(f"## {title}")
            # 包含"安装"、"配置"等 → H3
            elif any(kw in title for kw in ['安装', '配置', '使用', '示例', '实现', '详解']):
                fixed_lines.append(f"### {title}")
            # 默认 → H3
            else:
                fixed_lines.append(f"### {title}")
        else:
            fixed_lines.append(line)
    
    Path(filepath).write_text('\n'.join(fixed_lines), encoding='utf-8')
    
    # 统计修复后的H1数量
    new_content = '\n'.join(fixed_lines)
    h1_count = len(re.findall(r'^# [^#]', new_content, re.MULTILINE))
    return h1_count


def add_metadata_to_doc(filepath):
    """为文档添加标准元数据"""
    try:
        content = Path(filepath).read_text(encoding='utf-8')
    except:
        return False
    
    # 检查是否已有元数据
    first_500 = content[:500]
    if any(marker in first_500 for marker in ['版本:', 'Version:', '最后更新:', 'Last Updated:', 
                                                '作者:', 'Author:', '难度:', 'Difficulty:']):
        return False
    
    lines = content.split('\n')
    if len(lines) < 1:
        return False
    
    # 找到第一个标题后的位置
    insert_pos = 1
    while insert_pos < len(lines) and lines[insert_pos].strip() == '':
        insert_pos += 1
    
    # 推断难度
    difficulty = 'L3 进阶'
    content_lower = content.lower()
    if any(kw in content_lower for kw in ['入门', '基础', 'basic', 'beginner']):
        difficulty = 'L2 基础'
    elif any(kw in content_lower for kw in ['高级', '深入', 'advanced', 'expert']):
        difficulty = 'L4 高级'
    
    metadata = [
        "",
        "> **版本**: v1.0.0",
        "> **最后更新**: 2026-03-25",
        "> **作者**: C_Lang Team",
        f"> **难度**: {difficulty}",
        "",
        "---",
    ]
    
    new_lines = lines[:insert_pos] + metadata + lines[insert_pos:]
    Path(filepath).write_text('\n'.join(new_lines), encoding='utf-8')
    return True


def main():
    knowledge_path = Path('knowledge')
    
    # 所有需要修复的文件列表
    files_to_fix = [
        # Critical (200+ H1)
        '02_Formal_Semantics_and_Physics/16_Performance_Analysis_Hardware/02_Linux_Perf_Tutorial.md',
        '05_Deep_Structure_MetaPhysics/08_Debugging_Tools/03_Advanced_Debugging_Techniques.md',
        '10_WebAssembly_C/02_Emscripten_Advanced_Usage.md',
        
        # High (100-200 H1)
        '02_Formal_Semantics_and_Physics/19_Hardware_Debugging/01_JTAG_Debug.md',
        '01_Core_Knowledge_System/09_Safety_Standards/MISRA_C_2023/tools/01_MISRA_Toolchain_Guide.md',
        '10_WebAssembly_C/01_WebAssembly_Introduction_C.md',
        '07_Modern_Toolchain/05_Code_Quality_Toolchain/01_clang_format_Setup.md',
        '07_Modern_Toolchain/02_Build_Systems_Modern/03_Xmake_Guide.md',
        '01_Core_Knowledge_System/09_Safety_Standards/MISRA_C_2023/tools/02_Helix_QAC_Guide.md',
        '01_Core_Knowledge_System/09_Safety_Standards/04_Secure_Coding_Guide.md',
        '00_VERSION_TRACKING/GCC_15_C23_Default.md',
        
        # Medium (50-100 H1)
        '07_Modern_Toolchain/07_Performance_Tracing.md',
        '00_VERSION_TRACKING/LLVM_22_C23_Update.md',
        '02_Formal_Semantics_and_Physics/11_CompCert_Verification/04_Frama_C_2024_Guide.md',
        '02_Formal_Semantics_and_Physics/11_CompCert_Verification/05_Eva_Tutorial.md',
        '07_Modern_Toolchain/05_Case_Studies/04_Linux_Kernel_Toolchain.md',
        '04_Industrial_Scenarios/14_Embedded_IoT/ESP32/01_ESP32_Complete_Guide.md',
        '10_WebAssembly_C/04_WASM_Performance_Optimization.md',
        '07_Modern_Toolchain/03_CI_CD_DevOps/02_Docker_C_Development.md',
        '07_Modern_Toolchain/02_Build_Systems_Modern/04_Bazel_C_Projects.md',
        '02_Formal_Semantics_and_Physics/13_Hardware_Description/04_HLS_with_C/01_Vivado_HLS_Introduction.md',
        '03_System_Technology_Domains/02_Cbindgen_Usage.md',
        '07_Modern_Toolchain/06_Dynamic_Analysis_Tools.md',
        '04_Industrial_Scenarios/14_Embedded_IoT/RaspberryPi/01_RaspberryPi_Embedded_Linux.md',
        '07_Modern_Toolchain/02_Build_Systems_Modern/02_Meson_Build.md',
        '07_Modern_Toolchain/02_Build_Systems_Modern/README.md',
        '04_Industrial_Scenarios/14_Embedded_IoT/Baremetal/01_Baremetal_Programming.md',
        '01_Core_Knowledge_System/05_Engineering/01_Build_System/README.md',
    ]
    
    fixed_files = []
    print("🔧 开始批量修复文档结构...")
    
    for rel_path in files_to_fix:
        filepath = knowledge_path / rel_path
        if not filepath.exists():
            print(f"  ⚠️  文件不存在: {rel_path}")
            continue
        
        # 获取原始H1数量
        original_content = filepath.read_text(encoding='utf-8')
        original_h1 = len(re.findall(r'^# [^#]', original_content, re.MULTILINE))
        
        if original_h1 <= 1:
            print(f"  ✓ 无需修复: {rel_path} (H1={original_h1})")
            continue
        
        print(f"  修复: {rel_path} (H1={original_h1})")
        
        # 修复结构
        new_h1 = fix_document_structure(filepath)
        
        # 添加元数据
        has_metadata = add_metadata_to_doc(filepath)
        
        fixed_files.append({
            'file': rel_path,
            'original_h1': original_h1,
            'new_h1': new_h1,
            'metadata_added': has_metadata
        })
    
    print(f"\n✅ 完成修复: {len(fixed_files)} 个文件")
    
    # 保存修复记录
    fix_record = {
        'fix_date': datetime.now().strftime('%Y-%m-%d %H:%M:%S'),
        'fixed_files': fixed_files,
        'total_fixed': len(fixed_files),
        'total_h1_reduced': sum(f['original_h1'] - f['new_h1'] for f in fixed_files)
    }
    
    record_path = knowledge_path / '00_VERSION_TRACKING/structure_fix_record.json'
    record_path.write_text(json.dumps(fix_record, indent=2, ensure_ascii=False), encoding='utf-8')
    
    print(f"📊 H1总数减少: {fix_record['total_h1_reduced']} 个")
    print(f"📝 修复记录: {record_path}")
    
    return fixed_files


if __name__ == '__main__':
    main()
