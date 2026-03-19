/*
 * Auto-generated from: 05_Deep_Structure_MetaPhysics\13_Self_Modifying_Code\README.md
 * Line: 721
 * Language: c
 * Block ID: f9a5a528
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 控制流完整性保护
// 确保间接调用只能跳转到合法的目标

// 影子栈（Shadow Stack）
__thread void* shadow_stack[1024];
__thread int shadow_sp = 0;

// 函数序言/尾声插入的 CFI 检查
void cfi_function_prologue(void* return_addr) {
    // 将返回地址保存到影子栈
    shadow_stack[shadow_sp++] = return_addr;
}

void* cfi_function_epilogue(void* actual_return_addr) {
    // 从影子栈弹出预期返回地址
    void* expected = shadow_stack[--shadow_sp];

    // 检查匹配
    if (expected != actual_return_addr) {
        // CFI 违规！
        abort();
    }

    return actual_return_addr;
}

// 间接调用目标验证表
void* valid_call_targets[1024];
int num_valid_targets = 0;

void register_valid_call_target(void* target) {
    valid_call_targets[num_valid_targets++] = target;
}

int is_valid_call_target(void* target) {
    for (int i = 0; i < num_valid_targets; i++) {
        if (valid_call_targets[i] == target) {
            return 1;
        }
    }
    return 0;
}

// 安全的间接调用
void* safe_indirect_call(void* target, void** args) {
    if (!is_valid_call_target(target)) {
        fprintf(stderr, "CFI violation: invalid call target %p\n", target);
        abort();
    }

    typedef void* (*func_t)(void**);
    return ((func_t)target)(args);
}
