/*
 * Auto-generated from: 15_Memory_Safe_Languages\01_Rust_vs_C\02_Ownership_vs_Pointers.md
 * Line: 217
 * Language: c
 * Block ID: ca6d43db
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// C: 缓冲区溢出漏洞
void process_input(const char* user_input) {
    char buffer[64];

    // 危险：没有长度检查
    strcpy(buffer, user_input);  // 可能导致栈溢出

    // "安全"版本仍需手动计算
    strncpy(buffer, user_input, sizeof(buffer) - 1);
    buffer[sizeof(buffer) - 1] = '\0';  // 仍然容易遗漏
}

// 数组索引同样危险
void access_array(int index) {
    int arr[10] = {0};
    // 无边界检查！index可以是任意值
    arr[index] = 42;  // 可能写入任意内存
}
