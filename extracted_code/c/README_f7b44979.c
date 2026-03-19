/*
 * Auto-generated from: 01_Core_Knowledge_System\09_Safety_Standards\ISO_IEC_TS_17961\README.md
 * Line: 335
 * Language: c
 * Block ID: f7b44979
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* 同时满足 MISRA C:2025 和 ISO/IEC TS 17961 的代码示例 */

/* MISRA Rule 21.3: 不得使用 malloc/free (若适用)
 * ISO 17961 5.22: 避免重复释放
 */

#include <stddef.h>
#include <stdint.h>

/* 合规的安全内存管理封装 */
typedef struct {
    void* data;
    size_t size;
    uint32_t magic;  /* 完整性校验 */
} safe_buffer_t;

#define BUFFER_MAGIC 0x42464642U

bool buffer_init(safe_buffer_t* buf, size_t size) {
    if (buf == NULL || size == 0U) {
        return false;
    }

    buf->data = NULL;
    buf->size = 0U;
    buf->magic = 0U;

    /* 使用静态分析器验证的分配 */
    void* ptr = /* 分配实现 */ NULL;
    if (ptr == NULL) {
        return false;
    }

    buf->data = ptr;
    buf->size = size;
    buf->magic = BUFFER_MAGIC;
    return true;
}

void buffer_free(safe_buffer_t* buf) {
    if (buf == NULL || buf->magic != BUFFER_MAGIC) {
        return;
    }

    /* 单次释放保证 */
    if (buf->data != NULL) {
        /* 释放实现 */
        buf->data = NULL;
    }

    buf->size = 0U;
    buf->magic = 0U;  /* 清除魔数 */
}
