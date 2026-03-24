# 容器技术深度剖析

> **层级**: L4 (方法论层) + L3 (系统层)
> **目标**: 深入理解Linux容器技术原理，掌握Namespace、Cgroups、容器运行时实现
> **关联**: [Linux内核模块](../03_OS_Internals/05_Linux_Kernel_Module_Programming.md) | [系统编程](../01_System_Programming/readme.md)

---

## 1. 容器技术概述

### 1.1 虚拟化技术演进

```
┌─────────────────────────────────────────────────────────────┐
│              虚拟化技术演进史                                │
├─────────────────────────────────────────────────────────────┤
│                                                              │
│  物理机时代 (2000年前)                                        │
│  ┌─────────────────────────────────────────────────────┐   │
│  │  App A   App B   App C   App D                      │   │
│  │    │       │       │       │                        │   │
│  │    └───────┴───────┴───────┘                        │   │
│  │                    OS                               │   │
│  │              Hardware                               │   │
│  └─────────────────────────────────────────────────────┘   │
│  问题: 资源争抢、依赖冲突、难以隔离                          │
│                                                              │
│  硬件虚拟化时代 (2000s)                                       │
│  ┌─────────────────────────────────────────────────────┐   │
│  │ ┌─────┐ ┌─────┐ ┌─────┐ ┌─────┐                   │   │
│  │ │App A│ │App B│ │App C│ │App D│                   │   │
│  │ │ OS  │ │ OS  │ │ OS  │ │ OS  │                   │   │
│  │ └─────┘ └─────┘ └─────┘ └─────┘                   │   │
│  │    │       │       │       │                        │   │
│  │    └───────┴───────┴───────┘                        │   │
│  │              Hypervisor (KVM/Xen/ESXi)              │   │
│  │              Hardware                               │   │
│  └─────────────────────────────────────────────────────┘   │
│  优势: 强隔离、可运行不同OS                                  │
│  劣势: 资源开销大(每个VM一个内核)、启动慢                    │
│                                                              │
│  容器化时代 (2013-)                                           │
│  ┌─────────────────────────────────────────────────────┐   │
│  │ ┌─────┐ ┌─────┐ ┌─────┐ ┌─────┐                   │   │
│  │ │App A│ │App B│ │App C│ │App D│                   │   │
│  │ │Libs │ │Libs │ │Libs │ │Libs │                   │   │
│  │ └─────┘ └─────┘ └─────┘ └─────┘                   │   │
│  │    │       │       │       │                        │   │
│  │    └───────┴───────┴───────┘                        │   │
│  │              Container Runtime (Docker/containerd)  │   │
│  │              Linux Kernel (Namespace+Cgroups)       │   │
│  │              Hardware                               │   │
│  └─────────────────────────────────────────────────────┘   │
│  优势: 轻量、快速启动、高密度部署                            │
│  劣势: 隔离性弱于VM、共享内核                                │
│                                                              │
└─────────────────────────────────────────────────────────────┘
```

### 1.2 容器 vs 虚拟机对比

```
┌──────────────────┬──────────────────────┬──────────────────────┐
│ 特性             │ 容器 (Container)     │ 虚拟机 (VM)          │
├──────────────────┼──────────────────────┼──────────────────────┤
│ 隔离级别         │ 进程级               │ 硬件级               │
│ 启动时间         │ 毫秒-秒级            │ 分钟级               │
│ 内存开销         │ MB级                 │ GB级                 │
│ 磁盘开销         │ 镜像层共享           │ 完整OS镜像           │
│ 性能             │ 接近原生             │ 5-20%开销            │
│ 密度             │ 单节点1000+          │ 单节点10-100         │
│ 隔离强度         │ 中 (共享内核)        │ 强 (独立内核)        │
│ 安全性           │ 依赖内核安全         │ 硬件隔离             │
│ 可移植性         │ 高 (镜像标准化)      │ 中                   │
│ 适用场景         │ 微服务、CI/CD        │ 多租户、遗留系统     │
└──────────────────┴──────────────────────┴──────────────────────┘
```

---

