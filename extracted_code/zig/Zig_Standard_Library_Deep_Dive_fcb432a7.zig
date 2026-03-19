//
// Auto-generated from: Zig\Zig_Standard_Library_Deep_Dive.md
// Line: 394
// Language: zig
// Block ID: fcb432a7
//
// This file was automatically extracted from the C_Lang knowledge base.
// DO NOT MODIFY - Changes will be overwritten on next extraction.
//

fn hashmap_examples() !void {
    const allocator = std.heap.page_allocator;

    // StringHashMap - 字符串键
    var map = std.StringHashMap(i32).init(allocator);
    defer map.deinit();

    // 插入
    try map.put("one", 1);
    try map.put("two", 2);
    try map.put("three", 3);

    // 获取
    if (map.get("two")) |value| {
        std.log.info("two = {}", .{value});
    }

    // 获取或插入
    const entry = try map.getOrPut("four");
    if (!entry.found_existing) {
        entry.value_ptr.* = 4;
    }

    // 更新
    try map.put("one", 10);  // 覆盖

    // 删除
    _ = map.remove("three");

    // 遍历
    var it = map.iterator();
    while (it.next()) |kv| {
        std.log.info("{s} => {}", .{ kv.key_ptr.*, kv.value_ptr.* });
    }

    // 获取键值对数量
    std.log.info("Count: {}", .{map.count()});

    // 清空
    map.clearRetainingCapacity();  // 保留容量

    // AutoHashMap - 任意类型键
    var int_map = std.AutoHashMap(i32, []const u8).init(allocator);
    defer int_map.deinit();

    try int_map.put(1, "one");
    try int_map.put(2, "two");

    // 自定义哈希
    const Person = struct {
        name: []const u8,
        age: u32,

        pub fn hash(self: @This(), hasher: *std.hash.Wyhash) void {
            hasher.update(self.name);
            hasher.update(std.mem.asBytes(&self.age));
        }

        pub fn eql(self: @This(), other: @This()) bool {
            return std.mem.eql(u8, self.name, other.name) and
                   self.age == other.age;
        }
    };

    var person_map = std.HashMap(Person, i32, Person.hash, Person.eql, std.hash_map.default_max_load_percentage).init(allocator);
    defer person_map.deinit();

    try person_map.put(.{ .name = "Alice", .age = 30 }, 100);
}
