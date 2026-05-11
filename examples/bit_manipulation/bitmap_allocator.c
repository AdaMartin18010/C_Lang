/*
 * 位图内存分配器示例
 * 编译: gcc -Wall -Wextra -O2 -o bitmap_allocator bitmap_allocator.c
 */
#include <stdio.h>
#include <stdint.h>
#include <string.h>

#define BITMAP_BITS 1024
#define WORD_BITS 64

static uint64_t bitmap[BITMAP_BITS / WORD_BITS];

int bitmap_alloc(void) {
    for (int i = 0; i < BITMAP_BITS / WORD_BITS; i++) {
        if (bitmap[i] != ~0ULL) {
            int bit = __builtin_ctzll(~bitmap[i]);
            bitmap[i] |= (1ULL << bit);
            return i * WORD_BITS + bit;
        }
    }
    return -1;  /* 无空闲位 */
}

void bitmap_free(int index) {
    if (index < 0 || index >= BITMAP_BITS) return;
    int word = index / WORD_BITS;
    int bit = index % WORD_BITS;
    bitmap[word] &= ~(1ULL << bit);
}

int bitmap_check(int index) {
    if (index < 0 || index >= BITMAP_BITS) return 0;
    int word = index / WORD_BITS;
    int bit = index % WORD_BITS;
    return (bitmap[word] >> bit) & 1U;
}

int main(void) {
    memset(bitmap, 0, sizeof(bitmap));
    
    /* 分配10个位 */
    int handles[10];
    for (int i = 0; i < 10; i++) {
        handles[i] = bitmap_alloc();
        printf("Allocated bit %d\n", handles[i]);
    }
    
    /* 释放偶数位 */
    for (int i = 0; i < 10; i += 2) {
        bitmap_free(handles[i]);
        printf("Freed bit %d (check=%d)\n", handles[i], bitmap_check(handles[i]));
    }
    
    /* 再次分配，应复用释放的位 */
    int new_bit = bitmap_alloc();
    printf("New allocation: %d (should be 0, 2, 4, 6, or 8)\n", new_bit);
    
    return 0;
}
