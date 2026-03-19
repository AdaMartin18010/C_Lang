/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\06_C_Assembly_Mapping\README.md
 * Line: 77
 * Language: c
 * Block ID: 456f516d
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 活跃变量分析示例
void example(int x, int y) {
    int a = x + 1;      // x活跃, a新生成
    int b = y * 2;      // y活跃, a活跃, b新生成
    int c = a + b;      // a,b活跃, c新生成
    printf("%d", c);    // c活跃
}                      // 全部消亡
