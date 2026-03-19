/*
 * Auto-generated from: 08_Zig_C_Connection\04_Translation_Validation\02_CImport_Correctness.md
 * Line: 180
 * Language: c
 * Block ID: 7b9fb601
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// C 代码: 复杂类型

// 结构体
struct Node {
    int value;
    struct Node* next;
};

// 联合体
union Data {
    int i;
    float f;
    char bytes[4];
};

// 枚举
enum Color { RED, GREEN, BLUE };

// 函数指针
typedef int (*callback_t)(int, void*);

// 复杂声明
int (*signal(int sig, int (*func)(int)))(int);
