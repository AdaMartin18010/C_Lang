/*
 * Auto-generated from: 01_Core_Knowledge_System\02_Core_Layer\02_Memory_Management.md
 * Line: 1728
 * Language: c
 * Block ID: 41ccb7cb
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// ❌ 错误: 释放后使用
void uaf_example(void) {
    int *data = malloc(sizeof(int) * 10);
    for (int i = 0; i < 10; i++) data[i] = i;

    free(data);  // 释放

    printf("%d\n", data[0]);  // ❌ UAF! 已释放内存的读取
    data[5] = 100;           // ❌ UAF! 已释放内存的写入
}

// ❌ 错误: 双重引用导致的UAF
void uaf_alias(void) {
    int *ptr1 = malloc(sizeof(int));
    int *ptr2 = ptr1;  // 别名

    *ptr1 = 42;
    free(ptr1);  // ptr1和ptr2都变成悬挂指针

    *ptr2 = 100;  // ❌ UAF! 通过ptr2访问已释放内存
}

// ❌ 错误: 函数返回悬挂指针
int *get_data_bad(void) {
    int *data = malloc(sizeof(int) * 10);
    // ...
    free(data);  // ❌ 在函数内释放
    return data;  // 返回悬挂指针
}

// ✅ 正确: 释放后置NULL
void safe_free(void **pp) {
    if (pp && *pp) {
        free(*pp);
        *pp = NULL;  // 置NULL防止UAF
    }
}

// ✅ 正确: 明确所有权转移
void process_and_free(int *data) {
    // 处理数据...
    printf("%d\n", *data);
    free(data);  // 接收方负责释放
}

void caller_good(void) {
    int *data = malloc(sizeof(int));
    *data = 42;
    process_and_free(data);
    // data 现在是悬挂指针，但不再使用
}
