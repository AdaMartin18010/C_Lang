/*
 * Auto-generated from: 01_Core_Knowledge_System\01_Basic_Layer\04_Control_Flow.md
 * Line: 168
 * Language: c
 * Block ID: 5144a5bd
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 将最可能的情况放在前面（分支预测优化）
if (likely_condition) {      // 90%概率
    // 快速路径
} else if (other_condition) { // 9%概率
    // 次快速路径
} else {
    // 慢速路径（1%）
}

// C23 likely/unlikely 属性（如果可用）
#if __STDC_VERSION__ >= 202311L
    if (condition [[likely]]) { }
#endif
