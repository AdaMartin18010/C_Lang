/*
 * Auto-generated from: 03_System_Technology_Domains\13_RDMA_Network\01_Verbs_API.md
 * Line: 498
 * Language: c
 * Block ID: ebc4b46b
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* 轮询完成队列 */
int rdma_poll_cq(RDMACtx *rdma, int max_entries, struct ibv_wc *wc) {
    int n = ibv_poll_cq(rdma->cq, max_entries, wc);

    if (n < 0) {
        perror("ibv_poll_cq");
        return -1;
    }

    for (int i = 0; i < n; i++) {
        if (wc[i].status != IBV_WC_SUCCESS) {
            fprintf(stderr, "WC failed: wr_id=%lu, status=%s\n",
                    wc[i].wr_id, ibv_wc_status_str(wc[i].status));
            return -1;
        }

        switch (wc[i].opcode) {
        case IBV_WC_SEND:
            printf("Send completed\n");
            break;
        case IBV_WC_RECV:
            printf("Recv completed, imm=%u\n", ntohl(wc[i].imm_data));
            break;
        case IBV_WC_RDMA_WRITE:
            printf("RDMA Write completed\n");
            break;
        case IBV_WC_RDMA_READ:
            printf("RDMA Read completed\n");
            break;
        }
    }

    return n;
}

/* 等待完成 */
int rdma_wait_completion(RDMACtx *rdma, uint64_t target_wr_id) {
    struct ibv_wc wc;
    int timeout = 10000;  /* 10秒超时 */

    while (timeout-- > 0) {
        int n = rdma_poll_cq(rdma, 1, &wc);
        if (n > 0 && wc.wr_id == target_wr_id) {
            return 0;  /* 完成 */
        }
        usleep(1000);  /* 1ms */
    }

    return -1;  /* 超时 */
}
