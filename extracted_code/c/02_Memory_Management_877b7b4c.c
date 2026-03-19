/*
 * Auto-generated from: 01_Core_Knowledge_System\02_Core_Layer\02_Memory_Management.md
 * Line: 2234
 * Language: c
 * Block ID: 877b7b4c
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// ❌ 错误: malloc后用delete（C++）或反之
void mismatch_malloc_delete(void) {
    int *p = (int*)malloc(sizeof(int));
    delete p;  // ❌ C++中不匹配，C中无法编译
}

void mismatch_new_free(void) {
    int *p = new int;  // C++
    free(p);  // ❌ 不匹配
}

// ❌ 错误: 数组分配与非数组释放
typedef struct {
    int data[100];
} BigStruct;

void mismatch_array(void) {
    BigStruct *arr = malloc(10 * sizeof(BigStruct));
    // 某些C++混合代码可能出错
    delete arr;  // 应该是 delete[] 或 free
}

// ❌ 错误: 标量分配与数组释放
void mismatch_scalar(void) {
    int *p = (int*)malloc(sizeof(int));
    delete[] p;  // ❌ 不匹配
}

// ✅ 正确: C代码中保持malloc/free配对
void match_correct(void) {
    // C风格（推荐在C代码中）
    int *p1 = malloc(sizeof(int));
    free(p1);

    int *arr = calloc(10, sizeof(int));
    free(arr);

    int *aligned = aligned_alloc(64, 1024);
    free(aligned);
}
