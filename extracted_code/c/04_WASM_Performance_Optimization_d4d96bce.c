/*
 * Auto-generated from: 10_WebAssembly_C\04_WASM_Performance_Optimization.md
 * Line: 1492
 * Language: c
 * Block ID: d4d96bce
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// tail_call_optimization.c
#include <emscripten/emscripten.h>

// 启用尾调用优化（如果编译器支持）
// emcc -O3 -mtail-call ...

// 尾递归求和
int sum_tail_recursive(int n, int acc) {
    if (n <= 0) return acc;
    return sum_tail_recursive(n - 1, acc + n);  // 尾调用
}

// 相互尾递归
int even(int n);
int odd(int n);

int even(int n) {
    if (n == 0) return 1;
    return odd(n - 1);  // 尾调用
}

int odd(int n) {
    if (n == 0) return 0;
    return even(n - 1);  // 尾调用
}

// 状态机（使用尾调用）
typedef enum { STATE_A, STATE_B, STATE_C } State;

State run_state(State current, int* data, int* result);

State state_a(int* data, int* result) {
    *result += data[0];
    return run_state(STATE_B, data + 1, result);
}

State state_b(int* data, int* result) {
    *result *= data[0];
    return run_state(STATE_C, data + 1, result);
}

State state_c(int* data, int* result) {
    *result -= data[0];
    return run_state(STATE_A, data + 1, result);
}

State run_state(State current, int* data, int* result) {
    switch (current) {
        case STATE_A: return state_a(data, result);
        case STATE_B: return state_b(data, result);
        case STATE_C: return state_c(data, result);
    }
    return current;
}
