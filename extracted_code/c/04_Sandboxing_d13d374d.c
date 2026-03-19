/*
 * Auto-generated from: 05_Deep_Structure_MetaPhysics\04_Sandboxing.md
 * Line: 330
 * Language: c
 * Block ID: d13d374d
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

#define _GNU_SOURCE
#include <sched.h>
#include <sys/syscall.h>
#include <unistd.h>

// 创建新的Namespace
int create_namespaces() {
    // unshare - 将当前进程移到新的namespace
    int flags = CLONE_NEWNS |      // Mount namespace
                CLONE_NEWUTS |      // UTS namespace
                CLONE_NEWIPC |      // IPC namespace
                CLONE_NEWPID |      // PID namespace
                CLONE_NEWNET |      // Network namespace
                CLONE_NEWUSER;      // User namespace

    if (unshare(flags) == -1) {
        perror("unshare");
        return -1;
    }

    return 0;
}

// 使用clone创建新进程并设置namespace
int spawn_in_new_namespace(void (*func)(void*), void* arg) {
    const int STACK_SIZE = 1024 * 1024;
    char* stack = mmap(NULL, STACK_SIZE,
                       PROT_READ | PROT_WRITE,
                       MAP_PRIVATE | MAP_ANONYMOUS | MAP_STACK,
                       -1, 0);

    if (stack == MAP_FAILED) {
        perror("mmap");
        return -1;
    }

    int flags = CLONE_NEWNS | CLONE_NEWUTS | CLONE_NEWIPC |
                CLONE_NEWPID | CLONE_NEWNET | CLONE_NEWUSER |
                SIGCHLD;

    pid_t pid = clone(func, stack + STACK_SIZE, flags, arg);

    if (pid == -1) {
        perror("clone");
        return -1;
    }

    return pid;
}

// 配置Mount Namespace
void setup_mount_namespace() {
    // 创建私有挂载传播
    mount("none", "/", NULL, MS_REC | MS_PRIVATE, NULL);

    // 创建新的根文件系统
    mount("tmpfs", "/tmp/sandbox-root", "tmpfs", 0,
          "size=100M,mode=755");

    // 绑定挂载必要的目录
    mount("/bin", "/tmp/sandbox-root/bin", NULL,
          MS_BIND | MS_RDONLY, NULL);
    mount("/lib", "/tmp/sandbox-root/lib", NULL,
          MS_BIND | MS_RDONLY, NULL);
    mount("/lib64", "/tmp/sandbox-root/lib64", NULL,
          MS_BIND | MS_RDONLY, NULL);

    // 切换根目录
    chroot("/tmp/sandbox-root");
    chdir("/");
}

// 配置Network Namespace
void setup_network_namespace() {
    // 创建虚拟网卡对
    system("ip link add veth0 type veth peer name veth1");

    // 将veth1移入新的namespace
    // (需要在创建namespace的进程中执行)

    // 配置回环接口
    system("ip link set lo up");
    system("ip link set veth1 up");
    system("ip addr add 10.0.0.2/24 dev veth1");
    system("ip route add default via 10.0.0.1");
}

// 配置User Namespace
void setup_user_namespace() {
    // 映射UID/GID
    // 写入 /proc/self/uid_map 和 /proc/self/gid_map

    int fd = open("/proc/self/uid_map", O_WRONLY);
    if (fd < 0) {
        perror("open uid_map");
        return;
    }

    // 映射: 容器内0 -> 宿主1000
    const char* map = "0 1000 1";
    write(fd, map, strlen(map));
    close(fd);

    // 设置gid_map (需要先禁用setgroups)
    fd = open("/proc/self/setgroups", O_WRONLY);
    if (fd >= 0) {
        write(fd, "deny", 4);
        close(fd);
    }

    fd = open("/proc/self/gid_map", O_WRONLY);
    if (fd >= 0) {
        const char* gid_map = "0 1000 1";
        write(fd, gid_map, strlen(gid_map));
        close(fd);
    }
}
