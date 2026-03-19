/*
 * Auto-generated from: 01_Core_Knowledge_System\09_Safety_Standards\ISO_IEC_TS_17961\README.md
 * Line: 245
 * Language: c
 * Block ID: cf1789c4
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 示例: 规则 5.21 - 越界指针或数组下标
void array_access(void) {
    int arr[10];
    int* ptr = &arr[10];      // 分析器必须诊断: 越界指针
    int x = arr[10];          // 分析器必须诊断: 越界访问
}

// 示例: 规则 5.37 - 对指针使用 sizeof 而非所指类型
void buffer_alloc(void) {
    int* p = malloc(10 * sizeof(p));  // 错误: sizeof(int*) 而非 sizeof(int)
    // 分析器必须诊断: 可能分配不足
}
