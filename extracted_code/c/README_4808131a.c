/*
 * Auto-generated from: 03_System_Technology_Domains\13_RDMA_Network\README.md
 * Line: 285
 * Language: c
 * Block ID: 4808131a
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 原子比较并交换（Compare and Swap）
struct ibv_sge atomic_sg;
struct ibv_send_wr atomic_wr = {};
uint64_t compare = expected_value;
uint64_t swap = new_value;

atomic_sg.addr = (uint64_t)&local_result;
atomic_sg.length = sizeof(uint64_t);
atomic_sg.lkey = local_mr->lkey;

atomic_wr.wr_id = 5;
atomic_wr.opcode = IBV_WR_ATOMIC_CMP_AND_SWP;
atomic_wr.sg_list = &atomic_sg;
atomic_wr.num_sge = 1;
atomic_wr.wr.atomic.remote_addr = remote_addr;
atomic_wr.wr.atomic.rkey = remote_rkey;
atomic_wr.wr.atomic.compare_add = compare;
atomic_wr.wr.atomic.swap = swap;

ret = ibv_post_send(qp, &atomic_wr, &bad_wr);
