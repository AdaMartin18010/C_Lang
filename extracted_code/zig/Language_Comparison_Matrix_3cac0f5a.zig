//
// Auto-generated from: Zig\matrices\Language_Comparison_Matrix.md
// Line: 300
// Language: zig
// Block ID: 3cac0f5a
//
// This file was automatically extracted from the C_Lang knowledge base.
// DO NOT MODIFY - Changes will be overwritten on next extraction.
//

// Zig - comptime 泛型
fn max(comptime T: type, a: T, b: T) T {
    return if (a > b) a else b;
}

const m1 = max(i32, 10, 20);
const m2 = max(f64, 3.14, 2.71);

// C++ - 模板
template<typename T>
T max(T a, T b) {
    return (a > b) ? a : b;
}

// Rust - 泛型
fn max<T: Ord>(a: T, b: T) -> T {
    if a > b { a } else { b }
}
