/*
 * Auto-generated from: 00_VERSION_TRACKING\C2y_Feature_Previews.md
 * Line: 1197
 * Language: c
 * Block ID: adb3d14f
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 与nullptr配合的检查

void safe_function([[nonnull]] int* ptr) {
    // 编译器可以假设ptr非空
    *ptr = 42;
}

// 调用检查
int* p = get_pointer();
safe_function(p);  // 警告: 可能为null

if (p != nullptr) {
    safe_function(p);  // OK
}
