/*
 * Auto-generated from: 01_Core_Knowledge_System\02_Core_Layer\03_String_Processing.md
 * Line: 303
 * Language: c
 * Block ID: 77558574
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// ❌ 严重安全漏洞
void log_message(const char *msg) {
    printf(msg);  // 如果msg包含%，崩溃或信息泄漏
}
// 攻击：msg = "%s%s%s" 读取任意内存

// ✅ 修复
void log_message_safe(const char *msg) {
    printf("%s", msg);  // 固定格式
}
