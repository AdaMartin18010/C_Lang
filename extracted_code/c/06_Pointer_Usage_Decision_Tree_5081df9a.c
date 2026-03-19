/*
 * Auto-generated from: 06_Thinking_Representation\01_Decision_Trees\06_Pointer_Usage_Decision_Tree.md
 * Line: 160
 * Language: c
 * Block ID: 5081df9a
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// ✅ 良好的实践
int* ptr = NULL;                    // 1. 初始化为NULL
ptr = malloc(sizeof(int) * n);      // 2. 分配内存
if (ptr != NULL) {                  // 3. 检查分配结果
    *ptr = 42;                      // 4. 安全使用
    free(ptr);                      // 5. 释放内存
    ptr = NULL;                     // 6. 置空避免悬空
}

// ❌ 避免的做法
int* bad;                           // 未初始化
*bad = 42;                          // 危险！可能崩溃
