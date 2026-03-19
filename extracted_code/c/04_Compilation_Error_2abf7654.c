/*
 * Auto-generated from: 06_Thinking_Representation\01_Decision_Trees\04_Compilation_Error.md
 * Line: 71
 * Language: c
 * Block ID: 2abf7654
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// ❌ 错误: 未包含头文件
int main() {
    printf("Hello");  // printf未声明
    return 0;
}

// ✅ 修正
#include <stdio.h>   // 添加头文件

int main() {
    printf("Hello");
    return 0;
}
