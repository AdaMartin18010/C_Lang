# 系统编程示例

Linux/POSIX 系统调用可编译示例。

## 文件说明

| 文件 | 说明 |
|------|------|
| `mmap_example.c` | 内存映射文件，`mmap`/`msync`/`munmap` |
| `signal_handler.c` | `sigaction` 信号处理，统计 Ctrl+C 次数 |
| `process_fork.c` | `fork` + `pipe` 父子进程 IPC |
| `epoll_server.c` | `epoll` 边缘触发多路复用 TCP 服务器 |

## 编译运行

```bash
make
./mmap_example
./signal_handler
./process_fork
./epoll_server
```

> `epoll_server.c` 仅限 Linux。`mmap_example` 和 `signal_handler` 需 POSIX 环境。
