/*
 * Auto-generated from: 03_System_Technology_Domains\01_Virtual_Machine_Interpreter\01_Bytecode_VM.md
 * Line: 444
 * Language: c
 * Block ID: 8e444fc9
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// ❌ 无栈溢出检查
void vm_push(VM *vm, int64_t val) {
    vm->stack[vm->sp++] = val;  // 可能溢出！
}

// ✅ 安全检查
bool vm_push_safe(VM *vm, int64_t val) {
    if (vm->sp >= vm->stack_size) {
        fprintf(stderr, "Stack overflow\n");
        return false;
    }
    vm->stack[vm->sp++] = val;
    return true;
}
