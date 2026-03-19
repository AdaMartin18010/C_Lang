/*
 * Auto-generated from: 07_Modern_Toolchain\09_System_Architecture\01_Microkernel_Architecture.md
 * Line: 201
 * Language: c
 * Block ID: b8685846
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* L4风格IPC实现 */

/* 消息寄存器 */
#define NUM_MR 64
typedef struct {
    uint64_t mr[NUM_MR];  /* 消息寄存器 */
    uint64_t br[NUM_MR];  /* 缓冲区寄存器 */
} msg_regs_t;

/* IPC系统调用 */
int sys_ipc(
    uint64_t to,           /* 目标线程 */
    uint64_t from_spec,    /* 接收来源 */
    msg_regs_t* msg,       /* 消息 */
    uint64_t timeout       /* 超时 */
) {
    thread_t* current = get_current_thread();

    if (to != 0) {
        /* 发送阶段 */
        thread_t* dest = lookup_thread(to);
        if (!dest || dest->state != THREAD_WAITING) {
            return ERR_INVALID_THREAD;
        }

        /* 复制消息 */
        copy_message(current, dest, msg);

        /* 唤醒接收者 */
        dest->state = THREAD_RUNNING;
        enqueue_ready(dest);

        /* 如果需要回复，当前线程阻塞 */
        if (is_call_ipc(to)) {
            current->state = THREAD_WAITING;
            current->wait_for = to;
            schedule();
        }
    }

    if (from_spec != 0) {
        /* 接收阶段 */
        if (!has_pending_message(current)) {
            current->state = THREAD_WAITING;
            current->wait_from = from_spec;
            schedule();
        }

        /* 复制接收到的消息 */
        receive_message(current, msg);
    }

    return SUCCESS;
}

/* 优化的IPC路径 */
__attribute__((noinline))
int fast_ipc_call(uint64_t server_tid, void* args, size_t len) {
    register uint64_t r0 asm("r0") = server_tid;
    register uint64_t r1 asm("r1") = (uint64_t)args;
    register uint64_t r2 asm("r2") = len;

    /* 直接系统调用 */
    asm volatile(
        "svc #0x100\n"
        : "=r"(r0)
        : "r"(r0), "r"(r1), "r"(r2)
        : "memory"
    );

    return (int)r0;
}