## 2. Linux Namespace

### 2.1 Namespace类型详解

```
┌─────────────────────────────────────────────────────────────┐
│              Linux Namespace 类型                            │
├─────────────────────────────────────────────────────────────┤
│                                                              │
│  1. PID Namespace (进程隔离)                                  │
│  ┌─────────────────────────────────────────────────────┐   │
│  │  Host PID: 1, 2, 3, ..., 1000, ...                  │   │
│  │                    │                                │   │
│  │  Container A:     PID 1 (init)                      │   │
│  │                   ├── PID 2 (nginx)                 │   │
│  │                   └── PID 3 (php-fpm)               │   │
│  │                                                      │   │
│  │  Container B:     PID 1 (init)                      │   │
│  │                   ├── PID 2 (apache)                │   │
│  │                   └── PID 3 (mysql)                 │   │
│  │                                                      │   │
│  │  每个容器有自己的PID空间，PID 1是init进程           │   │
│  └─────────────────────────────────────────────────────┘   │
│                                                              │
│  2. Network Namespace (网络隔离)                              │
│  ┌─────────────────────────────────────────────────────┐   │
│  │  Container A:                                       │   │
│  │    eth0: 172.17.0.2 (虚拟网卡veth)                  │   │
│  │    lo: 127.0.0.1                                    │   │
│  │    路由表、iptables规则独立                         │   │
│  │                                                      │   │
│  │  Container B:                                       │   │
│  │    eth0: 172.17.0.3                                 │   │
│  │    无法看到Container A的网络接口                    │   │
│  │                                                      │   │
│  │  容器间通信通过veth pair + bridge/docker0           │   │
│  └─────────────────────────────────────────────────────┘   │
│                                                              │
│  3. Mount Namespace (文件系统隔离)                            │
│  ┌─────────────────────────────────────────────────────┐   │
│  │  Container A视图:                                    │   │
│  │    / (容器rootfs)                                   │   │
│  │    ├── bin/  (来自ubuntu:20.04镜像)                 │   │
│  │    ├── lib/                                         │   │
│  │    ├── app/  (应用代码卷挂载)                       │   │
│  │    └── tmp/                                         │   │
│  │                                                      │   │
│  │  完全独立的文件系统视图，Copy-on-Write层叠          │   │
│  └─────────────────────────────────────────────────────┘   │
│                                                              │
│  4. UTS Namespace (主机名隔离)                                │
│     每个容器可设置独立hostname                               │
│                                                              │
│  5. IPC Namespace (进程间通信隔离)                            │
│     隔离System V IPC和POSIX消息队列                          │
│                                                              │
│  6. User Namespace (用户隔离)                                 │
│     容器内root映射到宿主机非特权用户                         │
│     容器UID 0 → 宿主机UID 100000                             │
│                                                              │
│  7. Cgroup Namespace (cgroups视图隔离) - Linux 4.6+          │
│  8. Time Namespace (时间隔离) - Linux 5.6+                   │
│                                                              │
└─────────────────────────────────────────────────────────────┘
```

### 2.2 Namespace系统调用

