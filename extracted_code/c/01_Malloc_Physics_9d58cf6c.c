/*
 * Auto-generated from: 05_Deep_Structure_MetaPhysics\06_Standard_Library_Physics\01_Malloc_Physics.md
 * Line: 681
 * Language: c
 * Block ID: 9d58cf6c
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 错误：未匹配分配和释放
void wrong_allocation_pattern(void) {
    char *buf = malloc(100);
    if (condition) {
        return;  // ❌ 泄漏！
    }
    free(buf);
}

// 正确：使用RAII或确保释放
void correct_allocation_pattern(void) {
    char *buf = malloc(100);
    if (!buf) return;

    if (condition) {
        free(buf);  // ✅
        return;
    }

    free(buf);
}

// 更好的方式：使用__attribute__((cleanup))
void auto_free(void *p) { free(*(void**)p); }

#define AUTO __attribute__((cleanup(auto_free)))

void improved_pattern(void) {
    AUTO char *buf = malloc(100);
    if (!buf) return;

    // buf会自动释放
}
