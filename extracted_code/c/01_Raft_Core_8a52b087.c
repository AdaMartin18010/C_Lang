/*
 * Auto-generated from: 03_System_Technology_Domains\08_Distributed_Consensus\01_Raft_Core.md
 * Line: 402
 * Language: c
 * Block ID: 8a52b087
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 网络分区导致双领导者
// 解决方案：只有复制到多数节点才算提交
// 旧领导者的未复制日志会被新领导者覆盖
