# MIT 6.S081 Lab 9: File System

## 1. 实验概述

### 1.1 目标

- 深入理解xv6文件系统的实现
- 掌握inode、目录、路径查找机制
- 理解日志系统的作用和实现
- 实现符号链接(symbolic link)
- 处理文件系统的一致性和恢复

### 1.2 xv6文件系统架构

```
xv6文件系统层次:

用户层
   │
   │ 系统调用: open, read, write, close
   ▼
┌─────────────────────────────────────────┐
│ 文件描述符层 (fdtable, file structure)   │
├─────────────────────────────────────────┤
│ 路径名层 (namei, nameiparent)            │
├─────────────────────────────────────────┤
│ 目录层 (dirlookup, dirlink)              │
├─────────────────────────────────────────┤
│ Inode层 (iget, iput, ilock, iunlock)     │
├─────────────────────────────────────────┤
│ 日志层 (begin_op, log_write, end_op)     │
├─────────────────────────────────────────┤
│ 缓冲区层 (bread, bwrite, brelse)         │
├─────────────────────────────────────────┤
│ 磁盘驱动层 (virtio_disk)                 │
└─────────────────────────────────────────┘
   │
   ▼
物理磁盘
```

### 1.3 磁盘布局

```
xv6磁盘布局 (mkfs创建的镜像):

0          1          2          3         ...        n-1
├──────────┼──────────┼──────────┼──────────┼──────────┤
│   Boot   │  Super   │   Log    │  Inode   │   Data   │
│  Block   │  Block   │          │  Blocks  │  Blocks  │
│  (未使用) │          │          │          │          │
└──────────┴──────────┴──────────┴──────────┴──────────┘

详细说明:
- Block 0: 启动块 (保留)
- Block 1: 超级块 (superblock) - 文件系统元数据
- Block 2~logstart-1: 日志区
- Block inodestart~bdainode-1: inode区
- Block bmapstart~nblocks-1: 空闲块位图
- 其余: 数据块

超级块内容:
struct superblock {
    uint magic;        // 0x10203045 (标识xv6文件系统)
    uint size;         // 文件系统总块数
    uint nblocks;      // 数据块数
    uint ninodes;      // inode数
    uint nlog;         // 日志块数
    uint logstart;     // 日志区起始块
    uint inodestart;   // inode区起始块
    uint bmapstart;    // 位图区起始块
};
```

---

## 2. Inode详解

### 2.1 Inode结构

```c
// kernel/fs.h

// Inode磁盘结构 (64字节)
struct dinode {
    short type;           // 文件类型: 0=空闲, 1=目录, 2=文件, 3=设备
    short major;          // 主设备号
    short minor;          // 次设备号
    short nlink;          // 硬链接计数
    uint size;            // 文件大小(字节)
    uint addrs[NDIRECT+1]; // 数据块地址
};

#define NDIRECT 12
#define NINDIRECT (BSIZE / sizeof(uint))  // 256
#define MAXFILE (NDIRECT + NINDIRECT)     // 268 blocks

// 内存中的inode结构
struct inode {
    uint dev;             // 设备号
    uint inum;            // inode编号
    int ref;              // 引用计数
    struct sleeplock lock; // 保护以下字段
    int valid;            // 已从磁盘读取

    // dinode的副本
    short type;
    short major;
    short minor;
    short nlink;
    uint size;
    uint addrs[NDIRECT+1];
};
```

### 2.2 文件块映射

```
文件数据块寻址:

小文件 (≤ 12KB):
┌─────────┐
│  inode  │──┬──► 数据块0
│ addrs[0]│  │
│ addrs[1]│──┼──► 数据块1
│   ...   │  │
│addrs[11]│──┼──► 数据块11
└─────────┘  │
             │
             ▼
            12个直接块
            (最大 12 * 1KB = 12KB)

大文件 (> 12KB):
┌─────────┐
│  inode  │
│ addrs[0]│──► 数据块0
│   ...   │
│addrs[11]│──► 数据块11
│addrs[12]│──► 间接块 ──┬──► 数据块12
└─────────┘            ├──► 数据块13
                       ├──► ...
                       └──► 数据块267

                       256个间接块
                       (最大 12 + 256 = 268KB)
```

### 2.3 Inode操作函数

