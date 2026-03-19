/*
 * Auto-generated from: 08_Zig_C_Connection\04_Translation_Validation\01_Translate_C_Semantics.md
 * Line: 785
 * Language: c
 * Block ID: 5825d768
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// C 代码 - 显式调用约定
#ifdef _WIN32
    #define API_CALL __stdcall
#else
    #define API_CALL __attribute__((cdecl))
#endif

void API_CALL callback_function(int code);

// Windows API 风格
__declspec(dllimport) void __stdcall WinApiFunction(HWND hwnd);

// 系统调用
__attribute__((syscall)) long syscall(long number, ...);
