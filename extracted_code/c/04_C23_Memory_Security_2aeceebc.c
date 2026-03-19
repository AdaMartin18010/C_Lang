/*
 * Auto-generated from: 01_Core_Knowledge_System\07_Modern_C\04_C23_Memory_Security.md
 * Line: 204
 * Language: c
 * Block ID: 2aeceebc
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 传统C (不安全)
void process_legacy(int *data, int n) {
    for (int i = 0; i <= n; i++) {  // 错误: 越界
        data[i] = i;
    }
}

// Checked C (安全)
void process_checked(array_ptr<int> data : count(n), int n) {
    for (int i = 0; i < n; i++) {
        data[i] = i;  // 自动边界检查
    }
    // data[n] = 0;  // 编译错误: 超出声明边界
}

// 安全字符串处理
void upper_case(nt_array_ptr<char> str : count(len), int len) {
    for (int i = 0; i < len && str[i] != '\0'; i++) {
        str[i] = toupper(str[i]);
    }
}
