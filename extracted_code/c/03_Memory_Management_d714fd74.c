/*
 * Auto-generated from: 14_Video_Tutorials\01_Video_Scripts\03_Memory_Management.md
 * Line: 334
 * Language: c
 * Block ID: d714fd74
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

void double_free() {
    int *p = malloc(100);
    free(p);
    free(p);  // 双重释放！堆损坏
}

// 安全模式
void safe_free(void **pp) {
    if (pp != NULL && *pp != NULL) {
        free(*pp);
        *pp = NULL;
    }
}

// 使用
safe_free((void**)&p);  // 第一次释放
safe_free((void**)&p);  // 安全，无操作
