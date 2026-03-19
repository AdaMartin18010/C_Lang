/*
 * Auto-generated from: 14_Video_Tutorials\01_Video_Scripts\03_Memory_Management.md
 * Line: 246
 * Language: c
 * Block ID: 32feb56e
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

空闲块链表: [16B] → [64B] → [128B] → [256B] → NULL

malloc(100):
├── 首次适应: 选择 [128B] 块
├── 最佳适应: 选择 [128B] 块 (最小满足)
└── 最差适应: 选择 [256B] 块 (最大块)

碎片影响:
├── 首次适应: 低地址聚集碎片
├── 最佳适应: 产生大量小碎片
└── 最差适应: 碎片分布较均匀
