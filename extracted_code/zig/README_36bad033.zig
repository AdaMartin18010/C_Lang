//
// Auto-generated from: Zig\README.md
// Line: 1429
// Language: zig
// Block ID: 36bad033
//
// This file was automatically extracted from the C_Lang knowledge base.
// DO NOT MODIFY - Changes will be overwritten on next extraction.
//

// 零分配日志系统
const RingBuffer = struct {
    buffer: [4096]u8,
    head: usize,
    tail: usize,

    pub fn write(self: *@This(), data: []const u8) void {
        for (data) |byte| {
            self.buffer[self.head] = byte;
            self.head = (self.head + 1) % self.buffer.len;
            if (self.head == self.tail) {
                self.tail = (self.tail + 1) % self.buffer.len;
            }
        }
    }
};
