/*
 * Auto-generated from: 01_Core_Knowledge_System\09_Safety_Standards\MISRA_C_2023\15_MISRA_C_2023_Rules_15.md
 * Line: 279
 * Language: c
 * Block ID: a84b5777
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* ✅ 合规 - 相同类型函数指针 */
int (*fp)(int) = func;
int result = fp(42);

/* ✅ 合规 - 使用typedef */
typedef int (*Operation)(int, int);

int add(int a, int b) { return a + b; }
int sub(int a, int b) { return a - b; }

Operation ops[] = {add, sub};

/* ✅ 合规 - 回调模式 */
typedef void (*Callback)(void *context, int result);

void register_callback(Callback cb, void *context);
