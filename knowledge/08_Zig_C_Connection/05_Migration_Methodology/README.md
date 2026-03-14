# C 到 Zig 迁移方法论

本目录提供从 C 代码库向 Zig 迁移的系统方法论，涵盖策略选择、混合代码库管理、构建系统集成以及风险缓解的完整指南。

## 目录结构

| 文件 | 主题 | 适用阶段 |
|------|------|----------|
| `01_Incremental_Migration_Patterns.md` | 渐进式迁移模式 | 规划/执行 |
| `02_Safety_Improvement_Metrics.md` | 安全性改进度量 | 评估/监控 |
| `03_Performance_Comparison_Framework.md` | 性能对比框架 | 验证/优化 |

## 迁移策略选择

### 大爆炸迁移 vs 渐进式迁移

```text
┌─────────────────────────────────────────────────────────────────┐
│                      迁移策略对比                                │
├─────────────────────────────────────────────────────────────────┤
│                                                                 │
│   大爆炸迁移                        渐进式迁移                   │
│   ┌──────────────┐                 ┌─────────────────────────┐  │
│   │   C 代码库    │                 │   C 代码库               │  │
│   │  (100%)      │                 │  ┌─────────────────┐    │  │
│   └──────┬───────┘                 │  │ C 核心           │    │  │
│          │  一次重写                 │  └────────┬────────┘    │  │
│          ▼                          │           │             │  │
│   ┌──────────────┐                 │  ┌────────┴────────┐     │  │
│   │   Zig 代码库  │                 │  │ Zig 模块 1       │     │  │
│   │  (100%)      │                 │  └────────┬────────┘     │  │
│   └──────────────┘                 │           │              │  │
│                                     │  ┌────────┴────────┐     │  │
│   特点: 快速切换                     │  │ Zig 模块 2       │     │  │
│   风险: 高 - 全部重写                │  └─────────────────┘     │  │
│   适合: 小型项目 (<10K LOC)          │                          │  │
│                                     │  特点: 逐步替换          │  │
│                                     │  风险: 低 - 可控迭代      │  │
│                                     │  适合: 中大型项目         │  │
│                                     └─────────────────────────┘  │
└─────────────────────────────────────────────────────────────────┘
```

### 策略选择决策树

```
┌─────────────────┐
│ 代码库规模评估?  │
└────────┬────────┘
         │
    ┌────┴────┐
    ▼         ▼
┌────────┐ ┌────────┐
|< 10K   │ | 10K+   │
|LOC     │ |LOC     │
└───┬────┘ └───┬────┘
    │          │
    ▼          ▼
┌────────┐  ┌──────────────┐
|大爆炸  │  |团队经验丰富? │
|迁移    │  └──────┬───────┘
└────────┘         │
              ┌────┴────┐
              ▼         ▼
           ┌───────┐ ┌───────┐
           |是     │ |否     │
           └───┬───┘ └───┬───┘
               │         │
               ▼         ▼
          ┌────────┐ ┌────────┐
          |渐进式  │ |混合策略│
          |模块替换│ |保守迁移│
          └────────┘ └────────┘
```

## 混合代码库管理

### C 与 Zig 共存架构

```
混合代码库目录结构:

project/
├── build.zig              # Zig 构建系统（主）
├── build.zig.zon          # 依赖管理
├── legacy/                # C 遗留代码
│   ├── include/           # C 头文件
│   ├── src/               # C 源文件
│   ├── CMakeLists.txt     # C 构建配置
│   └── CMakeLists.zig     # Zig 包装的 CMake
├── src/                   # Zig 新代码
│   ├── main.zig
│   ├── core/              # 核心模块
│   ├── ffi/               # 外部函数接口层
│   │   ├── c_bindings.zig
│   │   └── c_adapter.zig
│   └── utils/
├── tests/
│   ├── c_tests/           # C 侧测试
│   ├── zig_tests/         # Zig 侧测试
│   └── integration/       # 集成测试
└── docs/
    └── migration/         # 迁移文档
        ├── ADR-001-module-boundary.md
        ├── ADR-002-memory-management.md
        └── progress.md
```

### FFI 边界设计

