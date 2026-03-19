//
// Auto-generated from: 08_Zig_C_Connection\03_ABI_Formalization\README.md
// Line: 108
// Language: zig
// Block ID: d29e2067
//
// This file was automatically extracted from the C_Lang knowledge base.
// DO NOT MODIFY - Changes will be overwritten on next extraction.
//

// 默认使用 C 调用约定 (.C)
extern fn c_function(x: i32) i32;

// 显式指定不同调用约定
extern "cdecl" fn cdecl_function(ptr: *anyopaque) void;
extern "stdcall" fn stdcall_function(hwnd: usize, msg: u32) i64;
extern "fastcall" fn fastcall_function(a: i32, b: i32) i32;
extern "vectorcall" fn vectorcall_function(v: @Vector(4, f32)) f32;

// Windows 特定
extern "system" fn system_function() void;  // stdcall on Win32

// 内联汇编级别的控制
pub export fn precise_abi_control(arg1: i64, arg2: f64) i64 {
    return asm volatile (
        \\ mov %rdi, %rax    # 从正确寄存器读取
        \\ add %xmm0, %xmm0  # 处理浮点参数
        : [ret] "={rax}" (-> i64)
        : [a1] "{rdi}" (arg1),
          [a2] "{xmm0}" (arg2)
        : "memory"
    );
}
