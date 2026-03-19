/*
 * Auto-generated from: 04_Industrial_Scenarios\16_MIT_6S081_Labs\03_Lab3_Page_Tables.md
 * Line: 267
 * Language: c
 * Block ID: 2939a2db
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// kernel/vm.c

// 创建空页表
pagetable_t uvmcreate(void);

// 映射页面
int mappages(pagetable_t pagetable, uint64 va, uint64 size,
             uint64 pa, int perm);

// 取消映射
void uvmunmap(pagetable_t pagetable, uint64 va, uint64 npages, int do_free);

// 复制页表
int uvmcopy(pagetable_t old, pagetable_t new, uint64 sz);

//  walk页表，返回PTE地址
pte_t *walk(pagetable_t pagetable, uint64 va, int alloc);

// 用户地址复制
int copyout(pagetable_t pagetable, uint64 dstva, char *src, uint64 len);
int copyin(pagetable_t pagetable, char *dst, uint64 srcva, uint64 len);
int copyinstr(pagetable_t pagetable, char *dst, uint64 srcva, uint64 max);