```zig
/// ffi/c_bindings.zig - C 代码的 Zig 绑定层
const std = @import("std");

/// C 库的外部声明集中管理
pub const c = @cImport({
    @cDefine("LEGACY_API_EXPORT", "");
    @cInclude("legacy/core.h");
    @cInclude("legacy/utils.h");
});

/// 类型别名确保一致性
pub const CLegacyContext = c.LegacyContext;
pub const CLegacyStatus = c.LegacyStatus;

/// 错误码映射
pub const LegacyError = error{
    InvalidArgument,
    OutOfMemory,
    IoError,
    Unknown,
};

/// 将 C 错误码转换为 Zig 错误
pub fn statusToError(status: CLegacyStatus) LegacyError!void {
    return switch (status) {
        c.STATUS_OK => {},
        c.STATUS_INVALID_ARG => LegacyError.InvalidArgument,
        c.STATUS_NO_MEM => LegacyError.OutOfMemory,
        c.STATUS_IO_ERROR => LegacyError.IoError,
        else => LegacyError.Unknown,
    };
}
```

### 内存管理边界

```zig
/// ffi/memory_bridge.zig - 跨边界内存管理
const std = @import("std");
const c = @import("c_bindings.zig").c;

/// 内存所有权约定:
/// - C 分配的内存必须由 C 释放
/// - Zig 分配的内存必须由 Zig 释放
/// - 跨边界传递时，必须明确所有权

/// 从 C 获取数据，复制到 Zig 管理
pub fn copyFromC(c_ptr: ?[*]const u8, len: usize, allocator: std.mem.Allocator) ![]u8 {
    if (c_ptr == null) return error.NullPointer;

    const copy = try allocator.alloc(u8, len);
    errdefer allocator.free(copy);

    @memcpy(copy, c_ptr.?[0..len]);
    return copy;
}

/// 将 Zig 数据传递给 C（C 负责释放）
pub fn passToC(data: []const u8, c_allocator: *const fn (usize) callconv(.C) ?*anyopaque) !?*anyopaque {
    const c_mem = c_allocator(data.len) orelse return error.OutOfMemory;
    @memcpy(@as([*]u8, @ptrCast(c_mem))[0..data.len], data);
    return c_mem;
}

/// 安全的 RAII 包装器
pub fn CLegacyResource(comptime T: type, comptime deinit_fn: anytype) type {
    return struct {
        ptr: *T,

        const Self = @This();

        pub fn init(ptr: *T) Self {
            return .{ .ptr = ptr };
        }

        pub fn deinit(self: Self) void {
            deinit_fn(self.ptr);
        }
    };
}
```

## 构建系统集成

### Zig 构建系统整合 C 代码

```zig
// build.zig - 统一构建配置
const std = @import("std");

pub fn build(b: *std.Build) void {
    const target = b.standardTargetOptions(.{});
    const optimize = b.standardOptimizeOption(.{});

    // 创建 C 库静态链接
    const c_lib = b.addStaticLibrary(.{
        .name = "legacy",
        .target = target,
        .optimize = optimize,
    });

    // 添加 C 源文件
    c_lib.addCSourceFiles(.{
        .files = &.{
            "legacy/src/core.c",
            "legacy/src/utils.c",
            "legacy/src/parser.c",
        },
        .flags = &.{
            "-std=c11",
            "-Wall",
            "-Wextra",
            "-O2",
        },
    });

    c_lib.addIncludePath(b.path("legacy/include"));
    c_lib.linkLibC();

    // 创建 Zig 可执行文件
    const exe = b.addExecutable(.{
        .name = "migrated_app",
        .root_source_file = b.path("src/main.zig"),
        .target = target,
        .optimize = optimize,
    });

    // 链接 C 库
    exe.linkLibrary(c_lib);
    exe.addIncludePath(b.path("legacy/include"));
    exe.linkLibC();

    // 启用 translate-c 进行头文件导入
    exe.addAnonymousModule("c_headers", .{
        .root_source_file = b.path("legacy/include/legacy.h"),
    });

    b.installArtifact(exe);
}
```

### 混合测试配置

```zig
// build.zig - 测试配置

/// 运行 C 测试套件
const c_tests = b.addExecutable(.{
    .name = "c_tests",
    .target = target,
    .optimize = optimize,
});
c_tests.addCSourceFile(.{
    .file = b.path("tests/c_tests/runner.c"),
    .flags = &.{"-DTEST_ZIG_INTEROP"},
});
c_tests.linkLibrary(c_lib);

/// Zig 集成测试
const integration_tests = b.addTest(.{
    .root_source_file = b.path("tests/integration/main.zig"),
    .target = target,
    .optimize = optimize,
});
integration_tests.linkLibrary(c_lib);

const run_integration = b.addRunArtifact(integration_tests);
const test_step = b.step("integration-test", "运行集成测试");
test_step.dependOn(&run_integration.step);
```

## 逐步替换模块

### 模块替换优先级矩阵

