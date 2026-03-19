/*
 * Auto-generated from: 03_System_Technology_Domains\13_RDMA_Network\02_One_Sided_RDMA.md
 * Line: 289
 * Language: c
 * Block ID: 9fcf5253
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* 序号同步协议 - 确保按序到达 */
typedef struct {
    uint64_t seq_num;      /* 当前序列号 */
    uint64_t acked_seq;    /* 已确认的序列号 */
    uint64_t remote_seq;   /* 远程序列号 */

    struct ibv_qp *qp;
    uint32_t rkey;
    uint64_t remote_addr;
} RDMAStream;

/* 序号头 */
typedef struct {
    uint64_t seq_num;
    uint64_t ack_num;
    uint32_t length;
    uint32_t flags;
} SeqHeader;

/* 发送带序号的数据 */
int rdma_stream_send(RDMAStream *stream, void *data, size_t len) {
    /* 构造带头的消息 */
    char buf[4096];
    SeqHeader *hdr = (SeqHeader *)buf;
    hdr->seq_num = stream->seq_num++;
    hdr->ack_num = stream->remote_seq;
    hdr->length = len;
    hdr->flags = 0;

    memcpy(buf + sizeof(SeqHeader), data, len);

    /* RDMA Write到远程 */
    return rdma_write(stream->qp, buf, sizeof(SeqHeader) + len,
                      stream->remote_addr, stream->rkey, hdr->seq_num);
}

/* 检查序号连续性 */
bool check_seq_order(RDMAStream *stream, uint64_t seq_num) {
    if (seq_num == stream->remote_seq + 1) {
        stream->remote_seq = seq_num;
        return true;
    }
    /* 乱序处理... */
    return false;
}
