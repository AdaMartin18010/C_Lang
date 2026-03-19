//
// Auto-generated from: Zig\Zig_Formal_Semantics.md
// Line: 100
// Language: zig
// Block ID: 10f8afd2
//
// This file was automatically extracted from the C_Lang knowledge base.
// DO NOT MODIFY - Changes will be overwritten on next extraction.
//

// Zig 语义示例

// 赋值语义
const x = 5;     // σ' = σ[x ↦ 5]
var y = x + 3;   // 先求值 x → 5, 然后 5 + 3 → 8, σ'' = σ'[y ↦ 8]

// 错误传播语义
data: anyerror!i32 = error.Overflow;  // 错误值传播
const result = data catch 0;          // 捕获错误，默认值为 0
