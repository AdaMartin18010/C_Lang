/*
 * Auto-generated from: 01_Core_Knowledge_System\02_Core_Layer\02_Memory_Management.md
 * Line: 742
 * Language: c
 * Block ID: 1118a71c
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

定义: ValidPointer(p, T) 当且仅当:

V1: p ≠ NULL → p 指向已分配内存块内的有效位置
V2: p 满足 alignof(T) 的对齐要求
V3: 从 p 开始的 sizeof(T) 字节都在同一内存块内
V4: 如果 p 指向堆内存，则该内存块状态为 ALLOCATED

定义: SafeToDereference(p, T) 当且仅当:

S1: ValidPointer(p, T)
S2: p 指向的对象生命周期为 ACTIVE
S3: 对象的类型与 T 兼容（严格别名规则）
