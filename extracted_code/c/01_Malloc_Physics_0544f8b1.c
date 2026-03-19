/*
 * Auto-generated from: 05_Deep_Structure_MetaPhysics\06_Standard_Library_Physics\01_Malloc_Physics.md
 * Line: 719
 * Language: c
 * Block ID: 0544f8b1
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 错误：双重释放
void double_free_bug(void) {
    char *buf = malloc(100);
    free(buf);
    // ... 其他代码 ...
    free(buf);  // ❌ 双重释放！
}

// 正确：释放后置空
void correct_free(void) {
    char *buf = malloc(100);
    free(buf);
    buf = NULL;  // ✅

    // free(NULL) 是安全的
    free(buf);  // 无操作
}
