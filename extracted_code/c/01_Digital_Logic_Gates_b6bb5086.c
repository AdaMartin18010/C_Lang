/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\09_Physical_Machine_Layer\01_Digital_Logic_Gates.md
 * Line: 349
 * Language: c
 * Block ID: b6bb5086
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 复杂布尔函数的层次化实现

// f = ab + ac + ad + bcd  → 提取公因子
//   = a(b + c + d) + bcd

Bool optimized_function(Bool a, Bool b, Bool c, Bool d) {
    // 原始实现：4个AND + 4输入OR = 5级延迟
    // Bool f1 = (a && b) || (a && c) || (a && d) || (b && c && d);

    // 优化实现：提取a，减少门数
    Bool common = b || c || d;
    Bool term1 = a && common;
    Bool term2 = b && c && d;

    return term1 || term2;  // 3级延迟
}
