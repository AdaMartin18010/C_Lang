/*
 * Auto-generated from: 01_Core_Knowledge_System\07_Modern_C\04_C23_Memory_Security.md
 * Line: 244
 * Language: c
 * Block ID: b732c3d3
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// ✅ 安全模式
void *safe_malloc(size_t nmemb, size_t size) {
    if (nmemb == 0 || size == 0) return NULL;

    // C23检查溢出
    size_t total;
    if (ckd_mul(&total, nmemb, size)) {
        errno = ENOMEM;
        return NULL;
    }

    void *p = malloc(total);
    if (p == NULL) return NULL;

    return p;
}

// 使用
Item *arr = safe_malloc(count, sizeof(Item));
if (arr == NULL) handle_error();
