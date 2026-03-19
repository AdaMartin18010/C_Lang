/*
 * Auto-generated from: 01_Core_Knowledge_System\07_Modern_C\C2y_Countof_Depth.md
 * Line: 1238
 * Language: c
 * Block ID: 30d09a31
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

int arr[10];

// 场景1：需要知道元素个数
for (size_t i = 0; i < _Countof(arr); i++) {  // 清晰
    // ...
}

// 等价但冗长的写法
for (size_t i = 0; i < sizeof(arr) / sizeof(arr[0]); i++) {  // 繁琐
    // ...
}

// 场景2：需要知道内存大小
memcpy(dst, src, sizeof(src));  // 正确

// 场景3：需要知道元素大小
size_t elem_size = sizeof(arr[0]);  // 用于泛型编程
