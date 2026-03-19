/*
 * Auto-generated from: 10_WebAssembly_C\01_WebAssembly_Introduction_C.md
 * Line: 370
 * Language: c
 * Block ID: 9f6e35b5
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 示例: EMSCRIPTEN_KEEPALIVE 宏
#include <emscripten/emscripten.h>

// 标记此函数需要从 WASM 导出，即使看起来未被使用
EMSCRIPTEN_KEEPALIVE
int fibonacci(int n) {
    if (n <= 1) return n;
    return fibonacci(n - 1) + fibonacci(n - 2);
}

// 示例: 使用 Embind 进行类型绑定
#include <emscripten/bind.h>

class Calculator {
public:
    int add(int a, int b) { return a + b; }
    int multiply(int a, int b) { return a * b; }
};

EMSCRIPTEN_BINDINGS(my_module) {
    emscripten::class_<Calculator>("Calculator")
        .constructor()
        .function("add", &Calculator::add)
        .function("multiply", &Calculator::multiply);
}
