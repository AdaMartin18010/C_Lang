//
// Auto-generated from: 05_Deep_Structure_MetaPhysics\02_Algebraic_Topology\03_Homotopy_Type_Theory.md
// Line: 121
// Language: zig
// Block ID: 9e5151f2
//
// This file was automatically extracted from the C_Lang knowledge base.
// DO NOT MODIFY - Changes will be overwritten on next extraction.
//

// Zig类型系统的"同伦层级"类比

// Level -1: 单元类型 (类似命题)
const Unit = void;  // 只有一个值 {}

// Level 0: 基础数据类型 (集合)
const Integers = i32;  // 元素有唯一相等性
const Booleans = bool;  // true 和 false

// Level 1: 有结构相等性的类型
const Point = struct {
    x: i32,
    y: i32,
};
// 两个Point相等当且仅当所有字段相等

// 在Zig中，相等性通常是结构化的
const p1 = Point{ .x = 1, .y = 2 };
const p2 = Point{ .x = 1, .y = 2 };
// p1 和 p2 在所有方面相等

// 高阶相等性: 函数相等性
const IntFn = *const fn (i32) i32;
// 两个函数相等如果对所有输入产生相同输出
// (Zig中无法表达，需要外部证明)
