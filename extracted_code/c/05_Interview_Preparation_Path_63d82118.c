/*
 * Auto-generated from: 06_Thinking_Representation\06_Learning_Paths\05_Interview_Preparation_Path.md
 * Line: 151
 * Language: c
 * Block ID: 63d82118
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 内存泄漏示例
void leak() {
    int *p = malloc(sizeof(int) * 100);
    // 忘记free
}  // p离开作用域，内存无法访问也无法释放

// 检测方法：
// 1. Valgrind: valgrind --leak-check=full ./program
// 2. AddressSanitizer: gcc -fsanitize=address
// 3. 代码审查：malloc/free配对检查
// 4. 智能指针（C++）或RAII模式
