/*
 * Auto-generated from: 06_Thinking_Representation\05_Concept_Mappings\01_Pointer_Memory_Mapping.md
 * Line: 180
 * Language: c
 * Block ID: 1341aacd
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 模式1: 未初始化指针（野指针）
int *p;
*p = 10;  // UB - 指向随机地址

// 模式2: 悬挂指针
int *p = malloc(sizeof(int));
free(p);
*p = 10;  // UB - 使用已释放内存

// 模式3: 内存泄漏
void leak() {
    int *p = malloc(sizeof(int));
    // 未free，指针p超出作用域
}

// 模式4: 重复释放
int *p = malloc(sizeof(int));
free(p);
free(p);  // UB - 双重释放

// 模式5: 越界访问
int *p = malloc(10 * sizeof(int));
p[10] = 0;  // UB - 越界写入

// 模式6: 栈返回指针
int* bad() {
    int x = 10;
    return &x;  // 返回栈变量地址 - 悬挂指针
}
