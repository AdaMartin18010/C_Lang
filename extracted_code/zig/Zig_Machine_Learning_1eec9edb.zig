//
// Auto-generated from: Zig\Zig_Machine_Learning.md
// Line: 593
// Language: zig
// Block ID: 1eec9edb
//
// This file was automatically extracted from the C_Lang knowledge base.
// DO NOT MODIFY - Changes will be overwritten on next extraction.
//

// 数据加载器
pub const DataLoader = struct {
    allocator: std.mem.Allocator,
    data: Tensor,
    labels: Tensor,
    batch_size: usize,
    shuffle: bool,
    indices: []usize,
    current: usize,

    pub fn init(
        allocator: std.mem.Allocator,
        data: Tensor,
        labels: Tensor,
        batch_size: usize,
        shuffle: bool,
    ) !DataLoader {
        var indices = try allocator.alloc(usize, data.shape[0]);
        for (indices, 0..) |*idx, i| {
            idx.* = i;
        }

        if (shuffle) {
            var prng = std.rand.DefaultPrng.init(@intCast(std.time.milliTimestamp()));
            const random = prng.random();
            std.rand.Random.shuffle(random, usize, indices);
        }

        return .{
            .allocator = allocator,
            .data = data,
            .labels = labels,
            .batch_size = batch_size,
            .shuffle = shuffle,
            .indices = indices,
            .current = 0,
        };
    }

    pub fn deinit(self: *DataLoader) void {
        self.allocator.free(self.indices);
    }

    pub fn next(self: *DataLoader) ?struct { Tensor, Tensor } {
        if (self.current >= self.data.shape[0]) return null;

        const end = @min(self.current + self.batch_size, self.data.shape[0]);
        const actual_batch = end - self.current;

        // 创建批次张量
        const batch_data = self.data;
        const batch_labels = self.labels;

        // 注意: 这里应该复制数据到新的张量
        // 简化处理，实际应该创建子张量或复制数据

        self.current = end;
        return .{ batch_data, batch_labels };
    }

    pub fn reset(self: *DataLoader) void {
        self.current = 0;

        if (self.shuffle) {
            var prng = std.rand.DefaultPrng.init(@intCast(std.time.milliTimestamp()));
            const random = prng.random();
            std.rand.Random.shuffle(random, usize, self.indices);
        }
    }
};
