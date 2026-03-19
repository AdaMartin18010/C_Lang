/*
 * Auto-generated from: 03_System_Technology_Domains\03_RDMA_Connection.md
 * Line: 84
 * Language: c
 * Block ID: 7f9d0b69
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 1. SEND/RECV - 双边操作（类似消息传递）
// 发送方发送数据，接收方需预先发布接收请求
ibv_post_send(qp, &send_wr, ...);
ibv_post_recv(qp, &recv_wr, ...);

// 2. WRITE - 单边写操作（远程内存写入）
// 直接写入远程节点的注册内存区域
ibv_post_send(qp, &write_wr, ...);

// 3. READ - 单边读操作（远程内存读取）
// 直接从远程节点读取数据到本地
ibv_post_send(qp, &read_wr, ...);

// 4. ATOMIC - 原子操作（CAS/FAA）
// 用于实现分布式锁和计数器
ibv_post_send(qp, &atomic_wr, ...);
