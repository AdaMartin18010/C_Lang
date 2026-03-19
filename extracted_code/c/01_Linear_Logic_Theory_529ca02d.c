/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\03_Linear_Logic\01_Linear_Logic_Theory.md
 * Line: 102
 * Language: c
 * Block ID: 529ca02d
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 经典视角：任意复制
int duplicate_classic(int x) {
    return x + x;  // x被使用两次
}

// 线性视角：需要显式复制权限
// linear int x 表示x是线性资源
// 使用dup操作符显式复制
// linear int y = dup(x);  // x变为无效，y是x的副本
