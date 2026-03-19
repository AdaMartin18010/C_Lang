//
// Auto-generated from: Zig\Zig_Security_Cryptography.md
// Line: 705
// Language: zig
// Block ID: 89d31666
//
// This file was automatically extracted from the C_Lang knowledge base.
// DO NOT MODIFY - Changes will be overwritten on next extraction.
//

// 自动清零的秘密数据
pub fn Secret(comptime T: type) type {
    return struct {
        data: T,

        pub fn init(data: T) @This() {
            return .{ .data = data };
        }

        pub fn deinit(self: *@This()) void {
            // 清零内存
            const bytes = std.mem.asBytes(&self.data);
            crypto.utils.secureZero(u8, bytes);
        }

        pub fn get(self: @This()) T {
            return self.data;
        }
    };
}
