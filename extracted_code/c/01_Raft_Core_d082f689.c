/*
 * Auto-generated from: 03_System_Technology_Domains\08_Distributed_Consensus\01_Raft_Core.md
 * Line: 410
 * Language: c
 * Block ID: d082f689
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 跟随者日志可能比领导者新（旧领导者）
// 解决方案：领导者强制覆盖跟随者日志
// 注意：只有未提交的日志会被覆盖
