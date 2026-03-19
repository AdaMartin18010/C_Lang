/*
 * Auto-generated from: 05_Deep_Structure_MetaPhysics\09_Linking_Algebraic_Topology\01_Relocation_Group_Action.md
 * Line: 604
 * Language: c
 * Block ID: b5d3894d
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 32位PC相对重定位在高地址空间可能溢出
int32_t pc_relative_reloc(uint64_t S, int64_t A, uint64_t P) {
    int64_t result = (int64_t)S + A - (int64_t)P;

    // 必须检查32位溢出
    if (result < INT32_MIN || result > INT32_MAX) {
        // 错误：需要使用PLT/GOT重定位
        handle_overflow();
    }

    return (int32_t)result;
}