```c
/* ============================================================
 * Linux Namespace 系统调用示例
 * ============================================================ */

#define _GNU_SOURCE
#include <sched.h>
#include <sys/wait.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

/* Namespace类型标志 */
#define CLONE_NEWNS     0x00020000  /* Mount namespace */
#define CLONE_NEWCGROUP 0x02000000  /* Cgroup namespace */
#define CLONE_NEWUTS    0x04000000  /* UTS namespace */
#define CLONE_NEWIPC    0x08000000  /* IPC namespace */
#define CLONE_NEWUSER   0x10000000  /* User namespace */
#define CLONE_NEWPID    0x20000000  /* PID namespace */
#define CLONE_NEWNET    0x40000000  /* Network namespace */

/* 创建新Namespace的进程 */
int container_main(void* arg) {
    printf("Container - Inside PID: %d\n", getpid());

    /* 在新的Mount Namespace中重新挂载proc */
    mount("proc", "/proc", "proc", 0, NULL);

    /* 设置hostname */
    sethostname("container", 9);

    /* 执行shell */
    execlp("/bin/bash", "bash", NULL);
    return 0;
}

#define STACK_SIZE (1024 * 1024)

int main() {
    char* stack = malloc(STACK_SIZE);
    if (!stack) {
        perror("malloc");
        exit(1);
    }

    printf("Host - Parent PID: %d\n", getpid());

    /* clone创建新Namespace的进程 */
    pid_t pid = clone(
        container_main,
        stack + STACK_SIZE,
        CLONE_NEWNS |    /* 新的Mount Namespace */
        CLONE_NEWPID |   /* 新的PID Namespace */
        CLONE_NEWNET |   /* 新的Network Namespace */
        CLONE_NEWUTS |   /* 新的UTS Namespace */
        CLONE_NEWIPC |   /* 新的IPC Namespace */
        SIGCHLD,
        NULL
    );

    if (pid == -1) {
        perror("clone");
        exit(1);
    }

    /* 等待子进程结束 */
    waitpid(pid, NULL, 0);
    free(stack);

    return 0;
}

/* ============================================================
 * unshare: 将当前进程移到新Namespace
 * ============================================================ */

#include <sched.h>

int unshare_namespaces() {
    /* 将当前进程移到新的Namespace */
    int flags = CLONE_NEWNS | CLONE_NEWPID | CLONE_NEWNET;

    if (unshare(flags) == -1) {
        perror("unshare");
        return -1;
    }

    printf("Moved to new namespaces\n");
    printf("New PID: %d\n", getpid());

    return 0;
}

/* ============================================================
 * setns: 加入已存在的Namespace
 * ============================================================ */

#include <fcntl.h>

int join_namespace(pid_t target_pid, const char* ns_type) {
    char path[256];
    snprintf(path, sizeof(path), "/proc/%d/ns/%s", target_pid, ns_type);

    /* 打开Namespace文件 */
    int fd = open(path, O_RDONLY);
    if (fd == -1) {
        perror("open");
        return -1;
    }

    /* 加入Namespace */
    if (setns(fd, 0) == -1) {
        perror("setns");
        close(fd);
        return -1;
    }

    close(fd);
    printf("Joined %s namespace of PID %d\n", ns_type, target_pid);

    return 0;
}

/* 使用示例: 进入容器的Network Namespace */
/* nsenter --target <pid> --net /bin/bash */
```

---

## 3. Cgroups (控制组)

### 3.1 Cgroups v1 vs v2

```
┌─────────────────────────────────────────────────────────────┐
│              Cgroups 架构对比                                │
├─────────────────────────────────────────────────────────────┤
│                                                              │
│  Cgroups v1 (传统)                                           │
│  ┌─────────────────────────────────────────────────────┐   │
│  │  每个子系统独立挂载                                    │   │
│  │                                                      │   │
│  │  /sys/fs/cgroup/cpu/docker/...                      │   │
│  │  /sys/fs/cgroup/memory/docker/...                   │   │
│  │  /sys/fs/cgroup/blkio/docker/...                    │   │
│  │  /sys/fs/cgroup/pids/docker/...                     │   │
│  │                                                      │   │
│  │  问题: 层次结构不一致，管理复杂                       │   │
│  └─────────────────────────────────────────────────────┘   │
│                                                              │
│  Cgroups v2 (统一)                                           │
│  ┌─────────────────────────────────────────────────────┐   │
│  │  统一层次结构，所有控制器挂载在同一位置               │   │
│  │                                                      │   │
│  │  /sys/fs/cgroup/                                    │   │
│  │  ├── system.slice/                                  │   │
│  │  ├── user.slice/                                    │   │
│  │  └── docker/                                        │   │
│  │      ├── container_a/                               │   │
│  │      │   ├── cpu.max                                │   │
│  │      │   ├── memory.max                             │   │
│  │      │   └── io.max                                 │   │
│  │      └── container_b/                               │   │
│  │                                                      │   │
│  │  优势: 统一接口、更好的一致性、root级别委派          │   │
│  └─────────────────────────────────────────────────────┘   │
│                                                              │
│  控制器 (Controllers):                                        │
│  ┌───────────────┬───────────────────────────────────────┐   │
│  │ 控制器        │ 功能                                  │   │
│  ├───────────────┼───────────────────────────────────────┤   │
│  │ cpu           │ CPU时间分配 (CFS调度器)               │   │
│  │ cpuacct       │ CPU使用统计                           │   │
│  │ cpuset        │ CPU/内存节点绑定                      │   │
│  │ memory        │ 内存限制与统计                        │   │
│  │ blkio         │ 块设备I/O限制                         │   │
│  │ pids          │ 进程数限制                            │   │
│  │ devices       │ 设备访问控制                          │   │
│  │ net_cls       │ 网络流量标记 (tc)                     │   │
│  │ net_prio      │ 网络优先级                            │   │
│  │ freezer       │ 进程挂起/恢复                         │   │
│  │ perf_event    │ 性能事件监控                          │   │
│  │ hugetlb       │ 大页内存限制                          │   │
│  │ rdma          │ RDMA资源限制                          │   │
│  └───────────────┴───────────────────────────────────────┘   │
│                                                              │
└─────────────────────────────────────────────────────────────┘
```

