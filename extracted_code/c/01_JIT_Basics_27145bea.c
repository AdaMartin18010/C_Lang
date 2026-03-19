/*
 * Auto-generated from: 05_Deep_Structure_MetaPhysics\12_Self_Modifying_Code\01_JIT_Basics.md
 * Line: 629
 * Language: c
 * Block ID: 27145bea
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 去优化：从优化代码回退到基线代码

// 去优化原因
typedef enum {
    DEOPT_TYPE_CHECK,       // 类型检查失败
    DEOPT_GUARD_FAILURE,    // 守卫条件失败
    DEOPT_MEMORY_BARRIER,   // 内存模型违规
    DEOPT_DEBUGGER,         // 调试器介入
    DEOPT_STACK_OVERFLOW    // 栈溢出检查
} DeoptReason;

// 去优化点
typedef struct {
    DeoptReason reason;
    int bytecode_pc;         // 回退到的字节码位置
    int num_safepoint_regs;  // 需要恢复的寄存器数
    int *reg_to_local_map;   // 寄存器到局部变量的映射
} DeoptimizationPoint;

// 去优化代码生成
// 在每个可能去优化的点插入检查

void emit_deopt_check(X86Encoder *enc, DeoptimizationPoint *point) {
    // 检查条件（例如类型标签）
    // cmp [obj], expected_tag
    // je continue
    // jmp deopt_stub

    // deopt_stub:
    // 1. 保存所有寄存器到栈
    // 2. 调用运行时去优化函数
    // 3. 恢复解释器状态
    // 4. 跳转到解释器
}

// 运行时去优化处理
void deoptimize(DeoptReason reason, void *deopt_point, RegisterState *regs) {
    // 查找对应的去优化点信息
    DeoptimizationPoint *point = find_deopt_point(deopt_point);

    // 重建解释器栈帧
    InterpreterFrame *interp_frame = rebuild_interpreter_frame(point, regs);

    // 标记此函数需要去优化（触发重新编译或解释执行）
    invalidate_optimized_code(point->function);

    // 恢复解释器执行
    resume_interpreter(interp_frame, point->bytecode_pc);
}

// 代码失效
void invalidate_optimized_code(BytecodeFunction *func) {
    // 将优化代码标记为无效
    func->optimized_code->is_valid = false;

    // 补丁所有调用点，使其调用基线版本
    patch_all_call_sites(func);

    // 如果函数正在执行，触发去优化
    // 这通常通过检查点（polling page）实现
}
