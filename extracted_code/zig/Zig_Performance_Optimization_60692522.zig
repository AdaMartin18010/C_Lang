//
// Auto-generated from: Zig\Zig_Performance_Optimization.md
// Line: 651
// Language: zig
// Block ID: 60692522
//
// This file was automatically extracted from the C_Lang knowledge base.
// DO NOT MODIFY - Changes will be overwritten on next extraction.
//

// 软件预取
pub fn prefetchExample(data: []const u8) u64 {
    var sum: u64 = 0;
    const PREFETCH_DISTANCE = 64;  // 提前 64 字节

    var i: usize = 0;
    while (i < data.len) : (i += 1) {
        // 预取未来数据
        if (i + PREFETCH_DISTANCE < data.len) {
            @prefetch(&data[i + PREFETCH_DISTANCE], .{ .rw = .read, .locality = 3 });
        }

        sum += data[i];
    }

    return sum;
}
