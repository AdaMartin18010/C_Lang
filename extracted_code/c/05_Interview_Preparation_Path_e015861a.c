/*
 * Auto-generated from: 06_Thinking_Representation\06_Learning_Paths\05_Interview_Preparation_Path.md
 * Line: 169
 * Language: c
 * Block ID: e015861a
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 栈溢出 - 通常由于递归太深或大局部变量
void stack_overflow() {
    char big_array[1024 * 1024 * 10];  // 10MB栈分配
    stack_overflow();  // 无限递归
}

// 堆溢出 - 写入超出分配内存
void heap_overflow() {
    char *p = malloc(10);
    strcpy(p, "this is way too long");  // 越界写入
    free(p);
}

// 防御方法：
// - 限制递归深度
// - 使用安全函数（strncpy替代strcpy）
// - 启用栈保护（-fstack-protector）
