/*
 * Auto-generated from: 01_Core_Knowledge_System\08_Application_Domains\03_Infrastructure_Software.md
 * Line: 832
 * Language: c
 * Block ID: ebc5a8dd
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 错误：检查-使用竞态
void bad_check(const char* path) {
    if (access(path, W_OK) == 0) {  // TOCTOU
        FILE* fp = fopen(path, "w");  // 可能已被修改!
    }
}

// 正确：原子操作
void good_check(const char* path) {
    int fd = open(path, O_WRONLY | O_CREAT | O_EXCL, 0644);
    if (fd < 0) {
        // 处理错误
    }
    // 安全使用fd
}
