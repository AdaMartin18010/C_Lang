/*
 * Auto-generated from: 05_Deep_Structure_MetaPhysics\13_Self_Modifying_Code\02_JIT_Physical_Constraints.md
 * Line: 802
 * Language: c
 * Block ID: 34090a90
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 错误：不考虑NUMA的内存分配
void numa_unaware_allocation(void) {
    void *code = mmap(NULL, size, PROT_EXEC, ...);  // ❌ 可能在远程NUMA节点

    // 在其他核心上执行
    run_on_core(code, remote_core);  // 性能差
}

// 正确：NUMA感知的代码分配
#ifdef __linux__
#include <numa.h>

void* numa_aware_allocation(int preferred_node, size_t size) {
    void *mem = numa_alloc_onnode(size, preferred_node);

    // 设置可执行权限
    mprotect(mem, size, PROT_READ | PROT_WRITE | PROT_EXEC);

    return mem;
}

void run_on_numa_node(void *code, int node) {
    // 绑定到特定NUMA节点
    numa_run_on_node(node);

    // 执行代码
    ((void(*)(void))code)();
}
#endif