### 3.2 Cgroups编程接口

```c
/* ============================================================
 * Cgroups v2 编程接口
 * ============================================================ */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <errno.h>

#define CGROUP_PATH "/sys/fs/cgroup"

/* 创建Cgroup */
int cgroup_create(const char* name) {
    char path[256];
    snprintf(path, sizeof(path), "%s/%s", CGROUP_PATH, name);

    if (mkdir(path, 0755) == -1 && errno != EEXIST) {
        perror("mkdir");
        return -1;
    }

    printf("Created cgroup: %s\n", path);
    return 0;
}

/* 设置CPU限制 (CFS配额) */
int cgroup_set_cpu_limit(const char* name, int quota_percent, int period_ms) {
    char path[256];
    int fd;
    char buf[64];

    /* cpu.max: "quota period" (quota=-1表示无限制) */
    snprintf(path, sizeof(path), "%s/%s/cpu.max", CGROUP_PATH, name);

    fd = open(path, O_WRONLY);
    if (fd == -1) {
        perror("open cpu.max");
        return -1;
    }

    int quota = (period_ms * 1000) * quota_percent / 100;
    snprintf(buf, sizeof(buf), "%d %d000\n", quota, period_ms);

    if (write(fd, buf, strlen(buf)) == -1) {
        perror("write cpu.max");
        close(fd);
        return -1;
    }

    close(fd);
    printf("Set CPU limit: %d%% of %dms period\n", quota_percent, period_ms);
    return 0;
}

/* 设置内存限制 */
int cgroup_set_memory_limit(const char* name, size_t limit_bytes) {
    char path[256];
    int fd;
    char buf[64];

    /* memory.max: 硬限制 */
    snprintf(path, sizeof(path), "%s/%s/memory.max", CGROUP_PATH, name);

    fd = open(path, O_WRONLY);
    if (fd == -1) {
        perror("open memory.max");
        return -1;
    }

    snprintf(buf, sizeof(buf), "%zu\n", limit_bytes);

    if (write(fd, buf, strlen(buf)) == -1) {
        perror("write memory.max");
        close(fd);
        return -1;
    }

    close(fd);
    printf("Set memory limit: %zu bytes\n", limit_bytes);
    return 0;
}

/* 设置进程数限制 */
int cgroup_set_pids_limit(const char* name, int max_pids) {
    char path[256];
    int fd;
    char buf[32];

    snprintf(path, sizeof(path), "%s/%s/pids.max", CGROUP_PATH, name);

    fd = open(path, O_WRONLY);
    if (fd == -1) {
        perror("open pids.max");
        return -1;
    }

    snprintf(buf, sizeof(buf), "%d\n", max_pids);

    if (write(fd, buf, strlen(buf)) == -1) {
        perror("write pids.max");
        close(fd);
        return -1;
    }

    close(fd);
    printf("Set pids limit: %d\n", max_pids);
    return 0;
}

/* 将进程加入Cgroup */
int cgroup_add_process(const char* name, pid_t pid) {
    char path[256];
    int fd;
    char buf[32];

    snprintf(path, sizeof(path), "%s/%s/cgroup.procs", CGROUP_PATH, name);

    fd = open(path, O_WRONLY);
    if (fd == -1) {
        perror("open cgroup.procs");
        return -1;
    }

    snprintf(buf, sizeof(buf), "%d\n", pid);

    if (write(fd, buf, strlen(buf)) == -1) {
        perror("write cgroup.procs");
        close(fd);
        return -1;
    }

    close(fd);
    printf("Added PID %d to cgroup %s\n", pid, name);
    return 0;
}

/* 删除Cgroup */
int cgroup_delete(const char* name) {
    char path[256];
    snprintf(path, sizeof(path), "%s/%s", CGROUP_PATH, name);

    if (rmdir(path) == -1) {
        perror("rmdir");
        return -1;
    }

    printf("Deleted cgroup: %s\n", path);
    return 0;
}

/* 使用示例 */
int main() {
    const char* cgroup_name = "mycontainer";

    /* 创建cgroup */
    cgroup_create(cgroup_name);

    /* 设置资源限制 */
    cgroup_set_cpu_limit(cgroup_name, 50, 100);  /* 50% CPU */
    cgroup_set_memory_limit(cgroup_name, 512 * 1024 * 1024);  /* 512MB */
    cgroup_set_pids_limit(cgroup_name, 100);  /* 最多100个进程 */

    /* 将当前进程加入cgroup */
    cgroup_add_process(cgroup_name, getpid());

    printf("Resource limits applied. Press Enter to exit...\n");
    getchar();

    /* 清理 */
    cgroup_delete(cgroup_name);

    return 0;
}
```

