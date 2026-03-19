/*
 * Auto-generated from: 01_Core_Knowledge_System\09_Safety_Standards\CERT_C_2024\README.md
 * Line: 120
 * Language: c
 * Block ID: b0cf09c5
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 不合规代码 - Use-After-Free
void process_data(char* data) {
    free(data);
    // 错误: 访问已释放内存
    if (data[0] == 'A') {  // UAF 漏洞!
        // ...
    }
}

// 合规代码
void process_data_safe(char* data) {
    if (data[0] == 'A') {  // 先访问，后释放
        // ...
    }
    free(data);
    data = NULL;  // 置空，避免悬挂指针
}
