/*
 * Auto-generated from: 01_Core_Knowledge_System\06_Advanced_Layer\02_Undefined_Behavior.md
 * Line: 102
 * Language: c
 * Block ID: dc061eab
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// ❌ 空指针解引用
int *p = NULL;
int x = *p;  // UB! (通常是段错误)

// ❌ 悬挂指针
int *dangling(void) {
    int local = 5;
    return &local;  // 返回局部变量地址
}
int *p = dangling();
int y = *p;  // UB! 栈帧已销毁

// ❌ 越界访问
int arr[10];
int z = arr[10];  // UB! 越界
