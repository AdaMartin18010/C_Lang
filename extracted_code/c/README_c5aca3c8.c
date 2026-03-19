/*
 * Auto-generated from: 03_System_Technology_Domains\13_RDMA_Network\README.md
 * Line: 310
 * Language: c
 * Block ID: c5aca3c8
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 轮询完成队列
void poll_completions(struct ibv_cq *cq, int expected) {
    struct ibv_wc wc;
    int completed = 0;

    while (completed < expected) {
        int ret = ibv_poll_cq(cq, 1, &wc);
        if (ret < 0) {
            fprintf(stderr, "Poll CQ failed\n");
            break;
        }
        if (ret == 0)
            continue;

        if (wc.status != IBV_WC_SUCCESS) {
            fprintf(stderr, "Work completion error: %s\n",
                    ibv_wc_status_str(wc.status));
            continue;
        }

        printf("Completion: wr_id=%lu, opcode=%d\n",
               wc.wr_id, wc.opcode);
        completed++;
    }
}
