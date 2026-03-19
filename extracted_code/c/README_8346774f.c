/*
 * Auto-generated from: 06_Thinking_Representation\02_Comparison_Matrices\README.md
 * Line: 161
 * Language: c
 * Block ID: 8346774f
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 有问题的代码示例
#include <stdio.h>

void unsafe_function(char* input) {
    char buffer[10];
    strcpy(buffer, input);  // 潜在的缓冲区溢出
    printf("%s", buffer);
}

int main() {
    int x;
    printf("%d", x);  // 使用未初始化的变量
    return 0;
}
