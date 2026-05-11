# 无锁数据结构（工业实战项目）

C11 `_Atomic` 实现的无锁单生产者单消费者（SPSC）环形缓冲区。

## 特性

- **100% 无锁**：仅使用 `_Atomic` 变量，无 mutex/spinlock
- **SPSC 安全**：单生产者 + 单消费者，无 ABA 问题
- **内存序优化**：`memory_order_release`/`acquire` 保证可见性
- **环绕支持**：自动处理缓冲区环绕读写
- **零动态分配**：创建后无堆操作

## 设计原理

```
        head (生产者)          tail (消费者)
           ↓                      ↓
    [=======WRITTEN=======|====FREE====]
           ↑                      ↑
        可读取区域            可写入区域
```

**关键 invariant**：
- 生产者只写 `head`，消费者只读 `head`
- 消费者只写 `tail`，生产者只读 `tail`
- 留一个空位区分满/空状态

## 内存序选择

| 操作 | 内存序 | 理由 |
|------|--------|------|
| 生产者读 tail | `acquire` | 看到消费者最后的写入 |
| 生产者写 head | `release` | 消费者能看到新数据 |
| 消费者读 head | `acquire` | 看到生产者最后的写入 |
| 消费者写 tail | `release` | 生产者能看到新空间 |

## 编译运行

```bash
make
./test_lockfree
```

> Windows MinGW 可能不支持 C11 threads.h，需要 POSIX 环境。

## 扩展方向

1. **MPMC**：使用 CAS 循环实现多生产者多消费者
2. **批量操作**：一次提交/消费多个元素，减少原子操作次数
3. **缓存行填充**：防止 head/tail 在同一缓存行产生 false sharing
4. **等待策略**：忙等待 → 指数退避 → futex/WaitOnAddress

## 性能对比

| 同步方式 | 单线程延迟 | 多线程扩展性 | 复杂度 |
|---------|-----------|------------|--------|
| Mutex | ~50 ns | 差（竞争时阻塞） | 低 |
| Spinlock | ~20 ns | 差（忙等待浪费 CPU） | 低 |
| **SPSC Ring** | ~10 ns | 理想（无竞争） | 中 |
| MPMC Queue | ~30 ns | 好（CAS 竞争） | 高 |