```c
// kernel/fs.c

// 从inode编号获取inode (增加引用计数)
struct inode*
iget(uint dev, uint inum)
{
    struct inode *ip, *empty;

    acquire(&itable.lock);

    // 查找是否已在缓存中
    empty = 0;
    for (ip = &itable.inode[0]; ip < &itable.inode[NINODE]; ip++) {
        if (ip->ref > 0 && ip->dev == dev && ip->inum == inum) {
            ip->ref++;
            release(&itable.lock);
            return ip;
        }
        if (empty == 0 && ip->ref == 0)
            empty = ip;
    }

    // 分配新inode缓存项
    if (empty == 0)
        panic("iget: no inodes");

    ip = empty;
    ip->dev = dev;
    ip->inum = inum;
    ip->ref = 1;
    ip->valid = 0;

    release(&itable.lock);
    return ip;
}

// 减少引用计数，如为0则释放
void
iput(struct inode *ip)
{
    acquire(&itable.lock);

    if (ip->ref == 1 && ip->valid && ip->nlink == 0) {
        // inode不再被引用，且无链接
        // 释放数据块和inode
        release(&itable.lock);

        itrunc(ip);      // 截断文件(释放数据块)
        ip->type = 0;
        iupdate(ip);     // 写入磁盘

        acquire(&itable.lock);
    }

    ip->ref--;
    release(&itable.lock);
}

// 从磁盘读取inode
void
ilock(struct inode *ip)
{
    // ...
    if (!ip->valid) {
        // 从磁盘读取dinode
        struct buf *bp = bread(ip->dev, IBLOCK(ip->inum, sb));
        struct dinode *dip = (struct dinode *)bp->data +
                             ip->inum % IPB;
        ip->type = dip->type;
        ip->major = dip->major;
        ip->minor = dip->minor;
        ip->nlink = dip->nlink;
        ip->size = dip->size;
        memmove(ip->addrs, dip->addrs, sizeof(ip->addrs));
        brelse(bp);
        ip->valid = 1;
    }
    // ...
}
```

---

## 3. 目录实现

### 3.1 目录结构

```c
// kernel/fs.h

// 目录项结构 (16字节)
struct dirent {
    ushort inum;      // inode编号 (0表示空闲项)
    char name[14];    // 文件名 (以null结尾)
};

// 目录就是一个特殊的文件
// 包含dirent数组
// 查找时顺序扫描
```

### 3.2 目录操作

```c
// kernel/fs.c

// 在目录中查找文件名
// 返回锁定的inode，设置*poff为目录项偏移
struct inode*
dirlookup(struct inode *dp, char *name, uint *poff)
{
    uint off, inum;
    struct dirent de;

    // 检查是否为目录
    if (dp->type != T_DIR)
        panic("dirlookup not DIR");

    // 遍历所有目录项
    for (off = 0; off < dp->size; off += sizeof(de)) {
        // 读取目录项
        if (readi(dp, 0, (uint64)&de, off, sizeof(de)) != sizeof(de))
            panic("dirlookup read");

        // 跳过空闲项
        if (de.inum == 0)
            continue;

        // 比较文件名
        if (strncmp(name, de.name, DIRSIZ) == 0) {
            // 找到匹配
            if (poff)
                *poff = off;
            inum = de.inum;
            return iget(dp->dev, inum);
        }
    }

    return 0;  // 未找到
}

// 在目录中创建链接
int
dirlink(struct inode *dp, char *name, uint inum)
{
    int off;
    struct dirent de;
    struct inode *ip;

    // 检查是否已存在
    if ((ip = dirlookup(dp, name, 0)) != 0) {
        iput(ip);
        return -1;
    }

    // 查找空闲目录项
    for (off = 0; off < dp->size; off += sizeof(de)) {
        if (readi(dp, 0, (uint64)&de, off, sizeof(de)) != sizeof(de))
            panic("dirlink read");
        if (de.inum == 0)
            break;  // 找到空闲项
    }

    // 填充目录项
    strncpy(de.name, name, DIRSIZ);
    de.inum = inum;

    // 写入目录
    if (writei(dp, 0, (uint64)&de, off, sizeof(de)) != sizeof(de))
        panic("dirlink");

    return 0;
}
```

### 3.3 路径解析

```c
// kernel/fs.c

// 解析路径，返回父目录和最终文件名
static struct inode*
namex(char *path, int nameiparent, char *name)
{
    struct inode *ip, *next;

    // 确定起始目录
    if (*path == '/')
        ip = iget(ROOTDEV, ROOTINO);  // 绝对路径
    else
        ip = idup(myproc()->cwd);      // 相对路径

    while ((path = skipelem(path, name)) != 0) {
        ilock(ip);

        // 必须是目录
        if (ip->type != T_DIR) {
            iunlockput(ip);
            return 0;
        }

        // 如果要获取父目录且这是最后一个元素
        if (nameiparent && *path == '\0') {
            iunlock(ip);
            return ip;
        }

        // 查找下一个组件
        if ((next = dirlookup(ip, name, 0)) == 0) {
            iunlockput(ip);
            return 0;
        }

        iunlockput(ip);
        ip = next;
    }

    // 如果只需要父目录，但路径已结束
    if (nameiparent) {
        iput(ip);
        return 0;
    }

    return ip;
}

struct inode*
namei(char *path)
{
    char name[DIRSIZ];
    return namex(path, 0, name);
}

struct inode*
nameiparent(char *path, char *name)
{
    return namex(path, 1, name);
}
```

