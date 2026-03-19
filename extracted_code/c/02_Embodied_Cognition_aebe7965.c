/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\04_Cognitive_Representation\02_Embodied_Cognition.md
 * Line: 96
 * Language: c
 * Block ID: aebe7965
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 具身隐喻在编程中的应用

// 容器隐喻：变量作为容器
int x = 5;  // "x包含5"

// 路径隐喻：控制流
if (condition) {
    // "走这条路"
} else {
    // "走另一条路"
}

// 力量动力学：指针所有权
void transfer_ownership(Resource **src, Resource **dst) {
    // "力量从src流向dst"
    *dst = *src;
    *src = NULL;
}
