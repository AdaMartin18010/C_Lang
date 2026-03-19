//
// Auto-generated from: Zig\Zig_Machine_Learning.md
// Line: 670
// Language: zig
// Block ID: e4edf3f5
//
// This file was automatically extracted from the C_Lang knowledge base.
// DO NOT MODIFY - Changes will be overwritten on next extraction.
//

// MNIST 分类器
pub const MNISTClassifier = struct {
    allocator: std.mem.Allocator,
    net: NeuralNetwork,
    optimizer: Adam,

    pub fn init(allocator: std.mem.Allocator) !MNISTClassifier {
        var net = NeuralNetwork.init(allocator);

        // 784 (28x28) -> 128 -> 64 -> 10
        try net.addLinear(784, 128);
        try net.addReLU();
        try net.addLinear(128, 64);
        try net.addReLU();
        try net.addLinear(64, 10);

        return .{
            .allocator = allocator,
            .net = net,
            .optimizer = Adam.init(0.001),
        };
    }

    pub fn deinit(self: *MNISTClassifier) void {
        self.net.deinit();
    }

    pub fn train(self: *MNISTClassifier, data: Tensor, labels: Tensor, epochs: usize) !void {
        const batch_size = 32;
        var loader = try DataLoader.init(self.allocator, data, labels, batch_size, true);
        defer loader.deinit();

        var epoch: usize = 0;
        while (epoch < epochs) : (epoch += 1) {
            var total_loss: f32 = 0;
            var batch_count: usize = 0;

            loader.reset();
            while (loader.next()) |batch| {
                const batch_data = batch[0];
                const batch_labels = batch[1];

                // 前向传播
                var output = try Tensor.init(self.allocator, &.{ batch_size, 10 });
                defer output.deinit();

                try self.net.forward(batch_data, output);

                // 计算损失 (交叉熵)
                const loss = try self.computeLoss(output, batch_labels);
                total_loss += loss;
                batch_count += 1;

                // 反向传播 (简化)
                // ...

                // 更新参数
                // self.optimizer.step(...)
            }

            std.log.info("Epoch {}: Loss = {d:.4}", .{ epoch + 1, total_loss / @as(f32, @floatFromInt(batch_count)) });
        }
    }

    pub fn predict(self: MNISTClassifier, input: Tensor, allocator: std.mem.Allocator) !Tensor {
        var output = try Tensor.init(allocator, &.{ input.shape[0], 10 });
        errdefer output.deinit();

        try self.net.forward(input, output);

        // 应用 softmax
        var probs = try Tensor.init(allocator, output.shape);
        errdefer probs.deinit();

        for (0..input.shape[0]) |i| {
            const row_out = output.data[i * 10 ..][0..10];
            const row_probs = probs.data[i * 10 ..][0..10];

            // softmax
            var max_val = row_out[0];
            for (row_out[1..]) |val| {
                if (val > max_val) max_val = val;
            }

            var sum: f32 = 0;
            for (row_out, row_probs) |v, *p| {
                const exp_v = std.math.exp(v - max_val);
                p.* = exp_v;
                sum += exp_v;
            }

            for (row_probs) |*p| {
                p.* /= sum;
            }
        }

        return probs;
    }

    fn computeLoss(self: MNISTClassifier, output: Tensor, labels: Tensor) !f32 {
        _ = self;
        // 交叉熵损失
        var loss: f32 = 0;

        for (0..output.shape[0]) |i| {
            const label = @as(usize, @intFromFloat(labels.data[i]));
            const prob = output.data[i * 10 + label];
            loss -= std.math.log(prob + 1e-8);
        }

        return loss / @as(f32, @floatFromInt(output.shape[0]));
    }
};

// 主函数
pub fn main() !void {
    var gpa = std.heap.GeneralPurposeAllocator(.{}){};
    defer _ = gpa.deinit();
    const allocator = gpa.allocator();

    // 加载 MNIST 数据 (需要实现数据加载)
    // const train_data = try loadMNIST(allocator, "train-images.idx3-ubyte");
    // const train_labels = try loadMNISTLabels(allocator, "train-labels.idx1-ubyte");

    // 创建并训练模型
    var classifier = try MNISTClassifier.init(allocator);
    defer classifier.deinit();

    // try classifier.train(train_data, train_labels, 10);

    std.log.info("Training complete!", .{});
}