| 模块类型 | 安全风险 | 复杂度 | 测试覆盖 | 优先级 | 预估工作量 |
|----------|----------|--------|----------|--------|-----------|
| 字符串工具 | 中 | 低 | 高 | **P1** | 1周 |
| 配置解析 | 低 | 中 | 高 | **P1** | 1周 |
| 日志系统 | 低 | 低 | 中 | **P2** | 3天 |
| 网络协议 | 高 | 高 | 中 | **P2** | 3周 |
| 文件 I/O | 中 | 中 | 高 | **P1** | 1周 |
| 核心算法 | 高 | 高 | 高 | **P3** | 4周 |
| UI 层 | 低 | 中 | 低 | **P4** | 2周 |

### 模块替换步骤

```zig
/// 示例：将 C 的配置解析器替换为 Zig 实现
/// 阶段 1: 保持相同接口

// src/config/legacy_adapter.zig - 第一阶段：直接包装
const c = @import("../ffi/c_bindings.zig").c;

pub const Config = struct {
    c_handle: *c.Config,

    pub fn load(path: []const u8) !Config {
        const c_path = try std.heap.c_allocator.dupeZ(u8, path);
        defer std.heap.c_allocator.free(c_path);

        var handle: ?*c.Config = null;
        const status = c.config_load(c_path.ptr, &handle);
        try statusToError(status);

        return Config{ .c_handle = handle.? };
    }

    pub fn get(self: Config, key: []const u8) ?[]const u8 {
        const c_key = std.heap.c_allocator.dupeZ(u8, key) catch return null;
        defer std.heap.c_allocator.free(c_key);

        const value = c.config_get(self.c_handle, c_key.ptr);
        if (value == null) return null;
        return std.mem.span(value);
    }

    pub fn deinit(self: Config) void {
        c.config_free(self.c_handle);
    }
};
```

```zig
/// 阶段 2: 混合实现 - 逐步替换内部逻辑
const std = @import("std");
const c = @import("../ffi/c_bindings.zig").c;

pub const Config = struct {
    allocator: std.mem.Allocator,
    // 新：使用 Zig HashMap 替代 C 的数据结构
    values: std.StringHashMap([]const u8),
    // 保留：使用 C 解析器处理复杂格式
    c_parser: ?*c.ConfigParser,

    const Self = @This();

    pub fn load(allocator: std.mem.Allocator, path: []const u8) !Self {
        var self = Self{
            .allocator = allocator,
            .values = std.StringHashMap([]const u8).init(allocator),
            .c_parser = null,
        };
        errdefer self.deinit();

        // 使用 C 解析器读取文件
        const content = try std.fs.cwd().readFileAlloc(allocator, path, 1024 * 1024);
        defer allocator.free(content);

        // 调用 C 解析逻辑，但存储在 Zig 结构中
        try self.parseWithC(content);

        return self;
    }

    pub fn get(self: Self, key: []const u8) ?[]const u8 {
        return self.values.get(key);
    }

    pub fn deinit(self: *Self) void {
        var iter = self.values.iterator();
        while (iter.next()) |entry| {
            self.allocator.free(entry.key_ptr.*);
            self.allocator.free(entry.value_ptr.*);
        }
        self.values.deinit();
        if (self.c_parser) |p| c.config_parser_free(p);
    }

    fn parseWithC(self: *Self, content: []const u8) !void {
        // 调用 C 解析代码，但将结果存入 Zig HashMap
        _ = self;
        _ = content;
        // 实现细节...
    }
};
```

```zig
/// 阶段 3: 完全 Zig 实现
const std = @import("std");

pub const Config = struct {
    allocator: std.mem.Allocator,
    values: std.StringHashMap([]const u8),

    const Self = @This();

    pub fn load(allocator: std.mem.Allocator, path: []const u8) !Self {
        var self = Self{
            .allocator = allocator,
            .values = std.StringHashMap([]const u8).init(allocator),
        };
        errdefer self.deinit();

        const content = try std.fs.cwd().readFileAlloc(allocator, path, 1024 * 1024);
        defer allocator.free(content);

        try self.parse(content);

        return self;
    }

    fn parse(self: *Self, content: []const u8) !void {
        var lines = std.mem.splitScalar(u8, content, '\n');
        while (lines.next()) |line| {
            const trimmed = std.mem.trim(u8, line, " \t\r");
            if (trimmed.len == 0 or trimmed[0] == '#') continue;

            if (std.mem.indexOfScalar(u8, trimmed, '=')) |sep| {
                const key = try self.allocator.dupe(u8, std.mem.trim(u8, trimmed[0..sep], " \t"));
                errdefer self.allocator.free(key);

                const value = try self.allocator.dupe(u8, std.mem.trim(u8, trimmed[sep + 1 ..], " \t"));
                errdefer self.allocator.free(value);

                try self.values.put(key, value);
            }
        }
    }

    // ... get, deinit 等
};
```

