//
// Auto-generated from: Zig\README.md
// Line: 1346
// Language: zig
// Block ID: ab52f1af
//
// This file was automatically extracted from the C_Lang knowledge base.
// DO NOT MODIFY - Changes will be overwritten on next extraction.
//

// 检查类型是否实现了必要的方法
fn hasDrawMethod(comptime T: type) bool {
    return @hasDecl(T, "draw");
}

fn renderAll(items: anytype) void {
    const T = @TypeOf(items);
    comptime assert(hasDrawMethod(T));

    for (items) |item| {
        item.draw();
    }
}
