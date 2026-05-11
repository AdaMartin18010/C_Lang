# 2026年5月10日重大更新日志

> **更新日期**: 2026-05-10
> **更新范围**: C23/C2y标准对齐、编译器支持矩阵修正、MISRA C:2025更新、格式结构修复
> **影响文件数**: 30+ 文件修改，7个新建文件，3个新增示例

---

## 一、紧急内容修正（P1）

### 1.1 MSVC C23支持状态修正

**问题**: 项目对MSVC的C23支持状态过于乐观，将`_BitInt`、`nullptr`、`#embed`标记为部分支持或已支持。

**修正**: 统一将上述特性标记为 ❌ 不支持，并添加风险警示。

**涉及文件**:

- `knowledge/c_language_standards/C23_C2Y_SUPPORT_MATRIX.md`
- `knowledge/c_language_standards/C23_C2Y_ROADMAP.md`

### 1.2 GCC 15/C23默认标准状态更新

**问题**: 多个文件仍将GCC 15描述为"即将发布"或"计划中"。

**修正**: GCC 15已于2025年4月正式发布，C23成为默认标准（`-std=gnu23`）。

**涉及文件**:

- `knowledge/c_language_standards/C23_C2Y_ROADMAP.md`
- `knowledge/00_VERSION_TRACKING/C23_to_C2y_Roadmap.md`
- `knowledge/01_Core_Knowledge_System/07_Modern_C/C2y_Countof_Depth.md`
- `knowledge/01_Core_Knowledge_System/07_Modern_C/C2y_If_Declaration_Depth.md`
- `knowledge/00_VERSION_TRACKING/C2y_Feature_Previews.md`
- `knowledge/00_VERSION_TRACKING/C2y_Proposal_Tracker.md`

### 1.3 defer提案状态更新至TS CD r5

**问题**: defer状态停留在r4，未同步N3853（2026-03-22）。

**修正**: 更新为Committee Draft r5，标注GCC 15实验性支持。

**涉及文件**:

- `knowledge/c_language_standards/C23_C2Y_SUPPORT_MATRIX.md`
- `knowledge/c_language_standards/C23_C2Y_ROADMAP.md`
- `knowledge/01_Core_Knowledge_System/07_Modern_C/C2y_Preview/README.md`
- `knowledge/01_Core_Knowledge_System/07_Modern_C/C2y_Preview/01_Defer_Keyword.md`

### 1.4 `_Countof`状态修正

**问题**: 多处将`_Countof`错误描述为"C23引入"，且GCC支持状态不准确。

**修正**: 明确标注为C2y特性；GCC 16已完整支持。

**涉及文件**:

- `knowledge/c_language_standards/C23_C2Y_SUPPORT_MATRIX.md`
- `knowledge/c_language_standards/C23_C2Y_ROADMAP.md`
- `knowledge/01_Core_Knowledge_System/07_Modern_C/C2y_Countof_Depth.md`
- `examples/c23_c2y/20_compatibility_macros.c`

### 1.5 constexpr语义层次澄清

**问题**: C23对象级constexpr与C2y函数级constexpr被混淆。

**修正**: 明确区分两者；修正`04_constexpr_usage.c`中的函数示例。

**涉及文件**:

- `examples/c23_c2y/04_constexpr_usage.c`
- `knowledge/c_language_standards/C23_C2Y_SUPPORT_MATRIX.md`
- `knowledge/c_language_standards/C23_C2Y_ROADMAP.md`

### 1.6 C2y标准编号错误修正

**问题**: 多处将C2y错误标注为"ISO/IEC 9899:2024"。

**修正**: C23 = 9899:2024；C2y = 9899:202Y（草案阶段）。

**涉及文件**:

- `knowledge/01_Core_Knowledge_System/07_Modern_C/C2y_Defer_Depth.md`
- `knowledge/00_VERSION_TRACKING/C23_C2y_Comprehensive_Guide.md`

