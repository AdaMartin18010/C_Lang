//
// Auto-generated from: 08_Zig_C_Connection\03_ABI_Formalization\README.md
// Line: 200
// Language: zig
// Block ID: 1137f55f
//
// This file was automatically extracted from the C_Lang knowledge base.
// DO NOT MODIFY - Changes will be overwritten on next extraction.
//

/// fastcall - 使用寄存器传递前几个参数
/// x86: ECX, EDX 用于前两个整数参数
/// x64: 与标准 C 调用约定相同 (寄存器传参)
extern "fastcall" fn fastcall_example(
    a: i32,  // ECX (x86) / RDX (x64)
    b: i32,  // EDX (x86) / RCX (x64)
    c: i32,  // 栈
) i32;
