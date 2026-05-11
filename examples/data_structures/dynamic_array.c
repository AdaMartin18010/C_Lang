/*
 * 动态数组（类似C++ vector）示例
 * 编译: gcc -Wall -Wextra -O2 -o dynamic_array dynamic_array.c
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#define DA_INIT_CAP 4

#define da_append(da, item) do { \
    if ((da)->count >= (da)->capacity) { \
        (da)->capacity = (da)->capacity ? (da)->capacity * 2 : DA_INIT_CAP; \
        (da)->items = realloc((da)->items, (da)->capacity * sizeof(*(da)->items)); \
    } \
    (da)->items[(da)->count++] = (item); \
} while (0)

#define da_free(da) do { free((da)->items); (da)->items = NULL; \
    (da)->count = (da)->capacity = 0; } while (0)

typedef struct {
    int *items;
    size_t count;
    size_t capacity;
} IntArray;

typedef struct {
    const char **items;
    size_t count;
    size_t capacity;
} StrArray;

int main(void) {
    IntArray nums = {0};
    for (int i = 0; i < 20; i++) da_append(&nums, i * i);
    
    printf("IntArray: count=%zu, cap=%zu\n", nums.count, nums.capacity);
    for (size_t i = 0; i < nums.count; i++) {
        printf("%d ", nums.items[i]);
    }
    printf("\n");
    da_free(&nums);
    
    StrArray words = {0};
    da_append(&words, "hello");
    da_append(&words, "world");
    da_append(&words, "dynamic");
    da_append(&words, "array");
    
    printf("StrArray: ");
    for (size_t i = 0; i < words.count; i++) {
        printf("%s ", words.items[i]);
    }
    printf("\n");
    da_free(&words);
    
    return 0;
}
