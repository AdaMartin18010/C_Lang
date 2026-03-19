/*
 * Auto-generated from: 03_System_Technology_Domains\13_RDMA_Network\01_Verbs_API_Detailed.md
 * Line: 683
 * Language: c
 * Block ID: 13aff429
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* 简化的Work Completion结构 */
struct ibv_wc {
    uint64_t wr_id;          /* 用户提供的WR标识 */
    enum ibv_wc_status status;  /* 完成状态 */
    enum ibv_wc_opcode opcode;  /* 操作类型 */
    uint32_t qp_num;         /* QP编号 */
    uint32_t byte_len;       /* 接收的字节数 */
    uint32_t imm_data;       /* 即时数据 */
    uint32_t src_qp;         /* 源QP（用于UD） */
    uint16_t pkey_index;     /* PKey索引 */
    uint16_t slid;           /* 源LID */
    uint8_t  sl;             /* 服务级别 */
    uint8_t  dlid_path_bits; /* DLID路径位 */
};
