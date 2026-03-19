//
// Auto-generated from: 08_Zig_C_Connection\06_C23_Zig_Parallel\02_Constexpr_vs_Comptime.md
// Line: 1635
// Language: zig
// Block ID: 549d551b
//
// This file was automatically extracted from the C_Lang knowledge base.
// DO NOT MODIFY - Changes will be overwritten on next extraction.
//

// Zig: 强大的代码生成能力
const std = @import("std");

// ========== 编译时生成查找表 ==========

// 生成正弦查找表
const SIN_TABLE = comptime blk: {
    var table: [360]f32 = undefined;
    for (0..360) |i| {
        const rad = @as(f32, @floatFromInt(i)) * std.math.pi / 180.0;
        table[i] = @sin(rad);
    }
    break :blk table;
};

// 生成 CRC 查找表
const CRC_TABLE = comptime blk: {
    var table: [256]u32 = undefined;
    for (0..256) |i| {
        var crc: u32 = @intCast(i);
        var j: u8 = 0;
        while (j < 8) : (j += 1) {
            if (crc & 1 != 0) {
                crc = (crc >> 1) ^ 0xEDB88320;
            } else {
                crc >>= 1;
            }
        }
        table[i] = crc;
    }
    break :blk table;
};

// ========== 自动生成枚举和映射 ==========

fn makeEnumAndMap(comptime names: []const []const u8) type {
    var enum_fields: [names.len]std.builtin.Type.EnumField = undefined;
    var map_entries: [names.len]struct { []const u8, usize } = undefined;

    for (names, 0..) |name, i| {
        enum_fields[i] = .{
            .name = name,
            .value = i,
        };
        map_entries[i] = .{ name, i };
    }

    return struct {
        pub const Enum = @Type(.{
            .Enum = .{
                .tag_type = u32,
                .fields = &enum_fields,
                .decls = &.{},
                .is_exhaustive = true,
            },
        });

        pub const NameMap = std.StaticStringMap(usize).initComptime(&map_entries);
    };
}

// 使用
const ColorSet = makeEnumAndMap(&.{ "red", "green", "blue", "yellow" });
const Color = ColorSet.Enum;

// Color.red, Color.green, ... 自动可用
// ColorSet.NameMap.get("red") 返回 0

// ========== 自动生成操作函数 ==========

fn makeVectorOps(comptime n: usize, comptime T: type) type {
    return struct {
        pub const Vec = @Vector(n, T);

        pub inline fn add(a: Vec, b: Vec) Vec {
            return a + b;
        }

        pub inline fn sub(a: Vec, b: Vec) Vec {
            return a - b;
        }

        pub inline fn mul(a: Vec, b: Vec) Vec {
            return a * b;
        }

        pub inline fn dot(a: Vec, b: Vec) T {
            var sum: T = 0;
            const prod = a * b;
            inline for (0..n) |i| {
                sum += prod[i];
            }
            return sum;
        }
    };
}

// 生成 3D 浮点向量操作
const Vec3fOps = makeVectorOps(3, f32);

// ========== 条件代码生成 ==========

fn makeOptimizedCopy(comptime T: type) fn ([]T, []const T) void {
    comptime {
        if (@sizeOf(T) == 1) {
            // 字节拷贝优化
            return struct {
                fn copy(dest: []T, src: []const T) void {
                    @memcpy(dest[0..src.len], src);
                }
            }.copy;
        } else if (@alignOf(T) >= 8) {
            // 8 字节对齐优化
            return struct {
                fn copy(dest: []T, src: []const T) void {
                    // SIMD 或宽拷贝...
                    for (dest, src) |*d, s| {
                        d.* = s;
                    }
                }
            }.copy;
        } else {
            // 通用实现
            return struct {
                fn copy(dest: []T, src: []const T) void {
                    for (dest, src) |*d, s| {
                        d.* = s;
                    }
                }
            }.copy;
        }
    }
}

const copyU8 = makeOptimizedCopy(u8);
const copyU64 = makeOptimizedCopy(u64);
