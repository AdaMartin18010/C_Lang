//
// Auto-generated from: 12_Practice_Exercises\11_Zig_C_Migration_Projects.md
// Line: 362
// Language: zig
// Block ID: a6449b26
//
// This file was automatically extracted from the C_Lang knowledge base.
// DO NOT MODIFY - Changes will be overwritten on next extraction.
//

// main.zig
const std = @import("std");
const c_wrapper = @import("c_wrapper.zig");

pub fn main() !void {
    // 使用安全包装
    var person = try c_wrapper.SafePerson.init("Alice", 30);
    defer person.deinit();
    person.greet();

    // 直接使用（需要手动管理内存）
    const p = try c_wrapper.Person.create("Bob", 25);
    defer p.destroy();
    p.greet();
}
