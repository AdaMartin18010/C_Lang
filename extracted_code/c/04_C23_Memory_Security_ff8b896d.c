/*
 * Auto-generated from: 01_Core_Knowledge_System\07_Modern_C\04_C23_Memory_Security.md
 * Line: 56
 * Language: c
 * Block ID: ff8b896d
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 方案1: C23 <stdckdint.h> 检查运算
#include <stdckdint.h>

size_t total;
if (ckd_mul(&total, count, sizeof(Item))) {
    return NULL;  // 溢出保护
}
void *p = malloc(total);

// 方案2: 手动边界检查
static inline void safe_copy(char *dst, size_t dstsz,
                              const char *src) {
    if (dst == NULL || src == NULL) return;
    size_t srclen = strnlen(src, dstsz);
    if (srclen >= dstsz) {
        // 截断或错误处理
        dst[0] = '\0';
        return;
    }
    memcpy(dst, src, srclen + 1);
}
