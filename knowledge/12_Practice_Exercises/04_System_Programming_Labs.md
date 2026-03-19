# 系统编程实验 (System Programming Labs)

> **难度**: L3-L5 | **预计时间**: 20-30小时
> **环境**: Linux/Unix

---

## 实验1: 简易Shell实现

### 描述

实现一个支持基本功能的命令行解释器。

### 功能要求

```c
// 基础功能
$ ./mysh
mysh> ls -la
total 128
drwxr-xr-x  3 user user  4096 Mar 14 10:00 .
...
mysh> pwd
/home/user/project
mysh> cd ..
mysh> exit
$
```

### 需要实现

1. **命令执行**: fork + execve
2. **内建命令**: cd, pwd, exit, help
3. **输入重定向**: `cmd < input.txt`
4. **输出重定向**: `cmd > output.txt`
5. **管道**: `cmd1 | cmd2 | cmd3`
6. **后台执行**: `cmd &`

### 核心代码框架

```c
#include <sys/wait.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_ARGS 64
#define MAX_LINE 1024

typedef struct {
    char *args[MAX_ARGS];
    int argc;
    char *input_file;
    char *output_file;
    bool background;
} Command;

// 解析命令行
void parse_line(char *line, Command *cmd) {
    // 实现参数分割、重定向解析、管道检测
}

// 执行单个命令
int execute_command(Command *cmd) {
    pid_t pid = fork();
    if (pid == 0) {
        // 子进程
        if (cmd->input_file) {
            int fd = open(cmd->input_file, O_RDONLY);
            dup2(fd, STDIN_FILENO);
            close(fd);
        }
        if (cmd->output_file) {
            int fd = open(cmd->output_file, O_WRONLY | O_CREAT | O_TRUNC, 0644);
            dup2(fd, STDOUT_FILENO);
            close(fd);
        }
        execvp(cmd->args[0], cmd->args);
        perror("execvp");
        exit(1);
    } else if (pid > 0) {
        // 父进程
        if (!cmd->background) {
            waitpid(pid, NULL, 0);
        } else {
            printf("[%d] %d\n", job_id, pid);
        }
    }
    return 0;
}

// 执行管道命令
int execute_pipeline(Command *cmds, int num_cmds) {
    int pipes[num_cmds - 1][2];

    // 创建管道
    for (int i = 0; i < num_cmds - 1; i++) {
        pipe(pipes[i]);
    }

    // 创建进程并连接管道
    for (int i = 0; i < num_cmds; i++) {
        pid_t pid = fork();
        if (pid == 0) {
            // 设置输入
            if (i > 0) {
                dup2(pipes[i-1][0], STDIN_FILENO);
            }
            // 设置输出
            if (i < num_cmds - 1) {
                dup2(pipes[i][1], STDOUT_FILENO);
            }
            // 关闭所有管道fd
            for (int j = 0; j < num_cmds - 1; j++) {
                close(pipes[j][0]);
                close(pipes[j][1]);
            }
            execvp(cmds[i].args[0], cmds[i].args);
            exit(1);
        }
    }

    // 父进程关闭管道并等待
    for (int i = 0; i < num_cmds - 1; i++) {
        close(pipes[i][0]);
        close(pipes[i][1]);
    }
    for (int i = 0; i < num_cmds; i++) {
        wait(NULL);
    }
    return 0;
}
```

### 进阶功能

- 作业控制 (fg, bg, jobs)
- 命令历史 (上下箭头)
- Tab补全
- 信号处理 (Ctrl+C, Ctrl+Z)
- 环境变量支持

---

## 实验2: 文件拷贝工具

### 描述

实现一个支持多种功能的文件拷贝程序。

### 基础版本

```c
#include <fcntl.h>
#include <unistd.h>

int copy_file(const char *src, const char *dst) {
    int fd_in = open(src, O_RDONLY);
    int fd_out = open(dst, O_WRONLY | O_CREAT | O_TRUNC, 0644);

    char buffer[8192];
    ssize_t n;
    while ((n = read(fd_in, buffer, sizeof(buffer))) > 0) {
        write(fd_out, buffer, n);
    }

    close(fd_in);
    close(fd_out);
    return 0;
}
```

### 进阶要求

1. **进度条显示**

```c
void show_progress(off_t current, off_t total) {
    int percent = (int)(current * 100 / total);
    printf("\r[%3d%%] ", percent);
    int pos = percent / 2;
    for (int i = 0; i < 50; i++) {
        putchar(i < pos ? '=' : ' ');
    }
    printf(" %ld/%ld MB", current / 1024 / 1024, total / 1024 / 1024);
    fflush(stdout);
}
```

1. **零拷贝优化** (sendfile)

```c
#include <sys/sendfile.h>

off_t copy_sendfile(int fd_in, int fd_out, off_t count) {
    off_t offset = 0;
    return sendfile(fd_out, fd_in, &offset, count);
}
```

1. **内存映射拷贝** (mmap)

```c
#include <sys/mman.h>

void copy_mmap(const char *src, const char *dst) {
    int fd_in = open(src, O_RDONLY);
    int fd_out = open(dst, O_RDWR | O_CREAT | O_TRUNC, 0644);

    struct stat st;
    fstat(fd_in, &st);
    ftruncate(fd_out, st.st_size);

    void *src_map = mmap(NULL, st.st_size, PROT_READ, MAP_PRIVATE, fd_in, 0);
    void *dst_map = mmap(NULL, st.st_size, PROT_WRITE, MAP_SHARED, fd_out, 0);

    memcpy(dst_map, src_map, st.st_size);

    munmap(src_map, st.st_size);
    munmap(dst_map, st.st_size);
    close(fd_in);
    close(fd_out);
}
```

