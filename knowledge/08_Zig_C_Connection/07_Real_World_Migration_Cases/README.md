# 07 真实迁移案例 (Real World Migration Cases)

> **定位**: Zig-C Connection / 实践 | **难度**: ⭐⭐⭐⭐⭐

---

## 目录

| 文档 | 项目 | 内容 |
|:-----|:-----|:-----|
| [01_Redis_Data_Structure_Migration.md](./01_Redis_Data_Structure_Migration.md) | Redis | SDS、跳表、字典、内存池迁移 |

---

## 概述

本目录包含真实的 C 项目迁移到 Zig 的完整案例，展示如何：

1. 保持 C ABI 兼容
2. 逐步替换实现
3. 利用 Zig 的类型安全
4. 保持性能不降级

### 迁移原则

```text
1. 外部接口保持 C 兼容
2. 内部实现使用 Zig 特性
3. 渐进式迁移，可回滚
4. 性能不降级
5. 添加 Zig 风格的 API 层
```

### 技术要点

- `export` 关键字导出 C 兼容函数
- `extern struct` 保证内存布局兼容
- `callconv(.C)` 确保调用约定一致
- `@ptrCast` 用于必要的指针转换

---

## 计划中的案例

- [x] Redis 数据结构迁移
- [ ] SQLite 迁移
- [ ] Lua 虚拟机迁移
- [ ] Nginx 模块迁移

---

> **状态**: 持续添加中
