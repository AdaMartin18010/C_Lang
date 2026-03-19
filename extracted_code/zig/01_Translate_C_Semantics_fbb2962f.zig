//
// Auto-generated from: 08_Zig_C_Connection\04_Translation_Validation\01_Translate_C_Semantics.md
// Line: 802
// Language: zig
// Block ID: fbb2962f
//
// This file was automatically extracted from the C_Lang knowledge base.
// DO NOT MODIFY - Changes will be overwritten on next extraction.
//

// 生成的 Zig 代码
pub extern "stdcall" fn callback_function(code: c_int) void;

pub extern "stdcall" fn WinApiFunction(hwnd: HWND) void;

pub extern "syscall" fn syscall(number: c_long, ...) c_long;

// 调用约定映射表
// C Declaration              Zig callconv
// ─────────────────────────────────────────────────────────
// default (cdecl)            .C
// __cdecl                    .C
// __stdcall                  .Stdcall
// __fastcall                 .Fastcall
// __thiscall                 .Thiscall
// __vectorcall               .Vectorcall
// __attribute__((regparm(n))) .Stdcall (近似)
// syscall                    .SysV (Linux) 或特殊处理
