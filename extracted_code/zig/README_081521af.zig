//
// Auto-generated from: 08_Zig_C_Connection\03_ABI_Formalization\README.md
// Line: 183
// Language: zig
// Block ID: 081521af
//
// This file was automatically extracted from the C_Lang knowledge base.
// DO NOT MODIFY - Changes will be overwritten on next extraction.
//

/// stdcall - 被调用者清理栈
/// Win32 API 标准调用约定
extern "stdcall" fn stdcall_example(
    arg1: i32,
    arg2: i32,
) i32;

// 汇编理解:
// push arg2
// push arg1
// call stdcall_example
// ; 无需清理栈，被调用函数使用 ret 8
