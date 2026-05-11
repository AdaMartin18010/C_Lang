# C 语言系统编程实战指南

> **难度**: L4-L5
> **平台**: Linux/POSIX
> **核心目标**: 掌握进程、内存、文件、信号的系统级接口

---

## 1. 进程管理

### 1.1 fork/exec 模式

```c
#include <unistd.h>
#include <sys/wait.h>

// 创建子进程执行命令
int run_command(const char *cmd, char *const argv[]) {
    pid_t pid = fork();

    if (pid < 0) {
        perror("fork");
        return -1;
    }

    if (pid == 0) {
        // 子进程
        execvp(cmd, argv);
        // exec 只有失败时才返回
        perror("execvp");
        _exit(127);
    }

    // 父进程：等待子进程完成
    int status;
    pid_t wpid = waitpid(pid, &status, 0);
    if (wpid < 0) {
        perror("waitpid");
        return -1;
    }

    if (WIFEXITED(status)) {
        return WEXITSTATUS(status);  // 子进程退出码
    }
    return -1;
}

// 使用
char *args[] = {"ls", "-la", "/tmp", NULL};
int rc = run_command("ls", args);
```

### 1.2 进程间通信：管道

```c
// 单向管道：父进程写，子进程读
int pipe_example(void) {
    int pipefd[2];
    pipe(pipefd);

    pid_t pid = fork();
    if (pid == 0) {
        // 子进程：读
        close(pipefd[1]);  // 关闭写端
        dup2(pipefd[0], STDIN_FILENO);
        close(pipefd[0]);
        execlp("wc", "wc", "-l", NULL);
        _exit(1);
    }

    // 父进程：写
    close(pipefd[0]);  // 关闭读端
    const char *data = "line1\nline2\nline3\n";
    write(pipefd[1], data, strlen(data));
    close(pipefd[1]);  // 关闭写端 → 子进程收到 EOF

    wait(NULL);
    return 0;
}
```

---

## 2. 内存映射 (mmap)

### 2.1 文件映射

```c
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>

// 将文件映射到内存，实现高效读写
void *map_file(const char *path, size_t *out_size) {
    int fd = open(path, O_RDONLY);
    if (fd < 0) return NULL;

    struct stat st;
    fstat(fd, &st);
    *out_size = st.st_size;

    // MAP_PRIVATE: 写操作触发 COW（不影响原文件）
    void *addr = mmap(NULL, st.st_size, PROT_READ, MAP_PRIVATE, fd, 0);
    close(fd);  // 映射后可以关闭 fd

    if (addr == MAP_FAILED) return NULL;
    return addr;
}

// 使用
size_t size;
char *data = map_file("data.bin", &size);
if (data) {
    process(data, size);
    munmap(data, size);
}
```

### 2.2 匿名映射（替代 malloc 大块内存）

```c
// 分配 1GB 虚拟地址空间（物理内存按需分配）
size_t len = 1UL << 30;
void *addr = mmap(NULL, len, PROT_READ | PROT_WRITE,
                   MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);

// 使用...

// 释放
munmap(addr, len);
```

### 2.3 共享内存（进程间通信）

```c
// 创建共享内存对象
int shm_fd = shm_open("/myshm", O_CREAT | O_RDWR, 0666);
ftruncate(shm_fd, 4096);

void *ptr = mmap(NULL, 4096, PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);

// 写入
sprintf(ptr, "Hello from PID %d", getpid());

// 另一个进程读取
int shm_fd2 = shm_open("/myshm", O_RDWR, 0666);
void *ptr2 = mmap(NULL, 4096, PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd2, 0);
printf("Read: %s\n", (char *)ptr2);
```

---

## 3. 信号处理

### 3.1 可靠信号处理

```c
#include <signal.h>

// 信号安全的全局标志
volatile sig_atomic_t g_reload_config = 0;

void signal_handler(int sig) {
    // 信号处理器中只能调用异步信号安全函数！
    // 安全：write, _exit, kill, sigaction, 设置 volatile sig_atomic_t 变量
    // 不安全：malloc, printf, 非原子变量操作

    if (sig == SIGHUP) {
        g_reload_config = 1;  // 主循环中处理
    }
}

void setup_signals(void) {
    struct sigaction sa = {
        .sa_handler = signal_handler,
        .sa_flags = SA_RESTART  // 自动重启被中断的系统调用
    };
    sigemptyset(&sa.sa_mask);

    sigaction(SIGHUP, &sa, NULL);   // 配置重载
    sigaction(SIGTERM, &sa, NULL);  // 优雅退出

    // 忽略 SIGPIPE（避免向关闭的 socket 写入导致崩溃）
    signal(SIGPIPE, SIG_IGN);
}

// 主循环
int main(void) {
    setup_signals();

    while (!g_should_exit) {
        if (g_reload_config) {
            g_reload_config = 0;
            load_config();  // 安全的上下文执行
        }
        // ...
    }
    return 0;
}
```

### 3.2 实时信号（队列化）

```c
// 标准信号（SIGUSR1）会合并，实时信号（SIGRTMIN+1）会排队
#define MY_SIGNAL (SIGRTMIN + 1)

// 发送带数据的信号
union sigval value = { .sival_int = 42 };
sigqueue(pid, MY_SIGNAL, value);

// 接收带数据的信号
void rt_handler(int sig, siginfo_t *info, void *context) {
    int data = info->si_value.sival_int;
    int sender_pid = info->si_pid;
}
```

---

## 4. 文件与 I/O

### 4.1 分散/聚集 I/O (readv/writev)

