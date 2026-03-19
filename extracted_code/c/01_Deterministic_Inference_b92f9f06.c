/*
 * Auto-generated from: 16_Embedded_AI_Agents\08_Real_Time_AI\01_Deterministic_Inference.md
 * Line: 307
 * Language: c
 * Block ID: b92f9f06
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 编写"WCET友好"的代码

// ❌ 避免递归
int fib_recursive(int n) {
    if (n <= 1) return n;
    return fib_recursive(n-1) + fib_recursive(n-2);  // 无法分析
}

// ✅ 使用迭代
int fib_iterative(int n) {
    if (n <= 1) return n;
    int a = 0, b = 1;
    for (int i = 2; i <= n; i++) {  // 有界循环
        int temp = a + b;
        a = b;
        b = temp;
    }
    return b;
}

// ✅ 使用__builtin_expect提示分支概率 (GCC)
void process_data(int* data, int size) {
    for (int i = 0; i < size; i++) {
        // 提示: data[i] == 0 是罕见情况
        if (__builtin_expect(data[i] == 0, 0)) {
            handle_zero();  // WCET分析中视为冷路径
        } else {
            handle_nonzero();
        }
    }
}
