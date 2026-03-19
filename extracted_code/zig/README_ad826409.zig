//
// Auto-generated from: 08_Zig_C_Connection\06_C23_Zig_Parallel\README.md
// Line: 284
// Language: zig
// Block ID: ad826409
//
// This file was automatically extracted from the C_Lang knowledge base.
// DO NOT MODIFY - Changes will be overwritten on next extraction.
//

// Zig: 使用comptime
fn max(comptime T: type, a: T, b: T) T {
    return if (a > b) a else b;
}

const m1 = max(i32, 10, 20);
const m2 = max(f64, 3.14, 2.71);
