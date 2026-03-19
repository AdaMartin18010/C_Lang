/*
 * Auto-generated from: 03_System_Technology_Domains\13_RDMA_Network\01_Verbs_API_Detailed.md
 * Line: 517
 * Language: c
 * Block ID: 176cc136
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/**
 * 内存注册与管理
 */

#include <infiniband/verbs.h>
#include <stdlib.h>
#include <string.h>

/* 内存区域管理结构 */
typedef struct {
    void *buf;                      /* 虚拟地址 */
    size_t size;                    /* 大小 */
    struct ibv_mr *mr;              /* 内存区域句柄 */
    uint32_t lkey;                  /* 本地密钥 */
    uint32_t rkey;                  /* 远程密钥 */
} rdma_buffer_t;

/**
 * 注册内存区域
 */
rdma_buffer_t* rdma_register_memory(struct ibv_pd *pd, size_t size,
                                     int access_flags) {
    rdma_buffer_t *buf = calloc(1, sizeof(rdma_buffer_t));
    if (!buf) {
        perror("calloc");
        return NULL;
    }

    /* 分配对齐的内存 */
    if (posix_memalign(&buf->buf, 4096, size)) {
        perror("posix_memalign");
        free(buf);
        return NULL;
    }

    memset(buf->buf, 0, size);
    buf->size = size;

    /* 注册内存 */
    buf->mr = ibv_reg_mr(pd, buf->buf, size, access_flags);
    if (!buf->mr) {
        perror("ibv_reg_mr");
        free(buf->buf);
        free(buf);
        return NULL;
    }

    buf->lkey = buf->mr->lkey;
    buf->rkey = buf->mr->rkey;

    printf("内存注册成功:\n");
    printf("  虚拟地址: %p\n", buf->buf);
    printf("  大小: %zu bytes\n", buf->size);
    printf("  LKey: 0x%08x\n", buf->lkey);
    printf("  RKey: 0x%08x\n", buf->rkey);

    return buf;
}

/**
 * 注册具有特定访问权限的内存
 */
rdma_buffer_t* rdma_register_buffer_full(struct ibv_pd *pd, void *addr,
                                          size_t size, int local_write,
                                          int remote_read, int remote_write,
                                          int remote_atomic) {
    int access = 0;

    if (local_write) access |= IBV_ACCESS_LOCAL_WRITE;
    if (remote_read) access |= IBV_ACCESS_REMOTE_READ;
    if (remote_write) access |= IBV_ACCESS_REMOTE_WRITE;
    if (remote_atomic) access |= IBV_ACCESS_REMOTE_ATOMIC;

    rdma_buffer_t *buf = calloc(1, sizeof(rdma_buffer_t));
    if (!buf) return NULL;

    buf->buf = addr;
    buf->size = size;

    buf->mr = ibv_reg_mr(pd, addr, size, access);
    if (!buf->mr) {
        free(buf);
        return NULL;
    }

    buf->lkey = buf->mr->lkey;
    buf->rkey = buf->mr->rkey;

    return buf;
}

/**
 * 使用ODP（On-Demand Paging）注册内存
 */
rdma_buffer_t* rdma_register_odp(struct ibv_pd *pd, size_t size) {
    rdma_buffer_t *buf = calloc(1, sizeof(rdma_buffer_t));
    if (!buf) return NULL;

    /* ODP不需要预分配内存 */
    buf->size = size;

    /* 使用ON_DEMAND标志 */
    buf->mr = ibv_reg_mr(pd, NULL, size,
                         IBV_ACCESS_LOCAL_WRITE |
                         IBV_ACCESS_REMOTE_READ |
                         IBV_ACCESS_REMOTE_WRITE |
                         IBV_ACCESS_ON_DEMAND);
    if (!buf->mr) {
        perror("ibv_reg_mr ODP");
        free(buf);
        return NULL;
    }

    buf->lkey = buf->mr->lkey;
    buf->rkey = buf->mr->rkey;

    return buf;
}

/**
 * 注销并释放内存
 */
void rdma_deregister_buffer(rdma_buffer_t *buf) {
    if (!buf) return;

    if (buf->mr) {
        ibv_dereg_mr(buf->mr);
    }
    if (buf->buf) {
        free(buf->buf);
    }
    free(buf);
}

/**
 * 修改内存区域访问权限
 */
int rdma_reregister_memory(struct ibv_mr *mr, int new_access) {
    /* 注意：ibv_rereg_mr需要较新版本的rdma-core */
    /* 这里展示概念性代码 */

    struct ibv_rereg_mr_attr attr = {
        .pd = NULL,  /* 保持原PD */
        .addr = mr->addr,
        .len = mr->length,
        .access = new_access,
    };

    return ibv_rereg_mr(mr, IBV_REREG_MR_CHANGE_ACCESS, &attr);
}
