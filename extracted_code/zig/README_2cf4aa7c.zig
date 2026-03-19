//
// Auto-generated from: 08_Zig_C_Connection\04_Translation_Validation\README.md
// Line: 81
// Language: zig
// Block ID: 2cf4aa7c
//
// This file was automatically extracted from the C_Lang knowledge base.
// DO NOT MODIFY - Changes will be overwritten on next extraction.
//

/// c2zig 生成的更符合 Zig 风格代码的示例
const std = @import("std");

/// C 原始代码:
/// typedef struct {
///     char* name;
///     int age;
///     struct Person* next;
/// } Person;
///
/// Person* create_person(const char* name, int age);

// c2zig 生成的 Zig 代码:
pub const Person = extern struct {
    name: ?[*:0]u8,  // 可空、以null结尾的指针
    age: c_int,
    next: ?*Person,  // 可选指针表示可能为null
};

/// Zig 风格的创建函数 - 返回错误而非 null
pub fn createPerson(name: []const u8, age: i32, allocator: std.mem.Allocator) !*Person {
    const person = try allocator.create(Person);
    errdefer allocator.destroy(person);

    person.name = try allocator.dupeZ(u8, name);
    errdefer allocator.free(std.mem.span(person.name));

    person.age = age;
    person.next = null;

    return person;
}

pub fn destroyPerson(person: *Person, allocator: std.mem.Allocator) void {
    if (person.name) |name| {
        allocator.free(std.mem.span(name));
    }
    allocator.destroy(person);
}
