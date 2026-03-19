/*
 * Auto-generated from: 06_Thinking_Representation\06_Learning_Paths\05_Interview_Preparation_Path.md
 * Line: 638
 * Language: c
 * Block ID: bf4015aa
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 核心问题：
// 1. 数据分片（Sharding）
//    - 一致性哈希
//    - 虚拟节点
//
// 2. 复制（Replication）
//    - 主从复制 vs 多主复制
//    - 同步复制 vs 异步复制
//
// 3. 一致性协议
//    - Raft/Paxos（强一致性）
//    - 最终一致性（Gossip）
//
// 4. 故障处理
//    - 故障检测（心跳）
//    - 自动故障转移
//
// 5. 性能优化
//    - 读从节点
//    - 写缓冲区
//    - 压缩

typedef struct {
    // 节点信息
    NodeID id;
    Address addr;

    // Raft状态机
    RaftState raft;

    // 存储引擎（LSM-Tree或B+树）
    StorageEngine *storage;

    // 分片管理
    ShardManager *shards;

    // 集群成员管理
    Membership *members;
} KVServer;

// 关键设计决策：
// - CAP权衡：CP还是AP？
// - 存储引擎选择：读优化还是写优化？
// - 一致性级别：强一致还是最终一致？
