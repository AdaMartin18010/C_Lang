/*
 * Auto-generated from: 01_Core_Knowledge_System\07_Modern_C\C2y_Countof_Depth.md
 * Line: 1507
 * Language: c
 * Block ID: 46e5e3f0
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

void test(void) {
    int arr[100];

    // 以下两行生成完全相同的机器码
    size_t a = _Countof(arr);
    size_t b = 100;  // 编译期常量
}