---

## 4. 容器运行时架构

### 4.1 OCI规范

```
┌─────────────────────────────────────────────────────────────┐
│              OCI (Open Container Initiative) 规范            │
├─────────────────────────────────────────────────────────────┤
│                                                              │
│  OCI Runtime Spec:                                           │
│  ┌─────────────────────────────────────────────────────┐   │
│  │  定义容器运行时的标准行为                              │   │
│  │                                                      │   │
│  │  1. config.json - 容器配置                          │   │
│  │     {                                               │   │
│  │       "ociVersion": "1.0.0",                        │   │
│  │       "process": {                                  │   │
│  │         "terminal": false,                          │   │
│  │         "user": { "uid": 0, "gid": 0 },             │   │
│  │         "args": ["/bin/sh"],                        │   │
│  │         "env": ["PATH=/usr/local/sbin:/usr/local/bin"]│   │
│  │       },                                            │   │
│  │       "root": { "path": "rootfs", "readonly": false },│   │
│  │       "hostname": "container",                      │   │
│  │       "linux": {                                    │   │
│  │         "namespaces": [                             │   │
│  │           {"type": "pid"},                          │   │
│  │           {"type": "network"},                      │   │
│  │           {"type": "ipc"},                          │   │
│  │           {"type": "uts"},                          │   │
│  │           {"type": "mount"}                         │   │
│  │         ],                                          │   │
│  │         "resources": {                              │   │
│  │           "cpu": { "shares": 512 },                 │   │
│  │           "memory": { "limit": 536870912 }          │   │
│  │         }                                           │   │
│  │       }                                             │   │
│  │     }                                               │   │
│  │                                                      │   │
│  │  2. 运行时操作 (Lifecycle)                          │   │
│  │     • create: 创建容器                              │   │
│  │     • start: 启动容器进程                           │   │
│  │     • kill: 发送信号                                │   │
│  │     • delete: 删除容器                              │   │
│  │     • state: 查询状态                               │   │
│  └─────────────────────────────────────────────────────┘   │
│                                                              │
│  OCI Image Spec:                                             │
│  ┌─────────────────────────────────────────────────────┐   │
│  │  定义容器镜像的标准格式                                │   │
│  │                                                      │   │
│  │  镜像结构:                                           │   │
│  │  ┌─────────────────────────────────────────────┐    │   │
│  │  │  Manifest (JSON)                            │    │   │
│  │  │  ├── config (容器配置)                      │    │   │
│  │  │  └── layers (层列表)                        │    │   │
│  │  │      ├── layer 1 (base OS)                  │    │   │
│  │  │      ├── layer 2 (dependencies)             │    │   │
│  │  │      └── layer 3 (application)              │    │   │
│  │  └─────────────────────────────────────────────┘    │   │
│  │                                                      │   │
│  │  层存储: 使用Content-Addressable Storage (CAS)     │   │
│  │  每个层用SHA256哈希标识                              │   │
│  └─────────────────────────────────────────────────────┘   │
│                                                              │
└─────────────────────────────────────────────────────────────┘
```

