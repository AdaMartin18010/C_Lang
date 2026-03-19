/*
 * Auto-generated from: 01_Core_Knowledge_System\07_Modern_C\C2y_Defer_Depth.md
 * Line: 2172
 * Language: c
 * Block ID: 2c3a8498
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

void exception_path_example(void) {
    void *r1 = acquire_resource1();
    defer release_resource1(r1);

    void *r2 = acquire_resource2();
    defer release_resource2(r2);

    void *r3 = acquire_resource3();
    defer release_resource3(r3);

    if (error_condition()) {
        return;  // 触发栈展开
    }
}

// 异常路径的栈展开：
// 1. 执行 defer release_resource3(r3)
// 2. 执行 defer release_resource2(r2)
// 3. 执行 defer release_resource1(r1)
// 4. 函数返回
