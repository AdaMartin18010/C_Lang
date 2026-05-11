# Arena 分配器（工业实战项目）

一个工业级 Arena（Region/Pool）内存分配器实现，展示了 C 语言内存管理的核心技术。

## 特性

- **O(1) 分配**： bump pointer 分配，无元数据开销
- **自动对齐**：8字节默认对齐，支持自定义对齐
- **批量释放**：`arena_reset()` 一次性重置所有内存
- **零碎片**：同生命周期对象集中管理
- **块扩展**：自动分配新块处理大对象

## 文件结构

| 文件 | 说明 |
|------|------|
| `arena.h` | 公共 API 头文件（不透明指针） |
| `arena.c` | 实现：Block 链表 + bump pointer |
| `test_arena.c` | 测试与基准程序 |
| `Makefile` | 构建 |

## API

```c
Arena *arena_create(size_t block_size);   /* 创建分配器 */
void arena_destroy(Arena *a);              /* 销毁所有内存 */
void arena_reset(Arena *a);                /* 重置（复用块） */
void *arena_alloc(Arena *a, size_t size);  /* 分配内存 */

/* 便捷宏 */
arena_new(a, Point);       /* 分配单个对象 */
arena_new_n(a, int, 100);  /* 分配数组 */
```

## 使用场景

| 场景 | 示例 |
|------|------|
| 编译器 AST 节点 | 解析阶段统一分配，结束时批量释放 |
| 游戏帧分配 | 每帧开始时 reset，帧内分配临时对象 |
| HTTP 请求处理 | 请求开始时创建 arena，响应后销毁 |
| 解析配置文件 | 解析期间分配字符串和结构体，完成后释放 |

## 性能基准

典型结果（x86-64, 100K x 64B 分配）：

| 分配器 | 时间 | 操作次数 |
|--------|------|---------|
| Arena | ~5 ms | 0 free |
| malloc/free | ~50 ms | 100K alloc + 100K free |
| **加速比** | **~10x** | - |

## 编译运行

```bash
make
./test_arena
```

## 与 malloc 的对比

| 特性 | Arena | malloc |
|------|-------|--------|
| 分配速度 | 极快（指针加法） | 慢（需查找空闲块） |
| 释放速度 | 批量 O(1) | 每次 O(1) |
| 碎片 | 无 | 有 |
| 生命周期 | 统一 | 独立 |
| 线程安全 | 需外部同步 | 通常线程安全 |
| 适用场景 | 同生命周期对象 | 独立生命周期对象 |
