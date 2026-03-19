/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\03_Linear_Logic\01_Linear_Logic_Theory.md
 * Line: 567
 * Language: c
 * Block ID: cfd833b5
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 错误：未使用资源
void leak(void) {
    Resource *r = create_resource();
    // r未被消耗也未被返回
}  // 资源泄漏！

// 正确：显式处理
void no_leak(void) {
    Resource *r = create_resource();
    use(r);  // 或
    // return r;
}
