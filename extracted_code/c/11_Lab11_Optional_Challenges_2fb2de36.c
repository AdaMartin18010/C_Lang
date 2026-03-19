/*
 * Auto-generated from: 04_Industrial_Scenarios\16_MIT_6S081_Labs\11_Lab11_Optional_Challenges.md
 * Line: 15
 * Language: c
 * Block ID: 2fb2de36
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 当前网络数据流:
网卡DMA → 内核缓冲区 → 用户缓冲区
(2次拷贝)

// 零拷贝优化:
网卡DMA → 共享内存 ← 直接访问
(0次拷贝)

实现要点:
1. 网卡DMA直接映射到用户空间
2. 用户态直接处理数据包
3. 需要处理缓存一致性和同步
