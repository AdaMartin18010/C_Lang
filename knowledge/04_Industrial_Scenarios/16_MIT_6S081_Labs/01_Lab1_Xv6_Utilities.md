# MIT 6.S081 Lab 1: Xv6 and Unix Utilities

## 1. 实验概述

### 1.1 目标

本实验帮助你：

- 熟悉xv6操作系统环境
- 掌握基本的Unix系统调用
- 理解进程创建与通信机制

### 1.2 实验环境

```bash
# 克隆xv6仓库
git clone git://github.com/mit-pdos/xv6-riscv.git
cd xv6-riscv

# 构建并运行
make qemu

# 退出QEMU
Ctrl+A然后X
```

---

## 2. sleep (简单)

### 2.1 任务描述

实现一个`sleep`程序，接受一个参数（ticks数），调用系统调用sleep暂停相应时间。

### 2.2 代码实现

```c
// user/sleep.c
#include "kernel/types.h"
#include "user/user.h"

int main(int argc, char *argv[]) {
    // 参数检查
    if (argc != 2) {
        fprintf(2, "Usage: sleep ticks\n");
        exit(1);
    }

    // 将字符串转换为整数
    int ticks = atoi(argv[1]);

    // 调用sleep系统调用
    sleep(ticks);

    exit(0);
}
```

### 2.3 关键知识点

```c
// 系统调用原型 (user.h)
int sleep(int);

// 进程退出
void exit(int status);

// 文件描述符
// 0 = 标准输入 (stdin)
// 1 = 标准输出 (stdout)
// 2 = 标准错误 (stderr)
```

### 2.4 Makefile修改

```makefile
# 在UPROGS中添加
UPROGS=\
    ...
    $U/_sleep\
    ...
```

---

## 3. pingpong (中等)

### 3.1 任务描述

使用管道在两个进程间通信。父进程发送字节给子进程，子进程接收后打印，再回复给父进程。

### 3.2 代码实现

```c
// user/pingpong.c
#include "kernel/types.h"
#include "user/user.h"

int main() {
    int p1[2], p2[2];
    char buf[1];

    // 创建两个管道
    // p1: 父进程 -> 子进程
    // p2: 子进程 -> 父进程
    pipe(p1);
    pipe(p2);

    if (fork() == 0) {
        // 子进程
        close(p1[1]);  // 关闭p1写端
        close(p2[0]);  // 关闭p2读端

        // 从父进程接收
        if (read(p1[0], buf, 1) != 1) {
            fprintf(2, "child read failed\n");
            exit(1);
        }

        // 打印接收信息
        printf("%d: received ping\n", getpid());

        // 回复父进程
        if (write(p2[1], buf, 1) != 1) {
            fprintf(2, "child write failed\n");
            exit(1);
        }

        close(p1[0]);
        close(p2[1]);
        exit(0);

    } else {
        // 父进程
        close(p1[0]);  // 关闭p1读端
        close(p2[1]);  // 关闭p2写端

        // 发送给子进程
        buf[0] = 'x';
        if (write(p1[1], buf, 1) != 1) {
            fprintf(2, "parent write failed\n");
            exit(1);
        }

        // 等待子进程回复
        if (read(p2[0], buf, 1) != 1) {
            fprintf(2, "parent read failed\n");
            exit(1);
        }

        printf("%d: received pong\n", getpid());

        close(p1[1]);
        close(p2[0]);

        // 等待子进程结束
        wait(0);
        exit(0);
    }
}
```

### 3.3 进程通信图示

```
父进程                  子进程
   |                       |
   |--write(p1)-->         |
   |      'x'              |
   |                       |--read(p1)-->
   |                       |    'x'
   |                       |
   |                       |printf("received ping")
   |                       |
   |                       |--write(p2)-->
   |                       |    'x'
   |--read(p2)-->          |
   |      'x'              |
   |                       |
 printf("received pong")   exit()
   |
 wait()
 exit()
```

---

## 4. primes (困难)

### 4.1 任务描述

使用管道和进程递归实现埃拉托斯特尼筛法，找出2到35之间的素数。

### 4.2 算法原理

```
阶段1: 2是素数，过滤所有2的倍数
阶段2: 3是素数，过滤所有3的倍数
阶段3: 5是素数，过滤所有5的倍数
...
```

### 4.3 代码实现

```c
// user/primes.c
#include "kernel/types.h"
#include "user/user.h"

void sieve(int p[2]) {
    int prime;

    // 从上一个进程读取第一个数（就是素数）
    if (read(p[0], &prime, sizeof(prime)) == 0) {
        close(p[0]);
        exit(0);
    }

    printf("prime %d\n", prime);

    // 创建新管道给下一个进程
    int p2[2];
    pipe(p2);

    if (fork() == 0) {
        // 子进程：继续筛
        close(p[0]);
        close(p2[1]);
        sieve(p2);
    } else {
        // 当前进程：过滤倍数
        close(p2[0]);

        int n;
        while (read(p[0], &n, sizeof(n)) != 0) {
            if (n % prime != 0) {
                // 不是倍数，传给下一个进程
                write(p2[1], &n, sizeof(n));
            }
        }

        close(p[0]);
        close(p2[1]);
        wait(0);
        exit(0);
    }
}

int main() {
    int p[2];
    pipe(p);

    if (fork() == 0) {
        // 子进程：开始筛
        close(p[1]);
        sieve(p);
    } else {
        // 父进程：生成2-35
        close(p[0]);

        for (int i = 2; i <= 35; i++) {
            write(p[1], &i, sizeof(i));
        }

        close(p[1]);
        wait(0);
        exit(0);
    }
}
```

