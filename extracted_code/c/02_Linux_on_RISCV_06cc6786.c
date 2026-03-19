/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\20_Industrial_Case_Studies\02_Linux_on_RISCV.md
 * Line: 661
 * Language: c
 * Block ID: 06cc6786
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 使用hugetlbfs减少TLB缺失
#include <sys/mman.h>
#include <fcntl.h>

void* allocate_huge_page(size_t size) {
    int fd = open("/mnt/hugepages/myfile", O_CREAT | O_RDWR, 0755);
    void *addr = mmap(NULL, size, PROT_READ | PROT_WRITE,
                      MAP_SHARED | MAP_HUGETLB, fd, 0);
    close(fd);
    return addr;
}

// NUMA感知内存分配
#include <numa.h>

void numa_optimized_alloc(void **ptr, size_t size, int node) {
    *ptr = numa_alloc_onnode(size, node);
    mlock(*ptr, size);  // 防止换出
}