### 4.2 runc实现分析

```c
/* ============================================================
 * runc 核心实现原理 (简化版)
 * ============================================================ */

/* runc是OCI规范的参考实现，核心流程如下: */

/* 1. 解析配置 */
struct container_config {
    char* rootfs;
    char** args;
    struct namespace_config namespaces;
    struct cgroup_config resources;
    struct user_config user;
};

/* 2. 创建容器 */
int container_create(const struct container_config* config) {
    /* 创建管道用于父子进程通信 */
    int sync_pipe[2];
    pipe(sync_pipe);

    /* clone创建新进程，进入新的Namespace */
    pid_t pid = clone(
        container_init,
        stack + STACK_SIZE,
        CLONE_NEWNS | CLONE_NEWPID | CLONE_NEWNET |
        CLONE_NEWIPC | CLONE_NEWUTS | CLONE_NEWUSER | SIGCHLD,
        (void*)config
    );

    if (pid < 0) {
        perror("clone");
        return -1;
    }

    /* 父进程: 设置cgroups */
    setup_cgroups(pid, &config->resources);

    /* 父进程: 设置网络 (创建veth pair) */
    setup_network(pid);

    /* 通知子进程继续 */
    write(sync_pipe[1], "1", 1);

    return pid;
}

/* 3. 容器初始化进程 */
int container_init(void* arg) {
    struct container_config* config = arg;

    /* 等待父进程设置完成 */
    char buf[1];
    read(sync_pipe[0], buf, 1);

    /* 设置hostname */
    sethostname(config->hostname, strlen(config->hostname));

    /* 重新挂载proc (必须在新的PID Namespace后) */
    mount("proc", "/proc", "proc", 0, NULL);
    mount("sysfs", "/sys", "sysfs", 0, NULL);
    mount("tmpfs", "/dev", "tmpfs", MS_NOSUID | MS_STRICTATIME, "mode=755");

    /* 设置rootfs */
    mount(config->rootfs, "/", NULL, MS_BIND | MS_REC, NULL);

    /* pivot_root切换到新的rootfs */
    setup_pivot_root(config->rootfs);

    /* 设置用户 */
    setgid(config->user.gid);
    setuid(config->user.uid);

    /* 执行用户指定的程序 */
    execvp(config->args[0], config->args);

    return -1;
}

/* 4. pivot_root实现 */
int setup_pivot_root(const char* new_root) {
    /* 确保new_root是挂载点 */
    mount(new_root, new_root, NULL, MS_BIND | MS_REC, NULL);

    /* 创建put_old目录 */
    char put_old[256];
    snprintf(put_old, sizeof(put_old), "%s/.pivot_root", new_root);
    mkdir(put_old, 0755);

    /* 执行pivot_root */
    if (syscall(SYS_pivot_root, new_root, put_old) == -1) {
        perror("pivot_root");
        return -1;
    }

    /* 切换到新的root */
    chdir("/");

    /* 卸载旧的root */
    umount2("/.pivot_root", MNT_DETACH);
    rmdir("/.pivot_root");

    return 0;
}
```

---

## 5. 容器安全

### 5.1 安全机制层次

