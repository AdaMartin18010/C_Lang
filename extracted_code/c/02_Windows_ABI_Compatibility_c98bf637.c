/*
 * Auto-generated from: 08_Zig_C_Connection\03_ABI_Formalization\02_Windows_ABI_Compatibility.md
 * Line: 230
 * Language: c
 * Block ID: c98bf637
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// C 代码: Windows x64 浮点参数示例

void float_args(float a, double b, float c, double d) {
    // a: XMM0 (32位)
    // b: XMM1 (64位)
    // c: XMM2 (32位)
    // d: XMM3 (64位)
}

void mixed_int_float(int a, double b, int c, float d) {
    // Windows x64: 共享槽位计数
    // a: ECX  (槽位1)
    // b: XMM1 (槽位2)
    // c: R8D  (槽位3)
    // d: XMM3 (槽位4)
}
