/*
 * Auto-generated from: 01_Core_Knowledge_System\02_Core_Layer\04_Functions_Scope.md
 * Line: 349
 * Language: c
 * Block ID: 2a8126d0
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 危险：无终止条件的递归
void infinite_recursion(void) {
    infinite_recursion();  // 栈溢出！
}

// 斐波那契的n不能太大
unsigned long long fib(int n) {
    if (n > 90) {  // 限制递归深度
        fprintf(stderr, "n too large\n");
        return 0;
    }
    // ...
}
