/*
 * Auto-generated from: 03_System_Technology_Domains\13_RDMA_Network\01_Verbs_API_Detailed.md
 * Line: 1777
 * Language: c
 * Block ID: 33b0ec91
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/**
 * 内存窗口(Memory Window)操作
 */

#include <infiniband/verbs.h>

/**
 * 分配内存窗口
 */
struct ibv_mw* allocate_memory_window(struct ibv_pd *pd, enum ibv_mw_type type) {
    struct ibv_mw *mw = ibv_alloc_mw(pd, type);
    if (!mw) {
        perror("ibv_alloc_mw");
        return NULL;
    }

    printf("内存窗口分配成功，RKey: 0x%08x\n", mw->rkey);
    return mw;
}

/**
 * 绑定内存窗口到内存区域
 */
int bind_memory_window(struct ibv_qp *qp, struct ibv_mw *mw,
                       struct ibv_mr *mr, uint64_t addr, size_t length,
                       int access_flags) {
    struct ibv_mw_bind bind_attr;

    memset(&bind_attr, 0, sizeof(bind_attr));
    bind_attr.wr_id = 30;
    bind_attr.send_flags = IBV_SEND_SIGNALED;
    bind_attr.bind_info.mr = mr;
    bind_attr.bind_info.addr = addr;
    bind_attr.bind_info.length = length;
    bind_attr.bind_info.mw_access_flags = access_flags;

    int ret = ibv_bind_mw(qp, mw, &bind_attr);
    if (ret) {
        perror("ibv_bind_mw");
        return ret;
    }

    printf("内存窗口绑定成功: addr=0x%lx, length=%zu, new_rkey=0x%08x\n",
           addr, length, mw->rkey);
    return 0;
}

/**
 * 解绑内存窗口
 */
int invalidate_memory_window(struct ibv_qp *qp, struct ibv_mw *mw) {
    struct ibv_send_wr wr;
    struct ibv_send_wr *bad_wr;

    memset(&wr, 0, sizeof(wr));
    wr.wr_id = 31;
    wr.opcode = IBV_WR_LOCAL_INV;
    wr.invalidate_rkey = mw->rkey;
    wr.send_flags = IBV_SEND_SIGNALED;

    int ret = ibv_post_send(qp, &wr, &bad_wr);
    if (ret) {
        perror("ibv_post_send (invalidate)");
        return ret;
    }

    printf("内存窗口无效化成功: rkey=0x%08x\n", mw->rkey);
    return 0;
}

/**
 * 释放内存窗口
 */
void deallocate_memory_window(struct ibv_mw *mw) {
    if (mw) {
        ibv_dealloc_mw(mw);
        printf("内存窗口已释放\n");
    }
}

/**
 * 内存窗口使用示例：动态权限控制
 */
int memory_window_example(struct ibv_pd *pd, struct ibv_qp *qp,
                          struct ibv_mr *mr) {
    struct ibv_mw *mw;
    int ret;

    /* 1. 分配MW */
    mw = allocate_memory_window(pd, IBV_MW_TYPE_1);
    if (!mw) return -1;

    /* 2. 绑定MW，授予只读权限 */
    ret = bind_memory_window(qp, mw, mr, (uint64_t)mr->addr, mr->length,
                             IBV_ACCESS_REMOTE_READ);
    if (ret) goto cleanup;

    /* 3. 使用MW的RKey进行RDMA Read... */
    /* post_rdma_read(qp, ..., mw->rkey); */

    /* 4. 解绑MW，撤销权限 */
    ret = invalidate_memory_window(qp, mw);
    if (ret) goto cleanup;

    /* 5. 重新绑定，授予只写权限 */
    ret = bind_memory_window(qp, mw, mr, (uint64_t)mr->addr, mr->length,
                             IBV_ACCESS_REMOTE_WRITE);
    if (ret) goto cleanup;

    /* 6. 使用新的RKey进行RDMA Write... */
    /* post_rdma_write(qp, ..., mw->rkey); */

cleanup:
    deallocate_memory_window(mw);
    return ret;
}
