# Zig 包生态系统：从 zigmod 到发布

> **定位**: Zig 生态 / 包管理 | **难度**: ⭐⭐⭐ | **目标**: 掌握 Zig 包管理

---

## 📋 目录

- [Zig 包生态系统：从 zigmod 到发布](#zig-包生态系统从-zigmod-到发布)
  - [📋 目录](#-目录)
  - [📑 目录](#-目录-1)
  - [一、包管理器概述](#一包管理器概述)
  - [二、Zig 内置包管理](#二zig-内置包管理)
    - [2.1 build.zig.zon 详解](#21-buildzigzon-详解)
    - [2.2 依赖哈希计算](#22-依赖哈希计算)
    - [2.3 在 build.zig 中使用依赖](#23-在-buildzig-中使用依赖)
    - [2.4 本地路径覆盖](#24-本地路径覆盖)
  - [三、第三方包管理器](#三第三方包管理器)
    - [3.1 Zigmod](#31-zigmod)
    - [3.2 Gyro](#32-gyro)
  - [四、创建和发布包](#四创建和发布包)
    - [4.1 包结构设计](#41-包结构设计)
    - [4.2 创建库包](#42-创建库包)
    - [4.3 库入口设计](#43-库入口设计)
    - [4.4 发布到 GitHub](#44-发布到-github)
    - [4.5 发布到 astrolabe.pm](#45-发布到-astrolabepm)
  - [五、私有仓库](#五私有仓库)
    - [5.1 搭建私有包仓库](#51-搭建私有包仓库)
    - [5.2 使用私有仓库](#52-使用私有仓库)
  - [六、版本管理](#六版本管理)
    - [6.1 语义化版本](#61-语义化版本)
    - [6.2 版本兼容性检查](#62-版本兼容性检查)
    - [6.3 锁定文件](#63-锁定文件)
  - [七、实战：完整工作流](#七实战完整工作流)
    - [7.1 创建新项目](#71-创建新项目)
    - [7.2 添加依赖](#72-添加依赖)
    - [7.3 发布流程](#73-发布流程)
    - [7.4 CI/CD 集成](#74-cicd-集成)
  - [深入理解](#深入理解)
    - [核心原理](#核心原理)
    - [实践应用](#实践应用)
    - [最佳实践](#最佳实践)

---


---

## 📑 目录

- [Zig 包生态系统：从 zigmod 到发布](#zig-包生态系统从-zigmod-到发布)
  - [📋 目录](#-目录)
  - [📑 目录](#-目录-1)
  - [一、包管理器概述](#一包管理器概述)
  - [二、Zig 内置包管理](#二zig-内置包管理)
    - [2.1 build.zig.zon 详解](#21-buildzigzon-详解)
    - [2.2 依赖哈希计算](#22-依赖哈希计算)
    - [2.3 在 build.zig 中使用依赖](#23-在-buildzig-中使用依赖)
    - [2.4 本地路径覆盖](#24-本地路径覆盖)
  - [三、第三方包管理器](#三第三方包管理器)
    - [3.1 Zigmod](#31-zigmod)
    - [3.2 Gyro](#32-gyro)
  - [四、创建和发布包](#四创建和发布包)
    - [4.1 包结构设计](#41-包结构设计)
    - [4.2 创建库包](#42-创建库包)
    - [4.3 库入口设计](#43-库入口设计)
    - [4.4 发布到 GitHub](#44-发布到-github)
    - [4.5 发布到 astrolabe.pm](#45-发布到-astrolabepm)
  - [五、私有仓库](#五私有仓库)
    - [5.1 搭建私有包仓库](#51-搭建私有包仓库)
    - [5.2 使用私有仓库](#52-使用私有仓库)
  - [六、版本管理](#六版本管理)
    - [6.1 语义化版本](#61-语义化版本)
    - [6.2 版本兼容性检查](#62-版本兼容性检查)
    - [6.3 锁定文件](#63-锁定文件)
  - [七、实战：完整工作流](#七实战完整工作流)
    - [7.1 创建新项目](#71-创建新项目)
    - [7.2 添加依赖](#72-添加依赖)
    - [7.3 发布流程](#73-发布流程)
    - [7.4 CI/CD 集成](#74-cicd-集成)
  - [深入理解](#深入理解)
    - [核心原理](#核心原理)
    - [实践应用](#实践应用)
    - [最佳实践](#最佳实践)


---

## 一、包管理器概述

```
┌─────────────────────────────────────────────────────────────────────┐
│                    Zig 包管理生态                                    │
├─────────────────────────────────────────────────────────────────────┤
│                                                                      │
│  官方工具 (Zig 0.11+)                                               │
│  ├── build.zig.zon         - 包清单                                │
│  ├── zig fetch             - 获取依赖                              │
│  └── zig build             - 构建                                  │
│                                                                      │
│  第三方工具                                                          │
│  ├── zigmod                - 社区包管理器                          │
│  ├── gyro                  - 替代包管理器                          │
│  └── zkg                   - 早期包管理器                          │
│                                                                      │
│  包仓库                                                              │
│  ├── astrolabe.pm          - 社区包索引                            │
│  ├── GitHub Releases       - 直接下载                              │
│  └── 私有仓库              - 企业内部                              │
│                                                                      │
└─────────────────────────────────────────────────────────────────────┘
```

---

## 二、Zig 内置包管理

### 2.1 build.zig.zon 详解

```zig
// build.zig.zon - 包清单文件

.{
    // 包名 (必需)
    .name = "my-awesome-lib",

    // 版本 (语义化版本)
    .version = "1.2.3",

    // 依赖列表
    .dependencies = .{
        // GitHub 依赖
        .zap = .{
            .url = "https://github.com/zigzap/zap/archive/refs/tags/v0.5.0.tar.gz",
            .hash = "12200d135ec55385defa4df6a7087e7b4fd2657d8d6b6a32e0ec8077a0a3a8e7c8f3",
        },

        // 本地路径依赖
        .local_utils = .{
            .path = "../utils",
        },

        // HTTP URL 依赖
        .http_lib = .{
            .url = "https://example.com/lib.tar.gz",
            .hash = "sha256-abc123...",
        },

        // Git 仓库依赖
        .git_lib = .{
            .url = "https://github.com/user/repo/archive/main.tar.gz",
            .hash = "...",
        },
    },

    // 包含在包中的文件路径
    .paths = .{
        "build.zig",
        "build.zig.zon",
        "src",
        "LICENSE",
        "README.md",
    },
}
```

### 2.2 依赖哈希计算

```bash
# 获取依赖的哈希值

---

## 🔗 全面知识关联体系

### 【全局层】知识库导航
| 维度 | 目标文档 | 导航作用 |
|:-----|:---------|:---------|
| **总索引** | [../00_GLOBAL_INDEX.md](../00_GLOBAL_INDEX.md) | 完整知识图谱入口，全局视角 |
| **本模块** | [../README.md](../README.md) | 模块总览与目录导航 |
| **学习路径** | [../06_Thinking_Representation/06_Learning_Paths/README.md](../06_Thinking_Representation/06_Learning_Paths/README.md) | 阶段化学习路线规划 |
| **概念映射** | [../06_Thinking_Representation/05_Concept_Mappings/README.md](../06_Thinking_Representation/05_Concept_Mappings/README.md) | 核心概念等价关系图 |

### 【阶段层】学习定位
**当前模块**: 知识库  
**难度等级**: L1-L6  
**前置依赖**: 核心知识体系  
**后续延伸**: 持续学习

```
学习阶段金字塔:
    L6 专家层 [形式验证、编译器]
    L5 高级层 [并发、系统编程] ⬅️ 可能在此
    L4 进阶层 [指针、内存管理]
    L3 基础层 [函数、结构体]
    L2 入门层 [语法、数据类型]
    L1 零基础 [环境搭建]
```

### 【层次层】纵向知识链
| 层级 | 关联文档 | 层次关系 |
|:-----|:---------|:---------|
| **理论基础** | [../02_Formal_Semantics_and_Physics/00_Core_Semantics_Foundations/README.md](../02_Formal_Semantics_and_Physics/00_Core_Semantics_Foundations/README.md) | 语义学理论基础 |
| **核心机制** | [../01_Core_Knowledge_System/02_Core_Layer/README.md](../01_Core_Knowledge_System/02_Core_Layer/README.md) | C语言核心机制 |
| **标准接口** | [../01_Core_Knowledge_System/04_Standard_Library_Layer/README.md](../01_Core_Knowledge_System/04_Standard_Library_Layer/README.md) | 标准库API |
| **系统实现** | [../03_System_Technology_Domains/README.md](../03_System_Technology_Domains/README.md) | 系统级实现 |

### 【局部层】横向关联网
| 关联类型 | 目标文档 | 关联说明 |
|:---------|:---------|:---------|
| **技术扩展** | [../03_System_Technology_Domains/14_Concurrency_Parallelism/README.md](../03_System_Technology_Domains/14_Concurrency_Parallelism/README.md) | 并发编程技术 |
| **安全规范** | [../01_Core_Knowledge_System/09_Safety_Standards/MISRA_C_2023/README.md](../01_Core_Knowledge_System/09_Safety_Standards/MISRA_C_2023/README.md) | 安全编码标准 |
| **工具支持** | [../07_Modern_Toolchain/README.md](../07_Modern_Toolchain/README.md) | 现代开发工具链 |
| **实践案例** | [../04_Industrial_Scenarios/README.md](../04_Industrial_Scenarios/README.md) | 工业实践场景 |

### 【总体层】知识体系架构
```
┌─────────────────────────────────────────────────────────────┐
│                     总体知识体系架构                          │
├─────────────────────────────────────────────────────────────┤
│  01 Core Knowledge          → 核心概念与机制                  │
│  02 Formal Semantics        → 理论与物理基础                  │
│  03 System Technology       → 系统级技术领域                  │
│  04 Industrial Scenarios    → 工业应用场景                    │
│  05 Deep Structure          → 深层结构与元物理                │
│  06 Thinking Representation → 思维表征与学习                  │
│  07 Modern Toolchain        → 现代工具链                      │
└─────────────────────────────────────────────────────────────┘
```

### 【决策层】学习路径选择
| 目标 | 推荐路径 | 关键文档 |
|:-----|:---------|:---------|
| **系统学习** | 01 → 02 → 03 → 04 | 按顺序阅读各模块 |
| **问题导向** | 06决策树 → 相关模块 | [决策树目录](../06_Thinking_Representation/01_Decision_Trees/README.md) |
| **项目驱动** | 04案例 → 所需知识 | [工业场景](../04_Industrial_Scenarios/README.md) |
| **深入研究** | 02形式语义 → 11CompCert | [形式语义](../02_Formal_Semantics_and_Physics/README.md) |

---

---

## 🔗 文档关联

### 核心关联
| 文档 | 关系类型 | 说明 |
|:-----|:---------|:-----|
| [内存管理](../../01_Core_Knowledge_System/02_Core_Layer/02_Memory_Management.md) | 核心关联 | 内存管理基础 |
| [指针深度](../../01_Core_Knowledge_System/02_Core_Layer/01_Pointer_Depth.md) | 核心关联 | 指针深度基础 |
| [并发编程](../../03_System_Technology_Domains/14_Concurrency_Parallelism/README.md) | 核心关联 | 并发编程基础 |
| [数据类型](../../01_Core_Knowledge_System/01_Basic_Layer/02_Data_Type_System.md) | 核心关联 | 数据类型基础 |
| [数组与指针](../../01_Core_Knowledge_System/02_Core_Layer/05_Arrays_Pointers.md) | 核心关联 | 数组与指针基础 |

### 扩展阅读
| 文档 | 关系类型 | 说明 |
|:-----|:---------|:-----|
| [软件工程](../../01_Core_Knowledge_System/05_Engineering_Layer/README.md) | 核心关联 | 软件工程基础 |
| [形式语义](../../02_Formal_Semantics_and_Physics/README.md) | 核心关联 | 形式语义基础 |
| [系统技术](../../03_System_Technology_Domains/README.md) | 核心关联 | 系统技术基础 |
| [工业场景](../../04_Industrial_Scenarios/README.md) | 核心关联 | 工业场景基础 |
| [思维表征](../../06_Thinking_Representation/README.md) | 核心关联 | 思维表征基础 |
zig fetch <url>

# 示例
zig fetch https://github.com/zigzap/zap/archive/refs/tags/v0.5.0.tar.gz
# 输出: 12200d135ec55385defa4df6a7087e7b4fd2657d8d6b6a32e0ec8077a0a3a8e7c8f3
```

### 2.3 在 build.zig 中使用依赖

```zig
// build.zig

const std = @import("std");

pub fn build(b: *std.Build) void {
    const target = b.standardTargetOptions(.{});
    const optimize = b.standardOptimizeOption(.{});

    // 获取依赖
    const zap_dep = b.dependency("zap", .{
        .target = target,
        .optimize = optimize,
    });

    // 创建可执行文件
    const exe = b.addExecutable(.{
        .name = "myapp",
        .root_source_file = .{ .path = "src/main.zig" },
        .target = target,
        .optimize = optimize,
    });

    // 添加依赖模块
    exe.addModule("zap", zap_dep.module("zap"));

    // 链接依赖库
    exe.linkLibrary(zap_dep.artifact("facil.io"));

    b.installArtifact(exe);
}
```

### 2.4 本地路径覆盖

```zig
// build.zig - 本地开发时使用本地路径

pub fn build(b: *std.Build) void {
    // 检查环境变量
    const local_zap = b.option([]const u8, "local-zap", "Path to local zap");

    const zap = if (local_zap) |path|
        b.createModule(.{
            .source_file = .{ .path = b.pathJoin(&.{ path, "src/zap.zig" }) },
        })
    else
        b.dependency("zap", .{}).module("zap");

    // ... 使用 zap
}
```

```bash
# 使用本地覆盖
zig build -Dlocal-zap=../zap
```

---

## 三、第三方包管理器

### 3.1 Zigmod

```yaml
# zigmod.yml
id: 8vj8p8zgtpqrqfhcf5tn
name: my-project
license: MIT
description: A cool Zig project

root_dependencies:
  - src: git https://github.com/ziglibs/known-folders.git
    version: v0.7.0

  - src: git https://github.com/Hejsil/zig-clap.git
    version: 0.6.0

  - src: git https://github.com/MasterQ32/zig-network.git
    version: v0.1.0
```

```bash
# 安装 zigmod
# 见 https://github.com/nektro/zigmod

# 获取依赖
zigmod fetch

# 更新依赖
zigmod update

# 生成依赖文件
zigmod ci
```

### 3.2 Gyro

```yaml
# gyro.zzz
pkgs:
  clap:
    version: 0.6.0
    root: clap.zig
    src:
      github:
        user: Hejsil
        repo: zig-clap
        ref: 0.6.0

  network:
    version: 0.1.0
    root: network.zig
    src:
      github:
        user: MasterQ32
        repo: zig-network
        ref: v0.1.0
```

```bash
# 安装 gyro
# 见 https://github.com/mattnite/gyro

# 获取依赖
gyro build

# 添加依赖
gyro add --github Hejsil/zig-clap --alias clap

# 发布包
gyro publish
```

---

## 四、创建和发布包

### 4.1 包结构设计

```
my-zig-lib/
├── build.zig              # 构建脚本
├── build.zig.zon          # 包清单
├── LICENSE                # 许可证
├── README.md              # 文档
├── src/
│   ├── lib.zig           # 库入口
│   ├── core/
│   │   ├── utils.zig
│   │   └── types.zig
│   └── modules/
│       ├── module_a.zig
│       └── module_b.zig
├── tests/
│   ├── test_core.zig
│   └── integration_tests.zig
├── examples/
│   ├── basic_usage.zig
│   └── advanced.zig
└── docs/
    └── api.md
```

### 4.2 创建库包

```zig
// build.zig

const std = @import("std");

pub fn build(b: *std.Build) void {
    const target = b.standardTargetOptions(.{});
    const optimize = b.standardOptimizeOption(.{});

    // 创建库模块
    const lib_mod = b.addModule("my_lib", .{
        .source_file = .{ .path = "src/lib.zig" },
    });

    // 创建静态库
    const lib = b.addStaticLibrary(.{
        .name = "my_lib",
        .root_source_file = .{ .path = "src/lib.zig" },
        .target = target,
        .optimize = optimize,
    });
    b.installArtifact(lib);

    // 单元测试
    const lib_tests = b.addTest(.{
        .root_source_file = .{ .path = "src/lib.zig" },
        .target = target,
        .optimize = optimize,
    });

    const run_lib_tests = b.addRunArtifact(lib_tests);
    const test_step = b.step("test", "Run library tests");
    test_step.dependOn(&run_lib_tests.step);

    // 示例
    const example = b.addExecutable(.{
        .name = "example",
        .root_source_file = .{ .path = "examples/basic_usage.zig" },
        .target = target,
        .optimize = optimize,
    });
    example.addModule("my_lib", lib_mod);

    const run_example = b.addRunArtifact(example);
    const example_step = b.step("example", "Run example");
    example_step.dependOn(&run_example.step);

    // 文档
    const docs = b.addInstallDirectory(.{
        .source_dir = lib.getEmittedDocs(),
        .install_dir = .prefix,
        .install_subdir = "docs",
    });
    const docs_step = b.step("docs", "Generate documentation");
    docs_step.dependOn(&docs.step);
}
```

### 4.3 库入口设计

```zig
// src/lib.zig

//! My Awesome Library
//!
//! A brief description of what this library does.
//!
//! ## Usage
//!
//! ```zig
//! const my_lib = @import("my_lib");
//!
//! pub fn main() void {
//!     const result = my_lib.doSomething();
//! }
//! ```

// 公共 API
pub const core = @import("core/utils.zig");
pub const types = @import("core/types.zig");
pub const module_a = @import("modules/module_a.zig");
pub const module_b = @import("modules/module_b.zig");

// 重新导出常用类型
pub const Point = types.Point;
pub const Rectangle = types.Rectangle;

// 版本信息
pub const version = @import("std").SemanticVersion.parse("1.2.3") catch unreachable;
```

### 4.4 发布到 GitHub

```bash
# 1. 创建版本标签
git tag -a v1.0.0 -m "Release version 1.0.0"
git push origin v1.0.0

# 2. 创建发布包
tar czvf my-lib-1.0.0.tar.gz \
    --exclude='.git' \
    --exclude='zig-out' \
    --exclude='zig-cache' \
    .

# 3. 上传到 GitHub Releases
# 通过 GitHub Web 界面或 CLI
gh release create v1.0.0 my-lib-1.0.0.tar.gz \
    --title "Version 1.0.0" \
    --notes "Release notes here"

# 4. 获取哈希供他人使用
zig fetch https://github.com/user/repo/releases/download/v1.0.0/my-lib-1.0.0.tar.gz
```

### 4.5 发布到 astrolabe.pm

```bash
# 1. 注册账号
# 访问 https://astrolabe.pm

# 2. 创建包
# 提交 build.zig.zon 和基本信息

# 3. 自动索引
# astrolabe.pm 会自动从 GitHub/GitLab 索引 Zig 项目
```

---

## 五、私有仓库

### 5.1 搭建私有包仓库

```zig
// 简单的包服务器
const std = @import("std");
const http = std.http;

pub fn main() !void {
    var server = http.Server.init(std.heap.page_allocator, .{});
    defer server.deinit();

    try server.listen(try std.net.Address.parseIp("0.0.0.0", 8080));

    while (true) {
        const res = try server.accept(.{ .dynamic = 4096 });
        try handleRequest(res);
    }
}

fn handleRequest(res: *http.Server.Response) !void {
    const target = res.request.target;

    if (std.mem.startsWith(u8, target, "/packages/")) {
        const package_name = target[10..];
        try servePackage(res, package_name);
    } else if (std.mem.eql(u8, target, "/index.json")) {
        try serveIndex(res);
    } else {
        res.status = .not_found;
        try res.do();
    }
}

fn servePackage(res: *http.Server.Response, name: []const u8) !void {
    const packages_dir = "packages";
    const path = try std.fs.path.join(std.heap.page_allocator, &.{ packages_dir, name });

    const file = std.fs.cwd().openFile(path, .{}) catch {
        res.status = .not_found;
        try res.do();
        return;
    };
    defer file.close();

    res.status = .ok;
    res.transfer_encoding = .{ .content_length = try file.getEndPos() };
    try res.do();

    var buf: [4096]u8 = undefined;
    while (true) {
        const bytes = try file.read(&buf);
        if (bytes == 0) break;
        try res.writeAll(buf[0..bytes]);
    }
    try res.finish();
}

fn serveIndex(res: *http.Server.Response) !void {
    // 生成包索引
    const index =
        \\{
        \\  "packages": [
        \\    { "name": "private-lib", "version": "1.0.0", "url": "http://localhost:8080/packages/private-lib-1.0.0.tar.gz" }
        \\  ]
        \\}
    ;

    res.status = .ok;
    res.transfer_encoding = .{ .content_length = index.len };
    try res.do();
    try res.writeAll(index);
    try res.finish();
}
```

### 5.2 使用私有仓库

```zig
// build.zig.zon

.{
    .name = "my-app",
    .version = "1.0.0",

    .dependencies = .{
        .private_lib = .{
            .url = "http://internal-server:8080/packages/private-lib-1.0.0.tar.gz",
            .hash = "...",
        },
    },
}
```

---

## 六、版本管理

### 6.1 语义化版本

```
版本格式: MAJOR.MINOR.PATCH

MAJOR: 不兼容的 API 修改
MINOR: 向下兼容的功能新增
PATCH: 向下兼容的问题修复

示例:
1.0.0 - 初始发布
1.1.0 - 新增功能
1.1.1 - Bug 修复
2.0.0 - 重大版本更新 (破坏性变更)
```

### 6.2 版本兼容性检查

```zig
// 版本兼容性工具

const std = @import("std");

pub const Version = struct {
    major: u32,
    minor: u32,
    patch: u32,

    pub fn parse(str: []const u8) !Version {
        var parts = std.mem.split(u8, str, ".");

        const major = try std.fmt.parseInt(u32, parts.next() orelse return error.InvalidVersion, 10);
        const minor = try std.fmt.parseInt(u32, parts.next() orelse return error.InvalidVersion, 10);
        const patch = try std.fmt.parseInt(u32, parts.next() orelse return error.InvalidVersion, 10);

        return .{ .major = major, .minor = minor, .patch = patch };
    }

    // 检查版本兼容性
    pub fn isCompatible(self: Version, required: Version) bool {
        // 主版本必须相同
        if (self.major != required.major) return false;

        // 次要版本必须 >= 要求
        if (self.minor < required.minor) return false;

        // 如果是相同次要版本，补丁必须 >= 要求
        if (self.minor == required.minor and self.patch < required.patch) {
            return false;
        }

        return true;
    }

    pub fn format(
        self: Version,
        comptime fmt: []const u8,
        options: std.fmt.FormatOptions,
        writer: anytype,
    ) !void {
        _ = fmt;
        _ = options;
        try writer.print("{}.{}.{}", .{ self.major, self.minor, self.patch });
    }
};

// 依赖版本约束
pub const VersionConstraint = struct {
    min: ?Version,
    max: ?Version,
    exact: ?Version,

    pub fn matches(self: VersionConstraint, version: Version) bool {
        if (self.exact) |exact| {
            return version.major == exact.major and
                   version.minor == exact.minor and
                   version.patch == exact.patch;
        }

        if (self.min) |min| {
            if (version.major < min.major) return false;
            if (version.major == min.major and version.minor < min.minor) return false;
            if (version.major == min.major and version.minor == min.minor and version.patch < min.patch) {
                return false;
            }
        }

        if (self.max) |max| {
            if (version.major > max.major) return false;
            if (version.major == max.major and version.minor > max.minor) return false;
            if (version.major == max.major and version.minor == max.minor and version.patch > max.patch) {
                return false;
            }
        }

        return true;
    }

    pub fn parse(str: []const u8) !VersionConstraint {
        // 解析版本约束字符串
        // ^1.2.3 - 兼容 1.x.x
        // ~1.2.3 - 兼容 1.2.x
        // >=1.2.3 - 大于等于
        // 1.2.3 - 精确版本

        if (str[0] == '^') {
            const min = try Version.parse(str[1..]);
            return .{ .min = min, .max = .{ .major = min.major + 1, .minor = 0, .patch = 0 }, .exact = null };
        }

        if (str[0] == '~') {
            const min = try Version.parse(str[1..]);
            return .{ .min = min, .max = .{ .major = min.major, .minor = min.minor + 1, .patch = 0 }, .exact = null };
        }

        const exact = try Version.parse(str);
        return .{ .min = null, .max = null, .exact = exact };
    }
};
```

### 6.3 锁定文件

```zig
// zig.lock - 依赖锁定

.{
    .version = "1",
    .packages = .{
        .{
            .name = "zap",
            .version = "0.5.0",
            .url = "https://github.com/zigzap/zap/archive/refs/tags/v0.5.0.tar.gz",
            .hash = "12200d135ec55385defa4df6a7087e7b4fd2657d8d6b6a32e0ec8077a0a3a8e7c8f3",
        },
        .{
            .name = "known-folders",
            .version = "0.7.0",
            .url = "https://github.com/ziglibs/known-folders/archive/v0.7.0.tar.gz",
            .hash = "...",
        },
    },
}
```

---

## 七、实战：完整工作流

### 7.1 创建新项目

```bash
# 1. 初始化项目
mkdir my-zig-project
cd my-zig-project
zig init-lib

# 2. 创建 build.zig.zon
cat > build.zig.zon << 'EOF'
.{
    .name = "my-zig-project",
    .version = "0.1.0",
    .dependencies = .{},
    .paths = .{
        "build.zig",
        "build.zig.zon",
        "src",
    },
}
EOF

# 3. 初始化 Git
git init
git add .
git commit -m "Initial commit"
```

### 7.2 添加依赖

```bash
# 1. 获取依赖哈希
zig fetch https://github.com/ziglibs/known-folders/archive/v0.7.0.tar.gz
# 输出: 1220bb...

# 2. 编辑 build.zig.zon
cat > build.zig.zon << 'EOF'
.{
    .name = "my-zig-project",
    .version = "0.1.0",
    .dependencies = .{
        .known-folders = .{
            .url = "https://github.com/ziglibs/known-folders/archive/v0.7.0.tar.gz",
            .hash = "1220bb...",
        },
    },
    .paths = .{
        "build.zig",
        "build.zig.zon",
        "src",
    },
}
EOF

# 3. 更新 build.zig
# 添加: exe.addModule("known-folders", b.dependency("known-folders", .{}).module("known-folders"));
```

### 7.3 发布流程

```bash
#!/bin/bash
# release.sh

VERSION=$1

if [ -z "$VERSION" ]; then
    echo "Usage: ./release.sh 1.0.0"
    exit 1
fi

# 1. 更新版本号
sed -i "s/.version = \"[^\"]*\"/.version = \"$VERSION\"/" build.zig.zon

# 2. 运行测试
zig build test || exit 1

# 3. 创建标签
git add build.zig.zon
git commit -m "Release version $VERSION"
git tag -a "v$VERSION" -m "Release version $VERSION"
git push origin "v$VERSION"

# 4. 创建发布包
mkdir -p dist
tar czvf "dist/my-zig-project-$VERSION.tar.gz" \
    --exclude='.git' \
    --exclude='zig-out' \
    --exclude='zig-cache' \
    --exclude='dist' \
    .

echo "Release v$VERSION created!"
```

### 7.4 CI/CD 集成

```yaml
# .github/workflows/release.yml
name: Release

on:
  push:
    tags:
      - 'v*'

jobs:
  release:
    runs-on: ubuntu-latest
    steps:
      - uses: actions/checkout@v3

      - name: Setup Zig
        uses: goto-bus-stop/setup-zig@v2
        with:
          version: 0.11.0

      - name: Run tests
        run: zig build test

      - name: Create release archive
        run: |
          VERSION=${GITHUB_REF#refs/tags/v}
          tar czvf "my-zig-project-${VERSION}.tar.gz" \
            --exclude='.git' \
            --exclude='zig-out' \
            --exclude='zig-cache' \
            .

      - name: Upload to GitHub Releases
        uses: softprops/action-gh-release@v1
        with:
          files: '*.tar.gz'
        env:
          GITHUB_TOKEN: ${{ secrets.GITHUB_TOKEN }}
```

---

> **文档信息**
>
> - 创建时间: 2026-03-10


---

## 深入理解

### 核心原理

深入探讨技术原理和实现细节。

### 实践应用

- 应用场景1
- 应用场景2
- 应用场景3

### 最佳实践

1. 理解基础概念
2. 掌握核心机制
3. 应用到实际项目

---

> **最后更新**: 2026-03-21
> **维护者**: AI Code Review
