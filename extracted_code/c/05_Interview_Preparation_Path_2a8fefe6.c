/*
 * Auto-generated from: 06_Thinking_Representation\06_Learning_Paths\05_Interview_Preparation_Path.md
 * Line: 795
 * Language: c
 * Block ID: 2a8fefe6
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 1. 极低延迟（微秒级）
//    - 内核旁路（DPDK/Onload）
//    - 共享内存IPC
//    - CPU亲和性绑定
//    - 禁用超线程

// 2. 高可靠（不丢单）
//    - 持久化日志（WAL）
//    - 主备热切换
//    - 幂等性设计

// 3. 确定性
//    - 禁止GC
//    - 预分配内存
//    - 无锁数据结构

// 4. 风控
//    - 实时风控检查
//    - 异常熔断

typedef struct {
    // 订单簿（红黑树）
    OrderBook *order_books[MAX_SYMBOLS];

    // 撮合引擎（无锁队列）
    LockFreeQueue *order_queue;

    // 风控检查器
    RiskChecker *risk_checker;

    // 持久化（环形缓冲区 + 后台刷盘）
    WAL *wal;

    // 网络（DPDK）
    DPDKContext *network;
} TradingSystem;
