/*
 * Auto-generated from: 00_VERSION_TRACKING\C2y_New_Proposals_2026.md
 * Line: 346
 * Language: c
 * Block ID: bfe2cf50
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 示例：Named Loops的兼容性写法
#if __STDC_VERSION__ >= 202700L && __has_feature(named_loops)
    // 使用C2y Named Loops
    outer: for (...) { break outer; }
#else
    // Fallback: 使用goto
    for (...) { goto outer_done; }
    outer_done:
#endif
