# MIT 6.S081 Lab 9: File System

## 1. 实验概述

### 1.1 目标

- 理解xv6文件系统结构
- 实现大文件支持
- 实现符号链接

### 1.2 xv6文件系统结构

```text
xv6文件系统布局:

磁盘分区:
┌──────────┬──────────┬──────────┬──────────┬──────────┐
│  Boot    │ Superblock│  Log     │  Inodes  │  Bitmap  │  Data   │
│  Block   │          │          │          │          │ Blocks  │
│   (0)    │   (1)    │ (2-31)   │(32-199)  │(200-233) │(234+)   │
└──────────┴──────────┴──────────┴──────────┴──────────┘

关键数据结构:
- Superblock: 文件系统元信息
- Inode: 文件索引节点
- Bitmap: 数据块分配图
- Data Blocks: 实际文件数据

Inode结构:
- 类型 (文件/目录/设备)
- 大小
- 直接块指针 (NDIRECT = 12)
- 间接块指针 (1个)
```

---

## 2. 大文件支持

### 2.1 原始设计限制

```c
// fs.h
#define NDIRECT 12
#define NINDIRECT (BSIZE / sizeof(uint))
#define MAXFILE (NDIRECT + NINDIRECT)  // 12 + 256 = 268 blocks
// 最大文件大小: 268 * 1024 = 274,432 bytes (约268KB)

// 原始inode结构
struct dinode {
    short type;
    short major;
    short minor;
    short nlink;
    uint size;
    uint addrs[NDIRECT + 1];  // 直接块 + 一级间接块
};
```

### 2.2 二级间接块设计

```c
// fs.h (修改后)
#define NDIRECT 11              // 减少1个直接块
#define NINDIRECT (BSIZE / sizeof(uint))   // 256
#define NDINDIRECT (NINDIRECT * NINDIRECT) // 65536
#define MAXFILE (NDIRECT + NINDIRECT + NDINDIRECT)
// 最大文件大小: 11 + 256 + 65536 = 65803 blocks
// 约 64MB

// 新的inode结构
struct dinode {
    short type;
    short major;
    short minor;
    short nlink;
    uint size;
    uint addrs[NDIRECT + 2];  // 直接块 + 一级间接 + 二级间接
};
// addrs[0-10]: 直接块
// addrs[11]: 一级间接块
// addrs[12]: 二级间接块
```

### 2.3 bmap函数实现

```c
// kernel/fs.c

// 将文件逻辑块号映射到磁盘块号
static uint bmap(struct inode *ip, uint bn) {
    uint addr, *a;
    struct buf *bp;

    // 直接块
    if (bn < NDIRECT) {
        if ((addr = ip->addrs[bn]) == 0)
            ip->addrs[bn] = addr = balloc(ip->dev);
        return addr;
    }
    bn -= NDIRECT;

    // 一级间接块
    if (bn < NINDIRECT) {
        // 加载间接块
        if ((addr = ip->addrs[NDIRECT]) == 0)
            ip->addrs[NDIRECT] = addr = balloc(ip->dev);

        bp = bread(ip->dev, addr);
        a = (uint*)bp->data;

        if ((addr = a[bn]) == 0) {
            a[bn] = addr = balloc(ip->dev);
            log_write(bp);
        }
        brelse(bp);
        return addr;
    }
    bn -= NINDIRECT;

    // 二级间接块 (新增)
    if (bn < NDINDIRECT) {
        // 加载二级间接块
        if ((addr = ip->addrs[NDIRECT + 1]) == 0)
            ip->addrs[NDIRECT + 1] = addr = balloc(ip->dev);

        bp = bread(ip->dev, addr);
        a = (uint*)bp->data;

        // 计算一级间接块索引
        uint idx1 = bn / NINDIRECT;
        uint idx2 = bn % NINDIRECT;

        // 加载一级间接块
        if ((addr = a[idx1]) == 0) {
            a[idx1] = addr = balloc(ip->dev);
            log_write(bp);
        }
        brelse(bp);

        bp = bread(ip->dev, addr);
        a = (uint*)bp->data;

        if ((addr = a[idx2]) == 0) {
            a[idx2] = addr = balloc(ip->dev);
            log_write(bp);
        }
        brelse(bp);
        return addr;
    }

    panic("bmap: out of range");
}
```

### 2.4 itrunc函数实现

```c
// 释放inode的数据块
void itrunc(struct inode *ip) {
    int i, j;
    struct buf *bp, *bp2;
    uint *a, *a2;

    // 释放直接块
    for (i = 0; i < NDIRECT; i++) {
        if (ip->addrs[i]) {
            bfree(ip->dev, ip->addrs[i]);
            ip->addrs[i] = 0;
        }
    }

    // 释放一级间接块
    if (ip->addrs[NDIRECT]) {
        bp = bread(ip->dev, ip->addrs[NDIRECT]);
        a = (uint*)bp->data;
        for (j = 0; j < NINDIRECT; j++) {
            if (a[j])
                bfree(ip->dev, a[j]);
        }
        brelse(bp);
        bfree(ip->dev, ip->addrs[NDIRECT]);
        ip->addrs[NDIRECT] = 0;
    }

    // 释放二级间接块 (新增)
    if (ip->addrs[NDIRECT + 1]) {
        bp = bread(ip->dev, ip->addrs[NDIRECT + 1]);
        a = (uint*)bp->data;

        for (i = 0; i < NINDIRECT; i++) {
            if (a[i]) {
                bp2 = bread(ip->dev, a[i]);
                a2 = (uint*)bp2->data;

                for (j = 0; j < NINDIRECT; j++) {
                    if (a2[j])
                        bfree(ip->dev, a2[j]);
                }
                brelse(bp2);
                bfree(ip->dev, a[i]);
            }
        }
        brelse(bp);
        bfree(ip->dev, ip->addrs[NDIRECT + 1]);
        ip->addrs[NDIRECT + 1] = 0;
    }

    ip->size = 0;
    iupdate(ip);
}
```

