/*
 * Auto-generated from: 01_Core_Knowledge_System\09_Safety_Standards\04_Secure_Coding_Guide.md
 * Line: 107
 * Language: c
 * Block ID: 76afb168
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 漏洞代码：栈缓冲区溢出
void vulnerable_function(char *input) {
    char buffer[64];
    strcpy(buffer, input);  // ⚠️ 无长度检查
    // 攻击者输入65+字节可覆盖返回地址
}