### 4.4 进程结构图

```
父进程(生成2-35)
    |
    v
进程2(读取2，过滤2的倍数)
    |
    v
进程3(读取3，过滤3的倍数)
    |
    v
进程5(读取5，过滤5的倍数)
    |
    v
...
```

---

## 5. find (中等)

### 5.1 任务描述

实现简化版Unix find命令，递归查找目录中匹配文件名的文件。

### 5.2 代码实现

```c
// user/find.c
#include "kernel/types.h"
#include "user/user.h"
#include "kernel/fs.h"
#include "kernel/fcntl.h"

void find(char *path, char *name) {
    char buf[512], *p;
    int fd;
    struct dirent de;
    struct stat st;

    // 打开目录
    if ((fd = open(path, O_RDONLY)) < 0) {
        fprintf(2, "find: cannot open %s\n", path);
        return;
    }

    // 获取文件状态
    if (fstat(fd, &st) < 0) {
        fprintf(2, "find: cannot stat %s\n", path);
        close(fd);
        return;
    }

    // 必须是目录
    if (st.type != T_DIR) {
        close(fd);
        return;
    }

    // 路径长度检查
    if (strlen(path) + 1 + DIRSIZ + 1 > sizeof(buf)) {
        printf("find: path too long\n");
        close(fd);
        return;
    }

    strcpy(buf, path);
    p = buf + strlen(buf);
    *p++ = '/';

    // 读取目录项
    while (read(fd, &de, sizeof(de)) == sizeof(de)) {
        if (de.inum == 0)
            continue;

        // 跳过 . 和 ..
        if (strcmp(de.name, ".") == 0 || strcmp(de.name, "..") == 0)
            continue;

        // 构建完整路径
        memmove(p, de.name, DIRSIZ);
        p[DIRSIZ] = 0;

        // 获取文件状态
        if (stat(buf, &st) < 0) {
            printf("find: cannot stat %s\n", buf);
            continue;
        }

        // 如果是文件且匹配，打印
        if (st.type == T_FILE && strcmp(de.name, name) == 0) {
            printf("%s\n", buf);
        }

        // 如果是目录，递归查找
        if (st.type == T_DIR) {
            find(buf, name);
        }
    }

    close(fd);
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(2, "Usage: find <path> <name>\n");
        exit(1);
    }

    find(argv[1], argv[2]);
    exit(0);
}
```

---

## 6. xargs (困难)

### 6.1 任务描述

实现简化版xargs，从标准输入读取行，为每行执行命令。

### 6.2 代码实现

```c
// user/xargs.c
#include "kernel/types.h"
#include "user/user.h"
#include "kernel/param.h"

int main(int argc, char *argv[]) {
    char buf[512];
    char *args[MAXARG];
    int i;

    // 复制原始参数
    for (i = 1; i < argc; i++) {
        args[i - 1] = argv[i];
    }

    // 从stdin读取行
    while (gets(buf, sizeof(buf))) {
        // 去掉换行符
        buf[strlen(buf) - 1] = 0;

        // 添加为最后一个参数
        args[argc - 1] = buf;
        args[argc] = 0;

        if (fork() == 0) {
            // 子进程执行命令
            exec(args[0], args);
            fprintf(2, "xargs: exec %s failed\n", args[0]);
            exit(1);
        } else {
            // 父进程等待
            wait(0);
        }
    }

    exit(0);
}
```

### 6.3 使用示例

```bash
$ echo hello | xargs echo
hello

$ ls | xargs echo
. .. README cat echo forktest grep init kill ln ls mkdir rm sh stressfs usertests wc zombie
```

---

## 7. 调试技巧

### 7.1 常见问题

```
问题1: make qemu报错
解决: 检查RISC-V工具链安装
$ sudo apt-get install git build-essential gdb-multiarch qemu-system-misc gcc-riscv64-linux-gnu binutils-riscv64-linux-gnu

问题2: 程序输出不正确
解决: 使用printf调试

问题3: 段错误
解决: 检查数组越界和空指针
```

### 7.2 测试命令

```bash
# 测试sleep
$ sleep 10

# 测试pingpong
$ pingpong
4: received ping
3: received pong

# 测试primes
$ primes
prime 2
prime 3
prime 5
...

# 测试find
$ mkdir -p a/b/c
$ echo > a/b/c/test.txt
$ find a test.txt
a/b/c/test.txt
```

---

## 8. 总结

本实验涵盖的核心概念：

- **系统调用**: sleep, exit, fork, wait, exec
- **进程间通信**: pipe, read, write
- **文件系统**: open, close, read, fstat
- **进程管理**: fork/exec模式

---

**难度**: ⭐
**预计时间**: 4-6小时
**创建日期**: 2026-03-19