1. **性能对比**

```
$ ./copy_benchmark large_file.bin
read/write:  500 MB/s
sendfile:    2.5 GB/s
mmap:        1.8 GB/s
```

---

## 实验3: 进程池实现

### 描述

实现一个进程池用于并行任务处理。

### 设计

```c
typedef struct ProcessPool ProcessPool;
typedef void (*TaskFunc)(void *arg);

typedef struct {
    TaskFunc func;
    void *arg;
    void *result;
} Task;

ProcessPool* pool_create(int num_workers);
void pool_destroy(ProcessPool *pool);

// 提交任务
int pool_submit(ProcessPool *pool, TaskFunc func, void *arg);

// 获取结果
void* pool_get_result(ProcessPool *pool, int task_id);

// 等待所有任务完成
void pool_wait_all(ProcessPool *pool);
```

### 实现思路

使用共享内存 + 信号量实现进程间通信：

```c
typedef struct {
    Task task_queue[QUEUE_SIZE];
    int head, tail;
    sem_t empty_slots;
    sem_t filled_slots;
    pthread_mutex_t mutex;
} SharedQueue;

// 主进程: 将任务放入队列
// 工作进程: 从队列取出任务执行
```

### 应用场景

- 并行图片处理
- 日志分析
- 批量文件转换

---

## 实验4: 网络聊天室

### 描述

实现一个基于TCP的多用户聊天服务器。

### 服务器功能

```c
// 支持多客户端（select/poll/epoll）
// 消息广播
// 用户注册/登录
// 私聊功能
// 在线用户列表

typedef struct Client {
    int fd;
    char username[32];
    bool authenticated;
    struct Client *next;
} Client;

typedef struct {
    int listen_fd;
    Client *clients;
    fd_set read_fds;
    int max_fd;
} ChatServer;

void server_init(ChatServer *server, int port);
void server_run(ChatServer *server);
void server_broadcast(ChatServer *server, const char *msg, int exclude_fd);
void server_private_msg(ChatServer *server, const char *to_user, const char *msg);
```

### 协议设计

```
客户端 -> 服务器:
  LOGIN username password
  MSG message_content
  PRIVMSG username message
  LIST
  QUIT

服务器 -> 客户端:
  OK message
  ERROR message
  MSG from_user message
  USERLIST user1,user2,user3
```

### 客户端实现

```c
// 使用select同时处理键盘输入和网络数据
fd_set fds;
FD_ZERO(&fds);
FD_SET(STDIN_FILENO, &fds);
FD_SET(sock_fd, &fds);

select(max_fd + 1, &fds, NULL, NULL, NULL);

if (FD_ISSET(STDIN_FILENO, &fds)) {
    // 读取用户输入并发送
}
if (FD_ISSET(sock_fd, &fds)) {
    // 接收服务器消息并显示
}
```

---

## 实验5: 内存分析工具

### 描述

实现一个简单的内存使用分析器。

### 功能

```c
// 拦截malloc/free
#define malloc(size) my_malloc(size, __FILE__, __LINE__)
#define free(ptr) my_free(ptr)

// 记录分配信息
typedef struct AllocInfo {
    void *ptr;
    size_t size;
    char file[64];
    int line;
    time_t timestamp;
    struct AllocInfo *next;
} AllocInfo;

// 报告
void mem_report(void);
void mem_dump_leaks(void);
void mem_stats(size_t *total_alloc, size_t *total_free, size_t *current);
```

### 输出示例

```
Memory Report:
  Total allocations: 1024
  Total frees: 1020
  Current usage: 4096 bytes
  Peak usage: 16384 bytes

Leak Report:
  [main.c:42] 256 bytes not freed
  [utils.c:15] 128 bytes not freed
```

---

## 实验6: 文件系统监控

### 描述

实现一个监控目录变化的工具（类似inotifywait）。

### 使用inotify API

```c
#include <sys/inotify.h>

int watch_directory(const char *path) {
    int fd = inotify_init();
    int wd = inotify_add_watch(fd, path,
        IN_CREATE | IN_DELETE | IN_MODIFY | IN_MOVED_FROM | IN_MOVED_TO);

    char buffer[4096];
    while (1) {
        ssize_t len = read(fd, buffer, sizeof(buffer));

        for (char *ptr = buffer; ptr < buffer + len; ) {
            struct inotify_event *event = (struct inotify_event *)ptr;

            if (event->mask & IN_CREATE)
                printf("Created: %s\n", event->name);
            if (event->mask & IN_DELETE)
                printf("Deleted: %s\n", event->name);
            if (event->mask & IN_MODIFY)
                printf("Modified: %s\n", event->name);

            ptr += sizeof(struct inotify_event) + event->len;
        }
    }

    inotify_rm_watch(fd, wd);
    close(fd);
    return 0;
}
```

### 功能扩展

- 递归监控子目录
- 输出JSON格式
- 执行触发命令
- 文件哈希校验

---

## 评估标准

| 项目 | 权重 |
|:-----|:----:|
| 功能完整性 | 30% |
| 代码质量 | 25% |
| 错误处理 | 20% |
| 文档注释 | 15% |
| 创新性 | 10% |

---

> **返回导航**: [练习模块总览](./README.md) | [知识库总览](../README.md)


---

## 深入理解

### 核心原理

深入探讨技术原理和实现细节。

### 实践应用

- 应用场景1
- 应用场景2
- 应用场景3

### 最佳实践

1. 理解基础概念
2. 掌握核心机制
3. 应用到实际项目

---

> **最后更新**: 2026-03-21  
> **维护者**: AI Code Review
