/*
 * Auto-generated from: 01_Core_Knowledge_System\07_Modern_C\03_C23_Core_Features.md
 * Line: 151
 * Language: c
 * Block ID: da22a963
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 1. 函数指针初始化
void (*callback)(int) = nullptr;  // 明确为空

// 2. 条件判断
void process_data(void* buffer) {
    if (buffer == nullptr) {
        // 明确处理空指针情况
        return;
    }
    // ...
}

// 3. 与 Zig 的对应
// C23:  void* p = nullptr;
// Zig: var p: ?*anyopaque = null;
