# 微内核架构模式

## 概述

```text
微内核架构将最小功能放入内核，其他服务作为用户态进程运行。

核心组件:
- 进程间通信 (IPC)
- 基本调度
- 内存管理
- 中断处理

用户态服务:
- 文件系统
- 设备驱动
- 网络协议栈
- GUI

优势: 可靠性、可维护性、灵活性
案例: MINIX, QNX, seL4, L4
```

## IPC实现

```c
/* 消息传递 */
#define MAX_MSG_SIZE 4096

typedef struct {
    int source;      /* 发送者PID */
    int dest;        /* 接收者PID */
    int type;
    size_t size;
    char data[MAX_MSG_SIZE];
} message_t;

/* 同步发送 */
int send(int dest, message_t* msg) {
    msg->source = getpid();

    /* 陷入内核 */
    return syscall(SYS_SEND, dest, msg);
}

/* 接收 */
int receive(int source, message_t* msg) {
    return syscall(SYS_RECEIVE, source, msg);
}

/* 调用-返回 */
int rpc_call(int service, int func, void* args, void* result) {
    message_t msg = {
        .dest = service,
        .type = MSG_RPC_CALL,
        .data = {func, args}
    };

    send(service, &msg);

    /* 等待回复 */
    message_t reply;
    receive(service, &reply);

    memcpy(result, reply.data, reply.size);
    return 0;
}
```

## 驱动作为服务

```c
/* 块设备驱动 (用户态) */
void block_driver_main(void) {
    message_t msg;

    while (1) {
        receive(ANY, &msg);

        switch (msg.type) {
            case BLOCK_READ:
                handle_read(msg.data);
                break;
            case BLOCK_WRITE:
                handle_write(msg.data);
                break;
            case BLOCK_IOCTL:
                handle_ioctl(msg.data);
                break;
        }

        /* 回复 */
        msg.dest = msg.source;
        send(msg.source, &msg);
    }
}

/* 内核只处理底层中断 */
void disk_interrupt_handler(void) {
    /* 通知驱动进程 */
    notify(DISK_DRIVER_PID);
}
```

## 能力安全

```c
/* 能力 (Capability) */
typedef struct {
    uint64_t object_id;
    uint32_t rights;     /* 读/写/执行 */
    uint32_t issuer;
} capability_t;

/* 检查权限 */
bool check_cap(capability_t* cap, int required_right) {
    return (cap->rights & required_right) == required_right;
}

/* 使用能力访问资源 */
int cap_read(capability_t* cap, void* buf, size_t len) {
    if (!check_cap(cap, CAP_READ)) {
        return -EPERM;
    }

    return kernel_read(cap->object_id, buf, len);
}
```

---

## 参考

- [seL4 Microkernel](https://sel4.systems/)
- [MINIX 3](http://www.minix3.org/)
- [QNX](http://www.qnx.com/)
