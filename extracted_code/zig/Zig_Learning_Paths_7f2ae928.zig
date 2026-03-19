//
// Auto-generated from: Zig\Zig_Learning_Paths.md
// Line: 132
// Language: zig
// Block ID: 7f2ae928
//
// This file was automatically extracted from the C_Lang knowledge base.
// DO NOT MODIFY - Changes will be overwritten on next extraction.
//

Day 1-2: 环境准备
├── 安装 Zig (0.13+)
│   ├── Windows: winget install zig.zig
│   ├── macOS: brew install zig
│   └── Linux: 官网下载解压
├── 配置 IDE
│   ├── VS Code + Zig 扩展
│   └── ZLS (Zig Language Server)
└── 验证安装
    └── zig version

Day 3-4: 第一个程序
├── Hello World
│   └── const std = @import("std");
├── 理解 main 函数
├── 理解 @import
└── 编译和运行
    ├── zig run main.zig
    └── zig build-exe main.zig

Day 5-7: 基础语法
├── 变量和常量
│   ├── var vs const
│   ├── 类型推断
│   └── 块作用域
├── 基本数据类型
│   ├── 整数 (i32, u64, usize)
│   ├── 浮点 (f32, f64)
│   ├── 布尔
│   └── 可选类型 (?T)
└── 数组和切片
    ├── [N]T
    └── []T
