//
// Auto-generated from: Zig\Zig_Machine_Learning.md
// Line: 395
// Language: zig
// Block ID: 2636540b
//
// This file was automatically extracted from the C_Lang knowledge base.
// DO NOT MODIFY - Changes will be overwritten on next extraction.
//

// 神经网络层
pub const Linear = struct {
    allocator: std.mem.Allocator,
    in_features: usize,
    out_features: usize,
    weight: Tensor,
    bias: Tensor,

    pub fn init(allocator: std.mem.Allocator, in_features: usize, out_features: usize) !Linear {
        // Xavier 初始化
        const scale = std.math.sqrt(2.0 / @as(f32, @floatFromInt(in_features)));

        const weight = try Tensor.initRandom(
            allocator,
            &.{ out_features, in_features },
            -scale,
            scale,
        );

        const bias = try Tensor.init(allocator, &.{out_features});

        return .{
            .allocator = allocator,
            .in_features = in_features,
            .out_features = out_features,
            .weight = weight,
            .bias = bias,
        };
    }

    pub fn deinit(self: Linear) void {
        self.weight.deinit();
        self.bias.deinit();
    }

    pub fn forward(self: Linear, input: Tensor, output: Tensor) !void {
        // output = input @ weight.T + bias
        // 简化实现
        _ = self;
        _ = input;
        _ = output;
    }
};

pub const ReLULayer = struct {
    pub fn forward(input: Tensor, output: Tensor) !void {
        try TensorOps.relu(input, output);
    }
};

// 神经网络
pub const NeuralNetwork = struct {
    allocator: std.mem.Allocator,
    layers: std.ArrayList(Layer),

    pub const Layer = union(enum) {
        linear: Linear,
        relu: ReLULayer,
    };

    pub fn init(allocator: std.mem.Allocator) NeuralNetwork {
        return .{
            .allocator = allocator,
            .layers = std.ArrayList(Layer).init(allocator),
        };
    }

    pub fn deinit(self: *NeuralNetwork) void {
        for (self.layers.items) |layer| {
            switch (layer) {
                .linear => |l| l.deinit(),
                .relu => {},
            }
        }
        self.layers.deinit();
    }

    pub fn addLinear(self: *NeuralNetwork, in_features: usize, out_features: usize) !void {
        const linear = try Linear.init(self.allocator, in_features, out_features);
        try self.layers.append(.{ .linear = linear });
    }

    pub fn addReLU(self: *NeuralNetwork) !void {
        try self.layers.append(.{ .relu = .{} });
    }

    pub fn forward(self: NeuralNetwork, input: Tensor, output: Tensor) !void {
        var current = input;
        var temp = try Tensor.init(self.allocator, input.shape);
        defer temp.deinit();

        for (self.layers.items) |layer| {
            switch (layer) {
                .linear => |l| {
                    try l.forward(current, temp);
                    std.mem.swap(Tensor, &current, &temp);
                },
                .relu => {
                    try ReLULayer.forward(current, temp);
                    std.mem.swap(Tensor, &current, &temp);
                },
            }
        }

        // 复制到输出
        @memcpy(output.data, current.data);
    }
};
