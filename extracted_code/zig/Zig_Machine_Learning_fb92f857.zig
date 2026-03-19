//
// Auto-generated from: Zig\Zig_Machine_Learning.md
// Line: 510
// Language: zig
// Block ID: fb92f857
//
// This file was automatically extracted from the C_Lang knowledge base.
// DO NOT MODIFY - Changes will be overwritten on next extraction.
//

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
