/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\09_Physical_Machine_Layer\01_Digital_Logic_Gates.md
 * Line: 374
 * Language: c
 * Block ID: f5322f17
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 半加器（2输入，输出和与进位）
typedef struct {
    Bool sum;
    Bool carry;
} HalfAdderResult;

HalfAdderResult half_adder(Bool a, Bool b) {
    HalfAdderResult r;
    r.sum = a ^ b;      // XOR
    r.carry = a && b;   // AND
    return r;
}

// 全加器（3输入：a, b, cin）
typedef struct {
    Bool sum;
    Bool carry;
} FullAdderResult;

FullAdderResult full_adder(Bool a, Bool b, Bool cin) {
    FullAdderResult r;

    // 两级半加器构成全加器
    HalfAdderResult ha1 = half_adder(a, b);
    HalfAdderResult ha2 = half_adder(ha1.sum, cin);

    r.sum = ha2.sum;
    r.carry = ha1.carry || ha2.carry;

    return r;
}
