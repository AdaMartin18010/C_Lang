/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\09_Physical_Machine_Layer\01_Digital_Logic_Gates.md
 * Line: 325
 * Language: c
 * Block ID: 83087144
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 三变量卡诺图简化器

// 卡诺图表示（格雷码顺序）
//      b'c'  b'c   bc    bc'
// a'     0     1    1     0
// a      0     1    0     1

// 对应最小项：a'b'c(1), a'bc(3), ab'c(5), abc'(6)

Bool kmap_function(Bool a, Bool b, Bool c) {
    // 简化结果：a'c + bc' + ab'c
    // 或者：c(a'+b') + abc'

    Bool term1 = !a && c;      // a'c
    Bool term2 = b && !c;      // bc'  ← 修正：应为bc'
    Bool term3 = a && !b && c;  // ab'c

    return term1 || term2 || term3;
}
