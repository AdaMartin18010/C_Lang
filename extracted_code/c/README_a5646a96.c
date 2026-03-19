/*
 * Auto-generated from: 05_Deep_Structure_MetaPhysics\13_Self_Modifying_Code\README.md
 * Line: 1027
 * Language: c
 * Block ID: a5646a96
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// OSR: 在方法执行过程中切换实现
// 从解释执行切换到 JIT 编译版本

void on_stack_replacement(InterpreterFrame* frame, void* jited_code) {
    // 1. 暂停当前线程
    suspend_thread();

    // 2. 构建 JIT 函数的栈帧
    // 将解释器栈变量映射到 JIT 栈槽

    JITFrame jit_frame;
    jit_frame.local_vars = map_interpreter_locals(frame);
    jit_frame.operand_stack = map_interpreter_stack(frame);

    // 3. 计算 JIT 代码中的入口点
    // 不是函数开头，而是对应当前字节码位置的 OSR 入口
    void* osr_entry = find_osr_entry(jited_code, frame->current_pc);

    // 4. 修改返回地址，使其返回到 JIT 代码
    void** return_addr_slot = get_return_address_slot(frame);
    *return_addr_slot = osr_entry;

    // 5. 恢复执行
    resume_thread();
}