---

## 4. 日志系统

### 4.1 为什么需要日志

```
文件系统操作通常是多步骤的:

示例: 创建文件
1. 分配inode
2. 初始化inode
3. 在目录中添加条目

如果崩溃发生在步骤2和3之间:
- inode已分配但未使用 (空间泄漏)
- 或目录指向无效inode (文件系统损坏)

日志解决方案:
- 先将所有修改写入日志
- 提交后，批量写入实际位置
- 崩溃后可重放日志完成操作

特点:
- 原子性: 操作要么全完成，要么全不完成
- 一致性: 文件系统始终保持有效状态
```

### 4.2 日志结构

```c
// kernel/log.c

// 日志头 (存储在日志区的第一个块)
struct logheader {
    int n;              // 已提交的块数
    int block[LOGSIZE]; // 目标块号数组
};

struct log {
    struct spinlock lock;
    int start;          // 日志区起始块号
    int size;           // 日志区大小
    int outstanding;    // 未完成的事务数
    int committing;     // 是否正在提交
    int dev;
    struct logheader lh;
};

struct log log;
```

### 4.3 日志操作

```c
// kernel/log.c

// 开始事务
void
begin_op(void)
{
    acquire(&log.lock);

    while (1) {
        // 检查是否可以开始新事务
        if (log.committing) {
            // 正在提交，等待
            sleep(&log, &log.lock);
        } else if (log.lh.n + (log.outstanding + 1) * MAXOPBLOCKS > LOGSIZE) {
            // 日志空间不足，等待
            sleep(&log, &log.lock);
        } else {
            // 可以开始
            log.outstanding++;
            release(&log.lock);
            break;
        }
    }
}

// 结束事务
void
end_op(void)
{
    int do_commit = 0;

    acquire(&log.lock);
    log.outstanding--;

    // 如果是最后一个事务，负责提交
    if (log.outstanding == 0) {
        do_commit = 1;
        log.committing = 1;
    }

    release(&log.lock);

    if (do_commit) {
        commit();           // 提交日志
        acquire(&log.lock);
        log.committing = 0;
        wakeup(&log);       // 唤醒等待的begin_op
        release(&log.lock);
    }
}

// 写块到日志
void
log_write(struct buf *b)
{
    acquire(&log.lock);

    // 检查是否已在日志中
    int i;
    for (i = 0; i < log.lh.n; i++) {
        if (log.lh.block[i] == b->blockno)
            break;
    }

    // 记录目标块号
    log.lh.block[i] = b->blockno;
    if (i == log.lh.n) {
        // 新块，增加计数
        log.lh.n++;
    }

    release(&log.lock);

    // 标记缓冲区为已修改，但不立即写入磁盘
    b->flags |= B_DIRTY;
}

// 提交日志
static void
commit(void)
{
    if (log.lh.n > 0) {
        // 1. 将修改的块写入日志
        write_log();

        // 2. 写入日志头 (提交点)
        write_head();

        // 3. 将日志中的块安装到实际位置
        install_trans(0);

        // 4. 清空日志头
        log.lh.n = 0;
        write_head();
    }
}
```

---

## 5. 符号链接实现

### 5.1 符号链接概念

```
硬链接 vs 符号链接:

硬链接:
- 直接指向inode
- 不能跨文件系统
- 不能链接目录
- 删除原文件后仍然有效

符号链接:
- 包含目标路径字符串
- 可以跨文件系统
- 可以链接目录
- 删除原文件后失效 (悬挂链接)

实现:
- 新的文件类型 T_SYMLINK
- 文件内容是目标路径
- open时递归解析
```

### 5.2 系统调用实现

```c
// kernel/sysfile.c

// 创建符号链接
uint64
sys_symlink(void)
{
    char target[MAXPATH], path[MAXPATH];
    struct inode *ip;

    // 获取参数
    if (argstr(0, target, MAXPATH) < 0 ||
        argstr(1, path, MAXPATH) < 0)
        return -1;

    begin_op();

    // 创建新inode
    ip = create(path, T_SYMLINK, 0, 0);
    if (ip == 0) {
        end_op();
        return -1;
    }

    // 写入目标路径
    if (writei(ip, 0, (uint64)target, 0, strlen(target)) < 0) {
        iunlockput(ip);
        end_op();
        return -1;
    }

    iunlockput(ip);
    end_op();
    return 0;
}

// 修改open以支持符号链接
uint64
sys_open(void)
{
    // ... 原有代码 ...

    // 处理符号链接
    int depth = 0;
    while (ip->type == T_SYMLINK && !(omode & O_NOFOLLOW)) {
        // 防止循环链接
        if (depth++ > 10) {
            iunlockput(ip);
            end_op();
            return -1;
        }

        // 读取目标路径
        char target[MAXPATH];
        int n = readi(ip, 0, (uint64)target, 0, MAXPATH);
        iunlockput(ip);

        if (n < 0 || n >= MAXPATH) {
            end_op();
            return -1;
        }
        target[n] = '\0';

        // 解析目标路径
        if ((ip = namei(target)) == 0) {
            end_op();
            return -1;
        }
        ilock(ip);
    }

    // ... 原有代码 ...
}
```

