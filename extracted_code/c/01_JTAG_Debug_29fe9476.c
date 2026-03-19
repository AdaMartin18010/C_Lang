/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\19_Hardware_Debugging\01_JTAG_Debug.md
 * Line: 492
 * Language: c
 * Block ID: 29fe9476
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// TAP 复位伪代码
void tap_reset() {
    // 方法1：使用 TRST（如果可用）
    TRST_LOW();
    delay_us(100);
    TRST_HIGH();

    // 方法2：使用 TMS（通用方法）
    for (int i = 0; i < 5; i++) {
        TMS_HIGH();
        TCK_CYCLE();  // 产生一个TCK周期
    }
    TMS_LOW();
    TCK_CYCLE();  // 进入 Run-Test/Idle
}
