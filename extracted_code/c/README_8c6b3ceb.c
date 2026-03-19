/*
 * Auto-generated from: 03_System_Technology_Domains\13_RDMA_Network\README.md
 * Line: 366
 * Language: c
 * Block ID: 8c6b3ceb
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 创建共享接收队列
struct ibv_srq_init_attr srq_attr = {};
srq_attr.attr.max_wr = 1000;
srq_attr.attr.max_sge = 1;

struct ibv_srq *srq = ibv_create_srq(pd, &srq_attr);
