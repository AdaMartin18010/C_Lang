//
// Auto-generated from: 08_Zig_C_Connection\03_ABI_Formalization\02_Windows_ABI_Compatibility.md
// Line: 392
// Language: zig
// Block ID: 12b67953
//
// This file was automatically extracted from the C_Lang knowledge base.
// DO NOT MODIFY - Changes will be overwritten on next extraction.
//

// Zig 代码: Windows x64 结构体返回

const Small = extern struct { a: i32, b: i32 };
extern "C" fn returnSmall() Small;
// 返回在 RAX 中

const Medium = extern struct { a: i64, b: i64 };
extern "C" fn returnMedium() Medium;
// 返回在 RAX:RDX 中

const Large = extern struct { data: [10]i32 };
extern "C" fn returnLarge() Large;
// Zig 编译器会自动处理隐藏指针
