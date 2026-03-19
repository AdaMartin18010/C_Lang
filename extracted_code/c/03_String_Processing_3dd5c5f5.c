/*
 * Auto-generated from: 01_Core_Knowledge_System\02_Core_Layer\03_String_Processing.md
 * Line: 285
 * Language: c
 * Block ID: 3dd5c5f5
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// ❌ 截断不报告
char buf[10];
strncpy(buf, "very long string", sizeof(buf));
// buf现在是"very long"（无null终止）或截断
// 没有错误指示

// ✅ 检查截断
size_t needed = strlen(src) + 1;
if (needed > sizeof(buf)) {
    // 处理截断
    return ERROR_TRUNCATED;
}
strcpy(buf, src);