### 1.7 LLVM 22 → LLVM 20 版本号修正

**问题**: ROADMAP中"Clang 20/LLVM 22"表述错误。

**修正**: Clang 20 = LLVM 20。LLVM版本与Clang版本同步。

**涉及文件**:

- `knowledge/c_language_standards/C23_C2Y_ROADMAP.md`

---

## 二、MISRA C:2025更新

**问题**: MISRA C:2025内容基于早期草案，与2025年3月发布的最终版存在差异。

**修正**:

- 更新规则总数为225条活跃指南
- 新增4条规则：R.8.18, R.8.19, R.11.11, R.19.3
- 引入"停用(disapplied)"概念：Rule 15.5被停用
- 删除2条规则，编号不再复用
- 澄清AI生成代码与手写代码同等对待

**涉及文件**:

- `knowledge/01_Core_Knowledge_System/09_Safety_Standards/MISRA_C_2025/MISRA_C_2025_Comprehensive_Guide.md`

---

## 三、新建文档（P2）

| 文件 | 说明 |
|------|------|
| `knowledge/00_VERSION_TRACKING/C2y_Spring_2026_Update.md` | WG14 2026年春季会议进展 |
| `knowledge/00_VERSION_TRACKING/GCC_16_C2y_Support.md` | GCC 16编译器C2y支持状态 |
| `knowledge/00_VERSION_TRACKING/Clang_20_C23_Release_Notes.md` | Clang 20正式发布说明解析 |
| `knowledge/00_VERSION_TRACKING/glibc_2.43_C2y_Preview.md` | glibc 2.43 C23/C2y支持 |
| `knowledge/00_VERSION_TRACKING/musl_2.0_C23.md` | musl 2.0 C23完整支持 |
| `knowledge/00_VERSION_TRACKING/C23_Default_Standard_Migration_Guide.md` | GCC 15 C23默认标准迁移指南 |
| `examples/c23_c2y/README.md` | C23/C2y示例代码索引 |

---

## 四、新增示例代码

| 文件 | 特性 | 标准 |
|------|------|------|
| `examples/c23_c2y/21_elvis_operator.c` | Elvis操作符 `?:` | C2y (N3753) |
| `examples/c23_c2y/22_counter_macro.c` | `__COUNTER__`宏 | C2y (N3636) |
| `examples/c23_c2y/23_typeof_return.c` | `typeof(return)` | C2y (N3454) |

---

## 五、导航与索引更新

**涉及文件**:

- `knowledge/00_VERSION_TRACKING/README.md` — 新增6个文档链接
- `NAVIGATION.md` — 更新C2y追踪路径

---

## 六、已知待完成项

### 格式结构问题（P0，后台处理中）

- [ ] 大量Markdown文件的multi_h1（多个一级标题）降级
- [ ] broken_link（死链接）修复
- [ ] unclosed_fence（未闭合代码块）修复
- [ ] 文件末尾模板填充内容清理

### 内容补全（P2）

- [ ] OpenSSL 3.0 EOL跟踪文档
- [ ] Linux内核DRM子系统Rust强制令分析
- [ ] DARPA "Hardening Legacy C" 结果跟踪

### 战略增强（P3）

- [ ] 单一真相源重构（合并重叠的VERSION_TRACKING文件）
- [ ] 形式语义 ↔ 标准演进桥接文档
- [ ] 外部事件驱动更新机制设计

---

## 七、后续跟踪要点

| 跟踪项 | 预计时间 | 优先级 |
|--------|---------|--------|
| WG14 2026年8月会议（渥太华） | 2026年8月 | 🔴 高 |
| defer TS正式投票 | 2026年内 | 🔴 高 |
| MSVC C23路线图（如有） | 未知 | 🟡 中 |
| Clang 21+ `_Countof`支持 | 预计2026下半年 | 🟡 中 |
| musl 2.0正式发布 | 预计2026年 | 🟢 低 |
