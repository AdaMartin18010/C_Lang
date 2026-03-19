/*
 * Auto-generated from: 04_Industrial_Scenarios\17_CMU_15_213_Labs\06_Malloc_Lab.md
 * Line: 370
 * Language: c
 * Block ID: 2c150bfa
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 显式空闲链表块结构
┌─────────────────────────────────────────┐
│  块头部 (4字节)                         │
├─────────────────────────────────────────┤
│  前驱指针 (4/8字节) - 仅在空闲块中      │
├─────────────────────────────────────────┤
│  后继指针 (4/8字节) - 仅在空闲块中      │
├─────────────────────────────────────────┤
│  有效载荷 / 填充                        │
├─────────────────────────────────────────┤
│  块脚部 (4字节) - 仅在空闲块中          │
└─────────────────────────────────────────┘
