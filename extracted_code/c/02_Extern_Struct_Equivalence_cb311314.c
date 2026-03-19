/*
 * Auto-generated from: 08_Zig_C_Connection\01_Type_System_Mapping\02_Extern_Struct_Equivalence.md
 * Line: 887
 * Language: c
 * Block ID: cb311314
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// C 代码 - 柔性数组成员 (C99 起标准)
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// 柔性数组声明: 必须是最后一个成员，大小为 []
struct FlexibleArray {
    size_t len;
    int data[];  // 或 int data[0] (GCC 扩展)
};

// 内存布局:
// [ len (8 bytes) ][ data (0 bytes, 占位) ]
// 实际分配时:
// [ len ][ data[0] ][ data[1] ] ... [ data[n-1] ]

struct FlexibleArray* create_flexible(size_t n) {
    // 关键: 分配 sizeof(struct) + n * sizeof(element)
    struct FlexibleArray *fa = malloc(
        sizeof(struct FlexibleArray) + n * sizeof(int)
    );
    if (!fa) return NULL;

    fa->len = n;
    for (size_t i = 0; i < n; i++) {
        fa->data[i] = (int)(i * i);
    }
    return fa;
}

// 包含柔性数组的结构体数组注意事项
void demo_array_of_flexible() {
    // 危险: 柔性数组结构体不能作为数组元素
    // struct FlexibleArray arr[10];  // 未定义行为！

    // 安全: 使用指针数组
    struct FlexibleArray *arr[10];
    for (int i = 0; i < 10; i++) {
        arr[i] = create_flexible(i + 1);
    }

    // 清理
    for (int i = 0; i < 10; i++) {
        free(arr[i]);
    }
}

int main() {
    struct FlexibleArray *fa = create_flexible(5);
    if (fa) {
        printf("len = %zu\n", fa->len);
        for (size_t i = 0; i < fa->len; i++) {
            printf("data[%zu] = %d\n", i, fa->data[i]);
        }
        free(fa);
    }
    return 0;
}
