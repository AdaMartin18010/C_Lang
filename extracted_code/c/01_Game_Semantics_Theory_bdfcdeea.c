/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\01_Game_Semantics\01_Game_Semantics_Theory.md
 * Line: 387
 * Language: c
 * Block ID: bdfcdeea
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// ❌ 错误理解：认为所有移动都是对称的
void wrong_interpretation(void) {
    // 忽略了O和P角色的根本差异
}

// ✅ 正确理解：O和P有本质区别
// O是"挑战者"，P是"辩护者"
// 在函数调用中：O调用函数，P执行函数体
