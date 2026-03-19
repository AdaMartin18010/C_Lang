/*
 * Auto-generated from: 04_Industrial_Scenarios\16_MIT_6S081_Labs\02_Lab2_System_Calls.md
 * Line: 379
 * Language: c
 * Block ID: 90e0f55c
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 从内核复制到用户空间
int copyout(pagetable_t pagetable, uint64 dstva, char *src, uint64 len);

// 从用户空间复制到内核
int copyin(pagetable_t pagetable, char *dst, uint64 srcva, uint64 len);

// 复制字符串
int copyinstr(pagetable_t pagetable, char *dst, uint64 srcva, uint64 max);
