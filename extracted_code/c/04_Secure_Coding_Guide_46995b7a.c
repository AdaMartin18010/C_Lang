/*
 * Auto-generated from: 01_Core_Knowledge_System\09_Safety_Standards\04_Secure_Coding_Guide.md
 * Line: 40
 * Language: c
 * Block ID: 46995b7a
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/**
 * C语言固有风险分析
 */

// 风险1: 无边界检查
void unsafe_buffer(void) {
    char buf[10];
    gets(buf);  // ⚠️ 极度危险，已移除C11标准
}

// 风险2: 手动内存管理
void unsafe_memory(void) {
    char *p = malloc(100);
    free(p);
    free(p);  // ⚠️ 双重释放
}

// 风险3: 未定义行为
void undefined_behavior(void) {
    int a[10];
    int x = a[20];  // ⚠️ 未定义行为，可能泄露信息
}

// 风险4: 整数溢出
void integer_overflow(void) {
    size_t len = 0xFFFFFFFF;
    char *buf = malloc(len + 1);  // ⚠️ 溢出后分配过小缓冲区
}
