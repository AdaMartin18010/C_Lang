/*
 * Auto-generated from: 01_Core_Knowledge_System\06_Advanced_Layer\01_Language_Extensions.md
 * Line: 196
 * Language: c
 * Block ID: 1cdfd088
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// Clang块语法（Blocks）
#if defined(__BLOCKS__)
void apply_block(int *arr, int n, int (^block)(int)) {
    for (int i = 0; i < n; i++) {
        arr[i] = block(arr[i]);
    }
}

// 使用
int multiplier = 2;
apply_block(arr, 10, ^(int x) {
    return x * multiplier;  // 捕获外部变量
});
#endif

// MSVC __declspec
#ifdef _MSC_VER
    __declspec(dllexport) void exported_func(void);
    __declspec(align(64)) int aligned_var;
    __declspec(thread) int thread_local_var;  // 线程局部存储
    __declspec(noinline) void noinline_func(void);
    __declspec(noreturn) void fatal_error(void);
#endif

// 调用约定
#ifdef _WIN32
    void __stdcall stdcall_func(void);
    void __cdecl cdecl_func(void);
    void __fastcall fastcall_func(void);
#endif
