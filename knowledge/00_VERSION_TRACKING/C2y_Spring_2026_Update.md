# C2y 2026年春季进展更新

> **文档版本**: 1.0.0
> **最后更新**: 2026-05-10
> **覆盖周期**: 2026年2月 — 2026年5月
> **来源**: WG14 文档日志、GCC/Clang 发布说明

---

## 目录

- [C2y 2026年春季进展更新](#c2y-2026年春季进展更新)
  - [目录](#目录)
  - [会议概览](#会议概览)
  - [关键文档清单](#关键文档清单)
    - [工作组正式文档](#工作组正式文档)
    - [新提案与修订](#新提案与修订)
  - [核心提案更新](#核心提案更新)
    - [defer TS CD r5](#defer-ts-cd-r5)
    - [Earthly Demons 与 Ghosts 清理](#earthly-demons-与-ghosts-清理)
    - [函数契约 (Function Contracts)](#函数契约-function-contracts)
    - [新提案速览](#新提案速览)
      - [`compile_assert` (N3846)](#compile_assert-n3846)
      - [comparable types (N3844/N3870/N3875)](#comparable-types-n3844n3870n3875)
  - [编译器实现更新](#编译器实现更新)
    - [GCC 16](#gcc-16)
    - [Clang 20 正式版](#clang-20-正式版)
  - [已确认的状态变更](#已确认的状态变更)
  - [风险警示](#风险警示)
    - [🔴 跨平台可移植性危机](#-跨平台可移植性危机)
    - [🟡 默认标准跃迁的隐性破坏](#-默认标准跃迁的隐性破坏)
  - [后续跟踪要点](#后续跟踪要点)
  - [参考](#参考)

---

## 会议概览

| 会议 | 时间 | 形式 | 关键产出 |
|------|------|------|----------|
| WG14 2026冬季会议 | 2026年2月 | 混合(布拉格/线上) | defer TS r4 → r5、多项UB清理提案审议 |
| 2026年3月CFP电话会议 | 2026年3月9-13日 | 线上 | N3846-N3855系列文档定稿 |
| 2026年4月CFP电话会议 | 2026年4月 | 线上 | N3870-N3878系列、fromfp符号问题 |
| WG14 2026春季会议 | 2026年4月(伦敦) | 线下 | **关键投票会议**：多项C2y提案正式采纳/拒绝 |

> **重点提示**：2026年4月伦敦会议是C2y周期中的**关键决策节点**，`defer`、bounds-checked pointer、K&R函数定义废弃等提案可能在此被正式投票。

---

## 关键文档清单

### 工作组正式文档

| 编号 | 标题 | 作者 | 日期 | 状态 |
|------|------|------|------|------|
| N3853 | defer Technical Specification (CD), r5 | Meneide | 2026-03-22 | **TS CD r5** |
| N3852 | Improved `__attribute__((cleanup(...)))` Through defer, r5 | Meneide | 2026-03-22 | 伴随r5更新 |
| N3854 | Working Draft (post February-March 2026) | Meneide | 2026-03-22 | C2y工作草案 |
| N3855 | Editor's Report (post February-March 2026) | Meneide | 2026-03-22 | 编辑报告 |
| N3856 | Draft 2026 (spring) WG14 minutes | Svoboda | 2026-03-15 | 春季会议纪要草案 |
| N3857 | Final 2026 (winter) WG14 minutes | Svoboda | 2026-03-15 | 冬季会议纪要终稿 |

### 新提案与修订

| 编号 | 标题 | 作者 | 日期 | 类别 |
|------|------|------|------|------|
| N3846 | `compile_assert` – optimization-enforced conditions at compile time | Grant | 2026-03-10 | 新提案 |
| N3844/N3870/N3875 | comparable types | Colomar | 2026-03-22 / 04-12 | 新提案/修订 |
| N3849 | Memory allocation with size feedback v2 | Munger | 2026-03-22 | 修订 |
| N3848 | Namespacing with prefixes r1 | Almkainzi | 2026-03-15 | 新提案 |
| N3871 | Slay Earthly Demon: Storage-Class Specifiers with Compound Literals | Uecker | 2026-04-05 | UB清理 |
| N3867 | C semantics for contracts, v3 | Gustedt | 2026-04-12 | 修订 |
| N3861 | Ghosts and Demons: Undefined Behavior in C2Y (Status 26-03-16) | Uecker | 2026-03-29 | **状态全景** |
| N3863 | Redefining implementation-defined | Ballman | 2026-03-29 | 语义澄清 |
| N3866 | Remove UB for non-basic source characters (modified) | Karl | 2026-04-12 | UB清理 |
| N3872 | Thread safe signals handling rev 3 | Douglas | 2026-04-12 | 修订 |
| N3869 | The `malloc_allocated_size` function | Veldmeijer | 2026-04-19 | 新提案 |

---

## 核心提案更新

### defer TS CD r5

**文档**: N3853 (2026-03-22) / N3852 (伴随更新)

相对于r4的关键演进：

- 技术规范 Committee Draft 进入第五轮修订
- 与GCC 15实验性实现的偏差正在被逐条记录
- `defer`与`__attribute__((cleanup(...)))`的语义等价性论证更加精确

**编译器状态更新**:

| 编译器 | 版本 | 状态 | 备注 |
|--------|------|------|------|
| GCC | 15+ | ✅ 实验性支持 | `-std=c2y` / `-std=gnu2y` |
| GCC | 16+ | ✅ 功能更完整 | 与TS草案同步改进 |
| Clang | - | 🔄 开发中 | 尚未合并主线 |
| MSVC | - | ❌ 无计划 | 无C23路线图，C2y更无时间表 |

---

### Earthly Demons 与 Ghosts 清理

**N3861** (Uecker, 2026-03-29) 提供了C2y中UB清理工作的**全景状态图**。这是目前跟踪UB→约束违规/明确定义迁移的最权威索引。

与项目形式语义章节的关联：

- Ghost状态（N3740/N3861）↔ 分离逻辑中的"幻指针"概念
- 消费操作退役（N3607）↔ C11内存模型release-acquire语义的简化
- 这些清理工作本质上是为**形式化验证**扫清障碍

**关键已采纳清理**:

| 提案 | 主题 | 结果 |
|------|------|------|
| N3786 | Remove the imaginary `I` macro | 委员会推荐直接删除 |
| N3607 | Retire consume operations | 消费操作正式退役 |
| N3866 | Non-basic source characters → 非UB | 约束违规替代 |

---

### 函数契约 (Function Contracts)

**N3867 v3** (Gustedt, 2026-04-12)

函数契约在C2y中的语义定位：

- 不是C++ Contracts的移植，而是面向C的**轻量级契约**
- 与`_Optional` qualifier (N3422/N3876) 的设计存在交叉影响
- 当前争议点：契约失败时的默认行为（终止 vs 忽略 vs 回调）

---

### 新提案速览

#### `compile_assert` (N3846)

Grant提出的编译期优化断言，与`static_assert`的区别：

```c
// static_assert: 必须诊断，不依赖NDEBUG
static_assert(sizeof(int) == 4, "int must be 32-bit");

// compile_assert: 允许编译器利用其进行优化
// 若条件不满足，行为未指定（允许更激进的优化）
compile_assert(p != nullptr);
```

> ⚠️ 注意：`compile_assert`与现有`_Assert` (N3760)提案的关系尚在讨论中。

#### comparable types (N3844/N3870/N3875)

Colomar提出的类型可比较性概念，目标是为C提供一种**显式声明类型可安全比较**的机制：

```c
// 概念性语法（非最终）
comparable struct Point {
    int x, y;
};
```

与C++ `operator==`的区别：comparable types是**编译器生成memcmp式比较**，而非用户自定义语义。

---

## 编译器实现更新

### GCC 16

**发布时间**: 2026年3-4月

| 特性 | GCC 15 | GCC 16 | 说明 |
|------|--------|--------|------|
| 默认C标准 | C23 | C23 | 无变化 |
| 默认C++标准 | C++17 | **C++20** | 重大跃迁 |
| `_Countof` | ⚠️ 实验性 | ✅ 完整 | N3469实现完成 |
| defer | ⚠️ 实验性 | ✅ 更完整 | 与TS r5同步 |
| constexpr函数 | ❌ | 🔄 部分 | N3833隐式constexpr |
| gccrs (Rust前端) | 🔄 早期 | 🔄 可编译简单内核模块 | C-Rust混编里程碑 |

### Clang 20 正式版

**发布时间**: 2026年初

C23新实现：

- N3029 Improved Normal Enumerations ✅
- N3030 Enhancements to Enumerations ✅
- N3298 Complex literals (`i`/`j`后缀) ✅
- N3370 Case range expressions ✅
- N3341 Empty structs/unions (C2y实现定义) ✅

---

## 已确认的状态变更

| 提案/特性 | 之前状态 | 当前状态 | 变更时间 |
|-----------|---------|---------|---------|
| defer TS | r4 | **r5** | 2026-03 |
| `_Countof` | 讨论中 | **GCC 16已实现** | 2026-03-04 |
| 消费操作(consume) | C11标准 | **C2y正式退役** | 2025-06(N3607) |
| Imaginary `I` macro | C99 Annex G | **C2y建议删除** | 2026-01(N3786) |
| MSVC C23 | "进行中/接近完整" | **实际停滞** | 持续 |

---

## 风险警示

### 🔴 跨平台可移植性危机

截至2026年5月，**没有任何一个C23核心特性能在GCC/Clang/MSVC三端同时完整可用**：

| 特性 | GCC 15 | Clang 20 | MSVC |
|------|--------|----------|------|
| `_BitInt` | ✅ | ✅ | ❌ |
| `nullptr` | ✅ | ✅ | ❌ |
| `#embed` | ✅ | ✅ | ❌ |
| `constexpr` | ✅ | ✅ | ⚠️ |

> **结论**："可移植C23代码"在当前生态下是**伪命题**。跨平台项目必须维持C17兼容层。

### 🟡 默认标准跃迁的隐性破坏

GCC 15默认C23意味着：

- `bool`/`true`/`false`成为关键字 → 旧代码中用作标识符的`bool`将编译失败
- `()`空参数列表语义变更 → K&R遗留代码面临静默行为变化
- 这些破坏**不通过`-Wall`或`-Wextra`提示**，必须使用`-Wc23-c2y-compat`

---

## 后续跟踪要点

| 跟踪项 | 预计时间 | 优先级 |
|--------|---------|--------|
| WG14 2026年8月会议（渥太华） | 2026年8月 | 🔴 高 |
| defer TS是否进入正式投票 | 2026年内 | 🔴 高 |
| TrapC最终处置（TS轨道或拒绝） | 2026年内 | 🟡 中 |
| MSVC C23官方路线图（如有） | 未知 | 🟡 中 |
| glibc 2.43 `_ISOC2Y_SOURCE` | 2026年2月已发布 | 🟡 中 |
| musl 2.0 C23完整支持 | 预计2026年 | 🟢 低 |

---

## 参考

- [WG14 Document Log](https://www.open-std.org/jtc1/sc22/wg14/www/wg14_document_log)
- [GCC 16 Changes](https://gcc.gnu.org/gcc-16/changes.html)
- [Clang 20 Release Notes](https://releases.llvm.org/20.1.0/tools/clang/docs/ReleaseNotes.html)
- [C23/C2y Roadmap](../c_language_standards/C23_C2Y_ROADMAP.md)
