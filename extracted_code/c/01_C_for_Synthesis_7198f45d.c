/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\14_C_to_Hardware_Mapping\01_C_for_Synthesis.md
 * Line: 496
 * Language: c
 * Block ID: 7198f45d
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

void type_conversions() {
    ap_uint<8>  u8 = 255;
    ap_int<8>   s8 = -128;
    ap_uint<16> u16;
    ap_int<16>  s16;

    // 零扩展（无符号到更宽无符号）
    u16 = u8;  // 0x00FF → 0x00FF

    // 符号扩展（有符号到更宽有符号）
    s16 = s8;  // 0xFF80 → 0xFF80

    // 截断（宽到窄）
    u8 = u16;  // 取低8位

    // 有符号/无符号转换
    s8 = u8;   // 255 → -1（位模式不变，解释改变）

    // 浮点到定点
    ap_fixed<16,8> fixed_val = 3.14159;

    // 定点到浮点
    float float_val = fixed_val.to_float();

    // 位 reinterpretation
    ap_uint<32> bits = fixed_val.range(31, 0);
}
