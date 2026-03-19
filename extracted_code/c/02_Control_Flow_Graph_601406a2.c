/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\06_C_Assembly_Mapping\02_Control_Flow_Graph.md
 * Line: 300
 * Language: c
 * Block ID: 601406a2
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 常量传播后的分支折叠
if (1) {          // 编译时确定
    x = 1;        // 唯一可达
} else {
    x = 2;        // 不可达，删除
}

// 优化后
x = 1;