## 风险缓解策略

### 风险矩阵与应对

| 风险 | 可能性 | 影响 | 缓解措施 |
|------|--------|------|----------|
| ABI 不兼容 | 中 | 高 | 自动化 ABI 测试套件 |
| 内存泄漏 | 中 | 中 | GPA 检测、Valgrind 集成 |
| 性能退化 | 低 | 高 | 基准测试、性能回归检测 |
| 功能不一致 | 中 | 高 | 模糊测试、属性测试 |
| 构建复杂度 | 高 | 中 | 统一构建系统、CI/CD 集成 |
| 团队学习成本 | 高 | 低 | 培训、配对编程 |

### 回滚策略

```zig
/// 功能开关模式 - 允许运行时回退到 C 实现
const std = @import("std");

pub const ConfigModule = struct {
    use_zig_impl: bool,
    c_impl: CConfig,
    zig_impl: ?ZigConfig,

    pub fn load(self: *ConfigModule, path: []const u8) !void {
        if (self.use_zig_impl) {
            self.zig_impl = try ZigConfig.load(path);
        } else {
            try self.c_impl.load(path);
        }
    }

    pub fn get(self: ConfigModule, key: []const u8) ?[]const u8 {
        return if (self.use_zig_impl)
            self.zig_impl.?.get(key)
        else
            self.c_impl.get(key);
    }

    // 动态切换实现
    pub fn switchImpl(self: *ConfigModule, use_zig: bool) !void {
        if (use_zig == self.use_zig_impl) return;

        // 保存当前状态
        // 重新加载另一种实现
        // 验证等价性
        self.use_zig_impl = use_zig;
    }
};
```

### 金丝雀部署

```zig
/// 金丝雀模式 - 部分流量使用新实现
const std = @import("std");

pub const CanaryDeployment = struct {
    zig_impl_ratio: f32,  // 0.0 - 1.0
    random: std.Random,

    pub fn processRequest(self: *CanaryDeployment, req: Request) !Response {
        const use_zig = self.random.float(f32) < self.zig_impl_ratio;

        if (use_zig) {
            return zigProcess(req) catch |err| {
                // 失败时回退到 C 实现
                std.log.warn("Zig impl failed: {s}, falling back to C", .{@errorName(err)});
                return cProcess(req);
            };
        } else {
            return cProcess(req);
        }
    }

    pub fn increaseRatio(self: *CanaryDeployment, delta: f32) void {
        self.zig_impl_ratio = @min(1.0, self.zig_impl_ratio + delta);
    }
};
```

## 成功案例分析

### 案例：命令行工具迁移

**项目背景**: 5K LOC 的 C 日志分析工具

```
迁移时间线:
Week 1-2: 设置构建系统，FFI 层
Week 3-4: 替换字符串处理模块
Week 5-6: 替换文件 I/O 模块
Week 7-8: 替换核心解析算法
Week 9:   集成测试、性能调优
Week 10:  文档、发布
```

**关键成果**:

- 内存安全漏洞: 3个 → 0个
- 测试覆盖率: 45% → 78%
- 运行时性能: +15%
- 二进制大小: -20%

### 案例：网络服务迁移

**项目背景**: 50K LOC 的 C 网络服务

```
迁移策略:
Phase 1 (3个月): 协议解析层 (风险隔离)
Phase 2 (2个月): 连接管理器
Phase 3 (4个月): 业务逻辑层
Phase 4 (2个月): 清理 C 代码
```

**关键成果**:

- 服务稳定性: 99.9% → 99.99%
- 内存泄漏: 定期重启 → 长期运行
- 代码审查效率: 提升 40%

## 迁移检查清单

### 开始前

- [ ] 现有测试覆盖率评估
- [ ] 性能基准建立
- [ ] 依赖关系分析
- [ ] 团队培训完成

### 进行中

- [ ] 每个模块功能等价测试
- [ ] 内存泄漏检测通过
- [ ] 性能回归不超过 5%
- [ ] 文档同步更新

### 完成后

- [ ] 全量集成测试通过
- [ ] 生产环境金丝雀验证
- [ ] 回滚方案测试
- [ ] 团队知识转移

## 延伸阅读

1. [Incremental Migration Patterns](./01_Incremental_Migration_Patterns.md) - 详细迁移模式
2. [Safety Improvement Metrics](./02_Safety_Improvement_Metrics.md) - 安全性度量方法
3. [Performance Comparison](./03_Performance_Comparison_Framework.md) - 性能对比框架

---

*本文档是 Zig-C 连接系列的一部分，专注于 C 到 Zig 的系统化迁移方法论。*