```
┌─────────────────────────────────────────────────────────────┐
│              容器安全机制层次                                │
├─────────────────────────────────────────────────────────────┤
│                                                              │
│  L4: 应用层安全                                              │
│  ├── 镜像安全扫描 (Trivy, Clair)                            │
│  ├── 最小化镜像 (distroless, scratch)                       │
│  └── 安全加固基线 (CIS Docker Benchmark)                    │
│                                                              │
│  L3: 运行时安全                                              │
│  ├── Seccomp (系统调用过滤)                                  │
│  ├── AppArmor/SELinux (强制访问控制)                         │
│  ├── Capabilities (特权精细化)                               │
│  └── 只读Rootfs                                              │
│                                                              │
│  L2: 内核隔离                                                │
│  ├── Namespace隔离                                           │
│  ├── Cgroups资源限制                                         │
│  └── User Namespace (UID/GID映射)                            │
│                                                              │
│  L1: 主机安全                                                │
│  ├── 主机内核加固                                            │
│  ├── 定期安全更新                                            │
│  └── 监控与审计                                              │
│                                                              │
└─────────────────────────────────────────────────────────────┘
```

### 5.2 Seccomp配置

```c
/* ============================================================
 * Seccomp (Secure Computing Mode)
 * 限制容器可使用的系统调用
 * ============================================================ */

#include <seccomp.h>
#include <sys/syscall.h>

/* 设置Seccomp过滤器 */
int setup_seccomp_filter() {
    scmp_filter_ctx ctx = seccomp_init(SCMP_ACT_ERRNO(EPERM));
    if (!ctx) {
        perror("seccomp_init");
        return -1;
    }

    /* 允许的基本系统调用 */
    seccomp_rule_add(ctx, SCMP_ACT_ALLOW, SCMP_SYS(read), 0);
    seccomp_rule_add(ctx, SCMP_ACT_ALLOW, SCMP_SYS(write), 0);
    seccomp_rule_add(ctx, SCMP_ACT_ALLOW, SCMP_SYS(open), 0);
    seccomp_rule_add(ctx, SCMP_ACT_ALLOW, SCMP_SYS(close), 0);
    seccomp_rule_add(ctx, SCMP_ACT_ALLOW, SCMP_SYS(exit), 0);
    seccomp_rule_add(ctx, SCMP_ACT_ALLOW, SCMP_SYS(exit_group), 0);

    /* 允许进程管理 */
    seccomp_rule_add(ctx, SCMP_ACT_ALLOW, SCMP_SYS(execve), 0);
    seccomp_rule_add(ctx, SCMP_ACT_ALLOW, SCMP_SYS(clone), 0);
    seccomp_rule_add(ctx, SCMP_ACT_ALLOW, SCMP_SYS(wait4), 0);

    /* 允许内存管理 */
    seccomp_rule_add(ctx, SCMP_ACT_ALLOW, SCMP_SYS(mmap), 0);
    seccomp_rule_add(ctx, SCMP_ACT_ALLOW, SCMP_SYS(munmap), 0);
    seccomp_rule_add(ctx, SCMP_ACT_ALLOW, SCMP_SYS(brk), 0);

    /* 允许文件操作 */
    seccomp_rule_add(ctx, SCMP_ACT_ALLOW, SCMP_SYS(stat), 0);
    seccomp_rule_add(ctx, SCMP_ACT_ALLOW, SCMP_SYS(fstat), 0);
    seccomp_rule_add(ctx, SCMP_ACT_ALLOW, SCMP_SYS(lstat), 0);

    /* 禁止危险系统调用 */
    seccomp_rule_add(ctx, SCMP_ACT_KILL, SCMP_SYS(mount), 0);
    seccomp_rule_add(ctx, SCMP_ACT_KILL, SCMP_SYS(umount2), 0);
    seccomp_rule_add(ctx, SCMP_ACT_KILL, SCMP_SYS(pivot_root), 0);
    seccomp_rule_add(ctx, SCMP_ACT_KILL, SCMP_SYS(swapon), 0);
    seccomp_rule_add(ctx, SCMP_ACT_KILL, SCMP_SYS(swapoff), 0);

    /* 应用过滤器 */
    if (seccomp_load(ctx) < 0) {
        perror("seccomp_load");
        seccomp_release(ctx);
        return -1;
    }

    seccomp_release(ctx);
    printf("Seccomp filter applied\n");
    return 0;
}
```

