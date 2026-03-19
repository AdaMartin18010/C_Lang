/*
 * Auto-generated from: 01_Core_Knowledge_System\09_Safety_Standards\CERT_C_2024\README.md
 * Line: 308
 * Language: c
 * Block ID: 84e2d85c
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 不合规代码 - 格式字符串攻击
void log_message(const char* user_msg) {
    printf(user_msg);  // 如果 user_msg 包含 %s %n 等，危险!
}

// 合规代码
void log_message_safe(const char* user_msg) {
    printf("%s", user_msg);  // 使用固定格式字符串
}

// 更好：使用 fputs
void log_message_best(const char* user_msg) {
    fputs(user_msg, stdout);
}
