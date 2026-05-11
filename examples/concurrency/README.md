# 并发编程示例

C11 `<threads.h>` 并发编程可编译示例。

## 文件说明

| 文件 | 说明 |
|------|------|
| `c11_threads.c` | `thrd_create`/`thrd_join` + `mtx_t` 互斥锁保护共享计数器 |
| `producer_consumer.c` | `cnd_t` 条件变量实现有界缓冲区生产者-消费者 |
| `atomic_operations.c` | `atomic_int` / `atomic_flag` 自旋锁，内存序演示 |

## 编译运行

```bash
make
./c11_threads
./producer_consumer
./atomic_operations
```

> 注：Windows MinGW 可能需要链接 `pthread` 或 `winpthread`。
