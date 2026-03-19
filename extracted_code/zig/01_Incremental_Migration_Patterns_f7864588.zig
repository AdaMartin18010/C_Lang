//
// Auto-generated from: 08_Zig_C_Connection\05_Migration_Methodology\01_Incremental_Migration_Patterns.md
// Line: 1935
// Language: zig
// Block ID: f7864588
//
// This file was automatically extracted from the C_Lang knowledge base.
// DO NOT MODIFY - Changes will be overwritten on next extraction.
//

// 嵌入式驱动 Zig 实现示例

const std = @import("std");

/// 强类型寄存器定义
const Register = packed struct(u32) {
    enable: u1,
    mode: u3,
    prescaler: u8,
    reserved: u20,
};

/// MMIO 寄存器访问
pub fn Mmio(comptime T: type) type {
    return extern struct {
        raw_volatile: volatile T,

        pub inline fn read(self: @This()) T {
            return self.raw_volatile;
        }

        pub inline fn write(self: *@This(), value: T) void {
            self.raw_volatile = value;
        }

        pub inline fn modify(self: *@This(), f: fn (*T) void) void {
            var val = self.read();
            f(&val);
            self.write(val);
        }
    };
}

/// 传感器设备驱动
pub fn SensorDriver(comptime Config: SensorConfig) type {
    return struct {
        const Self = @This();

        /// 编译时验证配置
        comptime {
            if (Config.sample_rate > 1000) {
                @compileError("Sample rate too high");
            }
            if (Config.buffer_size % 4 != 0) {
                @compileError("Buffer size must be multiple of 4");
            }
        }

        /// 静态分配的缓冲区
        buffer: [Config.buffer_size]u8,
        state: State,

        const State = enum {
            idle,
            sampling,
            processing,
        };

        pub fn init() Self {
            return .{
                .buffer = undefined,
                .state = .idle,
            };
        }

        /// 启动采样（无分配）
        pub fn startSampling(self: *Self) !void {
            if (self.state != .idle) {
                return error.Busy;
            }

            // 配置硬件
            ControlReg.modify(struct {
                fn f(r: *Register) void {
                    r.enable = 1;
                    r.mode = Config.mode;
                    r.prescaler = Config.prescaler;
                }
            }.f);

            self.state = .sampling;
        }

        /// 中断处理（无堆分配）
        pub fn handleInterrupt(self: *Self) void {
            const status = StatusReg.read();

            if (status & 0x01 != 0) {
                // 数据就绪
                const sample = DataReg.read();
                self.processSample(sample);
            }

            if (status & 0x02 != 0) {
                // 错误处理
                self.handleError();
            }
        }

        fn processSample(self: *Self, sample: u16) void {
            _ = self;
            _ = sample;
            // 处理样本数据
        }

        fn handleError(self: *Self) void {
            self.state = .idle;
            // 错误恢复
        }
    };
}

const SensorConfig = struct {
    sample_rate: u16,
    buffer_size: usize,
    mode: u3,
    prescaler: u8,
};

// 外设寄存器定义
const ControlReg: *Mmio(Register) = @ptrFromInt(0x4000_0000);
const StatusReg: *Mmio(u32) = @ptrFromInt(0x4000_0004);
const DataReg: *Mmio(u16) = @ptrFromInt(0x4000_0008);

// 具体驱动实例
pub const TempSensor = SensorDriver(.{
    .sample_rate = 100,
    .buffer_size = 256,
    .mode = 0b001,
    .prescaler = 16,
});
