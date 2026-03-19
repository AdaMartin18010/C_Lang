/*
 * Auto-generated from: 05_Deep_Structure_MetaPhysics\12_Self_Modifying_Code\01_JIT_Basics.md
 * Line: 784
 * Language: c
 * Block ID: fa7c41c9
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 错误：跳转偏移计算
void wrong_jump(void) {
    uint8_t *jmp_addr = code + offset;
    *jmp_addr = 0xE9;
    *(int32_t*)(jmp_addr + 1) = target - jmp_addr;  // ❌ 应该减去指令长度！
}

// 正确：跳转偏移是相对于下一条指令的
void correct_jump(void) {
    uint8_t *jmp_addr = code + offset;
    *jmp_addr = 0xE9;
    *(int32_t*)(jmp_addr + 1) = target - (jmp_addr + 5);  // ✅ +5是jmp指令长度
}
