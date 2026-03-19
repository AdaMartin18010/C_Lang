/*
 * Auto-generated from: 01_Core_Knowledge_System\09_Safety_Standards\04_Secure_Coding_Guide.md
 * Line: 274
 * Language: c
 * Block ID: 6bce0de9
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 漏洞代码
void vulnerable_log(const char *user_input) {
    printf(user_input);  // ⚠️ 用户可控格式化字符串
    // 攻击者输入: "%p %p %p %s" 可泄露栈内存
    // 攻击者输入: "%n" 可写入内存
}
