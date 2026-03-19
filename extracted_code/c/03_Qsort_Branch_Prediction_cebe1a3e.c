/*
 * Auto-generated from: 05_Deep_Structure_MetaPhysics\06_Standard_Library_Physics\03_Qsort_Branch_Prediction.md
 * Line: 792
 * Language: c
 * Block ID: cebe1a3e
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 错误：假设sizeof(int)
struct LargeStruct {
    char data[1024];
};

void wrong_swap(void *a, void *b) {
    int tmp = *(int*)a;  // ❌ 只复制了4字节！
    *(int*)a = *(int*)b;
    *(int*)b = tmp;
}

// 正确：使用元素大小
void correct_swap(void *a, void *b, size_t size) {
    // 方法1：逐字节交换
    char *pa = a, *pb = b;
    while (size--) {
        char tmp = *pa;
        *pa++ = *pb;
        *pb++ = tmp;
    }

    // 方法2：使用临时缓冲区
    char *tmp = malloc(size);
    memcpy(tmp, a, size);
    memcpy(a, b, size);
    memcpy(b, tmp, size);
    free(tmp);
}
