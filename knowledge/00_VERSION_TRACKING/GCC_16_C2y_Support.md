# GCC 16 C2y 支持状态

> **文档版本**: 1.0.0
> **最后更新**: 2026-05-10
> **编译器版本**: GCC 16.1 (2026年3-4月发布)

---

## 发布概览

| 属性 | 详情 |
|------|------|
| **版本号** | GCC 16.1 |
| **发布日期** | 2026年3-4月 |
| **默认C标准** | C23 (`-std=gnu23`) |
| **默认C++标准** | **C++20** (`-std=gnu++20`) |
| **C2y支持** | 实验性 → 成熟化 |

---

## C2y 特性实现状态

### 已完整支持的特性

| 特性 | 提案 | 编译选项 | 说明 |
|------|------|----------|------|
| `_Countof` | N3369, N3469 | `-std=c2y` | **GCC 16首次完整实现** |
| `defer` | N3853 r5 | `-std=c2y` | 与TS r5同步 |
| `if` 声明 | N3356 | `-std=c2y` | 选择语句中的声明 |
| named loops | N3355 | `-std=c2y` | 命名循环与break/continue |
| `typeof_unqual` | N3232 | `-std=c2y` | 无限定typeof |
| `__COUNTER__` | N3457 | 默认 | 预定义宏 |
| case range | N3370 | `-std=c2y` | `case 1 ... 10:` |
| 复数字面量 | N3298 | `-std=c2y` | `1.0i`, `2.0j` |

### 部分支持的特性

| 特性 | 提案 | 状态 | 说明 |
|------|------|------|------|
| constexpr函数 (隐式) | N3833 r4 | 🔄 部分 | ICE初始化的const整型隐式获得constexpr语义 |
| `constexpr` 显式函数 | N3298 | ❌ 不支持 | 仍在讨论中 |

### 移除/废弃

| 特性 | 状态 | 说明 |
|------|------|------|
| 消费操作 (consume) | ❌ 已移除 | C11 memory_order_consume 正式退役 |
| 隐式八进制字面量 | ⚠️ 废弃 | `010` 被标记为过时，推荐 `0o10` |

---

## 编译器行为变化

### C23 默认标准的持续影响

GCC 16延续了GCC 15的C23默认标准，但增加了更多兼容性诊断：

```bash
# 新增兼容性警告
gcc -Wc23-c2y-compat file.c    # C23到C2y兼容性检查
gcc -Wold-style-definition file.c # K&R函数定义警告（C2y将废弃）
```

### gccrs (Rust前端) 进展

GCC 16包含的gccrs前端取得重要里程碑：

- 可编译简单的Linux内核Rust模块
- 与C代码的混合编译无需LLVM依赖
- 对C-Rust互操作生态具有战略意义

---

## 与 GCC 15 的差异

| 特性 | GCC 15 | GCC 16 | 迁移建议 |
|------|--------|--------|----------|
| `_Countof` | ⚠️ 不完整 | ✅ 完整 | 升级到GCC 16可获得完整支持 |
| C++默认标准 | C++17 | C++20 | 影响混合C/C++项目 |
| LTO性能 | 基准 | +5~10% | 大型项目建议启用 `-flto` |
| 诊断精度 | 高 | 更高 | 新增 `-Wunterminated-string` 默认开启 |

---

## 参考

- [GCC 16 Release Notes](https://gcc.gnu.org/gcc-16/changes.html)
- [GCC C2y Support](https://gcc.gnu.org/projects/c-status.html#c2y)
- [C23/C2y Roadmap](../05_C_Language_Standards/C23_C2Y_ROADMAP.md)