```c
#include <sys/uio.h>

// 一次性写入多个不连续缓冲区（减少系统调用次数）
void writev_example(int fd) {
    struct iovec iov[3];

    char header[] = "HTTP/1.1 200 OK\r\n";
    char body[] = "Hello, World!";
    char footer[] = "\r\n";

    iov[0].iov_base = header;
    iov[0].iov_len = strlen(header);
    iov[1].iov_base = body;
    iov[1].iov_len = strlen(body);
    iov[2].iov_base = footer;
    iov[2].iov_len = strlen(footer);

    writev(fd, iov, 3);  // 原子写入所有部分
}
```

### 4.2 文件锁（建议性锁）

```c
#include <fcntl.h>

// 建议性文件锁（进程需主动检查）
int lock_file(int fd, int exclusive) {
    struct flock fl = {
        .l_type = exclusive ? F_WRLCK : F_RDLCK,
        .l_whence = SEEK_SET,
        .l_start = 0,
        .l_len = 0  // 锁整个文件
    };
    return fcntl(fd, F_SETLKW, &fl);  // W = wait（阻塞）
}

int unlock_file(int fd) {
    struct flock fl = { .l_type = F_UNLCK };
    return fcntl(fd, F_SETLK, &fl);
}
```

---

## 5. 定时器

### 5.1 高精度定时器（timerfd）

```c
#include <sys/timerfd.h>

// 与 epoll 集成的定时器
int create_timer_ms(int initial_ms, int interval_ms) {
    int fd = timerfd_create(CLOCK_MONOTONIC, TFD_NONBLOCK | TFD_CLOEXEC);

    struct itimerspec its = {
        .it_value = { .tv_sec = initial_ms / 1000,
                      .tv_nsec = (initial_ms % 1000) * 1000000 },
        .it_interval = { .tv_sec = interval_ms / 1000,
                         .tv_nsec = (interval_ms % 1000) * 1000000 }
    };
    timerfd_settime(fd, 0, &its, NULL);
    return fd;
}

// 在 epoll 循环中使用
void on_timer_expired(int timer_fd) {
    uint64_t expirations;
    read(timer_fd, &expirations, sizeof(expirations));
    // expirations = 自上次读取以来到期的次数
    printf("Timer expired %lu times\n", expirations);
}
```

---

## 6. 检查清单

- [ ] `fork` 后检查返回值，处理父/子/错误三种情况
- [ ] `exec` 只有失败时才返回，必须处理错误并 `_exit`
- [ ] 信号处理器中只使用异步信号安全函数
- [ ] `mmap` 后检查 `MAP_FAILED`，使用 `munmap` 释放
- [ ] 管道/文件描述符及时关闭未使用的端
- [ ] 文件锁是建议性的，所有进程必须遵守约定

---

## 7. 配套代码示例

本节配套可编译代码位于 `examples/system/` 目录：

| 示例文件 | 演示内容 | 编译命令 |
|---------|---------|---------|
| `mmap_example.c` | 内存映射文件，`mmap`/`msync`/`munmap` | `gcc -O2 -std=c11` |
| `signal_handler.c` | `sigaction` 信号处理，统计 Ctrl+C 次数 | `gcc -O2 -std=c11` |
| `process_fork.c` | `fork` + `pipe` 父子进程 IPC | `gcc -O2 -std=c11` |
| `epoll_server.c` | `epoll` 边缘触发多路复用 TCP 服务器 | `gcc -O2 -std=c11` |

网络目录还有配套客户端：

| 示例文件 | 演示内容 | 编译命令 |
|---------|---------|---------|
| `../network/tcp_server.c` | `select` 多路复用 Echo 服务器 | `gcc -O2 -std=c11` |
| `../network/tcp_client.c` | TCP 客户端 | `gcc -O2 -std=c11` |

### 系统调用开销对比（x86-64 Linux）

| 调用 | 典型延迟 | 备注 |
|------|---------|------|
| `getpid()` | ~5 ns | vDSO，无内核进入 |
| `read()` (缓存命中) | ~50 ns | 快速路径 |
| `write()` (小数据) | ~100 ns | 快速路径 |
| `mmap()` | ~1 μs | 页表操作 |
| `fork()` | ~100 μs | 写时复制设置 |
| `execve()` | ~500 μs | 加载 ELF，重定位 |
| `open()` | ~1 μs | 路径解析 |

---

## 8. 常见错误模式

| 错误模式 | 后果 | 修复方案 |
|---------|------|---------|
| **未检查 `fork` 返回值** | 父子逻辑混乱，子进程无限 fork | 区分 `pid < 0` / `pid == 0` / `pid > 0` |
| **`mmap` 未检查 `MAP_FAILED`** | 解引用 `(void *)-1`，段错误 | 比较返回值 `== MAP_FAILED` |
| **信号处理器中调用非安全函数** | 死锁、堆损坏 | 只使用 async-signal-safe 函数列表 |
| **`fork` 后忘记关闭不需要的 fd** | fd 泄漏，子进程持有父资源 | 关闭所有不需要的 pipe/文件端 |
| **`wait` 遗漏导致僵尸进程** | PID 耗尽，系统资源泄漏 | 父进程必须 `wait`/`waitpid` 子进程 |
| **`epoll` ET 模式单次读取** | 数据残留，连接假死 | `while ((n = read(fd, buf, sizeof(buf))) > 0)` |

---

> **最后更新**: 2026-05-11
> **参考**: APUE (Stevens), The Linux Programming Interface (Kerrisk)
