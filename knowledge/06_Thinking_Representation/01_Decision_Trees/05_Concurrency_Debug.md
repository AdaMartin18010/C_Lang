# 并发问题调试决策树

> **用途**: 死锁、竞态条件、线程问题诊断 | **难度**: L4 | **更新**: 2026-03-15

---

## 快速诊断流程

```text
程序出现并发问题?
        │
        ▼
┌─────────────────┐
│ 问题可重现?      │
└────────┬────────┘
    是 /   \ 否
      /     \
     ▼       ▼
┌────────┐  ┌─────────────────┐
│确定性  │  │ 竞态条件         │
│问题    │  │ (Heisenbug)     │
└───┬────┘  └────────┬────────┘
    │                │
    ▼                ▼
┌────────────┐  ┌─────────────────┐
│检查加锁顺序 │  │使用工具检测      │
│检查资源竞争 │  │ThreadSanitizer  │
└────────────┘  └─────────────────┘
```

## 死锁诊断

### 症状

- 程序无响应
- 线程卡在锁等待
- CPU使用率突然下降

### 诊断步骤

```bash
# 1. 查看线程状态
gdb -p <pid>
(gdb) info threads
(gdb) thread apply all bt

# 2. 检查锁的持有情况
(gdb) frame <n>
(gdb) info locals
(gdb) print mutex

# 3. 使用pstack快速查看
pstack <pid> | grep -A5 "mutex\|lock"
```

### 常见死锁模式

```c
// ❌ 死锁模式1: 加锁顺序不一致
Thread A: lock(m1); lock(m2);
Thread B: lock(m2); lock(m1);

// ✅ 解决方案: 统一加锁顺序
// 所有线程按相同顺序加锁: m1 -> m2

// ❌ 死锁模式2: 自锁
pthread_mutex_lock(&m);
// ... 同一线程再次加锁 ...
pthread_mutex_lock(&m);  // 死锁!

// ✅ 解决方案: 使用递归锁
pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE);
```

## 竞态条件诊断

### 症状

- 结果不稳定
- 随机崩溃
- 数据不一致

### 工具检测

```bash
# ThreadSanitizer (推荐)
gcc -fsanitize=thread program.c -o program
./program

# Helgrind (Valgrind)
valgrind --tool=helgrind ./program

# DRD (Data Race Detector)
valgrind --tool=drd ./program
```

### 常见竞态模式

```c
// ❌ 竞态条件: 非原子操作
int counter = 0;
// Thread A & B:
counter++;  // 读取-修改-写入,非原子

// ✅ 解决方案1: 原子操作
_Atomic int counter = 0;
counter++;

// ✅ 解决方案2: 互斥锁
pthread_mutex_lock(&m);
counter++;
pthread_mutex_unlock(&m);

// ✅ 解决方案3: C11原子
atomic_fetch_add(&counter, 1, memory_order_relaxed);
```

## 调试技巧

### 1. 日志记录

```c
#define DEBUG_LOG(fmt, ...) \
    printf("[%s:%d][%lu] " fmt "\n", \
           __FILE__, __LINE__, pthread_self(), ##__VA_ARGS__)

void thread_func(void* arg) {
    DEBUG_LOG("Starting, arg=%p", arg);

    DEBUG_LOG("Locking mutex");
    pthread_mutex_lock(&mutex);

    DEBUG_LOG("In critical section");
    // ...

    DEBUG_LOG("Unlocking mutex");
    pthread_mutex_unlock(&mutex);
}
```

### 2. 锁监控

```c
// 包装锁操作以追踪
void tracked_lock(pthread_mutex_t* m, const char* file, int line) {
    printf("[%s:%d] Locking at %s:%d\n",
           __FILE__, __LINE__, file, line);
    pthread_mutex_lock(m);
}

#define LOCK(m) tracked_lock(m, __FILE__, __LINE__)
```

### 3. 压力测试

```c
// 增加上下文切换概率
void stress_delay(void) {
    usleep(rand() % 1000);  // 随机延迟
    sched_yield();           // 强制让出CPU
}
```

## 工具速查

| 工具 | 用途 | 命令 |
|:-----|:-----|:-----|
| GDB | 线程调试 | `gdb -p <pid>` |
| ThreadSanitizer | 竞态检测 | `-fsanitize=thread` |
| Helgrind | 锁错误 | `valgrind --tool=helgrind` |
| strace | 系统调用 | `strace -f ./program` |
| perf | 性能分析 | `perf record -g ./program` |

## 最佳实践

1. **最小化临界区** - 减少锁持有时间
2. **避免嵌套锁** - 降低死锁风险
3. **使用条件变量** - 而非忙等待
4. **静态分析工具** - 编译时检测问题
5. **代码审查** - 重点检查并发逻辑

---

> **状态**: ✅ 已完成
