/*
 * Auto-generated from: 01_Core_Knowledge_System\04_Standard_Library_Layer\C23_Standard_Library\02_stdckdint_h_Complete_Reference.md
 * Line: 264
 * Language: c
 * Block ID: b8bbf8e1
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 案例 1: 安全数组分配
#include <stdckdint.h>
#include <stdlib.h>
#include <stdio.h>

void* safe_malloc_array(size_t nmemb, size_t size) {
    size_t total;
    if (ckd_mul(&total, nmemb, size)) {
        fprintf(stderr, "Array size overflow\n");
        return NULL;
    }
    return malloc(total);
}

// 案例 2: 金融计算
#include <stdckdint.h>
#include <stdint.h>

typedef struct {
    int64_t cents;
} Money;

bool money_add(Money* result, Money a, Money b) {
    return ckd_add(&result->cents, a.cents, b.cents);
}

// 案例 3: 图像处理 (尺寸计算)
#include <stdckdint.h>
#include <stdbool.h>

typedef struct {
    int width, height;
} ImageSize;

bool image_buffer_size(ImageSize size, int channels, size_t* out_size) {
    size_t pixel_count;
    if (ckd_mul(&pixel_count, size.width, size.height)) {
        return false;  // 溢出
    }
    if (ckd_mul(out_size, pixel_count, channels)) {
        return false;  // 溢出
    }
    return true;
}

// 案例 4: 网络协议 (序列号计算)
#include <stdckdint.h>
#include <stdint.h>

uint32_t safe_sequence_add(uint32_t seq, uint32_t offset) {
    uint32_t result;
    // 无符号溢出在序列号中是预期的
    ckd_add(&result, seq, offset);
    return result;
}
