/*
 * Auto-generated from: 03_System_Technology_Domains\13_RDMA_Network\README.md
 * Line: 128
 * Language: c
 * Block ID: f812c10b
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 注册内存区域
void *buf;
size_t buf_size = 1024 * 1024; // 1MB
struct ibv_mr *mr;

// 分配对齐内存
buf = posix_memalign(&buf, 4096, buf_size);
if (!buf) {
    perror("posix_memalign");
    return -1;
}

// 注册内存区域
mr = ibv_reg_mr(pd, buf, buf_size,
                IBV_ACCESS_LOCAL_WRITE |
                IBV_ACCESS_REMOTE_WRITE |
                IBV_ACCESS_REMOTE_READ);
if (!mr) {
    fprintf(stderr, "Failed to register MR\n");
    return -1;
}

printf("MR registered: lkey=0x%x, rkey=0x%x\n",
       mr->lkey, mr->rkey);
