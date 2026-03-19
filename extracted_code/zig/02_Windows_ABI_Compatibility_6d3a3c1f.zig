//
// Auto-generated from: 08_Zig_C_Connection\03_ABI_Formalization\02_Windows_ABI_Compatibility.md
// Line: 476
// Language: zig
// Block ID: 6d3a3c1f
//
// This file was automatically extracted from the C_Lang knowledge base.
// DO NOT MODIFY - Changes will be overwritten on next extraction.
//

// Zig 代码: Windows ABI 函数声明

const std = @import("std");

// 标准 Windows x64 调用约定 (默认)
extern "C" fn windowsFunc(x: i32, y: f64) i32;
// x: ECX, y: XMM1, 返回: EAX

// Win32 API 调用 (x86 使用 stdcall，x64 忽略)
extern "stdcall" fn WinApiFunction(hwnd: ?*anyopaque, msg: u32) i32;

// __vectorcall 调用约定
extern "vectorcall" fn vectorFunc(v1: @Vector(4, f32), v2: @Vector(4, f32)) @Vector(4, f32);

// Windows 特定类型
const HWND = *anyopaque;
const UINT = c_uint;
const WPARAM = usize;
const LPARAM = isize;
const LRESULT = isize;

extern "user32" fn MessageBoxA(
    hWnd: ?HWND,
    lpText: [*:0]const u8,
    lpCaption: [*:0]const u8,
    uType: UINT,
) c_int;
