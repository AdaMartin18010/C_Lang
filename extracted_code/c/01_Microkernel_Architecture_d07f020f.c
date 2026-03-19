/*
 * Auto-generated from: 07_Modern_Toolchain\09_System_Architecture\01_Microkernel_Architecture.md
 * Line: 460
 * Language: c
 * Block ID: d07f020f
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* seL4能力系统 */

/* 能力类型 */
typedef enum {
    seL4_CapNull = 0,
    seL4_CapUntyped,        /* 未类型化内存 */
    seL4_CapEndpoint,       /* IPC端点 */
    seL4_CapNotification,   /* 通知对象 */
    seL4_CapReply,          /* 回复能力 */
    seL4_CapCNode,          /* CNode (能力节点) */
    seL4_CapThread,         /* 线程控制 */
    seL4_CapIRQControl,     /* 中断控制 */
    seL4_CapIRQHandler,     /* 中断处理 */
    seL4_CapZombie,         /* 僵尸对象 */
    seL4_CapDomain,         /* 域控制 */
    seL4_NumCapabilities
} seL4_cap_type_t;

/* 能力结构 */
typedef struct {
    uint64_t words[2];
} seL4_CPtr;

/* 系统调用 - 发送IPC */
static inline seL4_MessageInfo_t seL4_Send(
    seL4_CPtr dest,                    /* 目标能力 */
    seL4_MessageInfo_t msgInfo         /* 消息信息 */
) {
    seL4_MessageInfo_t info;

    asm volatile(
        "mov %%rcx, %%r10\n"
        "syscall\n"
        : "=r"(info.words[0])
        : "a"(seL4_SysSend),            /* 系统调用号 */
          "D"(dest),                    /* 参数1: 目标 */
          "S"(msgInfo.words[0])         /* 参数2: 消息 */
        : "rcx", "r11", "memory"
    );

    return info;
}

/* 创建地址空间 */
int create_address_space(seL4_CPtr untyped, seL4_CPtr cnode, seL4_CPtr vspace) {
    seL4_Error error;

    /* 从untyped分配页目录 */
    error = seL4_Untyped_Retype(
        untyped,                    /* 未类型化内存 */
        seL4_X86_PageDirectoryObject, /* 对象类型 */
        0,                          /* 大小 */
        cnode,                      /* 目标CNode */
        0, 0,                       /* 路径数据 */
        vspace,                     /* 目标槽位 */
        1                           /* 数量 */
    );

    return error == seL4_NoError ? 0 : -1;
}
