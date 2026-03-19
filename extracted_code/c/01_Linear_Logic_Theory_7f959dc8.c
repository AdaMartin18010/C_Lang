/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\03_Linear_Logic\01_Linear_Logic_Theory.md
 * Line: 551
 * Language: c
 * Block ID: 7f959dc8
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 错误：试图复制线性资源
void bad(Resource *r) {
    use(r);
    use(r);  // 错误：r已被消耗
}

// 正确：使用指数类型
void good(Exponential *r) {
    use(exp_use(r));  // 使用但不消耗
    use(exp_use(r));  // 再次使用
}
