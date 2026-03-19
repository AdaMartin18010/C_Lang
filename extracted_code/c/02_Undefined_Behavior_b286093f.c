/*
 * Auto-generated from: 01_Core_Knowledge_System\06_Advanced_Layer\02_Undefined_Behavior.md
 * Line: 230
 * Language: c
 * Block ID: b286093f
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 编译器可能优化掉"不可能"的分支
void check(int *p) {
    if (p == NULL) {
        // 编译器假设这永远不会执行
        // 如果前面已经解引用了p
    }
}

int x = *p;  // 编译器假设p非NULL
if (p == NULL) {  // 可能被优化掉！
    // ...
}