---

## 6. 文件系统测试

### 6.1 功能测试

```c
// user/fstest.c
#include "kernel/types.h"
#include "user/user.h"

void test_create_write_read() {
    int fd;
    char buf[100];

    // 创建并写入
    fd = open("testfile", O_CREATE | O_WRONLY);
    write(fd, "hello world\n", 12);
    close(fd);

    // 读取
    fd = open("testfile", O_RDONLY);
    int n = read(fd, buf, sizeof(buf));
    buf[n] = '\0';
    printf("read: %s", buf);
    close(fd);

    // 删除
    unlink("testfile");
}

void test_symlink() {
    int fd;
    char buf[100];

    // 创建目标文件
    fd = open("target", O_CREATE | O_WRONLY);
    write(fd, "target content\n", 15);
    close(fd);

    // 创建符号链接
    symlink("target", "link");

    // 通过链接读取
    fd = open("link", O_RDONLY);
    read(fd, buf, sizeof(buf));
    printf("via symlink: %s", buf);
    close(fd);

    // 删除目标文件
    unlink("target");

    // 尝试读取悬挂链接
    fd = open("link", O_RDONLY);
    if (fd < 0) {
        printf("dangling link correctly detected\n");
    }

    unlink("link");
}

int main() {
    test_create_write_read();
    test_symlink();
    exit(0);
}
```

### 6.2 压力测试

```c
// 创建大量文件
void stress_test() {
    char name[32];

    for (int i = 0; i < 100; i++) {
        snprintf(name, sizeof(name), "file%d", i);
        int fd = open(name, O_CREATE | O_WRONLY);

        // 写入一些数据
        for (int j = 0; j < 10; j++) {
            write(fd, name, strlen(name));
        }
        close(fd);
    }

    // 删除所有文件
    for (int i = 0; i < 100; i++) {
        snprintf(name, sizeof(name), "file%d", i);
        unlink(name);
    }
}
```

---

## 7. 调试技巧

### 7.1 常见问题

```
问题1: 文件系统损坏
原因: 未正确使用日志或事务
解决: 确保所有修改在begin_op/end_op之间

问题2: inode泄漏
原因: iput调用不正确
解决: 检查所有错误路径的iput

问题3: 目录查找失败
原因: 目录项格式错误或inode号无效
解决: 检查dirlink和dirlookup实现

问题4: 符号链接循环
原因: 未限制链接深度
解决: 实现深度限制
```

### 7.2 调试工具

```c
// 打印inode信息
void debug_inode(struct inode *ip) {
    printf("inode %d:\n", ip->inum);
    printf("  type=%d major=%d minor=%d\n",
           ip->type, ip->major, ip->minor);
    printf("  nlink=%d size=%d\n",
           ip->nlink, ip->size);
    printf("  blocks:");
    for (int i = 0; i < NDIRECT + 1; i++) {
        if (ip->addrs[i])
            printf(" %d", ip->addrs[i]);
    }
    printf("\n");
}

// 打印目录内容
void debug_dir(struct inode *dp) {
    struct dirent de;

    printf("directory (inode %d):\n", dp->inum);
    for (int off = 0; off < dp->size; off += sizeof(de)) {
        readi(dp, 0, (uint64)&de, off, sizeof(de));
        if (de.inum) {
            printf("  %s -> inode %d\n", de.name, de.inum);
        }
    }
}
```

---

## 8. 总结

### 8.1 核心概念

| 概念 | 说明 |
|------|------|
| **Inode** | 文件元数据和数据块索引 |
| **目录** | 特殊的文件，包含dirent数组 |
| **硬链接** | 多个目录项指向同一inode |
| **符号链接** | 包含目标路径的特殊文件 |
| **日志** | 保证文件系统操作原子性 |
| **事务** | begin_op/end_op包围的一组操作 |

### 8.2 数据结构关系

```
文件系统核心数据结构:

进程 -> file -> inode -> dinode (磁盘)
            ↓
         数据块

目录 -> inode -> 数据块 -> dirent[] -> inode

路径 -> namei -> 逐级解析 -> dirlookup
```

---

**难度**: ⭐⭐⭐
**预计时间**: 10-12小时
**依赖**: Lab 8 (锁)
