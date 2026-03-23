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
﻿# C 语言标准路线图追踪

> **追踪范围**: C89 至 C3x | **最后更新**: 2026-03-12

---

## 目录

- [目录](#目录)
- [标准时间线](#标准时间线)
- [C23 (ISO/IEC 9899:2024) 详细追踪](#c23-isoiec-98992024-详细追踪)
  - [发布信息](#发布信息)
  - [编译器支持状态 (2026-03)](#编译器支持状态-2026-03)
  - [C23 特性清单](#c23-特性清单)
    - [✅ 已实现特性](#-已实现特性)
    - [🚧 开发中特性](#-开发中特性)
  - [文档覆盖计划](#文档覆盖计划)
- [C2y (未来标准) 预览](#c2y-未来标准-预览)
  - [提案追踪](#提案追踪)
  - [与 Zig 的趋同](#与-zig-的趋同)
- [知识库维护计划](#知识库维护计划)
  - [定期更新周期](#定期更新周期)
  - [更新检查清单](#更新检查清单)
- [参考链接](#参考链接)
  - [官方资源](#官方资源)
  - [编译器文档](#编译器文档)
  - [社区资源](#社区资源)
- [深入理解](#深入理解)
  - [技术原理](#技术原理)
  - [实践指南](#实践指南)
  - [相关资源](#相关资源)

## 标准时间线

```text
┌─────────────────────────────────────────────────────────────────────────────┐
│                        C 语言标准演进时间线                                   │
├─────────────────────────────────────────────────────────────────────────────┤
│                                                                              │
│  1989    1990    1999    2011    2017/8   2024    202?    202?              │
│    │       │       │       │       │       │       │       │                │
│    ▼       ▼       ▼       ▼       ▼       ▼       ▼       ▼                │
│  ┌───┐  ┌───┐  ┌───┐  ┌───┐  ┌───┐  ┌───┐  ┌───┐  ┌───┐                    │
│  │C89│  │C90│  │C99│  │C11│  │C17│  │C23│  │C2y│  │C3x│                    │
│  └───┘  └───┘  └───┘  └───┘  └───┘  └───┘  └───┘  └───┘                    │
│    │       │       │       │       │       │       │       │                │
│    └───────┘       │       │       │       │       │       │                │
│      ANSI/ISO      │       │       │       │       │       │                │
│                    │       │       │       │       │       │                │
│    ┌───────────────┘       │       │       │       │       │                │
│    │ 主要特性:             │       │       │       │       │                │
│    │ • 可变长度数组        │       │       │       │       │                │
│    │ • 复杂数学            │       │       │       │       │                │
│    │ • 内联函数            │       │       │       │       │                │
│    │ • // 注释             │       │       │       │       │                │
│    │                       │       │       │       │       │                │
│    └───────────────────────┘       │       │       │       │                │
│      主要特性:                     │       │       │       │                │
│      • _Generic                    │       │       │       │                │
│      • _Atomic                     │       │       │       │                │
│      • _Thread_local               │       │       │       │                │
│      • Anonymous structs/unions    │       │       │       │                │
│                                    │       │       │       │                │
│    ┌───────────────────────────────┘       │       │       │                │
│    │ Bugfix 版本，无新特性                 │       │       │                │
│    │                                       │       │       │                │
│    └───────────────────────────────────────┘       │       │                │
│      主要特性:                                     │       │                │
│      • nullptr                                     │       │                │
│      • constexpr                                   │       │                │
│      • typeof                                      │       │                │
│      • #embed                                      │       │                │
│      • stdbit.h                                    │       │                │
│      • stdckdint.h                                 │       │                │
│                                                    │       │                │
│    ┌───────────────────────────────────────────────┘       │                │
│    │ 预期特性 (C2y):                                      │                │
│    │ • 更强大的 constexpr                                 │                │
│    │ • 模块系统                                           │                │
│    │ • 改进的 Unicode 支持                                │                │
│    │                                                      │                │
│    └───────────────────────────────────────────────────────┘                │
│      长期愿景:                                                               │
│      • 更好的内存安全                                                         │
│      • 与 C++ 更紧密的协调                                                    │
│                                                                              │
└─────────────────────────────────────────────────────────────────────────────┘
```

---

## C23 (ISO/IEC 9899:2024) 详细追踪

### 发布信息

| 属性 | 值 |
|------|-----|
| 标准编号 | ISO/IEC 9899:2024 |
| 发布日期 | 2024年10月 |
| 替代标准 | ISO/IEC 9899:2018 (C17) |
| 文档 | [ISO Store](https://www.iso.org/standard/82075.html) |
| 免费草案 | [N3054](https://www.open-std.org/jtc1/sc22/wg14/www/docs/n3054.pdf) |

### 编译器支持状态 (2026-03)

| 编译器 | 版本 | 支持度 | 关键特性 |
|--------|------|--------|----------|
| GCC | 15.x | ⭐⭐⭐⭐⭐ | **C23 作为默认方言** (2025-04) |
| Clang | 18.x+ | ⭐⭐⭐⭐⭐ | 完整支持 |
| MSVC | 17.8+ | ⭐⭐⭐⭐ | 部分预览 |
| ICC | 2024 | ⭐⭐⭐ | 基本支持 |

> **重要更新**: GCC 15 已于 2025 年 4 月将 C23 设为默认 C 语言标准（替代 C17），标志着 C23 正式进入主流应用阶段。

### C23 特性清单

#### ✅ 已实现特性

| 特性 | WG14 文档 | 知识库覆盖 | 代码示例 |
|------|-----------|------------|----------|
| `nullptr` | N3042 | ✅ | ✅ |
| `constexpr` | N3017 | ✅ | ✅ |
| `typeof` / `typeof_unqual` | N2892 | ✅ | ✅ |
| 空初始化器 `{}` | N2895 | ✅ | ✅ |
| 二进制字面量 | N2858 | ✅ | ✅ |
| 数字分隔符 `'` | N2799 | ✅ | ✅ |
| `#embed` | N3017 | ✅ | ⚠️ |
| `bool` / `true` / `false` 关键字 | N2935 | ✅ | ✅ |
| `static_assert` 无需头文件 | N2900 | ✅ | ✅ |
| `thread_local` 替代 `_Thread_local` | N2958 | ✅ | ✅ |
| `alignas` / `alignof` 无需头文件 | N3002 | ✅ | ✅ |
| `<stdbit.h>` | N3080 | ✅ | ⚠️ |
| `<stdckdint.h>` | N3080 | ✅ | ⚠️ |
| `memset_explicit` | N2894 | ⚠️ | ⚠️ |
| `strdup` / `strndup` | N2353 | ✅ | ✅ |

#### 🚧 开发中特性

| 特性 | 状态 | 预期 |
|------|------|------|
| 改进的 `constexpr` | C2y | 2027-2028 |
| 简化的内存安全 | C2y | 2027-2028 |
| 模块化 | C3x | 2030+ |

### 文档覆盖计划

```
已完成:
├── 01_Core_Knowledge_System/07_Modern_C/03_C23_Core_Features.md
│   ├── nullptr 深度解析
│   ├── constexpr 语义分析
│   ├── typeof 类型推导
│   ├── 语法增强
│   └── 标准库扩展
│
进行中:
├── 01_Core_Knowledge_System/04_Standard_Library_Layer/
│   └── 05_C23_Standard_Library.md (新建)
│
计划中:
├── 05_Deep_Structure_MetaPhysics/
│   └── 05_C23_Formal_Semantics/ (新建目录)
│       ├── 01_Nullptr_Formal_Semantics.md
│       ├── 02_Constexpr_Operational_Semantics.md
│       └── 03_Typeof_Type_System.md
```

---

## C2y (未来标准) 预览

### 提案追踪

| 提案 | 作者 | 状态 | 描述 |
|------|------|------|------|
| N3226 | WG14 | Active | 标准草案工作文件 |
| Enhanced constexpr | Various | 讨论中 | 允许更多编译时计算 |
| Memory safety | Various | 早期 | 借鉴 Rust/Zig 的安全特性 |
| Modules | Various | 早期 | 替代头文件系统 |

### 与 Zig 的趋同

```
C2y 可能借鉴的 Zig 特性:
├── 显式错误处理 (类似 Zig 的错误联合)
├── 更好的可选类型支持
├── 编译时计算增强 (类似 comptime)
├── 分配器显式传递模式
└── 与 C++ 模块的互操作
```

---

## 知识库维护计划

### 定期更新周期

| 更新类型 | 频率 | 负责 |
|---------|------|------|
| 编译器支持状态 | 每季度 | 自动化 + 人工验证 |
| 新提案追踪 | 每月 | WG14 会议后 |
| 正式化内容 | 按需 | 学术进展 |
| 代码示例验证 | 每次发布 | CI/CD |

### 更新检查清单

- [ ] 检查 GCC 最新版本支持状态
- [ ] 检查 Clang 最新版本支持状态
- [ ] 检查 MSVC 更新
- [ ] 审查 WG14 会议记录
- [ ] 更新编译器特性检测矩阵
- [ ] 验证代码示例
- [ ] 更新链接有效性

---

## 参考链接

### 官方资源

- [ISO C Standard](https://www.iso.org/committee/4504218/x/catalog/p/1/u/0/w/0/d/0)
- [WG14 Document Repository](https://www.open-std.org/jtc1/sc22/wg14/www/wg14_document_log.htm)
- [C23 Draft N3054](https://www.open-std.org/jtc1/sc22/wg14/www/docs/n3054.pdf)

### 编译器文档

- [GCC C23 Status](https://gcc.gnu.org/wiki/C23Status)
- [Clang C Status](https://clang.llvm.org/c_status.html)
- [MSVC C23 Support](https://learn.microsoft.com/en-us/cpp/overview/visual-cpp-language-conformance)

### 社区资源

- [r/C_Programming](https://www.reddit.com/r/C_Programming/)
- [WG14 Mailing List](https://lists.isocpp.org/mailman/listinfo.cgi/wg14)

---

> **维护者**: C_Lang Knowledge Base Team
> **更新频率**: 每月
> **最后同步**: WG14 2026-02 会议


---

## 深入理解

### 技术原理

深入探讨相关技术原理和实现细节。

### 实践指南

- 步骤1：理解基础概念
- 步骤2：掌握核心原理
- 步骤3：应用实践

### 相关资源

- 文档链接
- 代码示例
- 参考文章

---

> **最后更新**: 2026-03-21
> **维护者**: AI Code Review
