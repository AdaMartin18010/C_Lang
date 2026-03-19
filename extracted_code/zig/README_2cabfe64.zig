//
// Auto-generated from: 08_Zig_C_Connection\03_ABI_Formalization\README.md
// Line: 164
// Language: zig
// Block ID: 2cabfe64
//
// This file was automatically extracted from the C_Lang knowledge base.
// DO NOT MODIFY - Changes will be overwritten on next extraction.
//

/// C 标准调用约定 - 调用者清理栈
/// 参数从右向左压栈
extern "cdecl" fn cdecl_example(
    arg1: i32,  // 压栈顺序: 最后
    arg2: i32,  // 压栈顺序: 其次
    arg3: i32,  // 压栈顺序: 首先
) i32;

// 等效的汇编理解:
// push arg3
// push arg2
// push arg1
// call cdecl_example
// add rsp, 12  ; 调用者清理栈
