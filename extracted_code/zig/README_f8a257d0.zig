//
// Auto-generated from: Zig\README.md
// Line: 1323
// Language: zig
// Block ID: f8a257d0
//
// This file was automatically extracted from the C_Lang knowledge base.
// DO NOT MODIFY - Changes will be overwritten on next extraction.
//

// 适用于任何可比较类型的泛型排序
fn bubbleSort(comptime T: type, items: []T) void {
    var i: usize = 0;
    while (i < items.len) : (i += 1) {
        var j: usize = 0;
        while (j < items.len - i - 1) : (j += 1) {
            if (items[j] > items[j + 1]) {
                const tmp = items[j];
                items[j] = items[j + 1];
                items[j + 1] = tmp;
            }
        }
    }
}

// 使用
var numbers = [_]i32{ 5, 3, 8, 1, 2 };
bubbleSort(i32, &numbers);
