/*
 * Auto-generated from: 05_Deep_Structure_MetaPhysics\13_Self_Modifying_Code\02_JIT_Physical_Constraints.md
 * Line: 339
 * Language: c
 * Block ID: 9d48906a
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/*
 * JIT生成的代码对分支预测器的影响：
 *
 * 1. 冷代码：无历史记录，预测器需要学习
 * 2. 间接跳转：目标地址动态确定，难预测
 * 3. 代码布局改变：影响BTB（分支目标缓冲区）
 */

// 分支预测友好的代码生成

// 1. 热路径内联
typedef enum {
    EDGE_HOT,      // 频繁执行
    EDGE_COLD,     // 很少执行
    EDGE_UNKNOWN   // 未知
} EdgeProfile;

void emit_conditional_jump(X86Encoder *enc, EdgeProfile profile,
                           void *hot_target, void *cold_target) {
    switch (profile) {
        case EDGE_HOT:
            // 热路径：向前跳转（预测不执行）
            // 或向后跳转（预测执行）
            emit_jcc(enc, JCC_ALWAYS_NOT_TAKEN, cold_target);
            break;

        case EDGE_COLD:
            // 冷路径：使用分支提示
            // 静态预测：向前跳转=不执行，向后跳转=执行
            emit_jcc_with_hint(enc, JCC_HINT_NOT_TAKEN, cold_target);
            break;

        case EDGE_UNKNOWN:
            // 使用PGO数据或默认策略
            emit_jcc(enc, JCC_DEFAULT, cold_target);
            break;
    }
}

// 2. 间接跳转优化（内联缓存）
void emit_optimized_indirect_call(X86Encoder *enc, void **vtable, int index) {
    // 内联缓存结构：
    // cmp [ic_shape], expected_shape
    // jne fallback
    // call [ic_target]

    // 生成IC检查代码
    emit_cmp_mem_imm32(enc, IC_SHAPE_OFFSET, 0);  // shape占位符
    emit_jcc(enc, JCC_NOT_EQUAL, fallback_label);

    // 直接调用缓存目标（可预测）
    emit_call_direct(enc, 0);  // 目标占位符
    emit_jmp(enc, done_label);

    // fallback：通用处理
    emit_label(enc, fallback_label);
    emit_mov_reg_mem(enc, RAX, vtable + index);
    emit_call_reg(enc, RAX);

    // 更新IC
    emit_mov_mem_imm32(enc, IC_SHAPE_OFFSET, actual_shape);
    emit_mov_mem_imm64(enc, IC_TARGET_OFFSET, actual_target);

    emit_label(enc, done_label);
}

// 3. 代码对齐优化
void emit_aligned_loop_header(X86Encoder *enc, int alignment) {
    // 循环头部对齐到16/32字节边界
    // 提高取指和解码效率

    size_t current = enc->offset;
    size_t aligned = (current + alignment - 1) & ~(alignment - 1);
    size_t padding = aligned - current;

    // 使用NOP填充（x86有专门的NOP序列）
    for (size_t i = 0; i < padding; i++) {
        emit_u8(enc, 0x90);  // NOP
    }
}

// x86多字节NOP序列
static const uint8_t nop_sequences[16][16] = {
    {},  // 0 bytes
    {0x90},  // 1 byte
    {0x66, 0x90},  // 2 bytes
    {0x0F, 0x1F, 0x00},  // 3 bytes
    // ... 更长的NOP序列
};

void emit_multi_byte_nop(X86Encoder *enc, int size) {
    while (size > 0) {
        int n = (size > 15) ? 15 : size;
        for (int i = 0; i < n; i++) {
            emit_u8(enc, nop_sequences[n][i]);
        }
        size -= n;
    }
}
