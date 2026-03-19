/*
 * Auto-generated from: 01_Core_Knowledge_System\04_Standard_Library_Layer\01_C89_Library.md
 * Line: 276
 * Language: c
 * Block ID: 49077784
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// ❌ 绝对不要使用（已从C11移除）
char buf[100];
gets(buf);  // 无边界检查，已废弃

// ✅ 使用fgets
if (fgets(buf, sizeof(buf), stdin)) {
    // 移除换行符
    size_t len = strlen(buf);
    if (len > 0 && buf[len-1] == '\n') {
        buf[len-1] = '\0';
    }
}
