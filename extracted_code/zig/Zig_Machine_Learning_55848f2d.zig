//
// Auto-generated from: Zig\Zig_Machine_Learning.md
// Line: 155
// Language: zig
// Block ID: 55848f2d
//
// This file was automatically extracted from the C_Lang knowledge base.
// DO NOT MODIFY - Changes will be overwritten on next extraction.
//

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