---

## 3. 符号链接实现

### 3.1 添加新的文件类型

```c
// kernel/stat.h
#define T_DIR     1   // 目录
#define T_FILE    2   // 文件
#define T_DEVICE  3   // 设备
#define T_SYMLINK 4   // 符号链接 (新增)
```

### 3.2 系统调用实现

```c
// kernel/syscall.h
#define SYS_symlink 22

// kernel/sysfile.c

// 创建符号链接
uint64 sys_symlink(void) {
    char target[MAXPATH], path[MAXPATH];
    struct inode *ip;
    int n;

    // 获取参数
    if ((n = argstr(0, target, MAXPATH)) < 0 ||
        argstr(1, path, MAXPATH) < 0)
        return -1;

    begin_op();

    // 创建inode
    ip = create(path, T_SYMLINK, 0, 0);
    if (ip == 0) {
        end_op();
        return -1;
    }

    // 将目标路径写入inode数据
    if (writei(ip, 0, (uint64)target, 0, n) != n) {
        iunlockput(ip);
        end_op();
        return -1;
    }

    iunlockput(ip);
    end_op();
    return 0;
}
```

### 3.3 修改open系统调用

```c
// kernel/sysfile.c

uint64 sys_open(void) {
    char path[MAXPATH];
    int fd, omode;
    struct file *f;
    struct inode *ip;
    int n;

    // ... 参数解析 ...

    if (omode & O_CREATE) {
        // 创建文件
        ip = create(path, T_FILE, 0, 0);
        // ...
    } else {
        // 解析符号链接 (最多10层)
        int depth = 0;
        char target[MAXPATH];

        while (depth < 10) {
            ip = namei(path);
            if (ip == 0)
                return -1;

            ilock(ip);

            if (ip->type != T_SYMLINK || (omode & O_NOFOLLOW)) {
                // 不是符号链接，或者要求不跟随
                break;
            }

            // 读取符号链接目标
            n = readi(ip, 0, (uint64)target, 0, MAXPATH);
            iunlockput(ip);

            if (n < 0)
                return -1;

            target[n] = '\0';
            safestrcpy(path, target, MAXPATH);
            depth++;
        }

        if (depth >= 10) {
            // 符号链接循环
            return -1;
        }
    }

    // ... 原有代码 ...
}
```

---

## 4. 测试

### 4.1 大文件测试

```c
// user/bigfiletest.c
#include "kernel/types.h"
#include "user/user.h"
#include "kernel/fcntl.h"

int main() {
    int fd;
    char buf[512];
    int i;
    int nblocks = 60000;  // 测试大文件

    fd = open("bigfile", O_CREATE | O_WRONLY);
    if (fd < 0) {
        printf("cannot create bigfile\n");
        exit(1);
    }

    // 写入大量数据
    for (i = 0; i < nblocks; i++) {
        ((int*)buf)[0] = i;
        if (write(fd, buf, 512) != 512) {
            printf("write failed at block %d\n", i);
            exit(1);
        }
    }

    close(fd);

    // 验证数据
    fd = open("bigfile", O_RDONLY);
    if (fd < 0) {
        printf("cannot open bigfile\n");
        exit(1);
    }

    for (i = 0; i < nblocks; i++) {
        if (read(fd, buf, 512) != 512) {
            printf("read failed at block %d\n", i);
            exit(1);
        }
        if (((int*)buf)[0] != i) {
            printf("data mismatch at block %d\n", i);
            exit(1);
        }
    }

    close(fd);
    printf("bigfile test passed\n");
    exit(0);
}
```

### 4.2 符号链接测试

```c
// user/symlinktest.c
#include "kernel/types.h"
#include "user/user.h"

int main() {
    int fd;
    char buf[10];

    // 创建普通文件
    fd = open("testfile", O_CREATE | O_WRONLY);
    write(fd, "hello", 5);
    close(fd);

    // 创建符号链接
    symlink("testfile", "linkfile");

    // 通过符号链接读取
    fd = open("linkfile", O_RDONLY);
    if (fd < 0) {
        printf("open linkfile failed\n");
        exit(1);
    }

    if (read(fd, buf, 5) != 5) {
        printf("read failed\n");
        exit(1);
    }

    if (strncmp(buf, "hello", 5) != 0) {
        printf("data mismatch\n");
        exit(1);
    }

    close(fd);
    printf("symlink test passed\n");
    exit(0);
}
```

---

**难度**: ⭐⭐⭐
**预计时间**: 6-8小时
