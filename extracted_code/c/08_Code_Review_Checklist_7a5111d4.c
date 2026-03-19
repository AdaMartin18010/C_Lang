/*
 * Auto-generated from: 01_Core_Knowledge_System\05_Engineering\08_Code_Review_Checklist.md
 * Line: 212
 * Language: c
 * Block ID: 7a5111d4
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// ❌ 错误示例：UAF漏洞
void process_data(char *data) {
    free(data);
    // 危险！data已被释放但仍被使用
    printf("Data: %s\n", data);  // UAF! 未定义行为
}

// ✅ 正确示例：释放后置空
void process_data_safe(char *data) {
    free(data);
    data = NULL;  // 置空指针
    // 即使误用也是安全的（会崩溃而非UAF）
    if (data != NULL) {
        printf("Data: %s\n", data);
    }
}

// ✅ 更好的设计：由调用者管理生命周期
void process_data_better(const char *data) {
    // 不拥有data，不释放
    printf("Data: %s\n", data);
}
