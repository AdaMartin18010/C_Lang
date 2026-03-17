# Zig 机器学习基础：张量运算与神经网络

> **定位**: Zig 应用 / 机器学习 | **难度**: ⭐⭐⭐⭐⭐ | **目标**: ML 基础实现

---

## 📋 目录

- [Zig 机器学习基础：张量运算与神经网络](#zig-机器学习基础张量运算与神经网络)
  - [📋 目录](#-目录)
  - [📑 目录](#-目录-1)
  - [一、张量基础](#一张量基础)
    - [1.1 张量实现](#11-张量实现)
    - [1.2 张量运算](#12-张量运算)
  - [二、自动微分](#二自动微分)
  - [三、层与网络](#三层与网络)
  - [四、优化器](#四优化器)
  - [五、数据加载](#五数据加载)
  - [六、实战：MNIST 分类](#六实战mnist-分类)

---


---

## 📑 目录

- [Zig 机器学习基础：张量运算与神经网络](#zig-机器学习基础张量运算与神经网络)
  - [📋 目录](#-目录)
  - [📑 目录](#-目录-1)
  - [一、张量基础](#一张量基础)
    - [1.1 张量实现](#11-张量实现)
    - [1.2 张量运算](#12-张量运算)
  - [二、自动微分](#二自动微分)
  - [三、层与网络](#三层与网络)
  - [四、优化器](#四优化器)
  - [五、数据加载](#五数据加载)
  - [六、实战：MNIST 分类](#六实战mnist-分类)


---

## 一、张量基础

### 1.1 张量实现

```zig
const std = @import("std");

pub const Tensor = struct {
    allocator: std.mem.Allocator,
    data: []f32,
    shape: []const usize,
    strides: []usize,

    pub fn init(allocator: std.mem.Allocator, shape: []const usize) !Tensor {
        var size: usize = 1;
        for (shape) |dim| size *= dim;

        const data = try allocator.alloc(f32, size);
        @memset(data, 0);

        const strides = try allocator.alloc(usize, shape.len);
        var stride: usize = 1;
        var i: usize = shape.len;
        while (i > 0) : (i -= 1) {
            strides[i - 1] = stride;
            stride *= shape[i - 1];
        }

        return .{
            .allocator = allocator,
            .data = data,
            .shape = try allocator.dupe(usize, shape),
            .strides = strides,
        };
    }

    pub fn initRandom(allocator: std.mem.Allocator, shape: []const usize, min: f32, max: f32) !Tensor {
        var tensor = try init(allocator, shape);

        var prng = std.rand.DefaultPrng.init(@intCast(std.time.milliTimestamp()));
        const random = prng.random();

        for (tensor.data) |*val| {
            val.* = min + random.float(f32) * (max - min);
        }

        return tensor;
    }

    pub fn deinit(self: Tensor) void {
        self.allocator.free(self.data);
        self.allocator.free(self.shape);
        self.allocator.free(self.strides);
    }

    pub fn get(self: Tensor, indices: []const usize) f32 {
        var idx: usize = 0;
        for (indices, self.strides) |i, s| {
            idx += i * s;
        }
        return self.data[idx];
    }

    pub fn set(self: Tensor, indices: []const usize, value: f32) void {
        var idx: usize = 0;
        for (indices, self.strides) |i, s| {
            idx += i * s;
        }
        self.data[idx] = value;
    }

    pub fn shapeEqual(self: Tensor, other: Tensor) bool {
        if (self.shape.len != other.shape.len) return false;
        for (self.shape, other.shape) |a, b| {
            if (a != b) return false;
        }
        return true;
    }

    pub fn format(
        self: Tensor,
        comptime fmt: []const u8,
        options: std.fmt.FormatOptions,
        writer: anytype,
    ) !void {
        _ = fmt;
        _ = options;

        try writer.print("Tensor(", .{});
        for (self.shape, 0..) |dim, i| {
            if (i > 0) try writer.print(", ", .{});
            try writer.print("{}", .{dim});
        }
        try writer.print(") [", .{});

        const show_count = @min(self.data.len, 10);
        for (self.data[0..show_count], 0..) |val, i| {
            if (i > 0) try writer.print(", ", .{});
            try writer.print("{d:.3}", .{val});
        }

        if (self.data.len > 10) {
            try writer.print(", ... ({} more)", .{self.data.len - 10});
        }

        try writer.print("]", .{});
    }
};
```

### 1.2 张量运算

```zig
pub const TensorOps = struct {
    // 逐元素加法
    pub fn add(a: Tensor, b: Tensor, out: Tensor) !void {
        if (!a.shapeEqual(b) or !a.shapeEqual(out)) return error.ShapeMismatch;

        const V = @Vector(8, f32);
        var i: usize = 0;

        while (i + 8 <= a.data.len) : (i += 8) {
            const va: V = a.data[i..][0..8].*;
            const vb: V = b.data[i..][0..8].*;
            out.data[i..][0..8].* = va + vb;
        }

        while (i < a.data.len) : (i += 1) {
            out.data[i] = a.data[i] + b.data[i];
        }
    }

    // 逐元素乘法
    pub fn mul(a: Tensor, b: Tensor, out: Tensor) !void {
        if (!a.shapeEqual(b) or !a.shapeEqual(out)) return error.ShapeMismatch;

        for (a.data, b.data, out.data) |va, vb, *vo| {
            vo.* = va * vb;
        }
    }

    // ReLU
    pub fn relu(input: Tensor, out: Tensor) !void {
        if (!input.shapeEqual(out)) return error.ShapeMismatch;

        for (input.data, out.data) |vi, *vo| {
            vo.* = if (vi > 0) vi else 0;
        }
    }

    // Sigmoid
    pub fn sigmoid(input: Tensor, out: Tensor) !void {
        if (!input.shapeEqual(out)) return error.ShapeMismatch;

        for (input.data, out.data) |vi, *vo| {
            vo.* = 1.0 / (1.0 + std.math.exp(-vi));
        }
    }

    // Softmax
    pub fn softmax(input: Tensor, out: Tensor) !void {
        if (!input.shapeEqual(out)) return error.ShapeMismatch;
        if (input.shape.len != 1) return error.InvalidShape;

        // 找最大值 (数值稳定)
        var max_val = input.data[0];
        for (input.data[1..]) |val| {
            if (val > max_val) max_val = val;
        }

        // 计算 exp 和总和
        var sum: f32 = 0;
        for (input.data, out.data) |vi, *vo| {
            const exp_val = std.math.exp(vi - max_val);
            vo.* = exp_val;
            sum += exp_val;
        }

        // 归一化
        for (out.data) |*vo| {
            vo.* /= sum;
        }
    }

    // 矩阵乘法
    pub fn matmul(a: Tensor, b: Tensor, out: Tensor) !void {
        if (a.shape.len != 2 or b.shape.len != 2 or out.shape.len != 2) {
            return error.InvalidShape;
        }

        const m = a.shape[0];
        const n = a.shape[1];
        const p = b.shape[1];

        if (b.shape[0] != n or out.shape[0] != m or out.shape[1] != p) {
            return error.ShapeMismatch;
        }

        // 基础实现
        var i: usize = 0;
        while (i < m) : (i += 1) {
            var j: usize = 0;
            while (j < p) : (j += 1) {
                var sum: f32 = 0;
                var k: usize = 0;
                while (k < n) : (k += 1) {
                    sum += a.get(&.{i, k}) * b.get(&.{k, j});
                }
                out.set(&.{i, j}, sum);
            }
        }
    }

    // 广播加法
    pub fn broadcastAdd(a: Tensor, b: f32, out: Tensor) !void {
        if (!a.shapeEqual(out)) return error.ShapeMismatch;

        for (a.data, out.data) |va, *vo| {
            vo.* = va + b;
        }
    }
};
```

---

## 二、自动微分

```zig
// 自动微分基础
pub const Variable = struct {
    data: f32,
    grad: f32,
    requires_grad: bool,

    pub fn init(data: f32, requires_grad: bool) Variable {
        return .{
            .data = data,
            .grad = 0,
            .requires_grad = requires_grad,
        };
    }

    pub fn zeroGrad(self: *Variable) void {
        self.grad = 0;
    }

    pub fn backward(self: *Variable) void {
        if (self.requires_grad) {
            self.grad = 1.0;
        }
    }
};

// 计算图节点
pub const Node = struct {
    inputs: []Variable,
    output: Variable,
    op: OpType,

    pub const OpType = enum {
        add,
        mul,
        relu,
        sigmoid,
    };

    pub fn forward(self: *Node) void {
        switch (self.op) {
            .add => {
                self.output.data = self.inputs[0].data + self.inputs[1].data;
            },
            .mul => {
                self.output.data = self.inputs[0].data * self.inputs[1].data;
            },
            .relu => {
                self.output.data = if (self.inputs[0].data > 0) self.inputs[0].data else 0;
            },
            .sigmoid => {
                self.output.data = 1.0 / (1.0 + std.math.exp(-self.inputs[0].data));
            },
        }
    }

    pub fn backward(self: *Node) void {
        if (!self.output.requires_grad) return;

        const grad = self.output.grad;

        switch (self.op) {
            .add => {
                if (self.inputs[0].requires_grad) self.inputs[0].grad += grad;
                if (self.inputs[1].requires_grad) self.inputs[1].grad += grad;
            },
            .mul => {
                if (self.inputs[0].requires_grad) self.inputs[0].grad += grad * self.inputs[1].data;
                if (self.inputs[1].requires_grad) self.inputs[1].grad += grad * self.inputs[0].data;
            },
            .relu => {
                if (self.inputs[0].requires_grad) {
                    self.inputs[0].grad += if (self.inputs[0].data > 0) grad else 0;
                }
            },
            .sigmoid => {
                if (self.inputs[0].requires_grad) {
                    const s = self.output.data;
                    self.inputs[0].grad += grad * s * (1 - s);
                }
            },
        }
    }
};

// 计算图
pub const Graph = struct {
    allocator: std.mem.Allocator,
    nodes: std.ArrayList(Node),

    pub fn init(allocator: std.mem.Allocator) Graph {
        return .{
            .allocator = allocator,
            .nodes = std.ArrayList(Node).init(allocator),
        };
    }

    pub fn deinit(self: *Graph) void {
        self.nodes.deinit();
    }

    pub fn addNode(self: *Graph, node: Node) !void {
        try self.nodes.append(node);
    }

    pub fn forward(self: *Graph) void {
        for (self.nodes.items) |*node| {
            node.forward();
        }
    }

    pub fn backward(self: *Graph) void {
        var i: i32 = @intCast(self.nodes.items.len - 1);
        while (i >= 0) : (i -= 1) {
            self.nodes.items[@intCast(i)].backward();
        }
    }
};
```

---

## 三、层与网络

```zig
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
```

---

## 四、优化器

```zig
// SGD 优化器
pub const SGD = struct {
    learning_rate: f32,
    momentum: f32,
    velocity: ?Tensor,

    pub fn init(learning_rate: f32, momentum: f32) SGD {
        return .{
            .learning_rate = learning_rate,
            .momentum = momentum,
            .velocity = null,
        };
    }

    pub fn step(self: *SGD, params: *Tensor, grads: Tensor) !void {
        // params = params - lr * grads
        for (params.data, grads.data) |*p, g| {
            p.* -= self.learning_rate * g;
        }
    }
};

// Adam 优化器
pub const Adam = struct {
    learning_rate: f32,
    beta1: f32,
    beta2: f32,
    epsilon: f32,
    t: u32,

    m: ?Tensor,  // 一阶矩
    v: ?Tensor,  // 二阶矩

    pub fn init(learning_rate: f32) Adam {
        return .{
            .learning_rate = learning_rate,
            .beta1 = 0.9,
            .beta2 = 0.999,
            .epsilon = 1e-8,
            .t = 0,
            .m = null,
            .v = null,
        };
    }

    pub fn step(self: *Adam, params: *Tensor, grads: Tensor, allocator: std.mem.Allocator) !void {
        self.t += 1;

        // 初始化 m 和 v
        if (self.m == null) {
            self.m = try Tensor.init(allocator, params.shape);
            self.v = try Tensor.init(allocator, params.shape);
        }

        const m = &self.m.?;
        const v = &self.v.?;

        const lr = self.learning_rate;
        const beta1 = self.beta1;
        const beta2 = self.beta2;
        const eps = self.epsilon;

        for (params.data, grads.data, m.data, v.data) |*p, g, *mi, *vi| {
            // 更新一阶矩和二阶矩
            mi.* = beta1 * mi.* + (1 - beta1) * g;
            vi.* = beta2 * vi.* + (1 - beta2) * g * g;

            // 偏差修正
            const m_hat = mi.* / (1 - std.math.pow(f32, beta1, @floatFromInt(self.t)));
            const v_hat = vi.* / (1 - std.math.pow(f32, beta2, @floatFromInt(self.t)));

            // 更新参数
            p.* -= lr * m_hat / (std.math.sqrt(v_hat) + eps);
        }
    }
};
```

---

## 五、数据加载

```zig
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
```

---

## 六、实战：MNIST 分类

```zig
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
```

---

> **文档信息**
>
> - 创建时间: 2026-03-10
