/*
 * Auto-generated from: 05_Deep_Structure_MetaPhysics\12_Self_Modifying_Code\README.md
 * Line: 423
 * Language: c
 * Block ID: 3a972beb
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 编译器辅助的CFI检查
// Clang: -fsanitize=cfi

// 间接调用检查
void safe_indirect_call(void *func, void *arg) {
    // 检查目标是否在合法函数地址表中
    if (!is_valid_function_pointer(func)) {
        abort();  // CFI违规
    }
    ((void(*)(void*))func)(arg);
}

// 影子栈 (Shadow Stack)
// 维护独立的返回地址栈，防止ROP
void function_prologue(void) {
    // 保存返回地址到影子栈
    shadow_stack[shadow_sp++] = __builtin_return_address(0);
}

void function_epilogue(void) {
    // 验证返回地址
    void *shadow_ret = shadow_stack[--shadow_sp];
    void *actual_ret = __builtin_return_address(0);
    if (shadow_ret != actual_ret) {
        abort();  // 栈被篡改
    }
}
