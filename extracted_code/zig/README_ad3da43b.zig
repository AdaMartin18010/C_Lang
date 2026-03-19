//
// Auto-generated from: 08_Zig_C_Connection\04_Translation_Validation\README.md
// Line: 290
// Language: zig
// Block ID: ad3da43b
//
// This file was automatically extracted from the C_Lang knowledge base.
// DO NOT MODIFY - Changes will be overwritten on next extraction.
//

/// C 代码 - 违反严格别名规则
/// float f = 1.0f;
/// int* i = (int*)&f;
/// int val = *i;  // 未定义行为!

// Zig 使用 @ptrCast 需要显式注意
const f: f32 = 1.0;
const i: *const i32 = @ptrCast(&f);
const val = i.*;  // 技术上仍是未定义行为

// 正确做法：使用 @bitCast
const val_safe: i32 = @bitCast(f);
