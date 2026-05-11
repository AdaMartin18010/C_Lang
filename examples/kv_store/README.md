# 内存 KV 存储引擎（工业实战项目）

一个基于开放寻址哈希表的内存键值存储引擎，展示了 C 语言数据结构、文件 I/O 和模块化设计的工业实践。

## 特性

- **O(1) 均摊操作**：开放寻址 + FNV-1a 哈希 + 线性探测
- **自动扩容**：负载因子达到 75% 时自动翻倍容量
- **墓碑机制**：删除标记支持安全探测链
- **持久化**：简单文本格式（`KEY\tVALUE\n`）
- **零外部依赖**：仅使用 C 标准库

## 文件结构

| 文件 | 说明 |
|------|------|
| `kv_store.h` | 公共 API（CRUD + 持久化 + 遍历） |
| `kv_store.c` | 实现：哈希表 + 开放寻址 |
| `test_kv.c` | 测试：CRUD、持久化、扩容、基准 |
| `Makefile` | 构建 |

## API

```c
KVStore *kv_create(size_t capacity);
void kv_destroy(KVStore *store);

int kv_set(KVStore *store, const char *key, const char *value);
const char *kv_get(KVStore *store, const char *key);
int kv_delete(KVStore *store, const char *key);

int kv_save(KVStore *store, const char *path);
int kv_load(KVStore *store, const char *path);
```

## 设计决策

| 决策 | 选择 | 理由 |
|------|------|------|
| 哈希算法 | FNV-1a | 简单、快速、分布均匀 |
| 冲突解决 | 线性探测 | 缓存友好，实现简单 |
| 负载因子 | 0.75 | 时间与空间平衡 |
| 键值存储 | strdup | 简单安全，调用方无需管理生命周期 |
| 持久化格式 | 文本 TSV | 人类可读，调试友好 |

## 编译运行

```bash
make
./test_kv
```

## 扩展方向

1. **WAL（预写日志）**：崩溃恢复
2. **LRU 淘汰**：内存限制下的过期策略
3. **快照**：写时复制实现一致性快照
4. **网络协议**：RESP 或自定义二进制协议
5. **LSM-Tree**：磁盘持久化 + 压缩
