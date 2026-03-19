/*
 * Auto-generated from: 06_Thinking_Representation\02_Comparison_Matrices\01_Type_Storage_Matrix.md
 * Line: 194
 * Language: c
 * Block ID: df74c05e
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// platform_differences.c
#include <stdio.h>
#include <stdint.h>

// 字节序检测
int is_little_endian(void) {
    union {
        uint16_t val;
        uint8_t bytes[2];
    } test = {0x0102};
    return test.bytes[0] == 0x02;
}

// 对齐检测
typedef struct {
    char c;
    int i;
} test_align_t;

int main(void) {
    printf("=== 平台特性检测 ===\n");
    printf("字节序: %s\n", is_little_endian() ? "小端" : "大端");
    printf("char + int 结构体大小: %zu\n", sizeof(test_align_t));
    printf("预期对齐填充: %zu bytes\n", sizeof(test_align_t) - sizeof(char) - sizeof(int));

    return 0;
}
