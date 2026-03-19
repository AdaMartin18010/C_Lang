/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\03_Linear_Logic\01_Linear_Logic_Theory.md
 * Line: 439
 * Language: c
 * Block ID: e2bdcd86
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// dereliction: !A ⊸ A
Resource *dereliction(Exponential *e) {
    Resource *r = e->inner;
    // e仍然有效（引用计数）
    return r;
}

// promotion: 从A的证明构造!A
// 要求A的证明不依赖任何线性假设
Exponential *promotion(Resource *(*proof)(void)) {
    // proof必须是"无状态"的
    Resource *r = proof();
    return exp_create(r);
}
