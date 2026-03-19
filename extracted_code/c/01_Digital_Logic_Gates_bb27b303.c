/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\09_Physical_Machine_Layer\01_Digital_Logic_Gates.md
 * Line: 172
 * Language: c
 * Block ID: bb27b303
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 逻辑门在C中的布尔模型（理想化）

typedef enum { FALSE = 0, TRUE = 1 } Bool;

// AND门
Bool gate_and(Bool a, Bool b) {
    return a && b;
}

// OR门
Bool gate_or(Bool a, Bool b) {
    return a || b;
}

// NOT门
Bool gate_not(Bool a) {
    return !a;
}

// XOR门（异或）
Bool gate_xor(Bool a, Bool b) {
    return a != b;  // 或 (a && !b) || (!a && b)
}

// NAND门
Bool gate_nand(Bool a, Bool b) {
    return !(a && b);
}

// NOR门
Bool gate_nor(Bool a, Bool b) {
    return !(a || b);
}