---

## 6. 容器网络

### 6.1 容器网络模型

```
┌─────────────────────────────────────────────────────────────┐
│              容器网络架构                                    │
├─────────────────────────────────────────────────────────────┤
│                                                              │
│  Bridge模式 (默认)                                           │
│  ┌─────────────────────────────────────────────────────┐   │
│  │  Host Network                                        │   │
│  │  ┌─────────────────────────────────────────────┐   │   │
│  │  │ docker0 (bridge, 172.17.0.1/16)              │   │   │
│  │  │        │            │                       │   │   │
│  │  │   ┌────┴────┐   ┌───┴────┐                  │   │   │
│  │  │   │vethxxx  │   │vethyyy │                  │   │   │
│  │  │   └────┬────┘   └───┬────┘                  │   │   │
│  │  │        │            │                       │   │   │
│  │  │   ┌────┴────┐   ┌───┴────┐                  │   │   │
│  │  │   │eth0     │   │eth0    │                  │   │   │
│  │  │   │172.17.0│.2  │172.17.0│.3                 │   │   │
│  │  │   └────┬────┘   └───┬────┘                  │   │   │
│  │  │   Container A   Container B                  │   │   │
│  │  └─────────────────────────────────────────────┘   │   │
│  │                                                      │   │
│  │  NAT: 容器通过docker0访问外部网络                  │   │
│  │  Port Mapping: 宿主机端口映射到容器端口            │   │
│  └─────────────────────────────────────────────────────┘   │
│                                                              │
│  Host模式                                                    │
│  ┌─────────────────────────────────────────────────────┐   │
│  │  容器直接使用宿主机网络栈                              │   │
│  │  无隔离，性能最高                                      │   │
│  │  端口冲突风险                                          │   │
│  └─────────────────────────────────────────────────────┘   │
│                                                              │
│  Overlay模式 (跨主机)                                        │
│  ┌─────────────────────────────────────────────────────┐   │
│  │  Host A                    Host B                   │   │
│  │  ┌──────────┐             ┌──────────┐             │   │
│  │  │ Container│             │ Container│             │   │
│  │  │ 10.0.0.2 │◄──────────►│ 10.0.0.3 │             │   │
│  │  └────┬─────┘   VXLAN    └────┬─────┘             │   │
│  │       │        Tunnel         │                    │   │
│  │  ┌────┴───────────────────────┴────┐              │   │
│  │  │    Overlay Network (10.0.0.0/24)│              │   │
│  │  └─────────────────────────────────┘              │   │
│  └─────────────────────────────────────────────────────┘   │
│                                                              │
└─────────────────────────────────────────────────────────────┘
```

---

## 7. 总结

```
┌─────────────────────────────────────────────────────────────┐
│                                                              │
│  容器技术核心要点：                                           │
│                                                              │
│  1. Namespace提供隔离视图                                     │
│     • PID、Network、Mount、UTS、IPC、User                     │
│                                                              │
│  2. Cgroups提供资源限制                                       │
│     • CPU、Memory、IO、Pids                                   │
│                                                              │
│  3. UnionFS提供层叠文件系统                                   │
│     • Copy-on-Write优化                                       │
│                                                              │
│  4. 容器运行时                                                │
│     • runc: OCI参考实现                                       │
│     • containerd: 容器管理守护进程                            │
│                                                              │
│  5. 安全加固                                                  │
│     • Seccomp、AppArmor、Capabilities                         │
│     • 只读rootfs、非特权用户                                  │
│                                                              │
│  容器vs虚拟机选择：                                           │
│  • 需要强隔离/多OS → VM                                       │
│  • 需要快速启动/高密度 → Container                            │
│                                                              │
└─────────────────────────────────────────────────────────────┘
```

---

*容器技术是现代云原生应用的基础，理解其底层原理对于构建可靠的分布式系统至关重要。*
