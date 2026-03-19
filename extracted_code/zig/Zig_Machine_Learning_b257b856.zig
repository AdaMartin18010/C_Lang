//
// Auto-generated from: Zig\Zig_Machine_Learning.md
// Line: 271
// Language: zig
// Block ID: b257b856
//
// This file was automatically extracted from the C_Lang knowledge base.
// DO NOT MODIFY - Changes will be overwritten on next extraction.
//

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
